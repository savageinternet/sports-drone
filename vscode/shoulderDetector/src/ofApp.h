#pragma once

#include <bitset>
#include <string>

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

#include "ShoulderCodec.hpp"

using namespace cv;
using namespace ofxCv;
using namespace std;

class ofApp : public ofBaseApp{
private:
    static const int WHITE = 255;
    static const int BLACK = 0;
    static const string OUTPUT_EXT;

    ofImage image;
    string projectRoot;

    int n;
    int x0;
    int y0;
    int theta;

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
