#pragma once

#include <string>
#include <vector>

using namespace std;

class StringUtils {
public:
    static void split(const string& str, const string& delim, vector<string>& out);
};