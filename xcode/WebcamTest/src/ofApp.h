#pragma once

#include <string>

#include <zxing/datamatrix/DataMatrixReader.h>
#include <zxing/common/Counted.h>
#include <zxing/ResultPoint.h>

#include "ofMain.h"
#include "ofxOpenCv.h"

using namespace zxing;
using namespace zxing::datamatrix;

class ofApp : public ofBaseApp{
private:
    static const string MSG_NO_CODE_DETECTED;
    void drawResultPointLine(Ref<ResultPoint> p0, Ref<ResultPoint> p1);
    
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
		
    ofVideoGrabber vidGrabber;
    ofxCvColorImage frame;
    ofxCvGrayscaleImage frameGray;
    Ref<DataMatrixReader> reader;
    Ref<Result> result;
    string resultText;
};
