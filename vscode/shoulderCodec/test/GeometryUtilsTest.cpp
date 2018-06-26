#include "catch.hpp"

#include "GeometryUtils.hpp"

using namespace cv;
using namespace std;

SCENARIO("GeometryUtilsTest", "[GeometryUtilsTest]") {
    GIVEN("GeometryUtils::distance") {
        WHEN("we take the distance from (1, 2) to (4, 6)") {
            THEN("we get exactly 5") {
                Point2f p0(1, 2);
                Point2f p1(4, 6);
                REQUIRE(GeometryUtils::distance(p0, p1) == 5);
            }
        }
    }
}