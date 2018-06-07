//
//  ShoulderCodec.cpp
//  WebcamTest
//
//  Created by Evan Savage on 2018-06-06.
//

#include <bitset>
#include <stdexcept>

#include "ShoulderCodec.hpp"

using namespace std;

int ShoulderCodec::parity(int n) {
    /*
     * See https://stackoverflow.com/questions/3202745/question-from-bit-twiddling-site
     * for this little gem.
     */
    n ^= n >> 16;
    n ^= n >> 8;
    n ^= n >> 4;
    n &= 0xf;
    return (0x6996 >> n) & 1;
}

void ShoulderCodec::encode(int n, bitset<16>& code) const {
    if (n < VALUE_MIN) {
        throw out_of_range("value too small");
    } else if (n > VALUE_MAX) {
        throw out_of_range("value too large");
    }
    const bitset<NUM_DATA_BITS> b(n);
    int i = 0;
    
    const int bL = b[0] | (b[2] << 1) | (b[5] << 3);
    code.set(i++, parity(bL & 0xb));
    code.set(i++, parity(bL & 0xd));
    code.set(i++, b[0]);
    code.set(i++, parity(bL & 0xe));
    code.set(i++, b[2]);
    code.set(i++, 0);
    code.set(i++, b[5]);
    code.set(i++, parity(bL & 0x7));
    
    const int bR = b[1] | (b[3] << 1) | (1 << 2) | (b[4] << 3);
    code.set(i++, parity(bR & 0xb));
    code.set(i++, parity(bR & 0xd));
    code.set(i++, b[1]);
    code.set(i++, parity(bR & 0xe));
    code.set(i++, b[3]);
    code.set(i++, 1);
    code.set(i++, b[4]);
    code.set(i++, parity(bR & 0x7));
}

int ShoulderCodec::decode(bitset<16>& code) const {
    const int b = static_cast<int>(code.to_ulong());
    const int bL = b & 0xff;
    const int bR = b >> 8;
    
    int sL = parity(bL & 0x55);
    sL |= parity(bL & 0x66) << 1;
    sL |= parity(bL & 0x78) << 2;
    sL |= parity(bL) << 3;
    if (sL != 0) {
        if (sL < 8) {
            throw invalid_argument("left: double error, cannot correct");
        }
        code.flip(sL - 9);
    }
    
    int sR = parity(bR & 0x55);
    sR |= parity(bR & 0x66) << 1;
    sR |= parity(bR & 0x78) << 2;
    sR |= parity(bR) << 3;
    if (sR != 0) {
        if (sR < 8) {
            throw invalid_argument("right: double error, cannot correct");
        }
        code.flip(sR - 1);
    }
    
    return code[2] | (code[10] << 1) | (code[4] << 2) | (code[12] << 3) | (code[14] << 4) | (code[6] << 5);
}
