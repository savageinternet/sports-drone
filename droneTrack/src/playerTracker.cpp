//
//  playerTracker.cpp
//  droneTrack
//
//  Created by Valkyrie Savage on 5/30/18.
//

#include "playerTracker.hpp"

using namespace ofxCv;
using namespace cv;

namespace ofxCv {
    float trackingDistance(const Player& a, const Player& b) {
        /* notes from valkyrie :D
        * we want to make this track "players" instead of "rectangles". hope this override works!
        * take into account:
              * difference in size/location (as with rectangles)
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
        
        // we use euclidean distance between HSB colors because we're fuckin' hacks
        float hue_weight = 1.0f, saturation_weight = 0.3f, brightness_weight = 0.3f;
        float color_similarity = sqrtf(pow(a.jerseyColor.getHue() - b.jerseyColor.getHue(),2)*hue_weight +
                                       pow(a.jerseyColor.getSaturation() - b.jerseyColor.getSaturation(),2)*saturation_weight +
                                       pow(a.jerseyColor.getBrightness() - b.jerseyColor.getBrightness(),2)*brightness_weight);
        float color_weight = 1.0f;
        
        // are the blobs in about the same spot?
        float d_x = (a.rect.x + a.rect.width / 2.) - (b.rect.x + b.rect.width / 2.);
        float d_y = (a.rect.y + a.rect.height / 2.) - (b.rect.y + b.rect.height / 2.);
        float loc_difference = sqrtf(d_x * d_x + d_y * d_y);
        // we know how humans work, so we can actually cap the maximum distance that a human could move
        // between two frames here.
        // and... we can check if one plus its velocity = the other! wouldn't that be clever...
        float loc_weight = 1.0f;
        
        // are the blobs about the same size?
        float d_wid = a.rect.width - b.rect.width;
        float d_ht = a.rect.height - b.rect.height;
        float size_difference = sqrtf(d_wid * d_wid + d_ht * d_ht);
        float size_weight = .5f;
        
        float similarity = velocity_similarity * velocity_weight +
                            color_similarity * color_weight +
                            numbers_similarity * numbers_weight +
                            loc_difference * loc_weight +
                            size_difference * size_weight;
        
        return similarity;
    }
}
