#include "ofApp.h"
#include "ofxCv.h"

#include "WebcamTestConstants.h"

using namespace cv;
using namespace ofxCv;
using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
    cam.setDeviceID(0);
    cam.setDesiredFrameRate(WebcamTestConstants::CAMERA_FPS);
    cam.initGrabber(WebcamTestConstants::CAMERA_WIDTH,
                    WebcamTestConstants::CAMERA_HEIGHT);
    
    skippedFirstFrame = false;
    frame.allocate(WebcamTestConstants::CAMERA_WIDTH,
                   WebcamTestConstants::CAMERA_HEIGHT,
                   OF_IMAGE_GRAYSCALE);
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(0, 0, 0);
    cam.update();
    if (cam.isFrameNew()) {
        if (skippedFirstFrame) {
            // get video frame and convert to grayscale
            convertColor(cam, frame, CV_RGB2GRAY);
            
            // threshold into binary image
            Mat frameMat = toCv(frame);
            adaptiveThreshold(frameMat, frameMat, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 13, 8);
            frame.update();
        } else {
            skippedFirstFrame = true;
        }
    }
}



//--------------------------------------------------------------
void ofApp::draw(){
    frame.draw(0, 0);
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
