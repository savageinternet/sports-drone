//
//  OfxLuminanceSource.hpp
//  WebcamTest
//
//  Created by Evan Savage on 2018-05-30.
//

#ifndef OfxLuminanceSource_hpp
#define OfxLuminanceSource_hpp

#include "ofxOpenCv.h"

#include <zxing/LuminanceSource.h>

using namespace zxing;

class OfxLuminanceSource : public LuminanceSource {
private:
    ofPixels _pixels;
    
public:
    OfxLuminanceSource(const ofPixels& pixels);
    OfxLuminanceSource(const ofxCvGrayscaleImage& image);
    ~OfxLuminanceSource();
    
    ArrayRef<char> getRow(int y, ArrayRef<char> row) const;
    ArrayRef<char> getMatrix() const;
    
    bool isCropSupported() const;
    Ref<LuminanceSource> crop(int left, int top, int width, int height) const;
};

#endif /* OfxLuminanceSource_hpp */
