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
#include "WebcamConstants.hpp"

using namespace cv;
using namespace ofxCv;
using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
    projectRoot = ofFilePath::getAbsolutePath("../../../..", false);
    
    image.allocate(WebcamConstants::CAMERA_WIDTH,
                   WebcamConstants::CAMERA_HEIGHT,
                   OF_IMAGE_COLOR);
    imageGrey.allocate(WebcamConstants::CAMERA_WIDTH,
                       WebcamConstants::CAMERA_HEIGHT,
                       OF_IMAGE_GRAYSCALE);

    cam.setDeviceID(0);
    cam.setDesiredFrameRate(WebcamConstants::CAMERA_FPS);
    cam.initGrabber(WebcamConstants::CAMERA_WIDTH,
                    WebcamConstants::CAMERA_HEIGHT);

    btnLoadImage.addListener(this, &ofApp::loadImage);
    btnLoadVideo.addListener(this, &ofApp::loadVideo);
    btnLoadWebcam.addListener(this, &ofApp::loadWebcam);

    gui.setup("Settings");
    gui.add(btnLoadImage.setup("Load Image"));
    gui.add(btnLoadVideo.setup("Load Video"));
    gui.add(btnLoadWebcam.setup("Load Webcam"));
    gui.setPosition(0, 0);

    detectionMode = DetectionMode::WEBCAM_SET_CONTOUR;
    loadWebcam();
}

void printGroundTruth(int n) {
    bitset<16> code;
    bitset<24> codeFormatted;
    ShoulderCodec::encode(n, code);
    ShoulderCodec::format(code, codeFormatted);
    ShoulderCodec::print(cout, codeFormatted);
}

void ofApp::performDetection() {
    if (detectionMode == DetectionMode::IMAGE) {
        printGroundTruth(imageParameters.n);
    }
    Mat mat = toCv(imageGrey.getPixels());
    vector<Point2f> contour = toCv(ofContour);
    Point2f centroid = toCv(ofCentroid);
    bool codeDetected = detector.detect(mat, contour, centroid, codeFormatted);
    if (codeDetected) {
        ShoulderCodec::unformat(codeFormatted, code);
        result = ShoulderCodec::decode(code);
    } else {
        result = -1;
        cout << "no code detected!" << endl;
    }
}

string ofApp::getModeName(DetectionMode detectionMode) {
    switch (detectionMode) {
    case IMAGE:
        return "Image";
    case VIDEO:
        return "Video";
    case WEBCAM_SET_CONTOUR:
        return "Webcam (Set Location)";
    case WEBCAM:
        return "Webcam";
    }
}

void ofApp::updateWindowTitle() {
    string modeName = ofApp::getModeName(detectionMode);
    int fps = ofGetFrameRate();
    ostringstream oss;
    oss << "Shoulder Live Detector [" << modeName << ", fps: " << fps << "]";
    ofSetWindowTitle(oss.str());
}

void ofApp::updateImage() {
    if (!detectionFinished) {
        performDetection();
        detectionFinished = true;
    }
}

void ofApp::updateVideo() {
    // TODO: implement this
}

void ofApp::updateWebcam() {
    cam.update();
    if (cam.isFrameNew()) {
        if (skippedFirstFrame) {
            // get video frame and convert to grayscale
            convertColor(cam, imageGrey, CV_RGB2GRAY);
            imageGrey.update();
        } else {
            skippedFirstFrame = true;
        }
        detectionFinished = false;
    } else if (!detectionFinished && detectionMode == DetectionMode::WEBCAM) {
        //performDetection();
        detectionFinished = true;
    }
}

//--------------------------------------------------------------
void ofApp::update() {
    updateWindowTitle();
    switch (detectionMode) {
    case IMAGE:
        updateImage();
        break;
    case VIDEO:
        updateVideo();
        break;
    case WEBCAM_SET_CONTOUR:
    case WEBCAM:
        updateWebcam();
        break;
    }
}

void ofApp::drawCodeLocation() {
    ofSetColor(ofColor(255, 0, 0));
    ofContour.draw();
    int x0 = ofCentroid.x;
    int y0 = ofCentroid.y;
    ofDrawLine(x0 - 5, y0, x0 + 5, y0);
    ofDrawLine(x0, y0 - 5, x0, y0 + 5);
    ofSetColor(WHITE);
}

void ofApp::drawDetectionResult() {
    ostringstream oss;
    oss << result << "!";
    ofSetColor(ofColor(255, 0, 0));
    ofDrawBitmapString(oss.str(), ofCentroid.x, ofCentroid.y);
    ofSetColor(WHITE);
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (imageGrey.isAllocated()) {
        imageGrey.draw(0, 0);
        drawCodeLocation();
        if (detectionFinished) {
            drawDetectionResult();
        }
    }
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'i') {
        loadImage();
    } else if (key == 'v') {
        loadVideo();
    } else if (key == 'w') {
        loadWebcam();
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
    if (detectionMode != DetectionMode::WEBCAM_SET_CONTOUR) {
        return;
    }
    ofContour.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (detectionMode != DetectionMode::WEBCAM_SET_CONTOUR) {
        return;
    }
    ofContour.clear();
    ofContour.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (detectionMode != DetectionMode::WEBCAM_SET_CONTOUR) {
        return;
    }
    ofContour.close();

    // compute convex hull
    vector<Point2f> contour = toCv(ofContour);
    vector<Point2f> hull;
    convexHull(contour, hull);
    ofContour = toOf(hull);
    
    ofCentroid = ofContour.getCentroid2D();
    detectionMode = DetectionMode::WEBCAM;
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

ImageParameters ofApp::parseImageParameters(const string& filename) {
    ImageParameters imageParameters;

    vector<string> parts;
    StringUtils::split(filename, "_", parts);
    /*
     * For now, we do absolutely no validation; the main point here is to get the parameters
     * so we can test the detector, not write a super-robust filename parser.
     */
    const char* part;
    part = parts[1].c_str();
    imageParameters.n = atoi(part + 1);
    part = parts[2].c_str();
    imageParameters.x0 = atoi(part + 1);
    part = parts[3].c_str();
    imageParameters.y0 = atoi(part + 1);
    part = parts[4].c_str();
    int thetaDeg = atoi(part + 1);
    imageParameters.theta = thetaDeg / 180.0 * M_PI;
    part = parts[5].c_str();
    imageParameters.size = atoi(part + 1);

    return imageParameters;
}

void ofApp::setImageCodeLocation() {
    int x0 = imageParameters.x0;
    int y0 = imageParameters.y0;
    int ds = imageParameters.size * 6;

    ofContour.clear();
    ofContour.addVertex(x0 - ds, y0 - ds);
    ofContour.addVertex(x0 + ds, y0 - ds);
    ofContour.addVertex(x0 + ds, y0 + ds);
    ofContour.addVertex(x0 - ds, y0 + ds);
    ofContour.close();
    ofCentroid = ofContour.getCentroid2D();
}

void ofApp::loadImage() {
    string codePath = ofFilePath::join(projectRoot, "../shoulderGenerator/out");
    ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an image", false, codePath);
    if (!openFileResult.bSuccess) {
        return;
    }
    string filename = openFileResult.getName();
    filename = ofFilePath::removeExt(filename);
    imageParameters = ofApp::parseImageParameters(filename);

    setImageCodeLocation();

    image.load(openFileResult.getPath());
    image.update();
    convertColor(image, imageGrey, CV_RGB2GRAY);
    imageGrey.update();

    detectionFinished = false;
    detectionMode = DetectionMode::IMAGE;
}

void ofApp::loadVideo() {
    // TODO: implement this
}

void ofApp::loadWebcam() {
    ofContour.clear();
    ofCentroid = ofVec2f(0, 0);

    detectionFinished = false;
    skippedFirstFrame = false;
    detectionMode = DetectionMode::WEBCAM_SET_CONTOUR;
}