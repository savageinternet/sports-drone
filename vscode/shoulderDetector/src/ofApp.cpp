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
    /*
    detector.detect(
        image.getPixels(),
        contour,
        code);
        */
    ranImageDetection = true;
}

// see https://stackoverflow.com/questions/14307158/how-do-you-check-for-intersection-between-a-line-segment-and-a-line-ray-emanatin
bool rayIntersection(Point2f p1, Point2f p2, Point2f o, float theta, Point2f& out) {
    Point2f r(cos(theta), sin(theta));
    Point2f s = p2 - p1;
    Point2f q = p1 - o;

    float qxr = q.x * r.y - q.y * r.x;
    float rxs = r.x * s.y - r.y * s.x;
    if (abs(rxs) < 1e-6) {
        return false;
    }
    float u = qxr / rxs;
    if (u < 0 || u > 1) {
        return false;
    }
    float qxs = q.x * s.y - q.y * s.x;
    float t = qxs / rxs;
    if (t < 0) {
        return false;
    }
    out = p1 + u * s;
    return true;
}

bool contourIntersection(const vector<Point2f>& contour, Point2f o, float theta, Point2f& out) {
    int n = contour.size();
    Point2f p1 = contour[0];
    Point2f p2;
    for (int i = 1; i < n; i++) {
        p2 = contour[i];
        if (rayIntersection(p1, p2, o, theta, out)) {
            return true;
        }
        p1 = p2;
    }
    p2 = contour[0];
    return rayIntersection(p1, p2, o, theta, out);
}

void ofDrawDetect(
        const ofPixels& pixels,
        const ofPolyline& ofContour,
        bitset<16>& out) {
    ofRectangle bbox = ofContour.getBoundingBox();
    ofVec2f centroid = ofContour.getCentroid2D();
    
    Mat mat = toCv(pixels);
    vector<Point2f> contour = toCv(ofContour);
    Point2f p1 = toCv(centroid);
    Point2f p2;

    /*
     * Cast multiple rays out from the centroid to the contour edges.
     */
    for (int theta = 0; theta < 360; theta += 5) {
        float thetaRad = theta * M_PI / 180.0;
        if (!contourIntersection(contour, p1, thetaRad, p2)) {
            // TODO: now what?  this shouldn't happen...
            continue;
        }
        LineIterator it(mat, p1, p2);
        int n = it.count;
        //vector<unsigned char> vs(n);
        for (int i = 0; i < n; i++, ++it) {
            //unsigned char v = mat.at(it.pos());
            Point2f p = it.pos();
            ofDrawRectangle(p.x, p.y, 1, 1);
            //vs.push_back(v);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (image.isAllocated()) {
        imageGrey.draw(0, 0);
        if (ranImageDetection) {
            ofSetColor(255, 0, 0);
            contour.draw();
            
            ofSetColor(0, 255, 0);
            ofDrawLine(x0 + 20 * cos(theta), y0 + 20 * sin(theta), x0 - 20 * cos(theta), y0 - 20 * sin(theta));
            ofDrawLine(x0, y0, x0 + 10 * sin(theta), y0 - 10 * cos(theta));

            ofSetColor(0, 255, 255);
            ofDrawDetect(image.getPixels(), contour, code);

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
    image.update();
    convertColor(image, imageGrey, CV_RGB2GRAY);
    imageGrey.update();
    ranImageDetection = false;
}
