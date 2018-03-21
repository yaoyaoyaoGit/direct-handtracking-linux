//
//  BaseApp.cpp
//  Base class for our apps.
// 
//  Created by Robert Xiao on April 10, 2015.
//
//

#include "BaseApp.h"
#include "BackgroundUpdaterThread.h"
#include "geomConfig.h"
//--------------------------------------------------------------
void BaseApp::setup(){

	ofSetFrameRate(60);

	setupKinect();
	ofLogVerbose("baseApp")<<"kinect started";
	/* Setup worker threads */
	bgthread = new BackgroundUpdaterThread(stream);
	bgthread->startThread();
	ofLogVerbose("baseApp")<<"bgthread started";
}

void BaseApp::setupKinect() {
	stream = new ofxFreenect2::Stream();
	stream->setup();
	stream->open();
}


void BaseApp::update(){
	stream->update();
	bgthread->update();
}

ofPoint BaseApp::getWorldPoint(const ofVec2f &depthPos, bool live) {
	int x0 = floor(depthPos.x);
	int y0 = floor(depthPos.y);
	if(x0 < 0 || x0 >= stream->width - 1 || y0 < 0 || y0 >= stream->height -1)
		return ofPoint(0,0,0);

	/* Linearly interpolate the world point */
	ofPoint ret;
	for(int x = x0; x <= x0+1; x++) {
		for(int y = y0; y <= y0+1; y++) {
			int dpt_x = x;
			int dpt_y = y;

			int depth;
			int index = (int)dpt_x * stream->width + (int)dpt_y;
			if(live) {
				depth = stream->getDepthPixels().getData()[index]; // current (finger) depth
			} else {
				depth = bgthread->getBackgroundMean().getPixels()[index]; // stable (background) depth
			}

			float weight = (1 - fabsf(depthPos.x - x)) * (1 - fabsf(depthPos.y - y));

			/* Map depth point to camera point */
			// CameraSpacePoint wpt;
			float wpt_x = 0;
			float wpt_y = 0;
			float wpt_z = 0;

			stream->getPointXYZ(dpt_x,dpt_y,depth,wpt_x,wpt_y,wpt_z);
			// HRESULT hr;
			// hr = kinect->getMapper()->MapDepthPointToCameraSpace(dpt, depth, &wpt);
			// if(!SUCCEEDED(hr)) {
			// 	ofLogError() << "MapDepthPointToCameraSpace failed";
			// 	return ofPoint(0, 0, 0);
			// }

			ret += weight * ofPoint(wpt_x, wpt_y, wpt_z);
		}
	}
	return ret;
}
//--------------------------------------------------------------
void BaseApp::teardown() {
	/* Destroy everything cleanly. */
	delete bgthread; // destructor stops the thread for us
	bgthread = NULL;
	delete stream;
	stream = NULL;
}

void BaseApp::keyPressed(int key){
	if(key == OF_KEY_ESC) {
		teardown();
	}
}

void BaseApp::keyReleased(int key){

}
