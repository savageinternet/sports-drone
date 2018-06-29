#include <bitset>
#include <iostream>

#include "ShoulderTypes.hpp"
#include "StringUtils.hpp"

using namespace cv;
using namespace std;

PointValue::PointValue(Point2f p, uchar v) : p(p), v(v) {}

Transition::Transition(int index, float strength) : index(index), strength(strength) {}

void DetectionState::reset() {
    cs.clear();
    csGrouped.clear();
    c0 = Candidate();
    c1 = Candidate();
    bs0.reset();
    bs1.reset();
}

ostream& operator<<(ostream& os, const PointValue& pv) {
    os << "{p: " << pv.p << ", v: " << pv.v << "}";
    return os;
}

ostream& operator<<(ostream& os, const Transition& t) {
    os << "{index: " << t.index << ", strength: " << t.strength << "}";
    return os;
}

ostream& operator<<(ostream& os, const Candidate& c) {
    os << "{theta: " << c.theta <<
        ", score: " << c.score <<
        ", center: " << c.center <<
        ", bitSize: " << c.bitSize << "}";
    return os;
}

ostream& operator<<(ostream& os, const DetectionState& ds) {
    os << "candidates: ";
    StringUtils::stringify(os, ds.cs);
    os << "grouped candidates: ";
    StringUtils::stringify(os, ds.csGrouped);
    os << "best candidates: (" << ds.c0 << ", " << ds.c1 << ")\n";
    os << "detected bits: (" << ds.bs0 << ", " << ds.bs1 << ")\n";
    return os;
}
