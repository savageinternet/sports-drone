#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include "ofApp.h"
#include "ofxCv.h"
#include "ofxGui.h"

#include "ShoulderCodec.hpp"
#include "StringUtils.hpp"

using namespace cv;
using namespace ofxCv;
using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
    projectRoot = ofFilePath::getAbsolutePath("../../../..", false);
    ranImageDetection = true;

    btnLoadImage.addListener(this, &ofApp::onClickLoadImage);
    gui.setup("Settings");
    gui.add(btnLoadImage.setup("Load Image"));
    gui.setPosition(0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!image.isAllocated() || ranImageDetection) {
        return;
    }
    contour.clear();
    contour.addVertex(x0 - 100, y0 - 100);
    contour.addVertex(x0 + 100, y0 - 100);
    contour.addVertex(x0 + 100, y0 + 100);
    contour.addVertex(x0 - 100, y0 + 100);
    contour.close();
    detector.detect(
        image.getPixels(),
        contour,
        theta,
        code);
    ranImageDetection = true;
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (image.isAllocated()) {
        //image.draw(0, 0);
        imageBinary.draw(0, 0);
        if (ranImageDetection) {
            ofSetColor(255, 0, 0);
            contour.draw();
            
            ofSetColor(0, 255, 0);
            ofDrawLine(x0 + 20 * cos(theta), y0 + 20 * sin(theta), x0 - 20 * cos(theta), y0 - 20 * sin(theta));
            ofDrawLine(x0, y0, x0 + 10 * sin(theta), y0 - 10 * cos(theta));

            ofSetColor(0, 255, 255);
            ofDrawLine(x0 - 5, y0, x0 + 5, y0);
            ofDrawLine(x0, y0 - 5, x0, y0 + 5);

            ofSetColor(WHITE);
        }
    }
    gui.draw();
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

void ofApp::onClickLoadImage() {
    string codePath = ofFilePath::join(projectRoot, "../shoulderGenerator/out");
    ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an image", false, codePath);
    if (!openFileResult.bSuccess) {
        return;
    }
    string filename = openFileResult.getName();
    filename = ofFilePath::removeExt(filename);
    vector<string> parts;
    StringUtils::split(filename, "_", parts);
    /*
     * For now, we do absolutely no validation; the main point here is to get the parameters
     * so we can test the detector, not write a super-robust filename parser.
     */
    const char* part;
    part = parts[1].c_str();
    n = atoi(part + 1);
    part = parts[2].c_str();
    x0 = atoi(part + 1);
    part = parts[3].c_str();
    y0 = atoi(part + 1);
    part = parts[4].c_str();
    int thetaDeg = atoi(part + 1);
    theta = thetaDeg / 180.0 * M_PI;

    image.load(openFileResult.getPath());

    // threshold into binary image

    convertColor(image, imageBinary, CV_RGB2GRAY);
    Mat imageBinaryMat = toCv(imageBinary);
    adaptiveThreshold(imageBinaryMat, imageBinaryMat, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 13, 8);
    imageBinary.update();
    dilate(imageBinary, 1);
    /*
    GaussianBlur(imageBinary, 3);
    Canny(imageBinary, imageBinary, 10, 100);
    */
    imageBinary.update();
    ranImageDetection = false;
}
