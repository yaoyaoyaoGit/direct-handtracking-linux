#include "ofMain.h"
#include "ofxFreenect2.h"
using namespace ofxFreenect2;

Stream::Stream(){
	ofLogVerbose("ofxFreenect2") <<"Device Constructor";


}
Stream::~Stream(){
	close();
}
bool Stream::setup(){

	ofLogVerbose("ofxFreenect2") <<"Device Set up stage";
	/// [discovery]
	if(freenect2.enumerateDevices() == 0)
	{
	  std::cout << "no device connected!" << std::endl;
	  return -1;
	}

	if(serial == "")
	{
	  serial = freenect2.getDefaultDeviceSerialNumber();
	}

	width = 512;
	height = 424;
	bOpened = false;
	bNewBuffer = false;
	bNewFrame = false;
	lastFrameNo = -1;
	/// [discovery]

	return true;	

}
bool Stream::open(){
	close();
	dev = freenect2.openDevice(serial);

	if(!dev){
		ofLogError("ofxFreenect2") << "can't open device: " << freenect2.getDefaultDeviceSerialNumber() << std::endl;
		return false;
	}

	int types = 0;
	// only IR and Depth IMAGE needed
	// types |= libfreenect2::Frame::Ir | libfreenect2::Frame::Depth | libfreenect2::Frame::Color;
	types |= libfreenect2::Frame::Ir | libfreenect2::Frame::Depth;

	listener = new libfreenect2::SyncMultiFrameListener(types);
	
	libfreenect2::FrameMap frames;
	// dev->setColorFrameListener(listener);
	dev->setIrAndDepthFrameListener(listener);

	//start the device 
	if(!dev->startStreams(false, true)){
		ofLogError("ofxFreenect2") << "Device start failed" << std::endl;
		return false;		
	}
	ofLogVerbose("ofxFreenect2")  << "device start succeed" << std::endl;
	ofLogVerbose("ofxFreenect2") << "device serial:"<< dev->getSerialNumber() << std::endl;
	ofLogVerbose("ofxFreenect2") << "device firmware:"<< dev->getFirmwareVersion() << std::endl;
	if(lastFrameNo == -1)
		startThread(true);
	else{
		ofLogError("ofxFreenect2") << "frame num error" << std::endl;
		return false;
	}

	irParams = dev->getIrCameraParams();

	bOpened = true;
	return true;
}

void Stream::threadedFunction(){
	while(isThreadRunning()){
		// ofLogVerbose("ofxFreenect2") << "isThreadRunning" << std::endl;
		listener->waitForNewFrame(frames);

		libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
		libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

		dbDepthPixels.getBackBuffer().setFromPixels((float*)depth->data, width, height, 1);
		dbIrPixels.getBackBuffer().setFromPixels((float*)ir->data, width, height, 1);
		
		dbDepthPixels.swap();
		dbIrPixels.swap();

		listener->release(frames);
		lock();
		bNewBuffer = true;
		unlock();
	}
}

void Stream::update(){
	if( ofGetFrameNum() != lastFrameNo){
		bNewFrame = false;
		lastFrameNo = ofGetFrameNum();
	}
	if(bNewBuffer){
		lock();
		rawIrPixels = dbIrPixels.getFrontBuffer();
		rawDepthPixels = dbDepthPixels.getFrontBuffer();
		unlock();

		if(rawDepthPixels.size() > 0){
			if(depthPixels.getWidth() != rawDepthPixels.getWidth())
				depthPixels.allocate(rawDepthPixels.getWidth(), rawDepthPixels.getHeight(), 1);
			unsigned short* pixels = depthPixels.getData();
			float fmax = std::numeric_limits<float>::min();
			float fmin = std::numeric_limits<float>::max();
			for(unsigned int i = 0; i < depthPixels.size(); i++){
				if(fmax < rawDepthPixels[i])
					fmax = rawDepthPixels[i];
				if(fmin > rawDepthPixels[i])
					fmin = rawDepthPixels[i];
			} 
			for(int i = 0; i < depthPixels.size(); i++){
				// pixels[i] = (float)(rawDepthPixels[i]-fmin)/(fmax-fmin)*65535.;
				pixels[i] = rawDepthPixels[i];
			}
		}
		if(rawIrPixels.size() > 0){
			if(irPixels.getWidth() != rawIrPixels.getWidth())
				irPixels.allocate(rawIrPixels.getWidth(), rawIrPixels.getHeight(), 1);
			unsigned short* pixels = irPixels.getData();
			float fmax = std::numeric_limits<float>::min();
			float fmin = std::numeric_limits<float>::max();
			for(unsigned int i = 0; i < irPixels.size(); i++){
				if(fmax < rawIrPixels[i])
					fmax = rawIrPixels[i];
				if(fmin > rawIrPixels[i])
					fmin = rawIrPixels[i];
			} 
			for(int i = 0; i < irPixels.size(); i++){
				// pixels[i] = (float)(rawIrPixels[i]-fmin)/(fmax-fmin)*65535.;
				pixels[i] = rawIrPixels[i];
			}

		}
		bNewFrame = true;
	}
}

void Stream::getPointXYZ(int r, int c, float dz, float &x, float &y, float &z){
  const float bad_point = std::numeric_limits<float>::quiet_NaN();
  const float cx(irParams.cx), cy(irParams.cy);
  const float fx(1/irParams.fx), fy(1/irParams.fy);
  const float depth_val =dz/1000.0f; //scaling factor, so that value of 1 is one meter.
  if (isnan(depth_val) || depth_val <= 0.001)
  {
    //depth value is not valid
    x = y = z = bad_point;
  }
  else
  {
    x = (c + 0.5 - cx) * fx * depth_val;
    y = (r + 0.5 - cy) * fy * depth_val;
    z = depth_val;
  }	
}

void Stream::close(){
	if(bOpened){
		waitForThread(true);
		listener->release(frames);
		delete listener;
		listener = NULL;
		if(dev){
		if(!dev->stop()){
			ofLogError("ofxFreenect2")  << "device can't be stopped " << std::endl;
		}
		if(!dev->close()){
			ofLogVerbose("ofxFreenect2")  << "device can't be closed" << std::endl;		
		}
	}
	}

	bOpened = false;
}

