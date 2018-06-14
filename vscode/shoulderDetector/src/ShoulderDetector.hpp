#pragma once

#include <bitset>

#include "ofMain.h"

class ShoulderDetector {
public:
    void detect(
            const ofPixels& pixels,
            const ofPolyline& contour,
            float theta,
            bitset<16>& out);
};