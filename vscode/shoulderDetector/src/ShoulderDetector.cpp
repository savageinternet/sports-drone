#include <bitset>
#include <iostream>
#include <vector>

#include "ofMain.h"
#include "ofxCv.h"

#include "ShoulderDetector.hpp"

using namespace cv;
using namespace ofxCv;
using namespace std;

ostream& operator<<(ostream& os, const Transition& t) {
    os << "(" << t.index << ", " << t.strength << ")";
    return os;
}

ostream& operator<<(ostream& os, const Candidate& c) {
    os << "{theta: " << c.theta <<
        ", score: " << c.score <<
        ", center: " << c.center <<
        ", bitSize: " << c.bitSize << "}";
    return os;
}

// see https://stackoverflow.com/questions/14307158/how-do-you-check-for-intersection-between-a-line-segment-and-a-line-ray-emanatin
bool ShoulderDetector::rayIntersection(Point2f p1, Point2f p2, Point2f o, float theta, Point2f& out) {
    Point2f r(cos(theta), sin(theta));
    Point2f s = p2 - p1;
    Point2f q = p1 - o;

    float qxr = q.x * r.y - q.y * r.x;
    float rxs = r.x * s.y - r.y * s.x;
    if (abs(rxs) < 1e-6) {
        return false;
    }
    float u = qxr / rxs;
    if (u < 0 || u > 1) {
        return false;
    }
    float qxs = q.x * s.y - q.y * s.x;
    float t = qxs / rxs;
    if (t < 0) {
        return false;
    }
    out = p1 + u * s;
    return true;
}

bool ShoulderDetector::contourIntersection(const vector<Point2f>& contour, Point2f o, float theta, Point2f& out) {
    int n = contour.size();
    Point2f p1 = contour[0];
    Point2f p2;
    for (int i = 1; i < n; i++) {
        p2 = contour[i];
        if (rayIntersection(p1, p2, o, theta, out)) {
            return true;
        }
        p1 = p2;
    }
    p2 = contour[0];
    return rayIntersection(p1, p2, o, theta, out);
}

template<typename T> void ShoulderDetector::printVector(vector<T> v) {
    int n = v.size();
    cout << "[\n";
    for (int i = 0; i < n; i++) {
        cout << "  " << v.at(i);
        if (i == n - 1) {
            cout << "\n]";
        } else {
            cout << ",\n";
        }
    }
}

float ShoulderDetector::scoreTransitions(const vector<Transition>& ts, int i) {
    int di10 = ts[i + 1].index - ts[i].index;
    int di21 = ts[i + 2].index - ts[i + 1].index;
    int di = abs(di21 - di10);
    float k = 20;
    float s = ts[i].strength + ts[i + 1].strength + ts[i + 2].strength;
    return s - k * di * di;
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

float ShoulderDetector::distance(const Point2f& p0, const Point2f& p1) {
    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    return sqrt(dx * dx + dy * dy);
}

Candidate ShoulderDetector::getCandidate(const vector<Transition>& ts, int theta, float score, const vector<Point2f> ps, int i) {
    Candidate c;
    c.theta = theta;
    c.score = score;
    int i0 = ts[i].index;
    int i1 = ts[i + 1].index;
    int i2 = ts[i + 2].index;
    c.center = (ps[i0] + ps[i1] + ps[i2]) / 3;
    c.bitSize = (distance(ps[i0], ps[i1]) + distance(ps[i1], ps[i2])) / 2;
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
        if (theta != endTheta + 5) {
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
    float k = 20;
    return score - dBitSize * dBitSize * k;
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

void ShoulderDetector::readBitLattice(const Mat& mat, const Candidate& c, bitset<12>& bs) {
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
        //ofSetColor(ofColor(255, 0, 0));
        //ofDrawBitmapString(b ? "1" : "0", p.x, p.y);
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

bool ShoulderDetector::verify(const bitset<24>& codeFormatted) {
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
        const ofPixels& pixels,
        const ofPolyline& ofContour,
        bitset<24>& codeFormatted) {
    ofVec2f centroid = ofContour.getCentroid2D();
    
    Mat mat = toCv(pixels);
    vector<Point2f> contour = toCv(ofContour);
    Point2f p1 = toCv(centroid);
    Point2f p2;

    /*
     * Cast multiple rays out from the centroid to the contour edges.
     */
    vector<Candidate> candidates;
    for (int theta = 0; theta < 360; theta += 5) {
        float thetaRad = theta * M_PI / 180.0;
        if (!contourIntersection(contour, p1, thetaRad, p2)) {
            // TODO: now what?  this shouldn't happen...
            continue;
        }
        LineIterator it(mat, p1, p2);
        int n = it.count;
        vector<Point2f> ps;
        vector<uchar> vs;
        for (int i = 0; i < n; i++, ++it) {
            Point2f p = it.pos();
            uchar v = mat.at<uchar>(p);
            ps.push_back(p);
            vs.push_back(v);
        }

        float threshold = 127;
        float alpha = 0.3;
        bool inTransition = false;
        vector<Transition> transitions;
        int muStart = 0;
        for (int i = 0; i < 4; i++) {
            muStart += vs[i];
        }
        float mu = muStart / 4;
        for (int i = 4; i < n; i++) {
            float dv = vs[i] - mu;
            mu += alpha * dv;
            dv = abs(dv);
            if (dv >= threshold) {
                if (!inTransition) {
                    Transition t;
                    t.index = i;
                    t.strength = dv;
                    transitions.push_back(t);
                }
                inTransition = true;
            } else {
                inTransition = false;
            }
        }
        float score;
        int i = findBestTransitions(transitions, score);
        if (i != -1) {
            Candidate c = getCandidate(transitions, theta, score, ps, i);
            candidates.push_back(c);
        }
    }
    
    cout << "candidates: ";
    printVector(candidates);
    cout << endl;

    vector<Candidate> groupedCandidates;
    groupCandidates(candidates, groupedCandidates);

    cout << "groups: ";
    printVector(groupedCandidates);
    cout << endl;

    // select best candidates
    Candidate c0, c1;
    if (!findBestCandidates(groupedCandidates, c0, c1)) {
        cout << "no best candidates!" << endl;
        return false;
    }
    cout << c0 << ", " << c1 << endl;

    // read bits!
    bitset<12> bs0;
    bitset<12> bs1;
    readBitLattice(mat, c0, bs0);
    readBitLattice(mat, c1, bs1);
    buildFormattedCode(bs0, bs1, codeFormatted);

    cout << "bs0: " << bs0 << endl;
    cout << "bs1: " << bs1 << endl;
    cout << codeFormatted << endl;

    // TODO: verify that timing row is intact
    return verify(codeFormatted);
}