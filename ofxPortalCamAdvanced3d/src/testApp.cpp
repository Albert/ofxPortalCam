#include "testApp.h"

GLfloat lightOnePosition[] = {40, 40, 0, 0.0};
GLfloat lightOneColor[] = {0.99, 0.99, 0.99, 0.4};

GLfloat lightTwoPosition[] = {-40.0, 40, -25, 0.0};
GLfloat lightTwoColor[] = {0.99, 0.99, 0.99, 0.4};

GLfloat lightThreePosition[] = {-40.0, 40, 50, 0.0};
GLfloat lightThreeColor[] = {0.99, 0.99, 0.99, 0.4};

/*
 * This is the program shown on YOUTUBE LINK
 */

//--------------------------------------------------------------
void testApp::setup() {
	ofSetVerticalSync(false);
    glEnable (GL_DEPTH_TEST);
    glShadeModel (GL_SMOOTH);
    /* initialize lighting */
    glLightfv (GL_LIGHT0, GL_POSITION, lightOnePosition);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, lightOneColor);
    glEnable (GL_LIGHT0);
    glLightfv (GL_LIGHT1, GL_POSITION, lightTwoPosition);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, lightTwoColor);
    glEnable (GL_LIGHT1);
    glLightfv (GL_LIGHT2, GL_POSITION, lightThreePosition);
    glLightfv (GL_LIGHT2, GL_DIFFUSE, lightThreeColor);
    glEnable (GL_LIGHT2);
    glEnable (GL_LIGHTING);
    glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable (GL_COLOR_MATERIAL);
    nalgeneModel.loadModel("nalgene.3ds", 95);
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
	if (myPortalCam.needsCalib()) {
		/*
		 * When you add GL attributes like in this program, you need to make
		 * sure to disable them for the drawing of the calibration
		 */

		glPushAttrib(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);
		myPortalCam.drawCalib();
		glPopAttrib();
	} else {
		myPortalCam.begin();
		ofBackground(0, 0, 0);

		// Place the nalgene bottle
		ofPushMatrix();
		ofTranslate(-ofGetWindowWidth()/2 + 200, -ofGetWindowHeight()/2, -800);
		ofRotateX(-90);
		ofRotateX(3);
		ofRotateZ(-120);
		glDisable (GL_LIGHT2);
		nalgeneModel.draw();
		glEnable (GL_LIGHT2);
		ofPopMatrix();
		
		// Place stacks of paper
		ofPushMatrix();
		ofPushStyle();
		ofTranslate(-ofGetWindowWidth()/2 + 110, -ofGetWindowHeight()/2 -75, -1130);
		ofRotateX(2);
		ofSetColor(255, 255, 255);
		// White stack of paper
		ofPushMatrix();
		/*
		 	The neato thing about this project is that you can define things in
		 	terms of real world units.  My original set of code had this:
		 
				ofScale(8.5, 2, 11);  // ie: a stack of paper -- 8.5" by 11" by 2" tall
				ofBox(0, 0, 0, 95);   // ie: 95 pixels per inch
		 
			However I had to ditch that because ofScale was also scaling out the
			location of the GL lights.  I just bumped things around by a factor
			of ten to accomodate.
		*/
		ofScale(.85, .2, 1.1);
		ofBox(0, 0, 0, 950);
		ofPopMatrix();  /* end white stack */
		
		// Blue stack of paper
		ofSetColor(14, 118, 214);
		ofTranslate(0, -2 * 95, 0);
		ofPushMatrix();
		ofScale(.85, .2, 1.1);
		ofBox(0, 0, 0, 950);
		ofPopMatrix(); /* end blue stack */
		
		ofPopStyle();
		ofPopMatrix(); /* end paper stacks */

		// Place desk block
		ofPushMatrix();
		ofPushStyle();
		ofSetColor(74, 48, 25);
		ofTranslate(50, -ofGetWindowHeight()/2 -(4*12*95/2) -4*95, -5*95);
		ofBox(0, 0, 0, (4*12*95));
		ofPopStyle();
		ofPopMatrix();

		// Sphere for hand -- note, kinect defines the body backwards... "LeftHand" refers to the users right : P
		ofVec3f rHandPos = myPortalCam.screenify(myPortalCam.getViewerBody().getLimb(LeftHand).worldBegin);
		ofSphere(rHandPos, 100);
	}
}

//--------------------------------------------------------------
void testApp::exit(){
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if (key == 'r'){
		myPortalCam.resetCalib();
	} else if (key=='t') {
		myPortalCam.tweakOrientation();
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	myPortalCam.createCalibRay();
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}

