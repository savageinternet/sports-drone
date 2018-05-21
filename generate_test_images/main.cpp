#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace cv;
using namespace std;
using namespace boost::program_options;

const int SHOULDER_LEFT = 1;
const int SHOULDER_RIGHT = 2;

int parseArgs(int argc, char **argv, variables_map& vm) {
  options_description desc("generate_test_images: test image generator for codes");
  desc.add_options()
      ("help", "show help message")
      ("n", value<int>()->default_value(1), "number of test images to generate")
      ("code", value<string>()->default_value("fw_blocks"), "code type (see codes folder for available types)");

  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help")) {
      cout << desc << "\n";
      return 1;
  }
  return 0;
}

string getLeftShoulderFilename(const string& code) {
  stringstream fmt;
  fmt << "codes/" << code << "_" << SHOULDER_LEFT << ".png";
  return fmt.str();
}

string getRightShoulderFilename(const string& code) {
  stringstream fmt;
  fmt << "codes/" << code << "_" << SHOULDER_RIGHT << ".png";
  return fmt.str();
}

bool fileExists(const string& filename) {
  ifstream file(filename);
  return !!file;
}

int main(int argc, char **argv) {
  variables_map vm;
  int rc = parseArgs(argc, argv, vm);
  if (rc != 0) {
    return rc;
  }

  string code = vm["code"].as<string>();
  
  string fL = getLeftShoulderFilename(code);
  if (!fileExists(fL)) {
    cout << "left shoulder code missing: " << fL << endl;
    return 1;
  }
  string fR = getRightShoulderFilename(code);
  if (!fileExists(fR)) {
    cout << "right shoulder code missing: " << fL << endl;
    return 1;
  }

  //Mat imgLeft = imread("filename", IMREAD_COLOR);
  
  int n = vm["n"].as<int>();
  for (int i = 0; i < n; i++) {
    // TODO: generate code...
    cout << i << endl;
  }
}
