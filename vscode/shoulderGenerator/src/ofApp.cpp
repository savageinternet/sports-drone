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

using namespace cv;
using namespace ofxCv;
using namespace std;

const string ofApp::OUTPUT_EXT = ".png";

//--------------------------------------------------------------
void ofApp::setup(){
    srand(time(NULL));

    projectRoot = ofFilePath::getAbsolutePath("../../../..", false);
    
    // load football pitch texture
    string imgPath = ofFilePath::join(projectRoot, "img/football_pitch_720p.jpg");
    background.load(imgPath);

    btnRandomizeParameters.addListener(this, &ofApp::onClickRandomizeParameters);

    gui.setup("Code Generation");

    codeParameters.setName("Code Parameters");
    codeParameters.add(n.set("Encoded Value:", 42, 0, 63));
    codeParameters.add(x0.set("Left:", 64, 128, 1152));
    codeParameters.add(y0.set("Top:", 64, 64, 592));
    codeParameters.add(size.set("Size:", 16, 2, 16));
    codeParameters.add(theta.set("Rotation:", 0, 0, 359));
    codeParameters.add(dTheta.set("Shoulder Rotation:", 0, 0, 30));
    codeParameters.add(dxUnits.set("Shoulder Separation:", 4.0, 3.0, 5.0));
    codeParameters.add(occTopUnits.set("Occlusion Top:", 0, 0, 0.5));
    codeParameters.add(occBottomUnits.set("Occlusion Bottom:", 0, 0, 0.5));
    codeParameters.add(occLeftUnits.set("Occlusion Left:", 0, 0, 0.5));
    codeParameters.add(occRightUnits.set("Occlusion Right:", 0, 0, 0.5));
    gui.add(codeParameters);

    gui.add(btnRandomizeParameters.setup("Random"));

    gui.setPosition(10, 10);
}

//--------------------------------------------------------------
void ofApp::update(){
    codec.encode(n, code);
}

void ofApp::ofDrawShoulderCode() {
    ofPushMatrix();
    ofTranslate(x0, y0, 0);

    int x = 0;
    int y = 0;
    ofSetColor(BLACK);
    ofDrawRectangle(x, y, size, size);
    for (int i = 4; i < 8; i++) {
        x += size;
        ofSetColor(code[i] ? BLACK : WHITE);
        ofDrawRectangle(x, y, size, size);
    }
    x += size * dxUnits;
    for (int i = 11; i >= 8; i--) {
        x += size;
        ofSetColor(code[i] ? BLACK : WHITE);
        ofDrawRectangle(x, y, size, size);
    }
    x += size;
    ofSetColor(BLACK);
    ofDrawRectangle(x, y, size, size);

    x = 0;
    y += size;

    ofSetColor(BLACK);
    ofDrawRectangle(x, y, size, size);
    for (int i = 0; i < 4; i++) {
        x += size;
        ofSetColor(code[i] ? BLACK : WHITE);
        ofDrawRectangle(x, y, size, size);
    }
    x += size * dxUnits;
    for (int i = 15; i >= 12; i--) {
        x += size;
        ofSetColor(code[i] ? BLACK : WHITE);
        ofDrawRectangle(x, y, size, size);
    }
    x += size;
    ofSetColor(BLACK);
    ofDrawRectangle(x, y, size, size);
    
    // reset for next operation
    ofSetColor(WHITE);
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::draw(){
    background.draw(0, 0);
    ofDrawShoulderCode();
    gui.draw();
}

string ofApp::getCurrentImageFilePath() {
    string outputDir = ofFilePath::join(projectRoot, "out");
    ostringstream filename;
    filename << "sc_" << time(NULL) << OUTPUT_EXT;
    return ofFilePath::join(outputDir, filename.str());
}

void ofApp::saveCurrentImage() {
    string filePath = getCurrentImageFilePath();
    ofImage image;
    image.grabScreen(0, 0, 1280, 720);
    image.save(filePath);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 's' || key == 'S') {
        saveCurrentImage();
    }
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

void ofApp::onClickRandomizeParameters() {
    n = rand() % 64;
    x0 = 128 + rand() % 1024;
    y0 = 64 + rand() % 528;
    size = 2 + rand() % 15;
    theta = rand() % 360;
    dTheta = rand() % 30;
    dxUnits = (300 + rand() % 200) / 100.0f;
    occTopUnits = (rand() % 50) / 100.0f;
    occBottomUnits = (rand() % 50) / 100.0f;
    occLeftUnits = (rand() % 50) / 100.0f;
    occRightUnits = (rand() % 50) / 100.0f;
}
