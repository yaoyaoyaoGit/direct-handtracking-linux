#pragma once

#include "ofMain.h"
#include "BaseApp.h"
#include "Touch.h"
#include "ofxLibwebsockets.h"

struct TouchTrackerWrapper {
	string name;
	map<int, FingerTouch> touches;
	map<int, int> states;
	int finger;
	class TouchTracker *tracker;
};

class ofApp : public BaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);

		vector<TouchTrackerWrapper> trackers;
		void handleTouches(const vector<FingerTouch> &newTouches);
		void drawProjector();

		void drawDebug();

		void setupDebug();

		void updateDebug();

		void teardown();

		void sendToServer();
		/* Debugging */
		ofImage depthviz;
		ofImage irviz;
		uint64_t lastDepthTimestamp;
		int curDepthFrame;

        ofxLibwebsockets::Server server;
        bool bSetup;
        set<int> touchDown;		
};
