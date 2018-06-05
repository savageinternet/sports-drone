//
//  Sport.cpp
//  droneTrack
//
//  Created by Valkyrie Savage on 6/1/18.
//

#include "sportVideo.hpp"

using namespace ofxCv;
using namespace cv;

// when we create a sport video, we initialize a lot of junk at once.
SportVideo::SportVideo(SportName name, PixelSize pixels, int expectedPlayerCount, std::vector<Player> expectedPlayers) {
    this->updateSport(name);
    
    this->updatePixelSize(pixels);
    
    this->expectedPlayerCount = expectedPlayerCount;
    this->expectedPlayers = expectedPlayers;
}

void SportVideo::updatePixelSize(PixelSize pixels) {
    this->pixels = pixels;
    
    // calculate pixels per M
    float x_pixels = 0.0f, y_pixels = 0.0f;
    switch(pixels) {
        case fourK:
            // 4K camera: 3840 x 2160 pixels
            x_pixels = 3840.0f / fieldDimsM.x;
            y_pixels = 2160.0f / fieldDimsM.y;
            this->pixelsPerM = (x_pixels < y_pixels? x_pixels : y_pixels);
            break;
        case teneightyP:
            // 1080p camera: 1920 x 1080 pixels
            x_pixels = 1920.0f / fieldDimsM.x;
            y_pixels = 1080.0f / fieldDimsM.y;
            this->pixelsPerM = (x_pixels < y_pixels? x_pixels : y_pixels);
            break;
        case seventwentyP:
            // 720p camera: 1280 x 720 pixels
            x_pixels = 1280.0f / fieldDimsM.x;
            y_pixels = 720.0f / fieldDimsM.y;
            this->pixelsPerM = (x_pixels < y_pixels? x_pixels : y_pixels);
            break;
        default:
            break; // no, seriously. it should break if this isn't happening right.
    }
}

void SportVideo::updatePixelSize(ofPixels pixels) {
    switch(pixels.getWidth()) {
        case 3840:
            // 4K camera: 3840 x 2160 pixels
            this->updatePixelSize(fourK);
            break;
        case 1920:
            // 1080p camera: 1920 x 1080 pixels
            this->updatePixelSize(teneightyP);
            break;
        case 1280:
            // 720p camera: 1280 x 720 pixels
            this->updatePixelSize(seventwentyP);
            break;
        default:
            break; // no, seriously. it should break if this isn't happening right.
    }
}

void SportVideo::updateSport(SportName sport) {
    this->name = name;
    
    switch(this->name) {
        case soccer:
            this->fieldDimsM = *new ofVec2f(100,60);
            break;
        case quidditch:
            this->fieldDimsM = *new ofVec2f(60,36);
            break;
        case jugger:
        default:
            this->fieldDimsM = *new ofVec2f(40,20);
            break;
    }
}

void SportVideo::updateExpectedPlayers(string expectedPlayers) {
    // TODO do some parsing magic here :D
}
