//
//  ShoulderCodecTest.cpp
//  WebcamTestTest
//
//  Created by Evan Savage on 2018-06-07.
//

#include <bitset>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "catch.hpp"

#include "ShoulderCodec.hpp"

using namespace std;

SCENARIO("ShoulderCodecTest", "[ShoulderCodecTest]" ) {
    GIVEN("A ShoulderCodec instance and bitset of size 16") {
        ShoulderCodec codec;
        bitset<16> code;
        
        WHEN("we encode each valid number into the bitset, then decode") {
            THEN("we get the same number back") {
                for (int i = 0; i < 64; i++) {
                    codec.encode(i, code);
                    REQUIRE(codec.decode(code) == i);
                }
            }
        }
        
        WHEN("we decode codes with a single error") {
            THEN("the error is corrected properly") {
                for (int i = 0; i < 64; i++) {
                    for (int j = 0; j < 16; j++) {
                        codec.encode(i, code);
                        code.flip(j);
                        REQUIRE(codec.decode(code) == i);
                    }
                }
            }
        }
    }
}
