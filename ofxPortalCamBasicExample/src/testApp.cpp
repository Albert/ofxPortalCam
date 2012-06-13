#include "testApp.h"

/*
 *  Hi there -- Thanks for checking out this project!
 *
 *	I've done my best to comment up this testApp so that it's super clear.
 *	Please let me know if anything is confusing.
 *	
 *	-Albert Hwang
 *	http://albert-hwang.com
 */


//--------------------------------------------------------------
void testApp::setup() {
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){

	if (myPortalCam.needsCalib()) {
	/*	
	 *	So this camera needs to be calibrated only once every time
	 *	either the kinect or screen is physically moved.  Once
	 *	calibrated "needsCalib()" will return as false.  This is
	 *	true for this session, as well as any future sessions (a
	 *	config for the calibration is saved and reloaded whenever
	 *	the app runs).
	 *
	 *	If, for whatever reason, you'd like to manually reset the
	 *	calibration, call the "resetCalib()" function (press the
	 *	"r" key as is defined in the key bindings below).
	 */
		myPortalCam.drawCalib();
	/*	
	 *	"drawCalib()" draws your calibration.  When initialized it will
	 *	show a view from your Kinect's camera.
	 *
	 *	To register your skeleton, do the cactus pose:
	 *		http://bit.ly/cactus_pose
	 *
	 *	Sometimes skeleton registration takes a while... You'll know
	 *	when it happens when a red drawing of your skeleton appears
	 *	superimposed on top of your body.  Once your body registers,
	 *	you're ready to start calibration.
	 *
	 *	Notice that there's a red dot in the top left corner of the
	 *	screen.  Grab your computer mouse and, standing at a distance,
	 *	extend your right arm forward so that your right hand obscures
	 *	your vision of the dot.  When you're there, click the mouse
	 *	button to set that calibration point.
	 *
	 *	A new dot will appear on the right side of the screen.  Do
	 *	the same thing for this dot.  Lastly, it will appear in the
	 *	bottom left corner of the screen.  Do that once again.
	 *
	 *	Great!  You've just calibrated one set of points.  Now we need
	 *	to calibrate 3 other sets of points (4 sets in total).  Stand
	 *	somewhere else in the room and repeat that action for those
	 *	three dots.  The further you can move your head around (while
	 *	keeping your whole head completely the in the Kinect's camera),
	 *	the stronger your calibration will tend to be.
	 *
	 *	When all 12 points (4 sets of 3) are calibrated, you're almost
	 *	done!
	 *
	 *	To finalize the calibration, you will need to "tweak" the
	 *	orientation.  Put your head in line with the center of the
	 *	screen.  It doesn't matter how far away you are from the screen
	 *	as long as your head is in line with the center.  Once there,
	 *	call the "tweakOrientation()" function by pressing "t".
	 *
	 *	Great, you're all calibrated!
	 *
	 */
	} else {
		myPortalCam.begin();
		ofSetColor(255, 255, 255);
		ofBackground(0, 0, 0, 1);
		
		// Draw the wall grids
		int portalDepth = -1000;
		ofNoFill();
		for (int i= 0; i >= portalDepth; i = i - 100) {
			ofPushMatrix();
			ofTranslate(0, 0, i);
			ofRect(-ofGetWindowWidth()/2, -ofGetWindowHeight()/2, ofGetWindowWidth(), ofGetWindowHeight());
			ofPopMatrix();
		}
		for (int i = 0; i < 18; i++) {
			float f = -ofGetWindowWidth()/2 + float(i) * (ofGetWindowWidth() / 18);
			ofPolyline line;
			line.addVertex(f,  ofGetWindowHeight()/2, 0);
			line.addVertex(f,  ofGetWindowHeight()/2, portalDepth);
			line.addVertex(f, -ofGetWindowHeight()/2, portalDepth);
			line.addVertex(f, -ofGetWindowHeight()/2, 0);
			line.draw();
		}
		for (int i = 0; i < 12; i++) {
			float f = -ofGetWindowHeight()/2 + float(i) * (ofGetWindowHeight() / 12);
			ofPolyline line;
			line.addVertex( ofGetWindowWidth()/2, f, 0);
			line.addVertex( ofGetWindowWidth()/2, f, portalDepth);
			line.addVertex(-ofGetWindowWidth()/2, f, portalDepth);
			line.addVertex(-ofGetWindowWidth()/2, f, 0);
			line.draw();
		}
		
		// Draw a sphere
		ofSetColor(0, 255, 0);
		ofSphere(0, 0, portalDepth/2, ofGetWindowHeight() / 4);
		
		myPortalCam.end();
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

