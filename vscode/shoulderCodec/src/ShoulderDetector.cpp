#include <bitset>
#include <iostream>
#include <vector>

#include "GeometryUtils.hpp"
#include "ShoulderDetector.hpp"
#include "ShoulderTypes.hpp"
#include "StringUtils.hpp"

using namespace cv;
using namespace std;

DetectionState ShoulderDetector::getDetectionState() const {
    return detectionState;
}

void ShoulderDetector::getPointValues(const Mat& mat, LineIterator& it, vector<PointValue>& pvs) {
    int n = it.count;
    for (int i = 0; i < n; i++, ++it) {
        Point2f p = it.pos();
        uchar v = mat.at<uchar>(p);
        PointValue pv(p, v);
        pvs.push_back(pv);
    }
}

void ShoulderDetector::getTransitions(const vector<PointValue>& pvs, vector<Transition>& ts) {
    int n = pvs.size();
    if (n == 0) {
        return;
    }
    float mu = pvs[0].v;
    bool inTransition = false;
    for (int i = 1; i < n; i++) {
        float dv = pvs[i].v - mu;
        mu += ROLLING_ALPHA * dv;
        dv = abs(dv);
        if (dv >= ROLLING_THRESHOLD) {
            if (!inTransition) {
                Transition t(i, dv);
                ts.push_back(t);
            }
            inTransition = true;
        } else {
            inTransition = false;
        }
    }
}

float ShoulderDetector::scoreTransitions(const vector<Transition>& ts, int i) {
    int di10 = ts[i + 1].index - ts[i].index;
    int di21 = ts[i + 2].index - ts[i + 1].index;
    int di = abs(di21 - di10);
    float s = ts[i].strength + ts[i + 1].strength + ts[i + 2].strength;
    return s - INDEX_DIFFERENCE_PENALTY * di * di;
}

int ShoulderDetector::findBestTransitions(const vector<Transition>& ts, float& score) {
    int n = ts.size();
    if (n < 3) {
        return -1;
    }
    int bestIndex = -1;
    float bestScore = 0;
    for (int i = 0; i < n - 2; i++) {
        float score = scoreTransitions(ts, i);
        if (score > bestScore) {
            bestIndex = i;
            bestScore = score;
        }
    }
    score = bestScore;
    return bestIndex;
}

Candidate ShoulderDetector::getCandidate(
        const vector<Transition>& ts,
        int theta,
        float score,
        const vector<PointValue>& pvs,
        int i) {
    int i0 = ts[i].index;
    int i1 = ts[i + 1].index;
    int i2 = ts[i + 2].index;
    Point2f p0 = pvs[i0].p;
    Point2f p1 = pvs[i1].p;
    Point2f p2 = pvs[i2].p;
    float d01 = GeometryUtils::distance(p0, p1);
    float d12 = GeometryUtils::distance(p1, p2);

    Candidate c;
    c.theta = theta;
    c.score = score;
    c.center = (p0 + p1 + p2) / 3;
    c.bitSize = (d01 + d12) / 2;
    return c;
}

Candidate ShoulderDetector::averageCandidates(const vector<Candidate>& cs2x, int start, int end) {
    float averageScore = 0.0;
    int n = end - start + 1;
    for (int i = start; i <= end; i++) {
        averageScore += cs2x[i].score / n;
    }

    // compute average center, bitSize, filtering out anything with sub-average score MEDIOCRE
    int goodCount = 0;
    float goodScore = 0;
    Candidate c;
    c.theta = 0;
    c.score = 0;
    c.center = Point2f(0, 0);
    c.bitSize = 0;
    for (int i = start; i <= end; i++) {
        float score = cs2x[i].score;
        if (cs2x[i].score < averageScore) {
            continue;
        }
        goodCount++;
        goodScore += score;
        c.theta += cs2x[i].theta * score;
        c.score += score;
        c.center += cs2x[i].center * score;
        c.bitSize += cs2x[i].bitSize * score;
    }
    c.theta /= goodScore;
    c.theta = c.theta % 360;
    c.score /= goodCount;
    c.center /= goodScore;
    c.bitSize /= goodScore;
    return c;
}

void ShoulderDetector::groupCandidates(const vector<Candidate>& cs, vector<Candidate>& out) {
    int n = cs.size();
    if (n == 0) {
        return;
    }

    /*
     * To help in grouping candidates that cross the 0 / 360 edge, append a copy
     * of our candidate vector to the end.
     */
    vector<Candidate> cs2x;
    cs2x.insert(cs2x.end(), cs.begin(), cs.end());
    cs2x.insert(cs2x.end(), cs.begin(), cs.end());
    for (int i = n; i < 2 * n; i++) {
        cs2x[i].theta += 360;
    }

    bool hasRun = false;
    int cycleEnd = 2 * n;
    int start = 0, end = 0, endTheta = cs2x[0].theta;
    for (int i = 1; i < cycleEnd; i++) {
        int theta = cs2x[i].theta;
        if (theta != endTheta + THETA_STEP) {
            if (hasRun) {
                // record last run
                Candidate c = averageCandidates(cs2x, start, end);
                out.push_back(c);
            } else {
                hasRun = true;
                cycleEnd = i + n;
            }
            // new run
            start = i;
        }
        end = i;
        endTheta = theta;
    }
    if (hasRun) {
        // record last run
        Candidate c = averageCandidates(cs2x, start, end);
        out.push_back(c);
    }
}

float ShoulderDetector::getPairedScore(const Candidate& c0, const Candidate& c1) {
    float dtheta = c1.theta - c0.theta;
    if (dtheta < 0) {
        dtheta += 360;
    }
    if (dtheta < 135 || dtheta > 225) {
        return -1;
    }
    float score = c1.score;
    float dBitSize = abs(c1.bitSize - c0.bitSize);
    return score - BITSIZE_DIFFERENCE_PENALTY * dBitSize * dBitSize;
}

bool ShoulderDetector::findBestCandidates(const vector<Candidate> cs, Candidate& c0, Candidate& c1) {
    int n = cs.size();
    if (n < 2) {
        return false;
    }

    // first, find the highest-ranked candidate
    int bestIndex = -1;
    float bestScore = 0;
    for (int i = 0; i < n; i++) {
        float score = cs[i].score;
        if (score > bestScore) {
            bestIndex = i;
            bestScore = score;
        }
    }
    c0 = cs[bestIndex];

    // next, find the candidate with the highest score that has a plausible angle and similar bit size
    int pairedIndex = -1;
    float pairedScore = 0;
    for (int i = 0; i < n; i++) {
        if (i == bestIndex) {
            continue;
        }
        float score = getPairedScore(c0, cs[i]);
        if (score > pairedScore) {
            pairedIndex = i;
            pairedScore = score;
        }
    }
    if (pairedIndex == -1) {
        return false;
    }
    c1 = cs[pairedIndex];
    return true;
}

bool ShoulderDetector::readBit(const Mat& mat, const Point2f& p) {
    uchar v = mat.at<uchar>(p);
    return v <= 127;
}

void ShoulderDetector::readBitsTiming(const Mat& mat, const Candidate& c, bitset<4>& timing) {
    float thetaRad = c.theta * M_PI / 180.0;
    float cosTheta = cos(thetaRad);
    float sinTheta = sin(thetaRad);
    Point2f dCol(cosTheta, sinTheta);
    for (int i = 0; i < 4; i++) {
        float col = (i % 4) - 1.5;
        Point2f p = c.center + c.bitSize * col * dCol;
        bool b = readBit(mat, p);
        timing.set(i, b);
    }
}

void ShoulderDetector::readBitsLattice(const Mat& mat, const Candidate& c, bitset<12>& bs) {
    // draw lattice to show off
    float thetaRad = c.theta * M_PI / 180.0;
    float cosTheta = cos(thetaRad);
    float sinTheta = sin(thetaRad);
    Point2f dCol(cosTheta, sinTheta);
    Point2f dRow(sinTheta, -cosTheta);
    for (int i = 0; i < 12; i++) {
        int row = 1 - (i / 4);
        float col = (i % 4) - 1.5;
        Point2f p = c.center + c.bitSize * (col * dCol + row * dRow);
        bool b = readBit(mat, p);
        bs.set(i, b);
    }
}

void ShoulderDetector::buildFormattedCode(const bitset<12>& bs0, const bitset<12>& bs1, bitset<24>& codeFormatted) {
    bool timing = bs0[4];
    if (timing) {
        // bs0 is right, bs1 is left: reverse bs1
        cout << "L:bs1 R:bs0" << endl;
        for (int i = 0; i < 12; i++) {
            codeFormatted.set(i, bs1[11 - i]);
        }
        for (int i = 0; i < 12; i++) {
            codeFormatted.set(i + 12, bs0[i]);
        }
    } else {
        // bs1 is right, bs0 is left: reverse bs0
        cout << "L:bs0 R:bs1" << endl;
        for (int i = 0; i < 12; i++) {
            codeFormatted.set(i, bs0[11 - i]);
        }
        for (int i = 0; i < 12; i++) {
            codeFormatted.set(i + 12, bs1[i]);
        }
    }
}

bool ShoulderDetector::verifyFormattedCode(const bitset<24>& codeFormatted) {
    for (int i = 4; i < 8; i++) {
        bool expected = i % 2 == 0;
        if (codeFormatted[i] != expected) {
            return false;
        }
        if (codeFormatted[i + 12] != expected) {
            return false;
        }
    }
    return true;
}

bool ShoulderDetector::detect(
        const Mat& mat,
        const vector<Point2f>& contour,
        const Point2f centroid,
        bitset<24>& codeFormatted) {
    reset();

    /*
     * Cast multiple rays out from the centroid to the contour edges, collecting
     * candidate code locations in the process.
     */
    Point2f boundary;
    for (int theta = 0; theta < 360; theta += THETA_STEP) {
        float thetaRad = theta * M_PI / 180.0;
        if (!GeometryUtils::contourIntersection(contour, centroid, thetaRad, boundary)) {
            continue;
        }
        LineIterator it(mat, centroid, boundary);

        vector<PointValue> pvs;
        getPointValues(mat, it, pvs);

        vector<Transition> ts;
        getTransitions(pvs, ts);

        float score;
        int i = findBestTransitions(ts, score);
        if (i != -1) {
            Candidate c = getCandidate(ts, theta, score, pvs, i);
            detectionState.cs.push_back(c);
        }
    }

    // group candidates into continuous runs by , then average
    groupCandidates(detectionState.cs, detectionState.csGrouped);

    // select best candidates
    if (!findBestCandidates(detectionState.csGrouped, detectionState.c0, detectionState.c1)) {
        cout << "no best candidates!" << endl;
        return false;
    }

    // read bits!
    readBitsLattice(mat, detectionState.c0, detectionState.bs0);
    readBitsLattice(mat, detectionState.c1, detectionState.bs1);
    buildFormattedCode(detectionState.bs0, detectionState.bs1, codeFormatted);

    return verifyFormattedCode(codeFormatted);
}

void ShoulderDetector::reset() {
    detectionState.reset();
}