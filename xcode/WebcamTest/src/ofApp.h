#pragma once

#include <string>

#include <zxing/Reader.h>
#include <zxing/ResultPoint.h>
#include <zxing/common/Counted.h>

#include "ofMain.h"
#include "ofxOpenCv.h"

using namespace zxing;

class ofApp : public ofBaseApp{
private:
    static const string MSG_NO_CODE_DETECTED;
    void drawResultPointLine(Ref<ResultPoint> p0, Ref<ResultPoint> p1);
    void detectCode();
    
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
    Ref<Reader> reader;
    Ref<Result> result;
    int dxResult;
    int dyResult;
    string resultText;
};
