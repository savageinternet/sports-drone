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

    bitset<16> code;
    int result;
    ShoulderCodec codec;
    ofPolyline contour;
    ShoulderDetector detector;
    ofImage image;
    ofImage imageGrey;
    string projectRoot;
    bool ranImageDetection;

    int n;
    int x0;
    int y0;
    float theta;
    int size;

    ofxPanel gui;
    ofxButton btnLoadImage;
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

    void onClickLoadImage();
};
