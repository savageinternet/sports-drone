#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

const float dyingTime = 1;

void Glow::setup(const cv::Rect& track) {
    color.setHsb(ofRandom(0, 255), 255, 255);
    cur = toOf(track).getCenter();
    smooth = cur;
}

void Glow::update(const cv::Rect& track) {
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
    
    grayFrame.allocate(movie.getWidth(), movie.getHeight(), OF_IMAGE_GRAYSCALE);
    
    threshold_b = true;
    
    // set the threshold value. higher number : more whiter
    threshold_value = 90;
    
    //create Background Subtractor object
    pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
    
    contourFinder.setMinAreaRadius(1);
    contourFinder.setMaxAreaRadius(10);
    contourFinder.setThreshold(15);
    
    // wait for a frame before forgetting something
    tracker.setPersistence(30);
    // an object can move up to 20 pixels per frame
    tracker.setMaximumDistance(20);
}

void ofApp::update() {
    movie.update();
    if(movie.isFrameNew()) {
        blur(movie, 10);
        grayFrame.setFromPixels(movie.getPixels());
        cv::cvtColor(toCv(grayFrame), grayFrameMat, cv::COLOR_RGB2GRAY);
        if (threshold_b) {
            // just do simple thresholding on the image
            threshold(grayFrameMat, grayFrameMat, threshold_value);
            contourFinder.findContours(grayFrameMat);
        } else {
            // ok, now we do the magic with the MOG2 shit for foreground/background finding
            pMOG2->apply(grayFrameMat, fgMaskMOG2);
            contourFinder.findContours(fgMaskMOG2);
        }
        tracker.track(contourFinder.getBoundingRects());
    }
}

void ofApp::draw() {
    ofSetColor(255);
    grayFrame.draw(0,0);
    
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
    }
}
