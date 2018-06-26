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
#include "ShoulderDetector.hpp"
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

void printGroundTruth(int n) {
    bitset<16> code;
    bitset<24> codeFormatted;
    ShoulderCodec::encode(n, code);
    ShoulderCodec::format(code, codeFormatted);
    ShoulderCodec::print(cout, codeFormatted);
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!image.isAllocated() || ranImageDetection) {
        return;
    }
    ofContour.clear();
    int ds = size * 6;
    ofContour.addVertex(x0 - ds, y0 - ds);
    ofContour.addVertex(x0 + ds, y0 - ds);
    ofContour.addVertex(x0 + ds, y0 + ds);
    ofContour.addVertex(x0 - ds, y0 + ds);
    ofContour.close();

    printGroundTruth(n);
    Mat mat = toCv(imageGrey.getPixels());
    vector<Point2f> contour = toCv(ofContour);
    ofVec2f ofCentroid = ofContour.getCentroid2D();
    Point2f centroid = toCv(ofCentroid);
    bool codeDetected = detector.detect(mat, contour, centroid, codeFormatted);
    if (codeDetected) {
        ShoulderCodec::unformat(codeFormatted, code);
        result = ShoulderCodec::decode(code);
    } else {
        result = -1;
        cout << "no code detected!" << endl;
    }
    ranImageDetection = true;
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (image.isAllocated()) {
        imageGrey.draw(0, 0);
        if (ranImageDetection) {
            ostringstream oss;
            oss << result << "!";
            ofDrawBitmapString(oss.str(), x0, y0);
            ofSetColor(255, 0, 0);
            ofContour.draw();
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
    part = parts[5].c_str();
    size = atoi(part + 1);

    image.load(openFileResult.getPath());
    image.update();
    convertColor(image, imageGrey, CV_RGB2GRAY);
    imageGrey.update();
    ranImageDetection = false;
}
