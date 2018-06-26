#pragma once

#include <bitset>

#include "ofMain.h"
#include "ofxCv.h"

using namespace cv;
using namespace ofxCv;
using namespace std;

struct Transition {
    int index;
    float strength;
};

struct Candidate {
    int theta;
    float score;
    Point2f center;
    float bitSize;
};

ostream& operator<<(ostream& os, const Transition& t);
ostream& operator<<(ostream& os, const Candidate& c);

class ShoulderDetector {
public:
    bool rayIntersection(Point2f p1, Point2f p2, Point2f o, float theta, Point2f& out);
    bool contourIntersection(const vector<Point2f>& contour, Point2f o, float theta, Point2f& out);
    template<typename T> void printVector(vector<T> v);
    float scoreTransitions(const vector<Transition>& ts, int i);
    int findBestTransitions(const vector<Transition>& ts, float& score);
    float distance(const Point2f& p0, const Point2f& p1);
    Candidate getCandidate(const vector<Transition>& ts, int theta, float score, const vector<Point2f> ps, int i);
    Candidate averageCandidates(const vector<Candidate>& cs2x, int start, int end);
    void groupCandidates(const vector<Candidate>& cs, vector<Candidate>& out);
    bool findBestCandidates(const vector<Candidate> cs, Candidate& c0, Candidate& c1);
    float getPairedScore(const Candidate& c0, const Candidate& c1);
    bool readBit(const Mat& mat, const Point2f& p);
    void readBitLattice(const Mat& mat, const Candidate& c, bitset<12>& bs);
    void buildFormattedCode(const bitset<12>& bs0, const bitset<12>& bs1, bitset<24>& codeFormatted);
    bool verify(const bitset<24>& codeFormatted);
    bool detect(const ofPixels& pixels, const ofPolyline& ofContour, bitset<24>& codeFormatted);
};