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

enum SportName { jugger=0, soccer=1, quidditch=2 };
enum PixelSize { fourK, teneightyP, seventwentyP };

class SportVideo {
public:
    int expectedPlayerCount;
    std::vector<Player> expectedPlayers;
    
private:
    SportName name;
    PixelSize pixels;
    ofVec2f pixelDims;
    float pixelsPerM;
    ofVec2f fieldDimsM;
    
public:
    SportVideo() :
        name(jugger), fieldDimsM(40,20), pixelDims(seventwentyP), pixelsPerM(50), expectedPlayerCount(10) {}
    
    SportVideo(SportName name, PixelSize pixels, int expectedPlayerCount, std::vector<Player> expectedPlayers);
    
    void updatePixelSize(PixelSize pixels);
    void updatePixelSize(ofPixels pixels);
    void updateSport(SportName sport);
    void updateExpectedPlayerCount(int expectedPlayerCount) { this->expectedPlayerCount = expectedPlayerCount; }
    void updateExpectedPlayers(string expectedPlayers);
    void updateExpectedPlayers(std::vector<Player> expectedPlayers) { this->expectedPlayers = expectedPlayers; }
};


#endif /* sportVideo_hpp */
