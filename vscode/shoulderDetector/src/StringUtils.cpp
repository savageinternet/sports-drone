#include "StringUtils.hpp"

#include <string>
#include <vector>

using namespace std;

void StringUtils::split(const string& str, const string& delim, vector<string>& out) {
    out.clear();
    size_t sizeDelim = delim.size();
    size_t last = 0;
    size_t cur = 0;
    while (cur != string::npos) {
        cur = str.find(delim, last);
        if (cur != string::npos) {
            out.push_back(str.substr(last, cur - last));
            last = cur + sizeDelim;
        }
    }
    out.push_back(str.substr(last));
}