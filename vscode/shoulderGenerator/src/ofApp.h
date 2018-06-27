#pragma once

#include <bitset>
#include <string>

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

#include "ShoulderCodec.hpp"
#include "ShoulderDetector.hpp"

using namespace cv;
using namespace ofxCv;
using namespace std;

class ofApp : public ofBaseApp{
private:
    static const int WHITE = 255;
    static const int BLACK = 0;
    static const string OUTPUT_EXT;

    ofImage background;
    ofImage noise;
    ofImage image;
    ofImage imageGrey;
    ShoulderDetector detector;
    bitset<24> codeFormatted;
    bitset<16> code;
    int result;
    string projectRoot;

    ofxPanel gui;

    ofParameterGroup imageParameters;
    ofParameter<bool> showBg;
    ofParameter<bool> addNoise;

    ofParameterGroup codeParameters;
    ofParameter<int> n;
    ofParameter<glm::vec2> p0;
    ofParameter<int> size;
    ofParameter<int> theta;
    ofParameter<int> dTheta;
    ofParameter<float> dxUnits;
    ofParameter<float> occTopUnits;
    ofParameter<float> occBottomUnits;
    ofParameter<float> occLeftUnits;
    ofParameter<float> occRightUnits;

    ofParameter<bool> runDetection;
    ofxButton btnRandomizeParameters;
    ofxButton btnSaveCurrentImage;

    void ofSetupNoise();

    void ofDrawShoulderCode();
    void ofDrawNoise();
    void ofDrawDetect();

    void getScreenImage(ofImage& image);

    string getCurrentImageFilePath();
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void randomizeParameters();
    void saveCurrentImage();
};
