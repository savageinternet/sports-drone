#include <vector>

#include <opencv2/opencv.hpp>

#include "GeometryUtils.hpp"

using namespace cv;
using namespace std;

// see https://stackoverflow.com/questions/14307158/how-do-you-check-for-intersection-between-a-line-segment-and-a-line-ray-emanatin
bool GeometryUtils::rayIntersection(Point2f p1, Point2f p2, Point2f o, float theta, Point2f& out) {
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

bool GeometryUtils::contourIntersection(const vector<Point2f>& contour, Point2f o, float theta, Point2f& out) {
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

float GeometryUtils::distance(const Point2f& p0, const Point2f& p1) {
    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    return sqrt(dx * dx + dy * dy);
}