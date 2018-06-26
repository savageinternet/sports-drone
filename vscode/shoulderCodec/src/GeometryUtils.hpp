#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class GeometryUtils {
public:
    static bool rayIntersection(Point2f p1, Point2f p2, Point2f o, float theta, Point2f& out);
    static bool contourIntersection(const vector<Point2f>& contour, Point2f o, float theta, Point2f& out);
    static float distance(const Point2f& p0, const Point2f& p1);
};