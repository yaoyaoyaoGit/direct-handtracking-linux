#pragma once

#include "ofMain.h"
#include "ofxFreenect2.h"
class ofApp : public ofBaseApp{
	ofxFreenect2::Device* device;
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);

		void teardown();
};
