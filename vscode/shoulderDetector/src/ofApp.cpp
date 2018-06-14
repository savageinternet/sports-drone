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

const string ofApp::OUTPUT_EXT = ".png";

//--------------------------------------------------------------
void ofApp::setup(){
    projectRoot = ofFilePath::getAbsolutePath("../../../..", false);

    btnLoadImage.addListener(this, &ofApp::onClickLoadImage);
    gui.setup("Settings");
    gui.add(btnLoadImage.setup("Load Image"));
    gui.setPosition(0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    if (image.isAllocated()) {
        image.draw(0, 0);
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
    vector<string> parts;
    StringUtils::split(filename, "_", parts);

    image.load(openFileResult.getPath());
}
