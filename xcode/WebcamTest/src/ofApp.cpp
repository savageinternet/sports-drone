#include <zxing/Exception.h>
#include <zxing/common/Counted.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/datamatrix/DataMatrixReader.h>
#include <zxing/ResultPoint.h>

#include "ofApp.h"
#include "OfxLuminanceSource.hpp"
#include "WebcamTestConstants.h"

using namespace std;
using namespace zxing;
using namespace zxing::datamatrix;

const string ofApp::MSG_NO_CODE_DETECTED = "No code detected";

//--------------------------------------------------------------
void ofApp::setup(){
    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(WebcamTestConstants::CAMERA_FPS);
    vidGrabber.initGrabber(WebcamTestConstants::CAMERA_WIDTH,
                           WebcamTestConstants::CAMERA_HEIGHT);
    
    frame.allocate(WebcamTestConstants::CAMERA_WIDTH,
                   WebcamTestConstants::CAMERA_HEIGHT);
    frameGray.allocate(WebcamTestConstants::CAMERA_WIDTH,
                       WebcamTestConstants::CAMERA_HEIGHT);
    reader = new DataMatrixReader();
    result = NULL;
    resultText = MSG_NO_CODE_DETECTED;
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(0, 0, 0);
    vidGrabber.update();
    if (vidGrabber.isFrameNew()) {
        frame.setFromPixels(vidGrabber.getPixels());
        frameGray = frame;
        
        // detect Aztec code
        Ref<OfxLuminanceSource> source(new OfxLuminanceSource(frameGray));
        Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
        Ref<BinaryBitmap> image(new BinaryBitmap(binarizer));
        try {
            result = reader->decode(image, DecodeHints::DATA_MATRIX_HINT);
            if (result.empty()) {
                resultText = MSG_NO_CODE_DETECTED;
            } else {
                resultText = result->getText()->getText();
            }
        } catch (const zxing::Exception& e) {
            resultText = e.what();
            result = NULL;
        }
    }
}



//--------------------------------------------------------------
void ofApp::drawResultPointLine(Ref<ResultPoint> p0, Ref<ResultPoint> p1) {
    ofDrawLine(p0->getX(), p0->getY(), p1->getX(), p1->getY());
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
