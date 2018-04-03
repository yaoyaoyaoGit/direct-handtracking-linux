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
#include "json.hpp"
using json = nlohmann::json;
//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(30);
	ofSetLogLevel(OF_LOG_WARNING);

    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = 9002;
	options.bUseSSL = false; // you'll have to manually accept this self-signed cert if 'true'!
    bSetup = server.setup( options );

	BaseApp::setup();

	TouchTrackerWrapper tracker;
	tracker.finger = 1;
	tracker.name = "IRDepthTouchTracker";
	tracker.tracker = new IRDepthTouchTracker(stream, *bgthread);
	trackers.push_back(tracker);
	tracker.tracker->startThread(); 
	setupDebug();
}

void ofApp::setupDebug() {
	lastDepthTimestamp = 0;
	curDepthFrame = 0;
	const int dw = stream->width;
	const int dh = stream->height;
	depthviz.allocate(dw, dh, OF_IMAGE_GRAYSCALE);
	irviz.allocate(dw, dh, OF_IMAGE_GRAYSCALE);
}

void ofApp::update(){
	BaseApp::update();
	for(auto &t: trackers){
		vector<FingerTouch> newTouches;
		map<int, FingerTouch> newTouchMap;
		if(t.tracker->update(newTouches)){
			map<int, FingerTouch> touchMap = t.touches;
			for(auto touch: newTouches){
				newTouchMap[touch.id] = touch;
				if(!touchMap.count(touch.id) || (!touchMap[touch.id].touched && touch.touched)){
					// t.touches[touch.id] = touch;
					t.states[touch.id] = 0; // touch down
				}
			}
			for(auto &i: t.touches){
				if(!newTouchMap.count(i.first) || (i.second.touched && !newTouchMap[i.first].touched))
				t.states[i.first] = 3;	// touch up		
			}
		}
		t.touches = newTouchMap;
	}
	updateDebug();
}

void ofApp::handleTouches(const vector<FingerTouch> &newTouches) {
	map<int, FingerTouch> newTouchMap;		
	set<int>touchUp;
	map<int, FingerTouch> allTouches;
	map<int, FingerTouch> touchMap;
	touchDown.clear();
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
	// uint32_t curDepthTimestamp = stream->timestamp;
	// if(lastDepthTimestamp == curDepthTimestamp)
	// 	return;
	// lastDepthTimestamp = curDepthTimestamp;
	// curDepthFrame++;
	if(stream->isFrameNew()){
		unsigned short *depthpx = stream->getDepthPixels().getData();
		const int dw = stream->width;
		const int dh = stream->height;
		unsigned char *depthvizpx = depthviz.getPixels().getData();		
		for(int i=0; i<dw*dh; i++) {
			depthvizpx[i] = depthpx[i];
		}
		depthviz.update();
		unsigned short *irpx = stream->getIrPixels().getData();
		unsigned char *irvizpx = irviz.getPixels().getData();
		for(int i=0; i<dw*dh; i++) {
			// if(irpx[i]/64>255)
			// 	irvizpx[i] = 255;
			// else
			// 	irvizpx[i] = irpx[i]/64;
			irvizpx[i] = irpx[i]/256;
		}
		// ofLogWarning("BasicTest_ofApp")<<avg;
		irviz.update();
	}
}

//--------------------------------------------------------------
void ofApp::drawProjector(){
	map<int, FingerTouch> touchMap;
	/* In this function, draw points in real-world coordinates (metres) */
	ofSetLineWidth(2);
	ofNoFill();
	ofPoint xx = homo(ofPoint(200, 200));
	ofDrawCircle(xx,10);
	/* Reproject touches */
	for(auto &entry : touchMap) {
		auto &touch = entry.second;
		// ofPoint worldPt = getBackgroundWorldPoint(touch.tip);
		ofPoint worldPt = touch.tip;
		if(touch.touched) {
			ofNoFill();
			ofSetColor(0, 255, 0);
		} else {
			ofNoFill();
			ofSetColor(255, 0, 0);
		}
		ofDrawCircle(worldPt, 10);
		// 	ofLogVerbose("BasicTest_ofApp")<<
		// "worldPt at:("<< worldPt.x<<", "<<worldPt.y<<")";
		ofDrawBitmapString(ofVAArgsToString("%.2f\n%d", touch.touchZ, touch.id), worldPt);
	}
}

void ofApp::drawDebug(){
	const int dw = stream->width;
	const int dh = stream->height;
	depthviz.draw(0, 0);
	drawText("Depth", 0, 0, HAlign::left, VAlign::top);
	irviz.draw(0,dh);
	drawText("IR", 0, dh, HAlign::left, VAlign::top);
	// bgthread->drawDebug(0, dh);
	trackers[0].tracker->drawDebug(dw, 0);

	drawText(ofVAArgsToString("FPS: %.1f\n", ofGetFrameRate())
		+ ofVAArgsToString("BG Update FPS: %.1f\n", bgthread->fps.fps)
		+ ofVAArgsToString("Touch Update FPS: %.1f\n", trackers[0].tracker->fps.fps), DISPW, 0, HAlign::right, VAlign::top);

	// // int debugMouseX = mouseX + DISPW;
	// int debugMouseX = mouseX;
	// int debugMouseY = mouseY;
	// if(0 <= debugMouseX && debugMouseX < dw && 0 <= debugMouseY && debugMouseY < dh) {
	// 	ofVec2f pos(debugMouseX, debugMouseY);

	// 	string description;
	// 	// ofPoint curPt = getLiveWorldPoint(pos);
	// 	ofPoint curPt = pos;
	// 	description += ofVAArgsToString("curpos: %.6f, %.6f, %.6f\n", curPt.x, curPt.y, curPt.z);
	// 	// 	ofLogVerbose("BasicTest_ofApp")<<
	// 	// "debugMouse at:("<< curPt.x<<", "<<curPt.y<<")";
	// 	// ofPoint bgPt = getBackgroundWorldPoint(pos);
	// 	ofPoint bgPt = pos;
	// 	description += ofVAArgsToString("bgpos:  %.6f, %.6f, %.6f\n", bgPt.x, bgPt.y, bgPt.z);

	// 	drawText(description, 0, DISPH, HAlign::left, VAlign::bottom);
	// }
}

void ofApp::draw(){
	ofClear(64);
	/* Draw onto projector */
	// ofPushMatrix();
	// ofPushStyle();
	// ofTranslate(DISPW, 0);
	// ofSetMatrixMode(OF_MATRIX_MODELVIEW);
 //    ofMultMatrix(projector_transpose);
	// drawProjector();
	// ofPopStyle();
	// ofPopMatrix();

	/* Draw debug info */
	sendToServer();
	ofPushMatrix();
	ofPushStyle();

	drawDebug();
	ofPopStyle();
	ofPopMatrix();


}


void ofApp::sendToServer(){
	json j;
	for(int i = 0; i < trackers.size(); i++){
		auto &t = trackers[i];
		for(auto &entry : t.touches){
			auto &touch = entry.second; 
			ofPoint p = homo(touch.tip);
			j.push_back({{"px", p.x}, {"py", p.y}, {"finger", t.finger},{"state", t.states[entry.first]}});
		}
	}
	// record touch Down
	// for(std::set<int>::iterator it = touchDown.begin(); it != touchDown.end(); ++it){
	// 	ofPoint p = homo(touchMap[*it].tip);
	// 	if(touchMap[*it].touched){
	// 		j.push_back({{"px", p.x}, {"py", p.y}, {"finger", 1}});
	// 	}
	// }

	// record all points
	// for(auto &entry : touchMap) {
	// 	auto &touch = entry.second;
	// 	ofPoint p = homo(touch.tip);
	// 	if(touch.touched) {
	// 		j.push_back({{"px", p.x}, {"py", p.y}, {"finger", 1}});
	// 	}
	// }
	// ofLogWarning("BasicTest_ofApp")<<touchMap.size();
	server.send(j.dump());
}
//--------------------------------------------------------------
void ofApp::teardown() {
	/* Destroy everything cleanly. */
	for(auto &t: trackers){
		delete t.tracker;
	}

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
