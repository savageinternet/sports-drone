//
//  ShoulderCodec.hpp
//  WebcamTest
//
//  Created by Evan Savage on 2018-06-06.
//

#ifndef ShoulderCodec_hpp
#define ShoulderCodec_hpp

#include "ofxCv.h"

using namespace cv;

class ShoulderCodec {
private:
    static const int NUM_DATA_BITS = 6;
    static const int NUM_EC_BITS = 3;
    static const int VALUE_MIN = 0;
    static const int VALUE_MAX = 1 << NUM_DATA_BITS;
    static const uint8 ROW_LEFT = 0;
    static const uint8 ROW_RIGHT = 1;
    
    Mat G;
    Mat H;
public:
    struct Code {
        Code(Mat& left, Mat& right);
        Mat left;
        Mat right;
        friend ostream& operator<<(ostream& os, const Code& code);
    };
    
    ShoulderCodec();
    ~ShoulderCodec();
    static int fromBits(int k, const uint8* bits);
    static void toBits(int n, int k, uint8* bits);
    static void mod2(Mat& mat);
    static Mat columnVector(int size, uint8* data);
    static Mat rowVector(int size, uint8* data);
    Code encode(int n);
    int decode(const Code& code);
};

#endif /* ShoulderCodec_hpp */
