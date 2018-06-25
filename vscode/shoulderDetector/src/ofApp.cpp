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
    int ds = size * 6;
    contour.addVertex(x0 - ds, y0 - ds);
    contour.addVertex(x0 + ds, y0 - ds);
    contour.addVertex(x0 + ds, y0 + ds);
    contour.addVertex(x0 - ds, y0 + ds);
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

template<typename T> void printVector(vector<T> v) {
    cout << "[";
    for (int i = 0; i < v.size() ; i++ ) {
        cout << v.at(i) << ",";
    }
    cout << "]";
}

struct Transition {
    int index;
    float strength;
};
struct Candidate {
    int theta;
    float score;
    Point2f center;
    float bitSize;
};

struct DetectedLocation {
    float theta;
    Point2f center;
    float bitSize;
};

ostream& operator<<(ostream& os, const Transition& t) {
    os << "(" << t.index << ", " << t.strength << ")";
    return os;
}

ostream& operator<<(ostream& os, const Candidate& c) {
    os << "{theta: " << c.theta <<
        ", score: " << c.score <<
        ", center: " << c.center <<
        ", bitSize: " << c.bitSize << "}";
    return os;
}

ostream& operator<<(ostream& os, const DetectedLocation& dl) {
    os << "{theta: " << dl.theta <<
        ", center: " << dl.center <<
        ", bitSize: " << dl.bitSize << "}";
    return os;
}

float scoreTransitions(const vector<Transition>& ts, int i) {
    int di10 = ts[i + 1].index - ts[i].index;
    int di21 = ts[i + 2].index - ts[i + 1].index;
    int di = abs(di21 - di10);
    float k = 10;
    float s = ts[i].strength + ts[i + 1].strength + ts[i + 2].strength;
    return s - k * di;
}

int findBestTransitions(const vector<Transition>& ts, float& score) {
    int n = ts.size();
    if (n < 3) {
        return -1;
    }
    int bestIndex = -1;
    float bestScore = 0;
    for (int i = 0; i < n - 2; i++) {
        float score = scoreTransitions(ts, i);
        if (score > bestScore) {
            bestIndex = i;
            bestScore = score;
        }
    }
    score = bestScore;
    return bestIndex;
}

float distance(const Point2f& p0, const Point2f& p1) {
    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    return sqrt(dx * dx + dy * dy);
}

Candidate getCandidate(const vector<Transition>& ts, int theta, float score, const vector<Point2f> ps, int i) {
    Candidate c;
    c.theta = theta;
    c.score = score;
    int i0 = ts[i].index;
    int i1 = ts[i + 1].index;
    int i2 = ts[i + 2].index;
    c.center = (ps[i0] + ps[i1] + ps[i2]) / 3;
    c.bitSize = (distance(ps[i0], ps[i1]) + distance(ps[i1], ps[i2])) / 2;
    return c;
}

Candidate averageCandidates(const vector<Candidate>& cs2x, int start, int end) {
    float averageScore = 0.0;
    int n = end - start + 1;
    for (int i = start; i <= end; i++) {
        averageScore += cs2x[i].score / n;
    }

    // compute average center, bitSize, filtering out anything with sub-average score MEDIOCRE
    int goodCount = 0;
    Candidate c;
    c.theta = 0;
    c.score = 0;
    c.center = Point2f(0, 0);
    c.bitSize = 0;
    for (int i = start; i <= end; i++) {
        if (cs2x[i].score < averageScore) {
            continue;
        }
        goodCount++;
        c.theta += cs2x[i].theta;
        c.score += cs2x[i].score;
        c.center += cs2x[i].center;
        c.bitSize += cs2x[i].bitSize;
    }
    c.theta /= goodCount;
    c.score /= goodCount;
    c.center /= goodCount;
    c.bitSize /= goodCount;
    return c;
}

void groupCandidates(const vector<Candidate>& cs, vector<Candidate>& out) {
    int n = cs.size();

    /*
     * To help in grouping candidates that cross the 0 / 360 edge, append a copy
     * of our candidate vector to the end.
     */
    vector<Candidate> cs2x;
    cs2x.insert(cs2x.end(), cs.begin(), cs.end());
    cs2x.insert(cs2x.end(), cs.begin(), cs.end());

    bool hasRun = false;
    int cycleEnd = 2 * n;
    int start = 0, end = 0, endTheta = cs2x[0].theta;
    for (int i = 1; i < cycleEnd; i++) {
        int theta = cs2x[i].theta;
        if (theta != (endTheta + 5) % 360) {
            if (hasRun) {
                // record last run
                Candidate c = averageCandidates(cs2x, start, end);
                out.push_back(c);
            } else {
                hasRun = true;
                cycleEnd = i + n;
            }
            // new run
            start = i;
        }
        end = i;
        endTheta = theta;
    }
    if (hasRun) {
        // record last run
        Candidate c = averageCandidates(cs2x, start, end);
        out.push_back(c);
    }
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
    vector<Candidate> candidates;
    for (int theta = 0; theta < 360; theta += 5) {
        float thetaRad = theta * M_PI / 180.0;
        if (!contourIntersection(contour, p1, thetaRad, p2)) {
            // TODO: now what?  this shouldn't happen...
            continue;
        }
        LineIterator it(mat, p1, p2);
        int n = it.count;
        vector<Point2f> ps;
        vector<uchar> vs;
        for (int i = 0; i < n; i++, ++it) {
            Point2f p = it.pos();
            uchar v = mat.at<uchar>(p);
            ps.push_back(p);
            vs.push_back(v);
        }

        ofSetColor(ofColor(255, 0, 0));
        float threshold = 127;
        float alpha = 0.3;
        bool inTransition = false;
        vector<Transition> transitions;
        int muStart = 0;
        for (int i = 0; i < 4; i++) {
            muStart += vs[i];
        }
        float mu = muStart / 4;
        for (int i = 4; i < n; i++) {
            float dv = vs[i] - mu;
            mu += alpha * dv;
            dv = abs(dv);
            if (dv >= threshold) {
                if (!inTransition) {
                    Transition t;
                    t.index = i;
                    t.strength = dv;
                    transitions.push_back(t);
                }
                inTransition = true;
            } else {
                inTransition = false;
            }
        }
        float score;
        int i = findBestTransitions(transitions, score);
        if (i != -1) {
            Candidate c = getCandidate(transitions, theta, score, ps, i);
            candidates.push_back(c);
        }
    }

    vector<Candidate> groupedCandidates;
    groupCandidates(candidates, groupedCandidates);

    printVector(groupedCandidates);
    cout << endl;

    ofSetColor(ofColor(255, 0, 0));
    for (int i = 0; i < groupedCandidates.size(); i++) {
        Point2f p = groupedCandidates[i].center;
        ofDrawRectangle(p.x - 1, p.y - 1, 3, 3);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (image.isAllocated()) {
        imageGrey.draw(0, 0);
        if (ranImageDetection) {
            ofSetColor(255, 0, 0);
            contour.draw();

            ofDrawDetect(imageGrey.getPixels(), contour, code);
            
            ofSetColor(0, 255, 0);
            ofDrawLine(x0 + 20 * cos(theta), y0 + 20 * sin(theta), x0 - 20 * cos(theta), y0 - 20 * sin(theta));
            ofDrawLine(x0, y0, x0 + 10 * sin(theta), y0 - 10 * cos(theta));

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
