//
//  playerTracker.hpp
//  droneTrack
//
//  Created by Valkyrie Savage on 5/30/18.
//

#ifndef playerTracker_hpp
#define playerTracker_hpp

#include <stdio.h>
#include <math.h>

#include "ofMain.h"
#include "ofxCv.h"

using namespace cv;
using namespace ofxCv;

namespace ofxCv {
    class Player {
        // note that we don't want to subclass "TrackedObject"; it's a template.
    public:
        ofVec2f velocity;
        ofColor jerseyColor;
        int jerseyID;
        int label;
        int index;
        cv::Rect rect;
        
        Player() : jerseyColor(0, 255, 0), jerseyID(1), label(0), index(0) {};
    };

    float trackingDistance(const Player& a, const Player& b);
};

#endif /* playerTracker_hpp */
