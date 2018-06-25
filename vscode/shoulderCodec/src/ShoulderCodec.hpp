//
//  ShoulderCodec.hpp
//  WebcamTest
//
//  Created by Evan Savage on 2018-06-06.
//

#ifndef ShoulderCodec_hpp
#define ShoulderCodec_hpp

#include <bitset>
#include <iostream>
#include <string>

using namespace std;

class ShoulderCodec {
private:
    static int parity(int n);
public:
    static const int NUM_DATA_BITS = 8;
    static const int VALUE_MIN = 0;
    static const int VALUE_MAX = 1 << NUM_DATA_BITS;
    
    void encode(int n, bitset<16>& code) const;
    void format(const bitset<16>& code, bitset<24>& codeFormatted) const;
    void print(ostream& os, const bitset<24>& codeFormatted) const;
    void unformat(const bitset<24>& codeFormatted, bitset<16>& code) const;
    int decode(bitset<16>& code) const;
};

#endif /* ShoulderCodec_hpp */
