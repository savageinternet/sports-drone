#pragma once

#include <bitset>

#include "ofMain.h"
#include "ofxCv.h"

using namespace cv;
using namespace ofxCv;
using namespace std;

class ShoulderDetector {
private:
    bool rayIntersection(Point2f p1, Point2f p2, Point2f o, float theta, Point2f& out);
    bool contourIntersection(const vector<Point2f>& contour, Point2f o, float theta, Point2f& out);
public:
    void detect(const ofPixels& pixels, const ofPolyline& ofContour, bitset<16>& out);
};