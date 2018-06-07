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
    processingGui.setup("Processing/Display");
    
    processingParameters.setName("Processing Parameters");
    processingParameters.add(minRadius.set("Min radius:",7,0,100));
    processingParameters.add(maxRadius.set("Max radius:",15,0,100));
    processingParameters.add(persistence.set("Persistence (frames):",54,0,160));
    processingParameters.add(maxVelocity.set("Max velocity:",15,0,100));
    processingParameters.add(blurRadius.set("Blur radius:",12,0,100));
    processingParameters.add(thresholdB.set("Simple threshold?",false));
    processingParameters.add(thresholdValue.set("Threshold:",90,0,255));
    processingParameters.add(useTeamColorSubtractB.set("Team color subtraction?",false));
    processingParameters.add(hRange.set("Hue range",66,0,180));
    processingParameters.add(sRange.set("Sat range",255,0,255));
    processingParameters.add(bRange.set("Bri range",160,0,255));
    processingGui.add(processingParameters);
    
    displayParameters.setName("Display Parameters");
    displayParameters.add(showOriginal.set("Show original", false));
    displayParameters.add(showColor.set("Show color", true));
    displayParameters.add(showGrayscale.set("Show grayscale", false));
    displayParameters.add(showT1Filter.set("Show Team1 filter", false));
    displayParameters.add(showT2Filter.set("Show Team2 filter", false));
    displayParameters.add(showContourFinder.set("Show found contours", true));
    processingGui.add(displayParameters);
    
    processingGui.setPosition(1070,10);
    
    // set listeners for all dem display booleans
    ofAddListener(displayParameters.parameterChangedE(), this, &ofApp::showVideoPressed);
    
    // parameters of the sport and video
    sportParameters.setName("Sport Infoz");
    sportParameters.add(numberPlayers.set("Num Players:",10,0,22));
    sportParameters.add(sportEnumChooser.set("Sport:",0,0,2));
    sportParameters.add(team1Color.set("Team 1 Color",ofColor(249,161,54),ofColor(0,0),ofColor(255)));
    sportParameters.add(team2Color.set("Team 2 Color",ofColor(255,253,145),ofColor(0,0),ofColor(255)));
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

void ofApp::showVideoPressed(ofAbstractParameter &pressed) {
    string pressedName = pressed.getName();
    
    if (pressedName == showContourFinder.getName()) {
        // do nothing. this is outside all the other stuff.
        return;
    }
    
    // shut off the listener, so we don't trigger hella shit changing these up. HAXXXXX
    ofRemoveListener(displayParameters.parameterChangedE(), this, &ofApp::showVideoPressed);
    // figure out which one it is and turn the others off.
    for (int i = 0; i < displayParameters.size(); i++) {
        // set everything that isn't this to false. should leave this one true.
        ofParameter<bool> ofp = displayParameters.get(i).cast<bool>();
        string ofpName = ofp.getName();
        if (ofpName == showContourFinder.getName()) continue; // do nothing, it behaves independently
        ofp.set(pressedName == ofpName);
    }
    // turn the listener back on. ^___^;
    ofAddListener(displayParameters.parameterChangedE(), this, &ofApp::showVideoPressed);
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
        blurAndGrayscaleVideo();
        filterColorToMask(team1Color.get(), team1ColorFilterMat);
        filterColorToMask(team2Color.get(), team2ColorFilterMat);
        
        if (thresholdB) {
            // just do simple thresholding on the image
            threshold(grayFrameMat, grayFrameMat, thresholdValue);
            contourFinder.findContours(grayFrameMat);
        } else if (useTeamColorSubtractB) {
            // would it work to do these separately? or...? (then we could know who is on which team... maybe we ultimately want to just do this processing post hoc and separate the colors we find in the &'d color image to find half people on one team and half people on the other team (based on the number we are expecting?)
            bgsub->apply(team1ColorFilterMat & team2ColorFilterMat, fgMask);
            contourFinder.findContours(fgMask);
        } else {
            // just plain ol' magic with foreground/background finding
            bgsub->apply(colorFrameBlurredMat, fgMask);
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

void ofApp::blurAndGrayscaleVideo() {
    colorFrame.setFromPixels(movie.getPixels());
    colorFrameBlurred.setFromPixels(colorFrame.getPixels());
    blur(colorFrameBlurred, blurRadius);
    colorFrameMat = toCv(colorFrame);
    colorFrameBlurredMat = toCv(colorFrameBlurred);
    cv::cvtColor(toCv(colorFrameBlurred), grayFrameMat, cv::COLOR_RGB2GRAY);
}

void ofApp::filterColorToMask(ofColor teamColor, cv::Mat& colorFilterMat) {
    // convert image to HSV color space
    cv::cvtColor(colorFrameBlurredMat, hsvColorFrameMat, COLOR_BGR2HSV);
    
    // now look at the selected colors and get a range around them
    int hrange = hRange.get();//20;
    int srange = sRange.get();//100;
    int brange = bRange.get();//100;
    
    // hsb-ify the colors
    float hue, sat, bri;
    teamColor.getHsb(hue, sat, bri);
    hue = ofColortoCVHue(hue);
    int hmin = hue-hrange, hmax = hue+hrange,
        smin = sat-srange, smax = sat+srange,
        bmin = bri-brange, bmax = bri+brange;
    clampHSB(hmin, smin, bmin);
    clampHSB(hmax, smax, bmax);
    
    // now, because hue wraps around 180, we do 2 masks and & them (if hue wraps 0 or 180)
    if (hmin < 0) {
        Mat mask1, mask2;
        inRange(hsvColorFrameMat, Scalar(0, smin, bmin), Scalar(hmax, smax, bmax), mask1);
        inRange(hsvColorFrameMat, Scalar(hmin+180, smin, bmin), Scalar(180, smax, bmax), mask2);
        colorFilterMat = mask1 | mask2;
    } else if (hmax > 180) {
        Mat mask1, mask2;
        inRange(hsvColorFrameMat, Scalar(hmin, smin, bmin), Scalar(180, smax, bmax), mask1);
        inRange(hsvColorFrameMat, Scalar(0, smin, bmin), Scalar(hmax-180, smax, bmax), mask2);
        colorFilterMat = mask1 | mask2;
    } else {
        inRange(hsvColorFrameMat, Scalar(hmin, smin, bmin), Scalar(hmax, smax, bmax), colorFilterMat);
    }
}

int ofApp::ofColortoCVHue(int hue) {
    // openframeworks uses hues 0-255, and opencv uses hues 0-180
    return hue * 180.0/255.0;
}

void ofApp::clampHSB(int& hue, int& saturation, int& brightness) {
    // don't do anything to hue. it will get dealt with in the other function next. :( could clamp to 180, but this loses info.
    // brightness and saturation just clamp
    saturation = (int)max(0,min(saturation,255));
    brightness = (int)max(0,min(brightness,255));
}

void ofApp::draw() {
    ofSetColor(255);
    
    // now we can jes draw the right thang
    if (showOriginal)
        movie.draw(0,0);
    if (showColor)
        drawMat(colorFrameBlurredMat,0,0);
    if (showGrayscale)
        drawMat(grayFrameMat,0,0);
    if (showT1Filter)
        drawMat(team1ColorFilterMat,0,0);
    if (showT2Filter)
        drawMat(team2ColorFilterMat,0,0);

    
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
    
    if (showContourFinder) {
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
        json j;
        j["width"] = movie.getWidth();
        j["height"] = movie.getHeight();
        j["sport"] = videoDetails.getNameString();
        // next we do a sketchy thing, where we mostly dump the string into the file, but go a lil bit lel and remove the last character. this makes it so we don't screw up our file and break the json that leads into all the tracked thingies.
        recordingFile << j.dump().erase(j.dump().length()-1, 1) << ", \"tracked\":[\n";
        movie.setLoopState(OF_LOOP_NONE);
        curFrame = 0;
        showContourFinder = true;
    }
}

void ofApp::finalizeRecording() {
    // wish we could get rid of the final comma in the file... but... not sure how to do :(
    recordingFile << "]}";
    movie.setLoopState(OF_LOOP_NORMAL);
    movie.play();
}
