#include <zxing/Exception.h>
#include <zxing/ResultPoint.h>
#include <zxing/aztec/AztecReader.h>
#include <zxing/common/Counted.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/datamatrix/DataMatrixReader.h>

#include "ofApp.h"
#include "OfxLuminanceSource.hpp"
#include "WebcamTestConstants.h"

using namespace std;
using namespace zxing;
using namespace zxing::aztec;
using namespace zxing::datamatrix;

const string ofApp::MSG_NO_CODE_DETECTED = "No code detected";

//--------------------------------------------------------------
void ofApp::setup(){
    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(WebcamTestConstants::CAMERA_FPS);
    vidGrabber.initGrabber(WebcamTestConstants::CAMERA_WIDTH,
                           WebcamTestConstants::CAMERA_HEIGHT);
    
    skippedFirstFrame = false;
    frame.allocate(WebcamTestConstants::CAMERA_WIDTH,
                   WebcamTestConstants::CAMERA_HEIGHT);
    frameGray.allocate(WebcamTestConstants::CAMERA_WIDTH,
                       WebcamTestConstants::CAMERA_HEIGHT);
    reader = new DataMatrixReader();
    result = NULL;
    dxResult = 0;
    dyResult = 0;
    resultText = MSG_NO_CODE_DETECTED;
}

//--------------------------------------------------------------
void ofApp::detectCode() {
    Ref<OfxLuminanceSource> source(new OfxLuminanceSource(frameGray));
    Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
    Ref<BinaryBitmap> image(new BinaryBitmap(binarizer));
    Ref<BinaryBitmap> imageCrop = image->crop(0, 0, image->getWidth(), image->getHeight());
    int width = image->getWidth();
    int height = image->getHeight();
    int kx = 128;
    int ky = 144;
    
    bool offset = false;
    for (int x = 0; x < width - kx; x += kx / 2) {
        int y = offset ? ky / 2 : 0;
        for (; y < height - ky; y += ky) {
            imageCrop = image->crop(x, y, kx, ky);
            try {
                result = reader->decode(imageCrop, DecodeHints::DATA_MATRIX_HINT);
                if (!result.empty()) {
                    resultText = result->getText()->getText();
                    dxResult = x;
                    dyResult = y;
                    return;
                }
            } catch (const zxing::Exception& e) {
                // do nothing for now
            }
        }
        offset = !offset;
    }
    result = NULL;
    dxResult = 0;
    dyResult = 0;
    resultText = MSG_NO_CODE_DETECTED;
}

void ofApp::update(){
    ofBackground(0, 0, 0);
    vidGrabber.update();
    if (vidGrabber.isFrameNew()) {
        if (skippedFirstFrame) {
            frame.setFromPixels(vidGrabber.getPixels());
            frameGray = frame;
            detectCode();
        } else {
            skippedFirstFrame = true;
        }
    }
}



//--------------------------------------------------------------
void ofApp::drawResultPointLine(Ref<ResultPoint> p0, Ref<ResultPoint> p1) {
    ofDrawLine(p0->getX() + dxResult, p0->getY() + dyResult,
               p1->getX() + dxResult, p1->getY() + dyResult);
}

void ofApp::draw(){
    frame.draw(0, 0);
    if (!result.empty()) {
        ArrayRef<Ref<ResultPoint>> ps = result->getResultPoints();
        int n = ps->size();
        if (n >= 4) {
            ofSetHexColor(0xff0000);
            Ref<ResultPoint> pA = ps[0];
            Ref<ResultPoint> pB = ps[1];
            Ref<ResultPoint> pC = ps[2];
            Ref<ResultPoint> pD = ps[3];
            drawResultPointLine(pA, pB);
            drawResultPointLine(pA, pC);
            drawResultPointLine(pB, pD);
            drawResultPointLine(pC, pD);
        }
    }
                       
    ofSetHexColor(0xffffff);
    ofDrawBitmapStringHighlight(resultText, 20, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
