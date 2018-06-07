//
//  ShoulderCodec.cpp
//  WebcamTest
//
//  Created by Evan Savage on 2018-06-06.
//

#include <stdexcept>

#include "ShoulderCodec.hpp"

using namespace std;

ShoulderCodec::Code::Code(Mat& left, Mat& right) :
left(left),
right(right) {}

ostream& operator<<(ostream& os, const ShoulderCodec::Code& code) {
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 5; x++) {
            os << code.left.at<uint8>(y, x);
        }
        os << ' ';
        for (int x = 0; x < 5; x++) {
            os << code.right.at<uint8>(y, x);
        }
        os << '\n';
    }
    return os;
}

ShoulderCodec::ShoulderCodec() {
    /*
     * See https://en.wikipedia.org/wiki/Hamming_code for explanation on
     * these matrices.
     */
    
    uint8 g[32] = {
        1, 1, 0, 1,
        1, 0, 1, 1,
        1, 0, 0, 0,
        0, 1, 1, 1,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
        1, 1, 1, 0
    };
    
    uint8 h[32] = {
        1, 0, 1, 0, 1, 0, 1, 0,
        0, 1, 1, 0, 0, 1, 1, 0,
        0, 0, 0, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1
    };
    
    G = Mat(8, 4, CV_8U, g);
    H = Mat(8, 4, CV_8U, h);
}

ShoulderCodec::~ShoulderCodec() {
    // TODO: implement this
}

int ShoulderCodec::fromBits(int k, const uint8* bits) {
    int n = 0;
    for (int i = 0, mask = 1; i < k; i++, mask <<= 1) {
        if (bits[i] == 1) {
            n |= mask;
        }
    }
    return n;
}

void ShoulderCodec::toBits(int n, int k, uint8* bits) {
    for (int i = 0, mask = 1; i < k; i++, mask <<= 1) {
        bits[i] = n & mask ? 1 : 0;
    }
}

void ShoulderCodec::mod2(Mat& mat) {
    cv::Size size = mat.size();
    if (mat.isContinuous()) {
        size.width *= size.height;
        size.height = 1;
    }
    for (int i = 0; i < size.height; i++) {
        uint8* ptr = mat.ptr<uint8>(i);
        for (int j = 0; j < size.width; j++) {
            *ptr &= 1;
        }
    }
}

Mat ShoulderCodec::columnVector(int size, uint8* data) {
    return Mat(size, 1, CV_8U, data);
}

Mat ShoulderCodec::rowVector(int size, uint8* data) {
    return Mat(1, size, CV_8U, data);
}

ShoulderCodec::Code ShoulderCodec::encode(int n) {
    if (n < VALUE_MIN) {
        throw out_of_range("value too small");
    } else if (n > VALUE_MAX) {
        throw out_of_range("value too large");
    }
    uint8 bits[NUM_DATA_BITS];
    toBits(n, NUM_DATA_BITS, bits);
    
    // left shoulder
    uint8 bitsLeftArr[4] = { bits[0], bits[2], ROW_LEFT, bits[5] };
    Mat bitsLeft = columnVector(4, bitsLeftArr);
    Mat codeLeft = G * bitsLeft;
    mod2(codeLeft);
    uint8 leftArr[10] = {
        1, codeLeft.at<uint8>(4, 0), codeLeft.at<uint8>(5, 0), codeLeft.at<uint8>(6, 0), codeLeft.at<uint8>(7, 0),
        1, codeLeft.at<uint8>(0, 0), codeLeft.at<uint8>(1, 0), codeLeft.at<uint8>(2, 0), codeLeft.at<uint8>(3, 0)
    };
    Mat left(2, 5, CV_8U, leftArr);
    
    // right shoulder
    uint8 bitsRightArr[4] = { bits[1], bits[3], ROW_RIGHT, bits[4] };
    Mat bitsRight = columnVector(4, bitsRightArr);
    Mat codeRight = G * bitsRight;
    mod2(codeRight);
    uint8 rightArr[10] = {
        codeRight.at<uint8>(3, 0), codeRight.at<uint8>(2, 0), codeRight.at<uint8>(1, 0), codeRight.at<uint8>(0, 0), 1,
        codeRight.at<uint8>(7, 0), codeRight.at<uint8>(6, 0), codeRight.at<uint8>(5, 0), codeRight.at<uint8>(4, 0), 1
    };
    Mat right(2, 5, CV_8U, rightArr);
    
    return Code(left, right);
}

int ShoulderCodec::decode(const ShoulderCodec::Code& code) {
    if (code.left.at<uint8>(0, 0) != 1 || code.left.at<uint8>(1, 0) != 1) {
        throw invalid_argument("left: finder pattern missing");
    } else if (code.right.at<uint8>(0, 4) != 1 || code.right.at<uint8>(1, 4) != 1) {
        throw invalid_argument("right: finder pattern missing");
    }
    
    uint8 codeLeftArr[8] = {
        code.left.at<uint8>(1, 1),
        code.left.at<uint8>(1, 2),
        code.left.at<uint8>(1, 3),
        code.left.at<uint8>(1, 4),
        code.left.at<uint8>(0, 1),
        code.left.at<uint8>(0, 2),
        code.left.at<uint8>(0, 3),
        code.left.at<uint8>(0, 4)
    };
    Mat codeLeft = columnVector(8, codeLeftArr);
    Mat syndromeLeft = H * codeLeft;
    mod2(syndromeLeft);
    const uint8* syndromeLeftArr = syndromeLeft.ptr<uint8>(0);
    int syndromeLeftNum = fromBits(4, syndromeLeftArr);
    if (syndromeLeftNum != 0) {
        if (syndromeLeftNum < 8) {
            throw invalid_argument("left: double error, cannot correct");
        }
        syndromeLeftNum -= 9;
        codeLeftArr[syndromeLeftNum] = 1 - codeLeftArr[syndromeLeftNum];
    }
    if (codeLeftArr[5] != ROW_LEFT) {
        throw invalid_argument("left: row bit error");
    }
    
    uint8 codeRightArr[8] = {
        code.right.at<uint8>(0, 3),
        code.right.at<uint8>(0, 2),
        code.right.at<uint8>(0, 1),
        code.right.at<uint8>(0, 0),
        code.right.at<uint8>(1, 3),
        code.right.at<uint8>(1, 2),
        code.right.at<uint8>(1, 1),
        code.right.at<uint8>(1, 0)
    };
    Mat codeRight = columnVector(8, codeRightArr);
    Mat syndromeRight = H * codeRight;
    mod2(syndromeRight);
    const uint8* syndromeRightArr = syndromeRight.ptr<uint8>(0);
    int syndromeRightNum = fromBits(4, syndromeRightArr);
    if (syndromeRightNum != 0) {
        if (syndromeRightNum < 8) {
            throw invalid_argument("right: double error, cannot correct");
        }
        syndromeRightNum -= 9;
        codeRightArr[syndromeRightNum] = 1 - codeLeftArr[syndromeRightNum];
    }
    if (codeRightArr[5] != ROW_RIGHT) {
        throw invalid_argument("right: row bit error");
    }
    
    uint8 bits[NUM_DATA_BITS] = {
        codeLeftArr[2],
        codeRightArr[2],
        codeLeftArr[4],
        codeRightArr[4],
        codeRightArr[6],
        codeLeftArr[6]
    };
    return fromBits(NUM_DATA_BITS, bits);
}
