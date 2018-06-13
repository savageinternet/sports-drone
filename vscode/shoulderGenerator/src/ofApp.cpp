#include <bitset>
#include <string>

#include "ofApp.h"
#include "ofxCv.h"
#include "ofxGui.h"

#include "ShoulderCodec.hpp"

using namespace cv;
using namespace ofxCv;
using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
    projectRoot = ofFilePath::getAbsolutePath("../../../..", false);
    
    // load football pitch texture
    string imgPath = ofFilePath::join(projectRoot, "img/football_pitch_720p.jpg");
    background.load(imgPath);

    gui.setup("Code Generation");

    codeParameters.setName("Code Parameters");
    codeParameters.add(n.set("Encoded Value:", 42, 0, 63));
    codeParameters.add(x0.set("Left:", 64, 128, 1152));
    codeParameters.add(y0.set("Top:", 64, 64, 592));
    codeParameters.add(size.set("Size:", 16, 2, 32));
    gui.add(codeParameters);

    gui.setPosition(10, 10);
}

//--------------------------------------------------------------
void ofApp::update(){
    codec.encode(n, code);
}

void ofApp::ofDrawShoulderCode(int x0, int y0, int size) {
    int x = x0;
    int y = y0;

    ofSetColor(BLACK);
    ofDrawRectangle(x, y, size, size);
    for (int i = 4; i < 8; i++) {
        x += size;
        ofSetColor(code[i] ? BLACK : WHITE);
        ofDrawRectangle(x, y, size, size);
    }
    x += size;
    for (int i = 11; i >= 8; i--) {
        x += size;
        ofSetColor(code[i] ? BLACK : WHITE);
        ofDrawRectangle(x, y, size, size);
    }
    x += size;
    ofSetColor(BLACK);
    ofDrawRectangle(x, y, size, size);

    x = x0;
    y += size;

    ofSetColor(BLACK);
    ofDrawRectangle(x, y, size, size);
    for (int i = 0; i < 4; i++) {
        x += size;
        ofSetColor(code[i] ? BLACK : WHITE);
        ofDrawRectangle(x, y, size, size);
    }
    x += size;
    for (int i = 15; i >= 12; i--) {
        x += size;
        ofSetColor(code[i] ? BLACK : WHITE);
        ofDrawRectangle(x, y, size, size);
    }
    x += size;
    ofSetColor(BLACK);
    ofDrawRectangle(x, y, size, size);
    ofSetColor(WHITE);
}

//--------------------------------------------------------------
void ofApp::draw(){
    background.draw(0, 0);
    ofDrawShoulderCode(x0, y0, size);
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
