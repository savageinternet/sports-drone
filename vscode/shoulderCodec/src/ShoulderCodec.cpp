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

void ShoulderCodec::encode(int n, bitset<16>& code) {
    if (n < VALUE_MIN) {
        throw out_of_range("value too small");
    } else if (n > VALUE_MAX) {
        throw out_of_range("value too large");
    }
    const bitset<NUM_DATA_BITS> b(n);
    
    code.reset();
    int i = 0;
    
    /*
     * This next section splits the 6 bits of n into left and right shoulders,
     * adds row bits, and performs mod-2 multiplication by the generator matrix.
     *
     * We store the left shoulder in bits 0-7, the right in bits 8-15.  For
     * simplicity, we do not reorder the bits into their display order.  This
     * makes the math easier and avoids unnecessary copying.
     */
    const int bL = b[0] | (b[3] << 1) | (b[4] << 2) | (b[7] << 3);
    code.set(i++, parity(bL & 0xb));
    code.set(i++, parity(bL & 0xd));
    code.set(i++, b[0]);
    code.set(i++, parity(bL & 0xe));
    code.set(i++, b[3]);
    code.set(i++, b[4]);
    code.set(i++, b[7]);
    code.set(i++, parity(bL & 0x7));
    
    const int bR = b[1] | (b[2] << 1) | (b[5] << 2) | (b[6] << 3);
    code.set(i++, parity(bR & 0xb));
    code.set(i++, parity(bR & 0xd));
    code.set(i++, b[1]);
    code.set(i++, parity(bR & 0xe));
    code.set(i++, b[2]);
    code.set(i++, b[5]);
    code.set(i++, b[6]);
    code.set(i++, parity(bR & 0x7));
}

void ShoulderCodec::format(const bitset<16>& code, bitset<24>& codeFormatted) {
    codeFormatted.reset();
    int i = 0;
    // left shoulder
    for (int j = 4; j < 8; j++) {
        codeFormatted.set(i++, code[j]);
    }
    for (int j = 0; j < 4; j++) {
        codeFormatted.set(i++, j % 2 == 0);
    }
    for (int j = 0; j < 4; j++) {
        codeFormatted.set(i++, code[j]);
    }
    // right shoulder
    for (int j = 11; j >= 8; j--) {
        codeFormatted.set(i++, code[j]);
    }
    for (int j = 0; j < 4; j++) {
        codeFormatted.set(i++, j % 2 == 0);
    }
    for (int j = 15; j >= 12; j--) {
        codeFormatted.set(i++, code[j]);
    }
}

void ShoulderCodec::print(ostream& os, const bitset<24>& codeFormatted) {
    for (int i = 0; i < 4; i++) {
        os << (codeFormatted[i] ? "1" : "0");
    }
    os << ' ';
    for (int i = 12; i < 16; i++) {
        os << (codeFormatted[i] ? "1" : "0");
    }
    os << '\n';
    for (int i = 4; i < 8; i++) {
        os << (codeFormatted[i] ? "1" : "0");
    }
    os << ' ';
    for (int i = 16; i < 20; i++) {
        os << (codeFormatted[i] ? "1" : "0");
    }
    os << '\n';
    for (int i = 8; i < 12; i++) {
        os << (codeFormatted[i] ? "1" : "0");
    }
    os << ' ';
    for (int i = 20; i < 24; i++) {
        os << (codeFormatted[i] ? "1" : "0");
    }
    os << '\n';
}

void ShoulderCodec::unformat(const bitset<24>& codeFormatted, bitset<16>& code) {
    code.reset();
    int i = 0;
    // left shoulder
    for (int j = 4; j < 8; j++) {
        code.set(j, codeFormatted[i++]);
    }
    i += 4;
    for (int j = 0; j < 4; j++) {
        code.set(j, codeFormatted[i++]);
    }
    // right shoulder
    for (int j = 11; j >= 8; j--) {
        code.set(j, codeFormatted[i++]);
    }
    i += 4;
    for (int j = 15; j >= 12; j--) {
        code.set(j, codeFormatted[i++]);
    }
}

int ShoulderCodec::decode(bitset<16>& code) {
    /*
     * Split the code into left and right parts, in preparation for
     * multiplying by the parity-check matrix.
     */
    const int b = static_cast<int>(code.to_ulong());
    const int bL = b & 0xff;
    const int bR = b >> 8;
    
    /*
     * Compute the left and right syndromes, and correct errors if possible.
     * If a double error is detected, return -1.
     */
    int sL = parity(bL & 0x55);
    sL |= parity(bL & 0x66) << 1;
    sL |= parity(bL & 0x78) << 2;
    sL |= parity(bL) << 3;
    if (sL != 0) {
        if (sL < 8) {
            return -1;
        } else if (sL == 8) {
            code.flip(7);
        } else {
            code.flip(sL - 9);
        }
    }
    
    int sR = parity(bR & 0x55);
    sR |= parity(bR & 0x66) << 1;
    sR |= parity(bR & 0x78) << 2;
    sR |= parity(bR) << 3;
    if (sR != 0) {
        if (sR < 8) {
            return -1;
        } else if (sR == 8) {
            code.flip(15);
        } else {
            code.flip(sR - 1);
        }
    }
    
    return code[2] | (code[10] << 1) | (code[12] << 2) | (code[4] << 3) |
           (code[5] << 4) | (code[13] << 5) | (code[14] << 6) | (code[6] << 7);
}
