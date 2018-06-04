//
//  Sport.cpp
//  droneTrack
//
//  Created by Valkyrie Savage on 6/1/18.
//

#include "sportVideo.hpp"

using namespace ofxCv;
using namespace cv;

SportVideo::SportVideo(SportName name, ofVec2f fieldDimsM, PixelSize pixels, int expectedPlayerCount, std::vector<Player> expectedPlayers) {
    this->name = name;
    
    this->fieldDimsM = fieldDimsM;
    // rotate so that we have the longer dimension first. that'll be what we see in the video.
    if (fieldDimsM.x < fieldDimsM.y) { fieldDimsM = fieldDimsM.getRotated(90); }
    
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
    
    this->expectedPlayerCount = expectedPlayerCount;
    this->expectedPlayers = expectedPlayers;
}
