
#include "testApp.h"
#include "ofMain.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 800, 600, OF_WINDOW);			// <-------- setup the GL context
	ofRunApp( new testApp());
}
