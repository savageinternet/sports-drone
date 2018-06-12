#pragma once

#include <algorithm>
#include <random>

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "playerTracker.hpp"
#include "sportVideo.hpp"

using namespace std;
using namespace cv;
using namespace ofxCv;
using json = nlohmann::json;

struct timepoint {
    float x=0.0, y=0.0;
    int timeStamp=0;
};

template <class F> class PlayerTrackerFollower : public TrackerFollower<Player, F> {};
class PlayerFollower : public RectFollower {};

class Glow : public PlayerFollower {
public:
    ofColor color;
    ofVec2f cur, smooth;
    float startedDying;
    bool recorded;
    ofPolyline all;
    vector<timepoint> timestampPositions;
    int bornFrame;
    int diedFrame;
public:
    Glow()
    :startedDying(0) {
    }
    void setup(const Player& track);
    void update(const Player& track);
    void kill();
    void updateDiedFrame(int diedFrame);
    void draw();
    void record(std::ofstream& fileStream);
    void record(std::ofstream& fileStream, bool forceRecord);

protected:
    void doRecord(std::ofstream& fileStream);
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    ofVideoPlayer movie;
    ofImage colorFrame;
    ofImage colorFrameBlurred;
    Mat colorFrameMat;
    Mat colorFrameBlurredMat;
    Mat hsvColorFrameMat;
    ofImage hsvColorFrame;
    Mat grassMat;
    Mat team1ColorFilterMat;
    Mat team2ColorFilterMat;
    Mat grayFrameMat;
    
    ofParameterGroup processingParameters;
    ofParameter<int> minRadius;
    ofParameter<int> maxRadius;
    ofParameter<int> persistence;
    ofParameter<int> maxVelocity;
    ofParameter<int> blurRadius;
    ofParameter<bool> thresholdB;
    ofParameter<int> thresholdValue;
    ofParameter<int> hRange;
    ofParameter<int> sRange;
    ofParameter<int> bRange;
    ofParameter<bool> useTeamColorSubtractB;
    
    ofParameterGroup displayParameters;
    ofParameter<bool> showOriginal;
    ofParameter<bool> showGrayscale;
    ofParameter<bool> showColor;
    ofParameter<bool> showT1Filter;
    ofParameter<bool> showT2Filter;
    // this is a little separate...
    ofParameter<bool> showContourFinder;
    
    ofParameterGroup matchingParameters;
    ofParameter<float> loc_weight;
    ofParameter<float> velocity_weight;
    ofParameter<float> color_weight;
    ofParameter<float> size_weight;
    ofParameter<float> hue_weight;
    ofParameter<float> saturation_weight;
    ofParameter<float> brightness_weight;
    ofParameter<float> number_weight;
    
    void showVideoPressed(ofAbstractParameter &pressed);
    
    ofxPanel processingGui;
    
    // otherwise, we use background subtraction:
    Mat fgMask;
    Ptr<BackgroundSubtractor> bgsub; //MOG2 Background subtractor
    
    ContourFinder contourFinder;
    PlayerTrackerFollower<Glow> tracker;
    SportVideo videoDetails;
    
    ofParameterGroup sportParameters;
    ofParameter<int> numberPlayers;
    ofParameter<int> sportEnumChooser;
    ofParameter<ofColor> team1Color;
    ofParameter<ofColor> team2Color;
    
    ofxButton loadVideo;
    ofParameter<bool> recordRunthrough;
    bool forceRecord;
    
    ofxPanel gui;
    
    void loadVideoPressed();
    void recordRunthroughPressed(bool& recordB);
    std::ofstream recordingFile;
    void finalizeRecording();
    
    void blurAndGrayscaleVideo();
    void filterColorToMask(ofColor teamColor, cv::Mat& colorFilterMat);
    int ofColortoCVHue(int hue);
    void clampHSB(int& hue, int& saturation, int& brightness);
    
    std::default_random_engine rng;
};
