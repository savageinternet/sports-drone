#pragma once

#include <bitset>
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

struct PointValue {
    Point2f p;
    uchar v;
    PointValue(Point2f p, uchar v);
};

struct Transition {
    int index;
    float strength;
    Transition(int index, float strength);
};

struct Candidate {
    int theta;
    float score;
    Point2f center;
    float bitSize;
};

struct DetectionState {
    vector<Candidate> cs;
    vector<Candidate> csGrouped;
    Candidate c0;
    Candidate c1;
    bitset<12> bs0;
    bitset<12> bs1;
    void reset();
};

ostream& operator<<(ostream& os, const PointValue& pv);
ostream& operator<<(ostream& os, const Transition& t);
ostream& operator<<(ostream& os, const Candidate& c);
ostream& operator<<(ostream& os, const DetectionState& ds);
