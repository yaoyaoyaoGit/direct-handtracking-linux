#pragma once

#include "ofMain.h"
#include "ofxFreenect2.h"
#include "BackgroundUpdaterThread.h"
class BaseApp : public ofBaseApp{

	public:
		void setup();
		void update();

		ofxFreenect2::Stream* stream;

		class BackgroundUpdaterThread *bgthread;

		ofPoint getWorldPoint(const ofVec2f &depthPt, bool live);
		ofPoint getBackgroundWorldPoint(const ofVec2f &depthPt) { return getWorldPoint(depthPt, false); }
		ofPoint getLiveWorldPoint(const ofVec2f &depthPt) { return getWorldPoint(depthPt, true); }

		void setupWindow();
		void setupKinect();
		
		virtual void teardown();

		void keyPressed(int key);
		void keyReleased(int key);
};
