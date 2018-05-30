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
    const ofPixels& _pixels;
    
public:
    OfxLuminanceSource(const ofxCvGrayscaleImage& image);
    ~OfxLuminanceSource();
    
    ArrayRef<char> getRow(int y, ArrayRef<char> row) const;
    ArrayRef<char> getMatrix() const;
};

#endif /* OfxLuminanceSource_hpp */
