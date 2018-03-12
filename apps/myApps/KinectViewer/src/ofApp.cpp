#include "ofApp.h"
#include "ofxFreenect2.h"
//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	device = new ofxFreenect2::Device();
	device->setup();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == OF_KEY_ESC) {
		teardown();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

void ofApp::teardown(){
	device->exit();
}
