//
//  playerTracker.hpp
//  droneTrack
//
//  Created by Valkyrie Savage on 5/30/18.
//

#ifndef playerTracker_hpp
#define playerTracker_hpp

#include <stdio.h>

#include "ofMain.h"
#include "ofxCv.h"

using namespace cv;
using namespace ofxCv;

class Player : public cv::Rect {
    // note that we don't want to subclass "TrackedObject"; it's a template.
public:
    ofVec2f velocity;
    ofColor jerseyColor;
    int jerseyID;
    int label;
    int index;
    
    Player() : jerseyColor(0, 255, 0), jerseyID(1), label(0), index(0) {};
    Player(cv::Rect rect) : cv::Rect(rect) {};
    Player(Player p, int label, int index) : cv::Rect((cv::Rect)p), label(label), index(index) {};
};

/*class PlayerTracker : public Tracker<Player> {
protected:*/
namespace ofxCv {
    float trackingDistance(const Player& a, const Player& b);
/*public:
    PlayerTracker() {}*/
};

#endif /* playerTracker_hpp */
