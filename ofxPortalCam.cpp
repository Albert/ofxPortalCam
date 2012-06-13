/*
 *  ofxPortalCam.cpp
 *  ofxPortalCam
 *
 *  Created by Albert Hwang on 5/25/12.
 *  Copyright 2012 Phedhex. All rights reserved.
 *
 */

#include "ofxPortalCam.h"

ofxPortalCam::ofxPortalCam() {
	kinectDevice.setup();
	kinectDevice.addDepthGenerator();
	kinectDevice.addUserGenerator();
	kinectDevice.setRegister(true);
    kinectDevice.setMaxNumUsers(1);
    ofAddListener(kinectDevice.userEvent, this, &ofxPortalCam::userEvent);

    kinectDevice.setBaseUserClass(user);	

	calibDone = false;
	calibStep = 0;
	screenNormal.set(0, 0, 1);
	myOfCamera.setFarClip(20000);
	loadCalib();
}

void ofxPortalCam::loadCalib() {
	if (calibFile.loadFile("calib.xml")) {
		calibFile.loadFile("calib.xml");
		string dxSetting = calibFile.getValue("calib:displaceFactor", "0, 0, 0");
		vector<string> dxSplit = ofSplitString(dxSetting, ", ", false, true);
		displaceFactor.set(ofToFloat(dxSplit[0]), ofToFloat(dxSplit[1]), ofToFloat(dxSplit[2]));

		scaleFactor = ofToFloat(calibFile.getValue("calib:scaleFactor", "0"));

		string r1pSetting = calibFile.getValue("calib:rotation1Perp", "0, 0, 0");
		vector<string> r1pSplit = ofSplitString(r1pSetting, ", ", false, true);
		rotation1Perp.set(ofToFloat(r1pSplit[0]), ofToFloat(r1pSplit[1]), ofToFloat(r1pSplit[2]));

		rotation1 = ofToFloat(calibFile.getValue("calib:rotation1", "0"));

		string r2pSetting = calibFile.getValue("calib:rotation2Perp", "0, 0, 0");
		vector<string> r2pSplit = ofSplitString(r2pSetting, ", ", false, true);
		rotation2Perp.set(ofToFloat(r2pSplit[0]), ofToFloat(r2pSplit[1]), ofToFloat(r2pSplit[2]));
		
		rotation2 = ofToFloat(calibFile.getValue("calib:rotation2", "0"));

		string tpSetting = calibFile.getValue("calib:tweakPerp", "0, 0, 0");
		vector<string> tpSplit = ofSplitString(tpSetting, ", ", false, true);
		tweakPerp.set(ofToFloat(tpSplit[0]), ofToFloat(tpSplit[1]), ofToFloat(tpSplit[2]));

		tweakAngle = ofToFloat(calibFile.getValue("calib:tweakAngle", "0"));

		calibDone = true;
	}
}

bool ofxPortalCam::needsCalib() {
	return !calibDone;
}

void ofxPortalCam::saveCalib() {
	calibFile.loadFile("calib.xml");
	calibFile.setValue("calib:displaceFactor", ofToString(displaceFactor));
	calibFile.setValue("calib:scaleFactor", ofToString(scaleFactor));
	calibFile.setValue("calib:rotation1Perp", ofToString(rotation1Perp));
	calibFile.setValue("calib:rotation1", ofToString(rotation1));
	calibFile.setValue("calib:rotation2Perp", ofToString(rotation2Perp));
	calibFile.setValue("calib:rotation2", ofToString(rotation2));
	calibFile.saveFile("calib.xml");	
}

void ofxPortalCam::resetCalib() {
	calibStep = 0;
	calibDone = false;
}

ofxOpenNIUser ofxPortalCam::getViewerBody() {
	return this->user;
}

void ofxPortalCam::drawCalib() {
    kinectDevice.update();
    for(int i = 0; i < kinectDevice.getNumTrackedUsers(); i++){
        user = kinectDevice.getTrackedUser(i);
	}
	bool canCalibrate = (kinectDevice.getNumTrackedUsers() > 0 ) ? true : false;
	ofPushStyle();
	ofSetColor(255, 255, 255);
	ofFill();
	kinectDevice.drawDebug(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	if (canCalibrate) {
		int rad = 30;
		int circleX = 0;
		if ((calibStep % 3) == 1) {
			circleX = ofGetWindowWidth();
		}
		int circleY = ofGetWindowHeight() * (calibStep % 3) / 2;
		ofPushStyle();
		ofSetColor(255, 0, 0);
		ofCircle(circleX, circleY, rad);
		ofPopStyle();
	}
	ofPopStyle();
}

void ofxPortalCam::begin() {
    kinectDevice.update();
    for(int i = 0; i < kinectDevice.getNumTrackedUsers(); i++){
        user = kinectDevice.getTrackedUser(i);
	}
	if (user.getLimb(Head).found == 1) {
		ofVec3f screenHead = screenify(user.getLimb(Head).worldBegin);
		myOfCamera.setPosition(screenHead);
	} else {
		myOfCamera.setPosition(0, 0, ofGetWindowWidth() * 2.5);
	}

	ofVec3f topLeft, bottomLeft, bottomRight;
	topLeft.set(-ofGetWindowWidth()/2, ofGetWindowHeight()/2, 0);
	bottomLeft.set(-ofGetWindowWidth()/2, -ofGetWindowHeight()/2, 0);
	bottomRight.set(ofGetWindowWidth()/2, -ofGetWindowHeight()/2, 0);
	myOfCamera.setupOffAxisViewPortal(topLeft, bottomLeft, bottomRight);

	myOfCamera.begin();
}

void ofxPortalCam::end() {
	myOfCamera.end();
}

void ofxPortalCam::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user " << event.id << " from device" ;
}

void ofxPortalCam::createCalibRay() {
	if ((!calibDone) && user.getLimb(Head).found == 1 && user.getLimb(LeftHand).found == 1) {
		ofVec3f headPosition, handPosition;
		headPosition = user.getLimb(Head).worldBegin;
		// for some reason "Left" is from the kinect's perspective, so I changed it to the human perspective
		handPosition = user.getLimb(LeftHand).worldBegin;
		calibRays[calibStep] = ofRay(headPosition, handPosition - headPosition);
		calibStep = calibStep + 1;
		ofLog() << headPosition;
		ofLog() << handPosition;
		if (calibStep == CALIBRATION_STEPS) {
			calcCalib();
		}
	}
}


//--------------------------------------------------------------
void ofxPortalCam::calcCalib(){
	ofVec3f planeDefinition[3];

	for (int i = 0; i < 3; i++) {
		ofVec3f averagePoint;
		int numOfIntersections = CALIBRATION_STEPS / 3; // num of intersections per plane definition point
		ofVec3f individualPoints[numOfIntersections];
		for (int j = 0; j < numOfIntersections; j++) {
			int thisInd = (i +  j    * 3);
			int nextInd = (i + (j+1) * 3) % CALIBRATION_STEPS;
			ofRay intersectionSegment;
			intersectionSegment = calibRays[thisInd].intersect(calibRays[nextInd]);
			ofVec3f intersectionMidpoint;
			intersectionMidpoint.set(intersectionSegment.getMidpoint());
			individualPoints[j] = intersectionMidpoint;
		}
		averagePoint.average(individualPoints, numOfIntersections);
		planeDefinition[i] = averagePoint;
	}

	// next, i need to find out how to transform kinect space into screen space.

	// find the scalar difference between planeDef[0]-planeDef[2] & window height
	float kinectSpaceWindowHeight = planeDefinition[0].distance(planeDefinition[2]);
	scaleFactor = ofGetWindowHeight() / kinectSpaceWindowHeight;
	for (int i = 0; i < 3; i++) {
		planeDefinition[i] = planeDefinition[i] * scaleFactor;
	}

	// find the vector difference between planeDef[0] and (- window width / 2, -window height / 2, 0)
	displaceFactor = planeDefinition[0];

	// find the rotation required to get planeDef[1] & planeDef[2] in place
	ofVec3f rotation1Init = planeDefinition[2] - planeDefinition[0];
	ofVec3f rotation1End;
	rotation1End.set(0, ofGetWindowHeight(), 0);

	rotation1Perp = rotation1Init.getPerpendicular(rotation1End);
	rotation1 = rotation1Init.angle(rotation1End);

	ofVec3f	rotation2PreInit, rotation2Init, rotation2InitDebugYSquash, rotation2End, rotation2EndSquash;
	rotation2PreInit = planeDefinition[1] - planeDefinition[0];
	rotation2Init = rotation2PreInit;
	rotation2Init.rotate(rotation1, rotation1Perp);
	rotation2InitDebugYSquash = rotation2Init;
	rotation2InitDebugYSquash.y = 0; // Force this whole thing into a 2d rotation
	rotation2End;
	rotation2End.set(ofGetWindowWidth(), ofGetWindowHeight()/2, 0);
	rotation2EndSquash = rotation2End;
	rotation2EndSquash.y = 0;
	rotation2Perp = rotation1End;
	rotation2 = rotation2InitDebugYSquash.angle(rotation2EndSquash);
	float rotation2CandidateA = rotation2InitDebugYSquash.angle(rotation2EndSquash);
	float rotation2CandidateB = -rotation2CandidateA;
	ofVec3f rotation2CandidateAPoint = rotation2Init.rotate(rotation2CandidateA, rotation2Perp);
	ofVec3f rotation2CandidateBPoint = rotation2Init.rotate(rotation2CandidateB, rotation2Perp);
	if ((rotation2CandidateAPoint - rotation2End).length() < (rotation2CandidateBPoint - rotation2End).length()) {
		rotation2 = rotation2CandidateA;
	} else {
		rotation2 = rotation2CandidateB;
	}
	calibDone = true;
	saveCalib();
}

//--------------------------------------------------------------
void ofxPortalCam::tweakOrientation(){
	if (calibDone) {
		tweakAngle = 0;
		ofVec3f screenHead = screenify(user.getLimb(Head).worldBegin);
		tweakPerp = screenHead.getPerpendicular(screenNormal);
		tweakAngle = screenHead.angle(screenNormal);
		calibFile.loadFile("calib.xml");
		calibFile.setValue("calib:tweakPerp", ofToString(tweakPerp));
		calibFile.setValue("calib:tweakAngle", ofToString(tweakAngle));
		calibFile.saveFile("calib.xml");
	}
}

//--------------------------------------------------------------
ofVec3f ofxPortalCam::screenify(ofVec3f kinectPoint){
	ofVec3f newPoint;
	newPoint = kinectPoint;
	newPoint = newPoint * scaleFactor;
	newPoint = newPoint - displaceFactor;
	newPoint.rotate(rotation1, rotation1Perp);
	newPoint.rotate(rotation2, rotation2Perp);
	newPoint.x = newPoint.x - ofGetWindowWidth()/2;
	newPoint.x = -newPoint.x;
	newPoint.y = newPoint.y - ofGetWindowHeight()/2;
	newPoint.rotate(180, screenNormal);
	newPoint.rotate(tweakAngle, tweakPerp);
	return newPoint;
}