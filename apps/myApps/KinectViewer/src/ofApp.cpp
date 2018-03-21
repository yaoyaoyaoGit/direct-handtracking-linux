#include "ofApp.h"
#include "ofxFreenect2.h"
//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	stream = new ofxFreenect2::Stream();
	stream->setup();
	stream->open();

	const int dw = stream->width;
	const int dh = stream->height;
	depthviz.allocate(dw, dh, OF_IMAGE_GRAYSCALE);
	// irviz.allocate(dw,dh,GL_R32F);
	irviz.allocate(dw,dh,OF_IMAGE_GRAYSCALE);
}

//--------------------------------------------------------------
void ofApp::update(){
	stream->update();
	// ofSleepMillis(1000);
	if(stream->isFrameNew()){
			/* Debugging */
		unsigned short *depthpx = stream->getDepthPixels().getData();
		const int dw = stream->width;
		const int dh = stream->height;
		unsigned char *depthvizpx = depthviz.getPixels().getData();
		/* Convert depth data for visualization purposes */
		unsigned short max = 0;
		unsigned short min = 65535;
		for(int i=0; i<dw*dh; i++) {
			if(depthpx[i] > max){
				max = depthpx[i];
			} 
			if(depthpx[i] < min)
				min = depthpx[i];
		}
		// ofLogVerbose("xxx")<<max<<" "<<min<<std::endl;
		for(int i = 0; i < dw * dh; i++){
			// depthvizpx[i] = (float)(depthpx[i]-min)/(max-min)*255.;
			depthvizpx[i] = depthpx[i];
		}

		depthviz.update();

		unsigned short *irpx = stream->getIrPixels().getData();
		// irviz.loadData(irpx,dw,dh, GL_FLOAT);
		unsigned char *irvizpx = irviz.getPixels().getData();
		/* Convert IR data for visualization purposes */

		max = 0;
		min = 65535;
		for(int i=0; i<dw*dh; i++) {
			if(irpx[i] > max){
				max = irpx[i];
			} 
			if(irpx[i] < min)
				min = irpx[i];
		}

		for(int i=0; i<dw*dh; i++){
			// irvizpx[i] = (float)(irpx[i]-min)/(max-min)*255.;
			irvizpx[i] = irpx[i];
			// if (i % 1000 == 0)
			// 	ofLogVerbose("ofxFreenect2") << int(irvizpx[i])<< std::endl;
		}
		// ofLogVerbose("ofxFreenect2")<<irviz.getPixels().getImageType()
		// <<" "<<irviz.getPixels().getBytesPerPixel()<<std::endl;
		irviz.update();

	}
}

//--------------------------------------------------------------
void ofApp::draw(){
		const int dw = stream->width;
		const int dh = stream->height;
		depthviz.draw(0, 0);
		irviz.draw(dw,0);
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
	delete stream;
	stream = NULL;
}
