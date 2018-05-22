#include "opencv2/opencv.hpp"

#include <iostream>

#define LOOP_MS 33

using namespace cv;
using namespace std;

const char* WINDOW_TITLE = "Sports Drone :: Webcam Test";

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
    VideoCapture cap(0);
    if(!cap.isOpened()) {
        cout << "Error opening webcam feed" << endl;
        return 1;
    }

    long startOfLoop;
    double tickFrequency = getTickFrequency();

    Mat frameGrey;
    namedWindow(WINDOW_TITLE, 1);
    while (true) {
        startOfLoop = getTickCount();

        Mat frame;
        cap >> frame;
        cvtColor(frame, frameGrey, COLOR_BGR2GRAY);
        imshow(WINDOW_TITLE, frameGrey);
        if (endOfLoopDelay(startOfLoop, tickFrequency)) {
            break;
        }
    }
    return 0;
}
