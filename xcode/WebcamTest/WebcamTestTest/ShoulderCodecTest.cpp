//
//  ShoulderCodecTest.cpp
//  WebcamTestTest
//
//  Created by Evan Savage on 2018-06-07.
//

#include <bitset>

#include "catch.hpp"

#include "ShoulderCodec.hpp"

using namespace std;

TEST_CASE("ShoulderCodecTest", "[ShoulderCodecTest]" ) {
    GIVEN("A ShoulderCodec instance and bitset of size 16") {
        ShoulderCodec codec;
        bitset<16> code;
        
        WHEN("we encode each valid number into the bitset, then decode") {
            THEN("we get the same number back back") {
                for (int i = 0; i < 64; i++) {
                    codec.encode(i, code);
                    REQUIRE(codec.decode(code) == i);
                }
            }
        }
    }
}
