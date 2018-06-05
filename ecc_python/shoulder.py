"""
shoulder.py

XDRDP  EDEEX    (R = 0)
XEEDE  PDRDX    (R = 1)

The proposed code uses two 5 x 2 shoulders as shown above.
The outer "X" is a "finder pattern" consisting of an outer black column.
The remainder of each shoulder is a Hamming (7, 4) code plus parity bit ("P").
This means that each shoulder can be decoded separately, and that we can
detect up to two errors.

In the Hamming code, "E" denotes error-correction bits, "D" denotes data bits.
The bits are laid out as follows:

X456P  3210X
X0123  P654X

This means that the error-correction bits are bits 0, 1, and 3, which is the
0-indexed equivalent of the 1-indexed binary presentation of Hamming codes.

In each shoulder, "R" (bit 6) is a special data bit called the row bit.
This is used to identify the row.

This row bit helps us decode rotated codes.  Note that the code layout
is rotationally symmetric, so that the detector knows how to interpret
bits regardless of orientation.

Bits are encoded as solid-color squares at least 2 cm on a side,
either white (0) or black (1).  The code may be repeated by alternating
the two rows over a larger area.  For decoding, the detector needs only
to find two consecutive rows.

These codes can therefore encode 6 bits worth of actual data -
i.e. values from 0 to 63.  We store bits 0, 2, 5 in the left shoulder and
1, 3, 4 in the right shoulder, so that player codes in 1-2-1 scenarios are
more likely to visibly differ.

Note that there are 15 values that result in one of the two degenerate
patterns "10000" or "11111":

0
2
8
10
16
18
24
26
27
30
31
58
59
62
63

This leaves 48 codes that have reasonable detection characteristics,
still more than enough for most games.  Non-degenerate values should
be preferred over degenerate values where possible.   (We may map the
range 0-47 onto the range 0-63 in future versions of the library.)
"""

import argparse
import numpy as np
import sys

G = np.matrix([
    [1, 1, 0, 1],
    [1, 0, 1, 1],
    [1, 0, 0, 0],
    [0, 1, 1, 1],
    [0, 1, 0, 0],
    [0, 0, 1, 0],
    [0, 0, 0, 1],
    [1, 1, 1, 0]
])

H = np.matrix([
    [1, 0, 1, 0, 1, 0, 1, 0],
    [0, 1, 1, 0, 0, 1, 1, 0],
    [0, 0, 0, 1, 1, 1, 1, 0],
    [1, 1, 1, 1, 1, 1, 1, 1]
])

MODE_DECODE = 'decode'
MODE_ENCODE = 'encode'
NUM_DATA_BITS = 6
NUM_EC_BITS = 3
VALUE_MIN = 0
VALUE_MAX = 2 ** NUM_DATA_BITS - 1
ROW_LEFT = 0
ROW_RIGHT = 1


def parseArgs():
    parser = argparse.ArgumentParser(
        description='Shoulder codec for Sports Drone project')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument(
        '--decode', dest='mode', action='store_const', const=MODE_DECODE)
    group.add_argument(
        '--encode', dest='mode', action='store_const', const=MODE_ENCODE)
    parser.add_argument(
        '--infile', nargs='?', type=argparse.FileType('r'),
        default=sys.stdin)
    parser.add_argument(
        '--outfile', nargs='?', type=argparse.FileType('w'),
        default=sys.stdout)
    return parser.parse_args()


def fromBits(bits):
    return sum(2 ** i for (i, b) in enumerate(bits) if b == 1)


def toBits(n, k):
    bits = []
    for i in range(k):
        mask = 1 << i
        if n & mask:
            bits.append(1)
        else:
            bits.append(0)
    return bits


def columnVector(rowVector):
    rowVector.shape = (rowVector.size, 1)
    return rowVector


def rowVector(columnVector):
    return np.squeeze(np.asarray(columnVector))


def deserializeShoulderRow(row):
    bits = []
    for c in row:
        if c == '0':
            bits.append(0)
        elif c == '1':
            bits.append(1)
        else:
            raise ValueError
    return bits


def serializeShoulderRow(row):
    return ''.join(map(str, row))


def deserialize(code_rows):
    if len(code_rows) != 2:
        raise ValueError
    if len(code_rows[0]) != 11 or len(code_rows[1]) != 11:
        raise ValueError
    if code_rows[0][5] != ' ' or code_rows[1][5] != ' ':
        raise ValueError
    shoulder_left = [
        deserializeShoulderRow(code_rows[0][:5]),
        deserializeShoulderRow(code_rows[1][:5])
    ]
    shoulder_right = [
        deserializeShoulderRow(code_rows[0][6:]),
        deserializeShoulderRow(code_rows[1][6:])
    ]
    return shoulder_left, shoulder_right


def serialize(shoulder_left, shoulder_right):
    """
    Serializes two 5 x 2 shoulder bit matrices as per the specification:

    XDRDP EDEEX
    XEEDE PDRDX

    The output is two 11-character rows consisting of the two 5 x 2 shoulders
    separated by a space.  Each bit is represented by "0" or "1".
    """
    return '{0} {1}\n{2} {3}'.format(
        serializeShoulderRow(shoulder_left[0]),
        serializeShoulderRow(shoulder_right[0]),
        serializeShoulderRow(shoulder_left[1]),
        serializeShoulderRow(shoulder_right[1]))


def decode(shoulder_left, shoulder_right):
    """
    XDRDP EDEEX    (R = 0)
    XEEDE PDRDX    (R = 1)

    X456P 3210X
    X0123 P654X
    """
    # check finder patterns
    if shoulder_left[0][0] != 1 or shoulder_left[1][0] != 1:
        raise ValueError('finder pattern missing in left shoulder')
    if shoulder_right[0][4] != 1 or shoulder_right[1][4] != 1:
        raise ValueError('finder pattern missing in right shoulder')

    code_left = np.array([
        shoulder_left[1][1],
        shoulder_left[1][2],
        shoulder_left[1][3],
        shoulder_left[1][4],
        shoulder_left[0][1],
        shoulder_left[0][2],
        shoulder_left[0][3],
        shoulder_left[0][4]
    ])
    code_left = columnVector(code_left)
    syndrome_left = H * code_left
    syndrome_left = rowVector(syndrome_left) % 2
    syndrome_left = fromBits(syndrome_left)
    if syndrome_left != 0:
        if syndrome_left < 8:
            raise ValueError('double error detected in left shoulder')
        syndrome_left -= 8
        code_left[syndrome_left - 1] = 1 - code_left[syndrome_left - 1]
    if code_left[5] != ROW_LEFT:
        raise ValueError('row bit error in left shoulder')

    code_right = np.array([
        shoulder_right[0][3],
        shoulder_right[0][2],
        shoulder_right[0][1],
        shoulder_right[0][0],
        shoulder_right[1][3],
        shoulder_right[1][2],
        shoulder_right[1][1],
        shoulder_right[1][0]
    ])
    code_right = columnVector(code_right)
    syndrome_right = H * code_right
    syndrome_right = rowVector(syndrome_right) % 2
    syndrome_right = fromBits(syndrome_right)
    if syndrome_right != 0:
        if syndrome_right < 8:
            raise ValueError('double error detected in right shoulder')
        syndrome_right -= 8
        code_right[syndrome_right - 1] = 1 - code_right[syndrome_right - 1]
    if code_right[5] != ROW_RIGHT:
        raise ValueError('row bit error in right shoulder')

    bits = [
        code_left[2],
        code_right[2],
        code_left[4],
        code_right[4],
        code_right[6],
        code_left[6]
    ]
    return fromBits(bits)


def decodeFile(infile, outfile):
    code_rows = []
    for line in infile:
        line = line.strip()
        if not line:
            continue
        try:
            code_rows.append(line)
            if len(code_rows) == 2:
                shoulder_left, shoulder_right = deserialize(code_rows)
                n = decode(shoulder_left, shoulder_right)
                print(n, file=outfile)
                code_rows = []
        except ValueError as e:
            print(e, code_rows, file=sys.stderr)
            continue


def encode(n):
    """
    Encodes a value 0-63 as per the specification:

    XDRDP  EDEEX
    XEEDE  PDRDX

    The output is two 5 x 2 shoulder bit matrices, to be passed to serialize().
    """
    if n < VALUE_MIN or n > VALUE_MAX:
        raise ValueError
    bits = toBits(n, NUM_DATA_BITS)

    # shoulder 1 (left)
    bits_left = np.array([bits[0], bits[2], ROW_LEFT, bits[5]])
    bits_left = columnVector(bits_left)
    code_left = G * bits_left
    code_left = rowVector(code_left) % 2
    shoulder_left = [
        [1, code_left[4], code_left[5], code_left[6], code_left[7]],
        [1, code_left[0], code_left[1], code_left[2], code_left[3]]
    ]

    # shoulder 2 (right)
    bits_right = np.array([bits[1], bits[3], ROW_RIGHT, bits[4]])
    bits_right = columnVector(bits_right)
    code_right = G * bits_right
    code_right = rowVector(code_right) % 2
    shoulder_right = [
        [code_right[3], code_right[2], code_right[1], code_right[0], 1],
        [code_right[7], code_right[6], code_right[5], code_right[4], 1],
    ]

    return shoulder_left, shoulder_right


def encodeFile(infile, outfile):
    for line in infile:
        line = line.strip()
        if not line:
            continue
        try:
            n = int(line)
            shoulder_left, shoulder_right = encode(n)
            code = serialize(shoulder_left, shoulder_right)
            print(code, file=outfile)
        except ValueError:
            print('invalid value', line, file=sys.stderr)
            continue


def main():
    args = parseArgs()
    if args.mode == MODE_DECODE:
        decodeFile(args.infile, args.outfile)
    else:
        encodeFile(args.infile, args.outfile)


if __name__ == '__main__':
    main()
