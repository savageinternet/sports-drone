#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <zxing/Reader.h>
#include <zxing/aztec/AztecReader.h>
#include <zxing/common/Counted.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/datamatrix/DataMatrixReader.h>
#include <zxing/Exception.h>

#include <iostream>
#include <string>

#include "CvMatBitmapSource.h"

#define LOOP_MS 33
#define TYPE_AZTEC "aztec"
#define TYPE_DATAMATRIX "datamatrix"

using namespace cv;
using namespace std;
using namespace zxing;
using namespace zxing::aztec;
using namespace zxing::datamatrix;

const char* WINDOW_TITLE_FOCUS = "GetFocus";
const char* WINDOW_TITLE = "Webcam Test";

CommandLineParser parseArgs(int argc, char** argv) {
    const string keys =
        "{help h         |      | print this message                }"
        "{t type         |aztec | type of code [aztec | datamatrix] }";
    CommandLineParser parser(argc, argv, keys);
    parser.about("webcam_test: real-time code detection test");
    return parser;
}

void openWindowOnTop(const char* title) {
    namedWindow(WINDOW_TITLE_FOCUS, CV_WINDOW_NORMAL);
    Mat img = Mat::zeros(100, 100, CV_8UC3);
    imshow(WINDOW_TITLE_FOCUS, img);
    setWindowProperty(WINDOW_TITLE_FOCUS, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    waitKey(1);
    setWindowProperty(WINDOW_TITLE_FOCUS, CV_WND_PROP_FULLSCREEN, CV_WINDOW_NORMAL);
    destroyWindow(WINDOW_TITLE_FOCUS);

    namedWindow(title, 1);
}

Ref<Reader> getCodeReader(const string& type) {
    Ref<Reader> reader(NULL);
    if (type == TYPE_AZTEC) {
        reader = new AztecReader();
    } else if (type == TYPE_DATAMATRIX) {
        reader = new DataMatrixReader();
    }
    return reader;
}

Ref<Result> readCodeFromFrame(Ref<Reader> reader, const Mat& frame) {
    try {
        Ref<CvMatBitmapSource> source(new CvMatBitmapSource(frame));
        Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
        Ref<BinaryBitmap> image(new BinaryBitmap(binarizer));
        return reader->decode(image);
    } catch (zxing::Exception& e) {
        cerr << "Error: " << e.what() << endl;
        return Ref<Result>(NULL);
    }
}

long endOfLoopDelay(long startOfLoop, double tickFrequency) {
    long ticks = getTickCount();
    long dTicks = ticks - startOfLoop;
    int dt = (int) (dTicks * 1000 / tickFrequency);
    int toDelay = 1;
    if (dt < LOOP_MS) {
        toDelay = LOOP_MS - dt;
    }
    return waitKey(toDelay) >= 0;
}

int main(int argc, char** argv) {
    // parse and validate args
    CommandLineParser parser = parseArgs(argc, argv);
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    string type = parser.get<string>("type");
    Ref<Reader> reader = getCodeReader(type);
    if (reader.empty()) {
        cerr << "Invalid type of code: " << type << endl;
        parser.printMessage();
        return 1;
    }

    // open webcam feed (0 is default camera)
    VideoCapture cap(0);
    if(!cap.isOpened()) {
        cerr << "Error opening webcam feed" << endl;
        return 1;
    }

    Mat frame;

    long startOfLoop;
    double tickFrequency = getTickFrequency();

    openWindowOnTop(WINDOW_TITLE);

    // main loop
    while (true) {
        startOfLoop = getTickCount();

        if (!cap.read(frame)) {
            cerr << "Error reading webcam frame" << endl;
            return 1;
        }

        Ref<Result> result = readCodeFromFrame(reader, frame);
        if (result.empty()) {
            return 1;
        }
        cout << result->getText()->getText() << endl;
        
        imshow(WINDOW_TITLE, frame);

        if (endOfLoopDelay(startOfLoop, tickFrequency)) {
            break;
        }
    }
    return 0;
}
