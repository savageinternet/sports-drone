//
//  playerTracker.cpp
//  droneTrack
//
//  Created by Valkyrie Savage on 5/30/18.
//

#include "playerTracker.hpp"

using namespace ofxCv;
using namespace cv;

float trackingDistance(const Player& a, const Player& b) {
    /* notes from valkyrie :D
    * we want to make this track "players" instead of "rectangles". hope this override works!
    * take into account:
          * difference in size/location (as here)
          * direction/velocity of the rectangle
          * can weight diff in location with knowledge of human travel stuff (based on field size)
          * average color?
          * eventually jersey id?
          * would like to be able to turn one two-player-blob into two one-player-blobs, but gotta figure out how.
          * ...what else...
    */
    // are the blobs going the same direction?
    float velocity_similarity = a.velocity.getNormalized().dot(b.velocity.getNormalized());
    float velocity_weight = 1.0f;
    
    // just check if the #s are the same. barcode reading makes it hard to say something is "close"
    bool numbers_same = a.jerseyID == b.jerseyID;
    float numbers_similarity = (numbers_same ? 1.0f : 0.0f);
    float numbers_weight = .5f;
    
    // we use euclidean distance between colors because we're fuckin' hacks
    float color_similarity = sqrtf((a.jerseyColor.r - b.jerseyColor.r)^2 +
                                   (a.jerseyColor.g - b.jerseyColor.g)^2 +
                                   (a.jerseyColor.b - b.jerseyColor.b)^2);
    float color_weight = 1.0f;
    
    // are the blobs in about the same spot?
    float d_x = (a.x + a.width / 2.) - (b.x + b.width / 2.);
    float d_y = (a.y + a.height / 2.) - (b.y + b.height / 2.);
    float loc_difference = sqrtf(d_x * d_x + d_y * d_y);
    // we know how humans work, so we can actually cap the maximum distance that a human could move
    // between two frames here.
    // and... we can check if one plus its velocity = the other! wouldn't that be clever...
    float loc_weight = 1.0f;
    
    // are the blobs about the same size?
    float d_wid = a.width - b.width;
    float d_ht = a.height - b.height;
    float size_difference = sqrtf(d_wid * d_wid + d_ht * d_ht);
    float size_weight = 1.0f;
    
    return velocity_similarity * velocity_weight +
            numbers_similarity * numbers_weight +
            loc_difference * loc_weight +
            size_difference * size_weight;
}
