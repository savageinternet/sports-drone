//
//  ShoulderCodecTest.cpp
//  WebcamTestTest
//
//  Created by Evan Savage on 2018-06-07.
//

#include <string>
#include <vector>

#include "catch.hpp"

#include "StringUtils.hpp"

using namespace std;

SCENARIO("StringUtilsTest", "[StringUtilsTest]" ) {
    GIVEN("StringUtils::split()") {
        vector<string> parts;
        WHEN("we split an empty string") {
            StringUtils::split("", ":", parts);
            THEN("we get a vector containing the empty string") {
                REQUIRE(parts.size() == 1);
                REQUIRE(parts[0] == "");
            }
        }
        WHEN("we split a single-character string") {
            StringUtils::split("q", ":", parts);
            THEN("we get a vector containing that string") {
                REQUIRE(parts.size() == 1);
                REQUIRE(parts[0] == "q");
            }
        }
        WHEN("we split a two-part string") {
            StringUtils::split("a:b", ":", parts);
            THEN("we get a vector containing the two parts") {
                REQUIRE(parts.size() == 2);
                REQUIRE(parts[0] == "a");
                REQUIRE(parts[1] == "b");
            }
        }
        WHEN("we split a string with the delimiter at the beginning") {
            StringUtils::split(":a:b", ":", parts);
            THEN("we get an empty string as the first part") {
                REQUIRE(parts.size() == 3);
                REQUIRE(parts[0] == "");
                REQUIRE(parts[1] == "a");
                REQUIRE(parts[2] == "b");
            }
        }
        WHEN("we split a string with the delimiter at the end") {
            StringUtils::split("a:b:", ":", parts);
            THEN("we get an empty string as the last part") {
                REQUIRE(parts.size() == 3);
                REQUIRE(parts[0] == "a");
                REQUIRE(parts[1] == "b");
                REQUIRE(parts[2] == "");
            }
        }
        WHEN("we split a string with two consecutive delimiters") {
            StringUtils::split("a::b", ":", parts);
            THEN("we get an empty string in between them") {
                REQUIRE(parts.size() == 3);
                REQUIRE(parts[0] == "a");
                REQUIRE(parts[1] == "");
                REQUIRE(parts[2] == "b");
            }
        }
        WHEN("we split a string on a multi-character delimiter") {
            StringUtils::split("a__b__c", "__", parts);
            THEN("it splits properly") {
                REQUIRE(parts.size() == 3);
                REQUIRE(parts[0] == "a");
                REQUIRE(parts[1] == "b");
                REQUIRE(parts[2] == "c");
            }
        }
        WHEN("we split a string on a multi-character delimiter [part 2]") {
            StringUtils::split("__a____b__c__", "__", parts);
            THEN("it splits properly") {
                REQUIRE(parts.size() == 6);
                REQUIRE(parts[0] == "");
                REQUIRE(parts[1] == "a");
                REQUIRE(parts[2] == "");
                REQUIRE(parts[3] == "b");
                REQUIRE(parts[4] == "c");
                REQUIRE(parts[5] == "");
            }
        }
    }
}
