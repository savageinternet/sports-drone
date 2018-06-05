//
//  ThresholdBinarizer.hpp
//  WebcamTest
//
//  Created by Evan Savage on 2018-06-01.
//

#ifndef ThresholdBinarizer_hpp
#define ThresholdBinarizer_hpp

#include <zxing/Binarizer.h>
#include <zxing/common/BitArray.h>
#include <zxing/common/BitMatrix.h>

using namespace zxing;

class ThresholdBinarizer : public Binarizer {
private:
    ArrayRef<char> luminances;
    int _blackPoint;
public:
    ThresholdBinarizer(Ref<LuminanceSource> source, int blackPoint = 70);
    virtual ~ThresholdBinarizer();
    
    virtual Ref<BitArray> getBlackRow(int y, Ref<BitArray> row);
    virtual Ref<BitMatrix> getBlackMatrix();
    Ref<Binarizer> createBinarizer(Ref<LuminanceSource> source);
};

#endif /* ThresholdBinarizer_hpp */
