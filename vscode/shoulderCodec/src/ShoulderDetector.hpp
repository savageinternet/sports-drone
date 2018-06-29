#pragma once

#include <bitset>

#include <opencv2/opencv.hpp>

#include "ShoulderTypes.hpp"

using namespace cv;
using namespace std;

class ShoulderDetector {
public:
    // constants
    static const int THETA_STEP = 5;
    static const int ROLLING_THRESHOLD = 80;
    static const constexpr float ROLLING_ALPHA = 0.3;
    static const constexpr float INDEX_DIFFERENCE_PENALTY = 20;
    static const constexpr float BITSIZE_DIFFERENCE_PENALTY = 20;

    // get internal state (for display)
    DetectionState getDetectionState() const;

    // detection functions
    static void getPointValues(const Mat& mat, LineIterator& it, vector<PointValue>& pvs);
    static void getTransitions(const vector<PointValue>& pvs, vector<Transition>& ts);
    static float scoreTransitions(const vector<Transition>& ts, int i);
    static int findBestTransitions(const vector<Transition>& ts, float& score);
    static Candidate getCandidate(
        const vector<Transition>& ts,
        int theta,
        float score,
        const vector<PointValue>& pvs,
        int i);
    static Candidate averageCandidates(const vector<Candidate>& cs2x, int start, int end);
    static void groupCandidates(const vector<Candidate>& cs, vector<Candidate>& out);
    static bool findBestCandidates(const vector<Candidate> cs, Candidate& c0, Candidate& c1);
    static float getPairedScore(const Candidate& c0, const Candidate& c1);
    static bool readBit(const Mat& mat, const Point2f& p);
    static void readBitsTiming(const Mat& mat, const Candidate& c, bitset<4>& timing);
    static void readBitsLattice(const Mat& mat, const Candidate& c, bitset<12>& bs);
    static void buildFormattedCode(const bitset<12>& bs0, const bitset<12>& bs1, bitset<24>& codeFormatted);
    static bool verifyFormattedCode(const bitset<24>& codeFormatted);

    bool detect(
        const Mat& mat,
        const vector<Point2f>& contour,
        Point2f centroid,
        bitset<24>& codeFormatted);

private:
    DetectionState detectionState;

    void reset();
};