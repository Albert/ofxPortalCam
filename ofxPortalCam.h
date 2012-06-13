#pragma once
/*
 *  ofxPortalCam.h
 *  ofxPortalCam
 *
 *  Created by Albert Hwang on 5/25/12.
 *  Copyright 2012 Phedhex. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxRay.h"
#include "ofxXmlSettings.h"

#define CALIBRATION_STEPS 12

class ofxPortalCam : public ofCamera {
public:

	ofxPortalCam();
	void begin();
	void end();

	void drawCalib();
	void createCalibRay();
	ofxOpenNIUser getViewerBody();

	void loadCalib();
	void saveCalib();
	void resetCalib();
	bool needsCalib();
	// transforms kinect space into screeen space
	ofVec3f screenify(ofVec3f kinectPoint);

	void tweakOrientation();

protected:
	ofCamera myOfCamera;

	ofxOpenNI kinectDevice;
	ofxOpenNIUser user;
	void userEvent(ofxOpenNIUserEvent & event);

	bool calibDone;
	int calibStep;
	void calcCalib();
	ofRay calibRays[CALIBRATION_STEPS];
	ofxXmlSettings calibFile;
	
	ofVec3f displaceFactor;
	float scaleFactor;
	ofVec3f rotation1Perp;
	float rotation1;
	ofVec3f rotation2Perp;
	float rotation2;
	ofVec3f tweakPerp;
	float tweakAngle;

	ofVec3f screenNormal;
};