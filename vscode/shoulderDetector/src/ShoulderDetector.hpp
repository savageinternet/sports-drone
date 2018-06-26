#pragma once

#include <bitset>

#include "ofMain.h"
#include "ofxCv.h"

using namespace cv;
using namespace ofxCv;
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

ostream& operator<<(ostream& os, const PointValue& t);
ostream& operator<<(ostream& os, const Transition& t);
ostream& operator<<(ostream& os, const Candidate& c);

class ShoulderDetector {
public:
    static const int THETA_STEP = 5;
    static const int ROLLING_THRESHOLD = 127;
    static const constexpr float ROLLING_ALPHA = 0.3;
    static const constexpr float INDEX_DIFFERENCE_PENALTY = 20;
    static const constexpr float BITSIZE_DIFFERENCE_PENALTY = 20;

    bool rayIntersection(Point2f p1, Point2f p2, Point2f o, float theta, Point2f& out);
    bool contourIntersection(const vector<Point2f>& contour, Point2f o, float theta, Point2f& out);
    template<typename T> void printVector(vector<T> v);
    void getPointValues(const Mat& mat, LineIterator& it, vector<PointValue>& pvs);
    void getTransitions(const vector<PointValue>& pvs, vector<Transition>& ts);
    float scoreTransitions(const vector<Transition>& ts, int i);
    int findBestTransitions(const vector<Transition>& ts, float& score);
    float distance(const Point2f& p0, const Point2f& p1);
    Candidate getCandidate(
        const vector<Transition>& ts,
        int theta,
        float score,
        const vector<PointValue>& pvs,
        int i);
    Candidate averageCandidates(const vector<Candidate>& cs2x, int start, int end);
    void groupCandidates(const vector<Candidate>& cs, vector<Candidate>& out);
    bool findBestCandidates(const vector<Candidate> cs, Candidate& c0, Candidate& c1);
    float getPairedScore(const Candidate& c0, const Candidate& c1);
    bool readBit(const Mat& mat, const Point2f& p);
    void readBitsTiming(const Mat& mat, const Candidate& c, bitset<4>& timing);
    void readBitsLattice(const Mat& mat, const Candidate& c, bitset<12>& bs);
    void buildFormattedCode(const bitset<12>& bs0, const bitset<12>& bs1, bitset<24>& codeFormatted);
    bool verify(const bitset<24>& codeFormatted);
    bool detect(const ofPixels& pixels, const ofPolyline& ofContour, bitset<24>& codeFormatted);
};