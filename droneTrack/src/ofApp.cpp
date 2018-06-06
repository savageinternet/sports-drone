#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

const float dyingTime = 1;

void Glow::setup(const Player& track) {
    color.setHsb(ofRandom(0, 255), 255, 255);
    cur = toOf(track).getCenter();
    smooth = cur;
    recorded = false;
}

void Glow::update(const Player& track) {
    cur = toOf(track).getCenter();
    smooth.interpolate(cur, .5);
    all.addVertex(cur.x, cur.y);
}

void Glow::kill() {
    float curTime = ofGetElapsedTimef();
    if(startedDying == 0) {
        startedDying = curTime;
    } else if(curTime - startedDying > dyingTime) {
        dead = true;
        // if it dies, we want to record!
        // but... this will get taken care of by another call
    }
}

void Glow::updateDiedFrame(int diedFrame) {
    if (startedDying == 0) {
        this->diedFrame = diedFrame;
    } else {} // do nothing if it already stared dying.
}

void Glow::draw() {
    ofPushStyle();
    float size = 16;
    ofSetColor(255);
    if(startedDying) {
        ofSetColor(ofColor::red);
        size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
    }
    ofNoFill();
    ofDrawCircle(cur, size);
    ofSetColor(color);
    all.draw();
    ofSetColor(255);
    ofDrawBitmapString(ofToString(label), cur);
    ofPopStyle();
}

void Glow::record(std::ofstream& fileStream, bool forceRecord) {
    if(((dead || startedDying) && !recorded) || forceRecord) {
        doRecord(fileStream);
    }
}

void Glow::record(std::ofstream& fileStream) {
    bool forceRecord = false;
    record(fileStream, forceRecord);
}

void Glow::doRecord(std::ofstream& fileStream) {
    json j;
    j["label"] = label;
    j["path"] = {};
    for (int i = 0; i < all.getVertices().size(); i++) {
        j["path"].push_back({all.getVertices().at(i).x, all.getVertices().at(i).y});
    }
    j["born"] = bornFrame;
    if(dead || startedDying) {
        j["died"] = diedFrame;
    } else {
        j["died"] = false;
    }
    // TODO: also include the largest size of the blob that was tracked here? would be helpful for filtering?
    fileStream << j.dump() << ",\n";
    recorded = true;
}

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofBackground(0);
    
    movie.load("/Users/valkyrie/projects/savage-internet/sports drone/video/soccer-cut.mp4");
    movie.setVolume(0);
    movie.play();
    
    colorFrame.allocate(movie.getWidth(), movie.getHeight(), OF_IMAGE_GRAYSCALE);
    
    //create Background Subtractor object
    bgsub = createBackgroundSubtractorMOG2(); //MOG2 or KNN (? can't get MOG or GMG?)
    
    contourFinder.setMinAreaRadius(5);
    contourFinder.setMaxAreaRadius(15);
    contourFinder.setThreshold(15);
    
    // wait half a second (at 32fps) before forgetting something
    tracker.setPersistence(16);
    // an object can move up to 30 pixels per frame
    tracker.setMaximumDistance(30);
    
    // the GUI bits
    // parameters of the video processing
    processingParameters.setName("Processing Parameters");
    processingParameters.add(minRadius.set("Min radius:",7,0,100));
    processingParameters.add(maxRadius.set("Max radius:",15,0,100));
    processingParameters.add(persistence.set("Persistence (frames):",54,0,160));
    processingParameters.add(maxVelocity.set("Max velocity:",15,0,100));
    processingParameters.add(blurRadius.set("Blur radius:",12,0,100));
    processingParameters.add(thresholdB.set("Simple threshold?",false));
    processingParameters.add(thresholdValue.set("Threshold:",90,0,255));
    processingGui.setup(processingParameters);
    processingGui.setPosition(1070,10);
    
    // parameters of the sport and video
    sportParameters.setName("Sport Infoz");
    sportParameters.add(numberPlayers.set("Num Players:",10,0,22));
    sportParameters.add(sportEnumChooser.set("Sport:",0,0,2));
    sportParameters.add(team1Color.set("Team 1 Color",ofColor(127),ofColor(0,0),ofColor(255)));
    sportParameters.add(team2Color.set("Team 2 Color",ofColor(127),ofColor(0,0),ofColor(255)));
    sportParameters.add(recordRunthrough.set("Record full run-through",false));
    gui.setup(sportParameters);
    
    loadVideo.addListener(this, &ofApp::loadVideoPressed);
    recordRunthrough.enableEvents(); // can we add an event to a boolean?
    recordRunthrough.addListener(this, &ofApp::recordRunthroughPressed);
    forceRecord = false;
    gui.add(loadVideo.setup("Load new video"));
    
    videoDetails = *new SportVideo();
    
    curFrame = 0;
}

void ofApp::update() {
    movie.update();
    
    // now change things the user input
    contourFinder.setMinAreaRadius(minRadius);
    contourFinder.setMaxAreaRadius(maxRadius);
    contourFinder.setThreshold(thresholdValue);
    tracker.setPersistence(persistence);
    tracker.setMaximumDistance(maxVelocity);
    videoDetails.updateExpectedPlayerCount(numberPlayers);
    videoDetails.updateSport(static_cast<SportName>(sportEnumChooser.get()));
    
    // stop recording when we get to the end
    if(recordRunthrough && movie.getIsMovieDone()) {
        recordRunthrough = false;
        forceRecord = true;
    }
    
    if(movie.isFrameNew()) {
        blur(movie, blurRadius);
        colorFrame.setFromPixels(movie.getPixels());
        colorFrameMat = toCv(colorFrame);
        cv::cvtColor(toCv(colorFrame), grayFrameMat, cv::COLOR_RGB2GRAY);
        if (thresholdB) {
            // just do simple thresholding on the image
            threshold(grayFrameMat, grayFrameMat, thresholdValue);
            contourFinder.findContours(grayFrameMat);
        } else {
            // ok, now we do the magic with the foreground/background finding
            bgsub->apply(colorFrameMat, fgMask);
            contourFinder.findContours(fgMask);
        }
        
        //NOTE from VALKYRIE : under construction :3
        std::vector<Player> foundPlayers;
        for(unsigned int i = 0; i < contourFinder.getBoundingRects().size(); i++) {
            Player p(contourFinder.getBoundingRects().at(i));
            p.velocity = toOf(contourFinder.getVelocity(i));
            
            // extract the average pixel color from around the centroid? need to think about exactly what to do here. be sure to use the blurred video, I guess, since it will de-noise shit?
            //p.jerseyColor = ???
            
            foundPlayers.push_back(p);
        }
        tracker.track(/*foundPlayers);*/contourFinder.getBoundingRects());
    }
}

void ofApp::draw() {
    ofSetColor(255);
    drawMat(grayFrameMat,0,0);
    
    if (grayFrameMat.dims > 0) { // don't run it before we've assigned some data here
        
        // this next part comes from https://stackoverflow.com/questions/35886307/how-to-make-white-part-of-the-image-transparent-by-using-android-opencv
        
        /* make the thresholded image with transparency...!
        // Splitting & adding Alpha
        vector<Mat> channels;   // C++ version of ArrayList<Mat>
        split(grayFrameMat, channels);   // Automatically splits channels and adds them to channels. The size of channels = 3
        
        Mat alpha;
        channels[0].copyTo(alpha);
        
        channels.push_back(alpha);   // Adds mask(alpha) channel. The size of channels = 4 (we use one of the channels again, since we're thresholding to white, anyway)
        
        // Merging
        Mat dst;
        merge(channels, dst);   // dst is created with 4-channel(BGRA).
        // Note that OpenCV applies BGRA by default if your array size is 4,
        // even if actual order is different. In this case this makes sense.
        
         drawMat(dst, 0, 0);*/ // for some reason... can't get this to work. :( want black to show up as alpha'd, but it doesn't seem to like me for some reason.
    }
    
    // draw the rest of the stuff
    contourFinder.draw();
    vector<Glow>& followers = tracker.getFollowers();
    for(int i = 0; i < followers.size(); i++) {
        followers[i].draw();
        followers[i].updateDiedFrame(curFrame);
        if (recordRunthrough) followers[i].record(recordingFile, forceRecord);
    }
    
    // once we've finished the video, we want to force everything to record into the file. after that, clear
    // our memories of EVERYTHING woooOOOOOooOOOoooooOooo...
    if(forceRecord) {
        forceRecord = false;
        finalizeRecording();
        recordingFile.close();
    }
    
    // visualize labels. from https://github.com/kylemcdonald/ofxCv/blob/master/example-contours-tracking/src/ofApp.cpp
    for(int i = 0; i < contourFinder.size(); i++) {
        ofPoint center = toOf(contourFinder.getCenter(i));
        ofPushMatrix();
        ofTranslate(center.x, center.y);
        int label = contourFinder.getLabel(i);
        uint age = tracker.getAge(label);
        if (age > 32) { // if it has lived for at least 1s, draw it. (I don't actually know what age is...)
            string msg = ofToString(label) + ":" + ofToString(age);
            ofDrawBitmapString(msg, 0, 0);
            ofVec2f velocity = toOf(contourFinder.getVelocity(i));
            ofScale(5, 5);
            ofDrawLine(0, 0, velocity.x, velocity.y);
        }
        ofPopMatrix();
    }
    
    // this chunk of code visualizes the creation and destruction of labels
    const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
    const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
    const vector<unsigned int>& newLabels = tracker.getNewLabels();
    const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
    ofSetColor(cyanPrint);
    for(int i = 0; i < currentLabels.size(); i++) {
        int j = currentLabels[i];
        ofDrawLine(j, 0, j, 4);
    }
    ofSetColor(magentaPrint);
    for(int i = 0; i < previousLabels.size(); i++) {
        int j = previousLabels[i];
        ofDrawLine(j, 4, j, 8);
    }
    ofSetColor(yellowPrint);
    for(int i = 0; i < newLabels.size(); i++) {
        int j = newLabels[i];
        ofDrawLine(j, 8, j, 12);
        // we also need to send the current timestamp to these.
        // ... which sadly means we have to iterate over all living followers to find the one with a matching label :(
        for(int i = 0; i < followers.size(); i++) {
            if (followers[i].getLabel() == j) { // j is the label
                followers[i].bornFrame = curFrame;
                break;
            }
        }
    }
    ofSetColor(ofColor::white);
    for(int i = 0; i < deadLabels.size(); i++) {
        int j = deadLabels[i];
        ofDrawLine(j, 12, j, 16);
    }
    
    // this is all we have to do to draw the GUI???? :D :D :D
    processingGui.draw();
    gui.draw();
    
    curFrame++;
}

void ofApp::loadVideoPressed() {
    ofFileDialogResult result = ofSystemLoadDialog("Load new video file");
    if(result.bSuccess) {
        string path = result.getPath();
        movie.load(path);
        movie.setVolume(0);
        movie.play();
        videoDetails.updatePixelSize(movie.getPixels());
        curFrame = 0;
    }
}

// when the user presses the record button, we want to reset everything.
void ofApp::recordRunthroughPressed(bool& recordB) {
    if (recordB) {
        movie.setPosition(0.0f);
        tracker = *new RectTrackerFollower<Glow>();
        recordingFile.open("/Users/valkyrie/projects/savage-internet/sports drone/video/runthrough.txt" , std::ofstream::out | std::ofstream::trunc); // delete everything that's there.
        recordingFile << "[";
        movie.setLoopState(OF_LOOP_NONE);
        curFrame = 0;
    }
}

void ofApp::finalizeRecording() {
    json j;
    //j["sport"] = sportVideo.
    recordingFile << "]";
    movie.setLoopState(OF_LOOP_NORMAL);
    movie.play();
}
