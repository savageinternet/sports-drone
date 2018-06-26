//
//  ShoulderCodecTest.cpp
//  WebcamTestTest
//
//  Created by Evan Savage on 2018-06-07.
//

#include <bitset>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "catch.hpp"

#include "ShoulderCodec.hpp"

using namespace std;

SCENARIO("ShoulderCodecTest", "[ShoulderCodecTest]" ) {
    GIVEN("A ShoulderCodec instance and bitset of size 16") {
        bitset<16> code;
        bitset<24> codeFormatted;
        
        WHEN("we encode each valid number into the bitset, then decode") {
            THEN("we get the same number back") {
                for (int i = 0; i < ShoulderCodec::VALUE_MAX; i++) {
                    ShoulderCodec::encode(i, code);
                    REQUIRE(ShoulderCodec::decode(code) == i);
                }
            }
        }

        WHEN("we encode each valid number into the bitset, then format, then unformat, then decode") {
            THEN("we get the same number back") {
                for (int i = 0; i < ShoulderCodec::VALUE_MAX; i++) {
                    ShoulderCodec::encode(i, code);
                    ShoulderCodec::format(code, codeFormatted);
                    ShoulderCodec::unformat(codeFormatted, code);
                    REQUIRE(ShoulderCodec::decode(code) == i);
                }
            }
        }
        
        WHEN("we decode codes with a single error") {
            THEN("the error is corrected properly") {
                for (int i = 0; i < ShoulderCodec::VALUE_MAX; i++) {
                    for (int j = 0; j < 16; j++) {
                        ShoulderCodec::encode(i, code);
                        code.flip(j);
                        REQUIRE(ShoulderCodec::decode(code) == i);
                    }
                }
            }
        }
        
        WHEN("we decode codes with a single error in each shoulder") {
            srand(static_cast<unsigned int>(time(NULL)));
            THEN("the error is corrected properly") {
                for (int i = 0; i < ShoulderCodec::VALUE_MAX; i++) {
                    ShoulderCodec::encode(i, code);
                    int j = rand() % 8;
                    int k = rand() % 8 + 8;
                    code.flip(j);
                    code.flip(k);
                    REQUIRE(ShoulderCodec::decode(code) == i);
                }
            }
        }
        
        WHEN("we decode codes with two errors in the left shoulder") {
            srand(static_cast<unsigned int>(time(NULL)));
            THEN("decode() returns -1") {
                for (int i = 0; i < ShoulderCodec::VALUE_MAX; i++) {
                    ShoulderCodec::encode(i, code);
                    int j = rand() % 8;
                    int k;
                    do {
                        k = rand() % 8;
                    } while (k == j);
                    code.flip(j);
                    code.flip(k);
                    REQUIRE(ShoulderCodec::decode(code) == -1);
                }
            }
        }
        
        WHEN("we decode codes with two errors in the right shoulder") {
            srand(static_cast<unsigned int>(time(NULL)));
            THEN("decode() returns -1") {
                for (int i = 0; i < ShoulderCodec::VALUE_MAX; i++) {
                    ShoulderCodec::encode(i, code);
                    int j = rand() % 8 + 8;
                    int k;
                    do {
                        k = rand() % 8 + 8;
                    } while (k == j);
                    code.flip(j);
                    code.flip(k);
                    REQUIRE(ShoulderCodec::decode(code) == -1);
                }
            }
        }
    }
}
