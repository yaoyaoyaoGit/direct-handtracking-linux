//
//  BasicTest_ofApp.cpp
//  Basic functionality test app.
// 
//  Created by Robert Xiao on April 9, 2015.
//
//

#include "BasicTest_ofApp.h"
#include "TextUtils.h"

#include "geomConfig.h"
#include "IRDepthTouchTracker.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);
	ofSetLogLevel(OF_LOG_VERBOSE);
	BaseApp::setup();

	touchTracker = new IRDepthTouchTracker(stream, *bgthread);
	touchTracker->startThread();

	setupDebug();
}

void ofApp::setupDebug() {
	lastDepthTimestamp = 0;
	curDepthFrame = 0;
	const int dw = stream->width;
	const int dh = stream->height;
	depthviz.allocate(dw, dh, OF_IMAGE_GRAYSCALE);
}

void ofApp::update(){
	BaseApp::update();
	vector<FingerTouch> newTouches;
	if(touchTracker->update(newTouches)) {
		for(FingerTouch &t : newTouches)
			OF_LOG_VERBOSE("BasicTest_ofApp")<<
		"new touches at:("<< t.tip.x<<", "<<t.tip.y<<") id: "<<t.id<< " Z:" << t.touchZ;
		handleTouches(newTouches);
	}

	updateDebug();
}

void ofApp::handleTouches(const vector<FingerTouch> &newTouches) {
	map<int, FingerTouch> newTouchMap;		
	set<int> touchDown, touchUp;
	map<int, FingerTouch> allTouches;

	/* Sort touches */
	for(auto &i : touchMap) {
		allTouches[i.first] = i.second;
	}

	for(auto touch : newTouches) {
		newTouchMap[touch.id] = touch;
		allTouches[touch.id] = touch;

		if(!touchMap.count(touch.id) || (!touchMap[touch.id].touched && touch.touched))
			touchDown.insert(touch.id);
	}

	for(auto &i : touchMap) {
		if(!newTouchMap.count(i.first) || (i.second.touched && !newTouchMap[i.first].touched))
			touchUp.insert(i.first);
	}

	touchMap = newTouchMap;
}

void ofApp::updateDebug() {
	stream->update();
	if(stream->isFrameNew()){
		unsigned short *depthpx = stream->getDepthPixels().getData();
		const int dw = stream->width;
		const int dh = stream->height;
		unsigned char *depthvizpx = depthviz.getPixels().getData();		
		for(int i=0; i<dw*dh; i++) {
			depthvizpx[i] = depthpx[i];
		}
		depthviz.update();
	}
}

//--------------------------------------------------------------
void ofApp::drawProjector(){
	/* In this function, draw points in real-world coordinates (metres) */
	ofSetLineWidth(2);
	ofNoFill();
	ofDrawCircle(100,100,10);
	/* Reproject touches */
	for(auto &entry : touchMap) {
		auto &touch = entry.second;
		ofPoint worldPt = getBackgroundWorldPoint(touch.tip);
		if(touch.touched) {
			ofNoFill();
			ofSetColor(0, 255, 0);
		} else {
			ofNoFill();
			ofSetColor(255, 0, 0);
		}
		ofDrawCircle(worldPt, 10);
		ofDrawBitmapString(ofVAArgsToString("%.2f\n%d", touch.touchZ, touch.id), worldPt);
	}
}

void ofApp::drawDebug(){
	const int dw = stream->width;
	const int dh = stream->height;

	depthviz.draw(0, 0);
	drawText("Depth", 0, 0, HAlign::left, VAlign::top);

	bgthread->drawDebug(0, dh);
	touchTracker->drawDebug(dw, 0);

	drawText(ofVAArgsToString("FPS: %.1f\n", ofGetFrameRate())
		+ ofVAArgsToString("BG Update FPS: %.1f\n", bgthread->fps.fps)
		+ ofVAArgsToString("Touch Update FPS: %.1f\n", touchTracker->fps.fps), DISPW, 0, HAlign::right, VAlign::top);

	int debugMouseX = mouseX - PROJW;
	int debugMouseY = mouseY;
	if(0 <= debugMouseX && debugMouseX < dw && 0 <= debugMouseY && debugMouseY < dh) {
		ofVec2f pos(debugMouseX, debugMouseY);

		string description;
		ofPoint curPt = getLiveWorldPoint(pos);
		description += ofVAArgsToString("curpos: %.6f, %.6f, %.6f\n", curPt.x, curPt.y, curPt.z);

		ofPoint bgPt = getBackgroundWorldPoint(pos);
		description += ofVAArgsToString("bgpos:  %.6f, %.6f, %.6f\n", bgPt.x, bgPt.y, bgPt.z);

		drawText(description, 0, DISPH, HAlign::left, VAlign::bottom);
	}
}

void ofApp::draw(){
	ofClear(64);
	/* Draw debug info */
	ofPushMatrix();
	ofPushStyle();
	drawDebug();
	ofPopStyle();
	ofPopMatrix();

	/* Draw onto projector */
	// ofPushMatrix();
	// ofPushStyle();
	// ofSetMatrixMode(OF_MATRIX_MODELVIEW);
 //    ofMultMatrix(projector_transpose);
 //    ofTranslate(DISPW, 0);
	// drawProjector();
	// ofPopStyle();
	// ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::teardown() {
	/* Destroy everything cleanly. */
	delete touchTracker;

	BaseApp::teardown();
}

void ofApp::keyPressed(int key){
	if(key == OF_KEY_ESC) {
		teardown();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}
