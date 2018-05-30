//
//  OfxLuminanceSource.cpp
//  WebcamTest
//
//  Created by Evan Savage on 2018-05-30.
//

#include "ofxOpenCv.h"

#include "OfxLuminanceSource.hpp"

using namespace std;
using namespace zxing;

OfxLuminanceSource::OfxLuminanceSource(const ofPixels& pixels) :
LuminanceSource(pixels.getWidth(), pixels.getHeight()),
_pixels(pixels) {}

OfxLuminanceSource::OfxLuminanceSource(const ofxCvGrayscaleImage& image) :
LuminanceSource(image.width, image.height),
_pixels(image.getPixels()) {}

OfxLuminanceSource::~OfxLuminanceSource() {}

ArrayRef<char> OfxLuminanceSource::getRow(int y, ArrayRef<char> row) const {
    int width = getWidth();
    if (!row) {
        row = ArrayRef<char>(new char[width], width);
    }
    for (int x = 0; x < width; x++) {
        int i = _pixels.getPixelIndex(x, y);
        row[x] = _pixels[i];
        i++;
    }
    return row;
}

ArrayRef<char> OfxLuminanceSource::getMatrix() const {
    int height = getHeight();
    int width = getWidth();
    int n = height * width;
    char* matrix = new char[n];
    char* p = matrix;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = _pixels.getPixelIndex(x, y);
            *p = _pixels[i];
            p++;
        }
    }
    return ArrayRef<char>(matrix, n);
}

bool OfxLuminanceSource::isCropSupported() const {
    return true;
}

Ref<LuminanceSource> OfxLuminanceSource::crop(int left, int top, int width, int height) const {
    ofPixels pixelsCrop;
    _pixels.cropTo(pixelsCrop, left, top, width, height);
    return Ref<LuminanceSource>(new OfxLuminanceSource(pixelsCrop));
}
