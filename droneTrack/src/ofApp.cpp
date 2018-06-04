#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

const float dyingTime = 1;

void Glow::setup(const Player& track) {
    color.setHsb(ofRandom(0, 255), 255, 255);
    cur = toOf(track).getCenter();
    smooth = cur;
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
    }
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
}

void ofApp::update() {
    movie.update();
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
        
        /* NOTE from VALKYRIE : under construction :3
         std::vector<Player> foundPlayers;
        for(unsigned int i = 0; i < contourFinder.getBoundingRects().size(); i++) {
            Player p(contourFinder.getBoundingRects().at(i));
            p.velocity = toOf(contourFinder.getVelocity(i));
            
            // extract the average pixel color from around the centroid? need to think about exactly what to do here. be sure to use the blurred video, I guess, since it will de-noise shit?
            //p.jerseyColor = ???
            
            foundPlayers.push_back(p);
        }*/
        tracker.track(contourFinder.getBoundingRects());//foundPlayers);
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
    /*for(int i = 0; i < followers.size(); i++) {
        followers[i].draw();
    }*/
    
    // visualize labels. from https://github.com/kylemcdonald/ofxCv/blob/master/example-contours-tracking/src/ofApp.cpp
    for(int i = 0; i < contourFinder.size(); i++) {
        ofPoint center = toOf(contourFinder.getCenter(i));
        ofPushMatrix();
        ofTranslate(center.x, center.y);
        int label = contourFinder.getLabel(i);
        uint age = tracker.getAge(label);
        if (age > 12) {
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
    }
    ofSetColor(ofColor::white);
    for(int i = 0; i < deadLabels.size(); i++) {
        int j = deadLabels[i];
        ofDrawLine(j, 12, j, 16);
    }
}

void ofApp::keyPressed(int keycode) {
    switch(keycode) {
        case 'l':
            ofFileDialogResult result = ofSystemLoadDialog("Load new video file");
            if(result.bSuccess) {
                string path = result.getPath();
                movie.load(path);
                movie.setVolume(0);
                movie.play();
            }
            break;
        /*default:
            break;*/
    }
}
