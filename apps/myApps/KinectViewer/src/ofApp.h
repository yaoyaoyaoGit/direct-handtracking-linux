#pragma once

#include "ofMain.h"
#include "ofxFreenect2.h"
class ofApp : public ofBaseApp{
private:
	ofxFreenect2::Stream* stream;
	ofImage depthviz;
	// ofTexture irviz;
	ofImage irviz;
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);

		void teardown();
};
