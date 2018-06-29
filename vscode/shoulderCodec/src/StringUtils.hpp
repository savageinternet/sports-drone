#pragma once

#include <string>
#include <vector>

#include "ShoulderTypes.hpp"

using namespace std;

class StringUtils {
public:
    static void split(const string& str, const string& delim, vector<string>& out);
    template<typename T> static ostream& stringify(ostream& os, vector<T> v);
    template<typename T> static string stringify(vector<T> v);
};

template<typename T> ostream& StringUtils::stringify(ostream& os, vector<T> v) {
    int n = v.size();
    if (n == 0) {
        os << "[]";
    } else {
        os << "[\n";
        for (int i = 0; i < n - 1; i++) {
            os << "  " << v.at(i) << ",\n";
        }
        os << "  " << v.at(n - 1) << "\n]";
    }
    return os;
}

template<typename T> string StringUtils::stringify(vector<T> v) {
    ostringstream oss;
    stringify(oss, v);
    return oss.str();
}

template ostream& StringUtils::stringify<>(ostream&, vector<PointValue>);
template ostream& StringUtils::stringify<>(ostream&, vector<Transition>);
template ostream& StringUtils::stringify<>(ostream&, vector<Candidate>);
template ostream& StringUtils::stringify<>(ostream&, vector<DetectionState>);

template string StringUtils::stringify<>(vector<PointValue>);
template string StringUtils::stringify<>(vector<Transition>);
template string StringUtils::stringify<>(vector<Candidate>);
template string StringUtils::stringify<>(vector<DetectionState>);