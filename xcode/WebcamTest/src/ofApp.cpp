#include "ofApp.h"
#include "OfxLuminanceSource.hpp"
#include "ThresholdBinarizer.hpp"
#include "WebcamTestConstants.h"

using namespace std;

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
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(0, 0, 0);
    vidGrabber.update();
    if (vidGrabber.isFrameNew()) {
        if (skippedFirstFrame) {
            // get video frame
            frame.setFromPixels(vidGrabber.getPixels());
            
            // convert to grayscale
            frameGray = frame;
            
            // threshold into binary image
            frameGray.adaptiveThreshold(17, 8, false, true);
            
            // detect connected components
            contourFinder.findContours(frameGray, 20, 500, 4, false, true);
        } else {
            skippedFirstFrame = true;
        }
    }
}



//--------------------------------------------------------------
void ofApp::draw(){
    frameGray.draw(0, 0);
    contourFinder.draw(0, 0);
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
