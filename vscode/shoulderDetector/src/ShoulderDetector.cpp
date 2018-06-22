#include <bitset>
#include <iostream>
#include <vector>

#include "ofMain.h"
#include "ofxCv.h"

#include "ShoulderDetector.hpp"

using namespace cv;
using namespace ofxCv;
using namespace std;

bool ShoulderDetector::rayIntersection(Point2f p1, Point2f p2, Point2f o, float theta, Point2f& out) {
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
    out = p1 + u * s;
    return true;
}

bool ShoulderDetector::contourIntersection(const vector<Point2f>& contour, Point2f o, float theta, Point2f& out) {
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

void ShoulderDetector::detect(
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
        vector<unsigned char> vs(n);
        for (int i = 0; i < n; i++, ++it) {
            unsigned char v = mat.at<unsigned char>(it.pos());
            vs.push_back(v);
        }
    }
}
