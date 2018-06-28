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

    btnRandomizeParameters.addListener(this, &ofApp::randomizeParameters);
    btnSaveCurrentImage.addListener(this, &ofApp::saveCurrentImage);

    gui.setup("Settings");

    imageParameters.setName("Image Parameters");
    imageParameters.add(showBg.set("Show Background?", false));
    imageParameters.add(addNoise.set("Add Noise?", false));
    gui.add(imageParameters);

    codeParameters.setName("Code Parameters");
    codeParameters.add(n.set("Encoded Value:", 42, 0, 255));
    codeParameters.add(p0.set("Center: ", ofVec2f(256, 120), ofVec2f(256, 120), ofVec2f(1024, 600)));
    codeParameters.add(size.set("Size:", 16, 4, 16));
    codeParameters.add(theta.set("Rotation:", 0, 0, 359));
    codeParameters.add(dTheta.set("Shoulder Rotation:", 0, -20, 20));
    codeParameters.add(dxUnits.set("Shoulder Separation:", 3.0, 2.5, 3.5));
    codeParameters.add(occTopUnits.set("Occlusion Top:", 0, 0, 0.25));
    codeParameters.add(occBottomUnits.set("Occlusion Bottom:", 0, 0, 0.25));
    codeParameters.add(occLeftUnits.set("Occlusion Left:", 0, 0, 0.25));
    codeParameters.add(occRightUnits.set("Occlusion Right:", 0, 0, 0.25));
    gui.add(codeParameters);

    gui.add(runDetection.set("Run Detection?", false));
    gui.add(btnRandomizeParameters.setup("Randomize"));
    gui.add(btnSaveCurrentImage.setup("Save"));

    gui.setPosition(0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){
    int fps = ofGetFrameRate();
    ostringstream oss;
	oss << "Shoulder Code Generator [fps: " << fps << "]";
	ofSetWindowTitle(oss.str());

    ShoulderCodec::encode(n, code);
    ShoulderCodec::format(code, codeFormatted);
}

void ofApp::ofDrawShoulderCode() {
    float top = occTopUnits.get();
    float bottom = occBottomUnits.get();
    float left = occLeftUnits.get();
    float right = occRightUnits.get();
    int i = 0;

    ofPushMatrix();  // shoulder code
    ofTranslate(p0->x, p0->y, 0);
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

void printGroundTruth(int n) {
    bitset<16> code;
    bitset<24> codeFormatted;
    ShoulderCodec::encode(n, code);
    ShoulderCodec::format(code, codeFormatted);
    ShoulderCodec::print(cout, codeFormatted);
}

void ofApp::ofDrawDetect() {
    getScreenImage(image);
    convertColor(image, imageGrey, CV_RGB2GRAY);
    imageGrey.update();

    ofPolyline ofContour;
    int ds = size * 6;
    ofContour.addVertex(p0->x - ds, p0->y - ds);
    ofContour.addVertex(p0->x + ds, p0->y - ds);
    ofContour.addVertex(p0->x + ds, p0->y + ds);
    ofContour.addVertex(p0->x - ds, p0->y + ds);
    ofContour.close();

    printGroundTruth(n);

    // run detection
    Mat mat = toCv(imageGrey.getPixels());
    vector<Point2f> contour = toCv(ofContour);
    ofVec2f ofCentroid = ofContour.getCentroid2D();
    Point2f centroid = toCv(ofCentroid);
    bool codeDetected = detector.detect(mat, contour, centroid, codeFormatted);
    if (codeDetected) {
        ShoulderCodec::unformat(codeFormatted, code);
        result = ShoulderCodec::decode(code);
        if (result == -1) {
            cout << "code detection failure!" << endl;
        }
    } else {
        result = -1;
        cout << "no code detected!" << endl;
    }
    
    // draw contour
    ofContour.draw();

    // draw result
    ofSetColor(ofColor(255, 0, 0));
    ostringstream oss;
    oss << result << "!";
    ofDrawBitmapString(oss.str(), p0->x, p0->y);
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
    if (runDetection) {
        ofDrawDetect();
    }
    gui.draw();
}

void ofApp::getScreenImage(ofImage& image) {
    image.grabScreen(0, 0, 1280, 720);
}

string ofApp::getCurrentImageFilePath() {
    string outputDir = ofFilePath::join(projectRoot, "out");
    ostringstream filename;
    filename << "sc" << (showBg ? "b" : "") << (addNoise ? "n" : "") << "_" <<
        "n" << n << "_" <<
        "x" << p0->x << "_" <<
        "y" << p0->y << "_" <<
        "r" << theta << "_" <<
        "s" << size << OUTPUT_EXT;
    return ofFilePath::join(outputDir, filename.str());
}

void ofApp::saveCurrentImage() {
    string filePath = getCurrentImageFilePath();
    getScreenImage(image);
    image.save(filePath);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == 'b') {
        showBg = !showBg;
    } else if (key == 'd') {
        runDetection = !runDetection;
    } else if (key == 'n') {
        addNoise = !addNoise;
    } else if (key == 'r') {
        randomizeParameters();
    } else if (key == 's') {
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

void ofApp::randomizeParameters() {
    n = rand() % ShoulderCodec::VALUE_MAX;
    p0 = ofVec2f(256 + rand() % 768, 120 + rand() % 480);
    size = 4 + rand() % 12;
    theta = rand() % 360;
    dTheta = -20 + rand() % 40;
    dxUnits = (250 + rand() % 100) / 100.0f;
    occTopUnits = (rand() % 25) / 100.0f;
    occBottomUnits = (rand() % 25) / 100.0f;
    occLeftUnits = (rand() % 25) / 100.0f;
    occRightUnits = (rand() % 25) / 100.0f;
}
