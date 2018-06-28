#include "ofMain.h"
#include "ofApp.h"

#include "WebcamConstants.hpp"

//========================================================================
int main() {
    ofSetupOpenGL(
        WebcamConstants::CAMERA_WIDTH,
        WebcamConstants::CAMERA_HEIGHT,
        OF_WINDOW);

	ofRunApp(new ofApp());
}
