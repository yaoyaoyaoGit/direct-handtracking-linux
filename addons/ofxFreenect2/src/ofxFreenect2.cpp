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
			for(int i = 0; i < depthPixels.size(); i++){
				pixels[i] = rawDepthPixels[i];
				// if(i % 1000 == 0){
				// 	ofLogVerbose("ofxFreenect2") << float(rawDepthPixels[i]) << " " << pixels[i] <<std::endl;
				// } 
				// pixels[i] = ofMap(rawDepthPixels[i], 500, 6000, 255, 0, true);
				// if(pixels[i] == 255){
				// 	pixels[i] = 0;
				// }
			}
		}
		if(rawIrPixels.size() > 0){
			if(irPixels.getWidth() != rawIrPixels.getWidth())
				irPixels.allocate(rawIrPixels.getWidth(), rawIrPixels.getHeight(), 1);
			unsigned short* pixels = irPixels.getData();
			for(int i = 0; i < irPixels.size(); i++){
				pixels[i] = rawIrPixels[i];
				// if(pixels[i] == 65535){
				// 	ofLogVerbose("ofxFreenect2") << float(rawIrPixels[i])<<" "<< int(rawIrPixels[i]) << std::endl;
				// 	ofSleepMillis(1000);					
				// }
				// pixels[i] = ofMap(rawIrPixels[i], 0, 65535, 0, 1024, true);
				// if(pixels[i] == 255){
				// 	pixels[i] = 0cdD	t;
				// }
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

