#include <bitset>
#include <iostream>

#include "ofMain.h"

#include "ShoulderDetector.hpp"

using namespace std;

void ShoulderDetector::detect(
        const ofPixels& pixels,
        const ofPolyline& contour,
        float theta,
        bitset<16>& out) {
    ofRectangle bbox = contour.getBoundingBox();

    ofPixels roi;
    pixels.cropTo(roi, bbox.x, bbox.y, bbox.width, bbox.height);

    ofVec2f centroid = contour.getCentroid2D();
    
}
