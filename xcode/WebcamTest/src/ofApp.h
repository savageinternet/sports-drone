#pragma once

#include <string>

#include "ofMain.h"
#include "ofxOpenCv.h"

using namespace zxing;

class ofApp : public ofBaseApp{
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
    
    bool skippedFirstFrame;
    ofVideoGrabber vidGrabber;
    ofxCvColorImage frame;
    ofxCvGrayscaleImage frameGray;
    ofxCvContourFinder contourFinder;
};
