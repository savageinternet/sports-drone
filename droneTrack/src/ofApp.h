#pragma once

#include "ofMain.h"
#include "ofxCv.h"

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
    void setup(const cv::Rect& track);
    void update(const cv::Rect& track);
    void kill();
    void draw();
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    ofVideoPlayer movie;
    ofImage grayFrame;
    Mat grayFrameMat;
    
    bool threshold_b;
    
    // if we want to threshold, we use this:
    int threshold_value;
    
    // otherwise, we use background subtraction:
    Mat fgMaskMOG2;
    Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
    
    
    ContourFinder contourFinder;
    RectTrackerFollower<Glow> tracker;
};
