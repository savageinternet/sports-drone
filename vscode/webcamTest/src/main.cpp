#include "ofMain.h"
#include "ofApp.h"

#include "WebcamTestConstants.h"

//========================================================================
int main() {
    ofSetupOpenGL(WebcamTestConstants::CAMERA_WIDTH,
                  WebcamTestConstants::CAMERA_HEIGHT,
                  OF_WINDOW);

	ofRunApp(new ofApp());
}
