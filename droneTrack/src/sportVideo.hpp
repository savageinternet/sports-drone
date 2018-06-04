//
//  Sport.hpp
//  droneTrack
//
//  Created by Valkyrie Savage on 6/1/18.
//

#ifndef sportVideo_hpp
#define sportVideo_hpp

#include <stdio.h>

#include "ofMain.h"
#include "ofxCv.h"
#include "playerTracker.hpp"

using namespace cv;
using namespace ofxCv;

enum SportName { jugger, soccer, rugby };
enum PixelSize { fourK, teneightyP, seventwentyP };

class SportVideo {
public:
    SportName name;
    ofVec2f fieldDimsM;
    PixelSize pixels;
    float pixelsPerM;
    int expectedPlayerCount;
    std::vector<Player> expectedPlayers;
    
private:
    ofVec2f pixelDims;
    
public:
    SportVideo() :
        name(jugger), fieldDimsM(40,20), pixelDims(seventwentyP), pixelsPerM(50), expectedPlayerCount(10) {}
    
    SportVideo(SportName name, ofVec2f fieldDimsM, PixelSize pixels, int expectedPlayerCount, std::vector<Player> expectedPlayers);
};


#endif /* sportVideo_hpp */
