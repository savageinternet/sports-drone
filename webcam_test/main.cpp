#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <iostream>

#define BG_SUB_FRAMES 60
#define LOOP_MS 33
#define CANNY_THRESHOLD_MAX 50
#define CANNY_RATIO 2
#define CANNY_KERNEL_SIZE 3

using namespace cv;
using namespace std;

const char* WINDOW_TITLE_FOCUS = "GetFocus";
const char* WINDOW_TITLE = "Webcam Test";

int cannyThreshold = 20;

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

static void onCannyThreshold(int, void*) {
    // do nothing
} 

int main(int argc, char** argv) {
    VideoCapture cap(0);
    if(!cap.isOpened()) {
        cerr << "Error opening webcam feed" << endl;
        return 1;
    }

    long startOfLoop;
    long frameNumber = -1L;

    Mat fgMaskMOG2;
    Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2();

    Mat frame;
    Mat frameGrey;

    double tickFrequency = getTickFrequency();

    openWindowOnTop(WINDOW_TITLE);
    createTrackbar("Canny Threshold:", WINDOW_TITLE, &cannyThreshold, CANNY_THRESHOLD_MAX, onCannyThreshold);

    while (true) {
        startOfLoop = getTickCount();
        frameNumber++;

        if (!cap.read(frame)) {
            cerr << "Error reading webcam frame" << endl;
            return 1;
        }
        pMOG2->apply(frame, fgMaskMOG2);
        cvtColor(frame, frameGrey, COLOR_BGR2GRAY);

        Mat frameGreyMasked;
        frameGrey.copyTo(frameGreyMasked, fgMaskMOG2);
        blur(frameGreyMasked, frameGreyMasked, Size(5, 5));
        Canny(frameGreyMasked, frameGreyMasked, cannyThreshold, cannyThreshold * CANNY_RATIO, CANNY_KERNEL_SIZE);
        imshow(WINDOW_TITLE, frameGreyMasked);

        if (endOfLoopDelay(startOfLoop, tickFrequency)) {
            break;
        }
    }
    return 0;
}
