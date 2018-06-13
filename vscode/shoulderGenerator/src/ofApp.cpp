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
    codeParameters.add(x0.set("Left:", 256, 256, 1024));
    codeParameters.add(y0.set("Top:", 120, 120, 600));
    codeParameters.add(size.set("Size:", 16, 4, 16));
    codeParameters.add(theta.set("Rotation:", 0, 0, 359));
    codeParameters.add(dTheta.set("Shoulder Rotation:", 0, -30, 30));
    codeParameters.add(dxUnits.set("Shoulder Separation:", 3.0, 2.5, 3.5));
    codeParameters.add(occTopUnits.set("Occlusion Top:", 0, 0, 0.25));
    codeParameters.add(occBottomUnits.set("Occlusion Bottom:", 0, 0, 0.25));
    codeParameters.add(occLeftUnits.set("Occlusion Left:", 0, 0, 0.25));
    codeParameters.add(occRightUnits.set("Occlusion Right:", 0, 0, 0.25));
    gui.add(codeParameters);

    gui.add(btnRandomizeParameters.setup("Random"));

    gui.setPosition(0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){
    codec.encode(n, code);
}

void ofApp::ofDrawShoulderCode() {
    ofPushMatrix();  // shoulder code
    ofTranslate(x0, y0, 0);
    ofRotateZDeg(theta);
    ofScale(size);

    ofPushMatrix();  // left shoulder
    ofRotateZDeg(-dTheta);
    ofTranslate(-5.0f - dxUnits / 2.0f, -1, 0);

    float top = occTopUnits.get();
    float bottom = occBottomUnits.get();
    float left = occLeftUnits.get();
    float right = occRightUnits.get();

    ofSetColor(BLACK);
    ofDrawRectangle(left, top, 1 - left, 1 - top);
    for (int i = 4; i < 8; i++) {
        ofSetColor(code[i] ? BLACK : WHITE);
        float w = i == 7 ? 1 - right : 1;
        ofDrawRectangle(i - 3, top, w, 1 - top);
    }
    ofSetColor(BLACK);
    ofDrawRectangle(left, 1, 1 - left, 1 - bottom);
    for (int i = 0; i < 4; i++) {
        ofSetColor(code[i] ? BLACK : WHITE);
        float w = i == 3 ? 1 - right : 1;
        ofDrawRectangle(i + 1, 1, w, 1 - bottom);
    }

    ofPopMatrix();  // end left shoulder

    ofPushMatrix();  // right shoulder
    ofRotateZDeg(dTheta);
    ofTranslate(dxUnits / 2.0f, -1, 0);

    for (int i = 15; i >= 12; i--) {
        ofSetColor(code[i] ? BLACK : WHITE);
        float dx = i == 15 ? left : 0;
        float w = i == 15 ? 1 - left : 1;
        ofDrawRectangle(15 - i + dx, top, w, 1 - top);
    }
    ofSetColor(BLACK);
    ofDrawRectangle(4, top, 1 - right, 1 - top);
    for (int i = 11; i >= 8; i--) {
        ofSetColor(code[i] ? BLACK : WHITE);
        float dx = i == 11 ? left : 0;
        float w = i == 11 ? 1 - left : 1;
        ofDrawRectangle(11 - i + dx, 1, w, 1 - bottom);
    }
    ofSetColor(BLACK);
    ofDrawRectangle(4, 1, 1 - right, 1 - bottom);

    ofPopMatrix();  // end right shoulder
    ofPopMatrix();  // end shoulder code

    // set color back to avoid tinting background
    ofSetColor(WHITE);
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
    x0 = 256 + rand() % 768;
    y0 = 120 + rand() % 480;
    size = 4 + rand() % 12;
    theta = rand() % 360;
    dTheta = -30 + rand() % 60;
    dxUnits = (250 + rand() % 100) / 100.0f;
    occTopUnits = (rand() % 25) / 100.0f;
    occBottomUnits = (rand() % 25) / 100.0f;
    occLeftUnits = (rand() % 25) / 100.0f;
    occRightUnits = (rand() % 25) / 100.0f;
}
