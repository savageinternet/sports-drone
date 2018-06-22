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

void ofApp::ofSetupNoise() {
    noise.allocate(1280, 720, OF_IMAGE_GRAYSCALE);
    float sx = 0.5;
	float sy = 0.5;
	float x0 = 593.2;
	float y0 = 43.7;
    ofPixels& noisePixels = noise.getPixels();
	for (int y = 0; y < 720; y++) {
		for (int x = 0; x < 1280; x++) {
			float v = ofNoise(x * sx + x0, y * sy + y0);
			ofColor c(v * 255);
            noisePixels.setColor(x, y, c);
		}
	}
    noise.update();
}

//--------------------------------------------------------------
void ofApp::setup(){
    srand(time(NULL));

    projectRoot = ofFilePath::getAbsolutePath("../../../..", false);
    
    // load football pitch texture
    string imgPath = ofFilePath::join(projectRoot, "img/football_pitch_720p.jpg");
    background.load(imgPath);
    
    // allocate (but do not compute) noise texture
    ofSetupNoise();

    btnRandomizeParameters.addListener(this, &ofApp::onClickRandomizeParameters);

    gui.setup("Code Generation");

    codeParameters.setName("Code Parameters");
    codeParameters.add(showBg.set("Show Background?", false));
    codeParameters.add(addNoise.set("Add Noise?", false));
    codeParameters.add(n.set("Encoded Value:", 42, 0, 255));
    codeParameters.add(x0.set("Left:", 256, 256, 1024));
    codeParameters.add(y0.set("Top:", 120, 120, 600));
    codeParameters.add(size.set("Size:", 16, 4, 16));
    codeParameters.add(theta.set("Rotation:", 0, 0, 359));
    codeParameters.add(dTheta.set("Shoulder Rotation:", 0, -20, 20));
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
    codec.format(code, codeFormatted);
}

void ofApp::ofDrawShoulderCode() {
    float top = occTopUnits.get();
    float bottom = occBottomUnits.get();
    float left = occLeftUnits.get();
    float right = occRightUnits.get();
    int i = 0;

    ofPushMatrix();  // shoulder code
    ofTranslate(x0, y0, 0);
    ofRotateZDeg(theta);
    ofScale(size);

    ofPushMatrix();  // left shoulder
    ofRotateZDeg(-dTheta);
    ofTranslate(-4.0f - dxUnits / 2.0f, -1.5, 0);

    for (int r = 0; r < 3; r++) {
        float y = r;
        float h = 1;
        if (r == 0) {
            y = top;
            h = 1 - top;
        } else if (r == 2) {
            h = 1 - bottom;
        }
        for (int c = 0; c < 4; c++) {
            ofSetColor(codeFormatted[i++] ? BLACK : WHITE);
            float x = c;
            float w = 1;
            if (c == 0) {
                x = left;
                w = 1 - left;
            } else if (c == 3) {
                w = 1 - right;
            }
            ofDrawRectangle(x, y, w, h);
        }
    }

    ofPopMatrix();  // end left shoulder

    ofPushMatrix();  // right shoulder
    ofRotateZDeg(dTheta);
    ofTranslate(dxUnits / 2.0f, -1.5, 0);

    for (int r = 0; r < 3; r++) {
        float y = r;
        float h = 1;
        if (r == 0) {
            y = top;
            h = 1 - top;
        } else if (r == 2) {
            h = 1 - bottom;
        }
        for (int c = 0; c < 4; c++) {
            ofSetColor(codeFormatted[i++] ? BLACK : WHITE);
            float x = c;
            float w = 1;
            if (c == 0) {
                x = left;
                w = 1 - left;
            } else if (c == 3) {
                w = 1 - right;
            }
            ofDrawRectangle(x, y, w, h);
        }
    }

    ofPopMatrix();  // end right shoulder
    ofPopMatrix();  // end shoulder code

    // set color back to avoid tinting background
    ofSetColor(WHITE);
}

void ofApp::ofDrawNoise() {
    ofSetColor(ofColor(WHITE), 0x33);
    noise.draw(0, 0);
    ofSetColor(WHITE);
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (showBg) {
        background.draw(0, 0);
    } else {
        ofSetBackgroundColor(84, 229, 130);
    }
    ofDrawShoulderCode();
    if (addNoise) {
        ofDrawNoise();
    }
    gui.draw();
}

string ofApp::getCurrentImageFilePath() {
    string outputDir = ofFilePath::join(projectRoot, "out");
    ostringstream filename;
    filename << "sc" << (showBg ? "b" : "") << (addNoise ? "n" : "") << "_" <<
        "n" << n << "_" <<
        "x" << x0 << "_" <<
        "y" << y0 << "_" <<
        "r" << theta << "_" <<
        "s" << size << OUTPUT_EXT;
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
    if (key == 'r' || key == 'R') {
        onClickRandomizeParameters();
    } else if (key == 's' || key == 'S') {
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
    n = rand() % ShoulderCodec::VALUE_MAX;
    x0 = 256 + rand() % 768;
    y0 = 120 + rand() % 480;
    size = 4 + rand() % 12;
    theta = rand() % 360;
    dTheta = -20 + rand() % 40;
    dxUnits = (250 + rand() % 100) / 100.0f;
    occTopUnits = (rand() % 25) / 100.0f;
    occBottomUnits = (rand() % 25) / 100.0f;
    occLeftUnits = (rand() % 25) / 100.0f;
    occRightUnits = (rand() % 25) / 100.0f;
}
