#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "playerTracker.hpp"
#include "sportVideo.hpp"

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
    
    ofVideoPlayer movie;
    ofImage colorFrame;
    Mat colorFrameMat;
    Mat grayFrameMat;
    
    ofParameterGroup processingParameters;
    ofParameter<int> minRadius;
    ofParameter<int> maxRadius;
    ofParameter<int> persistence;
    ofParameter<int> maxVelocity;
    ofParameter<int> blurRadius;
    ofParameter<bool> thresholdB;
    ofParameter<int> thresholdValue;
    
    ofxPanel processingGui;
    
    // otherwise, we use background subtraction:
    Mat fgMask;
    Ptr<BackgroundSubtractor> bgsub; //MOG2 Background subtractor
    
    ContourFinder contourFinder;
    RectTrackerFollower<Glow> tracker;
    //PlayerTracker tracker;
    SportVideo videoDetails;
    
    ofParameterGroup sportParameters;
    ofParameter<int> numberPlayers;
    ofParameter<int> sportEnumChooser;
    ofParameter<ofColor> team1Color;
    ofParameter<ofColor> team2Color;
    
    ofxButton loadVideo;
    
    ofxPanel gui;
    
    void loadVideoPressed();
};
