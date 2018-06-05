//
//  ThresholdBinarizer.cpp
//  WebcamTest
//
//  Created by Evan Savage on 2018-06-01.
//

#include <zxing/Binarizer.h>
#include <zxing/common/BitArray.h>
#include <zxing/common/BitMatrix.h>

#include "ThresholdBinarizer.hpp"

using namespace zxing;

ThresholdBinarizer::ThresholdBinarizer(Ref<LuminanceSource> source, int blackPoint) :
Binarizer(source),
_blackPoint(blackPoint) {}

ThresholdBinarizer::~ThresholdBinarizer() {}

Ref<BitArray> ThresholdBinarizer::getBlackRow(int y, Ref<BitArray> row) {
    LuminanceSource& source = *getLuminanceSource();
    int width = source.getWidth();
    if (row == NULL || static_cast<int>(row->getSize()) < width) {
        row = new BitArray(width);
    } else {
        row->clear();
    }
    
    ArrayRef<char> localLuminances = source.getRow(y, luminances);
    for (int x = 0; x < width; x++) {
        int luminance = localLuminances[x] & 0xff;
        if (luminance < _blackPoint) {
            row->set(x);
        }
    }
    return row;
}

Ref<BitMatrix> ThresholdBinarizer::getBlackMatrix() {
    LuminanceSource& source = *getLuminanceSource();
    int width = source.getWidth();
    int height = source.getHeight();
    Ref<BitMatrix> matrix(new BitMatrix(width, height));
    ArrayRef<char> localLuminances = source.getMatrix();
    int i = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pixel = localLuminances[i] & 0xff;
            if (pixel < _blackPoint) {
                matrix->set(x, y);
            }
            i++;
        }
    }
    
    return matrix;
}

Ref<Binarizer> ThresholdBinarizer::createBinarizer(Ref<LuminanceSource> source) {
    return Ref<Binarizer>(new ThresholdBinarizer(source));
}
