#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "playerTracker.hpp"

using namespace cv;
using namespace ofxCv;

class Glow : public RectFollower {
protected:
    ofColor color;
    ofVec2f cur, smooth;
    float startedDying;
    ofPolyline all;
public:
    Glow()
    :startedDying(0) {
    }
    void setup(const Player& track);
    void update(const Player& track);
    void kill();
    void draw();
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int keycode);
    
    ofVideoPlayer movie;
    ofImage colorFrame;
    Mat colorFrameMat;
    Mat grayFrameMat;
    
    int blurRadius = 10;
    
    bool thresholdB = false;
    
    // if we want to threshold, we use this:
    int thresholdValue = 90;
    
    // otherwise, we use background subtraction:
    Mat fgMask;
    Ptr<BackgroundSubtractor> bgsub; //MOG2 Background subtractor
    
    ContourFinder contourFinder;
    RectTrackerFollower<Glow> tracker;
    //PlayerTracker tracker;
};
