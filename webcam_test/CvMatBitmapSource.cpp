#include "CvMatBitmapSource.h"

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

namespace zxing {

CvMatBitmapSource::CvMatBitmapSource(const Mat& mat) :
    LuminanceSource(mat.cols, mat.rows) {
  cvtColor(mat, _mat, COLOR_BGR2GRAY);
}

CvMatBitmapSource::~CvMatBitmapSource() {}

int CvMatBitmapSource::getWidth() const {
  return _width;
}

int CvMatBitmapSource::getHeight() const {
  return _height;
}

ArrayRef<char> CvMatBitmapSource::getRow(int y, ArrayRef<char> row) const {
  int width = getWidth();
  if (!row) {
    row = ArrayRef<char>(new char[width], width);
  }
  for (int x = 0; x < _width; x++) {
    cout << "(" << y << ", " << x << ") => " << (int) (_mat.at<char>(y, x)) << endl;
    row[x] = _mat.at<char>(y, x);
  }
  return row;
}

ArrayRef<char> CvMatBitmapSource::getMatrix() const { 
  int n = _height * _width;
  char* matrix = new char[_height * _width];
  char* p = matrix;
  for (int y = 0; y < _height; y++) {
    for (int x = 0; x < _width; x++) {
      *p = _mat.at<char>(y, x);
      p++;
    }
  }
  return ArrayRef<char>(matrix, n);
}

} /* namespace zxing */
