#ifndef __CV_MAT_BITMAP_SOURCE_H__
#define __CV_MAT_BITMAP_SOURCE_H__

#include "opencv2/opencv.hpp"
#include <zxing/LuminanceSource.h>

using namespace cv;

namespace zxing {

class CvMatBitmapSource : public LuminanceSource {
private:
  Mat _mat;
  int _width;
  int _height;

public:
  CvMatBitmapSource(const Mat& mat);
  ~CvMatBitmapSource();

  int getWidth() const;
  int getHeight() const;
  ArrayRef<char> getRow(int y, ArrayRef<char> row) const;
  ArrayRef<char> getMatrix() const;
};

} /* namespace zxing */

#endif /* __CV_MAT_BITMAP_SOURCE_H__ */
