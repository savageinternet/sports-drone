#pragma once

#include <bitset>
#include <string>

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

#include "ShoulderCodec.hpp"
#include "ShoulderDetector.hpp"

using namespace cv;
using namespace ofxCv;
using namespace std;

enum DetectionMode {
    IMAGE,
    VIDEO,
    WEBCAM_SET_CONTOUR,
    WEBCAM
};

struct ImageParameters {
    int n;
    int x0;
    int y0;
    float theta;
    int size;
};

class ofApp : public ofBaseApp{
private:
    // Color constants.
    static const int BLACK = 0;
    static const int WHITE = 255;

    // Used to load images / videos and save test results.
    string projectRoot;

    /*
     * Code location.  In IMAGE mode, this is supplied by ground-truth parameters
     * embedded in the filename.  In VIDEO mode, this is detected using our object
     * tracking.  In WEBCAM mode, this is selected by the user via mouse gestures;
     * the special WEBCAM_SET_CONTOUR mode is provided to facilitate this.
     */
    ofPolyline ofContour;
    ofVec2f ofCentroid;

    /*
     * Code detection and decoding.  
     */
    bitset<24> codeFormatted;
    bitset<16> code;
    int result;
    ShoulderDetector detector;
    bool detectionFinished;

    /*
     * Color and greyscale images.  The detector requires a greyscale image.
     * In IMAGE mode, `image` is the loaded image.  In VIDEO and WEBCAM modes,
     * `image` is the current frame.
     */
    ofImage image;
    ofImage imageGrey;

    // Image ground-truth parameters for IMAGE mode.
    ImageParameters imageParameters;

    // Video playback for VIDEO mode.
    ofVideoPlayer video;

    // Webcam capture for WEBCAM mode.
    ofVideoGrabber cam;
    bool skippedFirstFrame;

    // Controls to set various modes.
    ofxPanel gui;
    ofxButton btnLoadImage;
    ofxButton btnLoadVideo;
    ofxButton btnLoadWebcam;

    // Current detection mode.
    DetectionMode detectionMode;

    void performDetection();

    void updateWindowTitle();
    void updateImage();
    void updateVideo();
    void updateWebcam();

    void drawCodeLocation();
    void drawDetectionResult();
    
    void setImageCodeLocation();

    static string getModeName(DetectionMode detectionMode);
    static ImageParameters parseImageParameters(const string& filename);
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void loadImage();
    void loadVideo();
    void loadWebcam();
};
