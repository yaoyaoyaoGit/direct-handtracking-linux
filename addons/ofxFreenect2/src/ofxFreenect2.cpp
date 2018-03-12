#include "ofMain.h"
#include "ofxFreenect2.h"

using namespace ofxFreenect2;

Device::Device(){
	ofLogVerbose("ofxFreenect2") <<"Device Constructor";
	bOpen = false;
}

bool Device::setup(){
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
	/// [discovery]	

	dev = freenect2.openDevice(serial);

	if(!dev){
		ofLogError("ofxFreenect2") << "can't open device: " << freenect2.getDefaultDeviceSerialNumber() << std::endl;
		return false;
	}

	int types = 0;
	bool enable_rgb = true;
	bool enable_depth = true;
	if(enable_rgb)
		types |= libfreenect2::Frame::Color;
	if(enable_depth)
		types |= libfreenect2::Frame::Ir | libfreenect2::Frame::Depth;
	libfreenect2::SyncMultiFrameListener listener(types);
	libfreenect2::FrameMap frames;

	dev->setColorFrameListener(&listener);

	dev->setIrAndDepthFrameListener(&listener);

	//start the device 

	if(enable_rgb && enable_depth){
		if(!dev->start()){
			ofLogError("ofxFreenect2") << "Device start failed" << std::endl;
			return false;
		}
	}
	else{
		if(!dev->startStreams(enable_rgb, enable_depth)){
			ofLogError("ofxFreenect2") << "Device start failed" << std::endl;
			return false;
		}
	}
	ofLogVerbose("ofxFreenect2")  << "device start succeed" << std::endl;
	ofLogVerbose("ofxFreenect2") << "device serial:"<< dev->getSerialNumber() << std::endl;
	ofLogVerbose("ofxFreenect2") << "device firmware:"<< dev->getFirmwareVersion() << std::endl;
	return true;
}


void Device::exit(){
	if(dev){
		if(!dev->stop()){
			ofLogError("ofxFreenect2")  << "device can't be stopped " << std::endl;
		}
		if(!dev->close()){
			ofLogVerbose("ofxFreenect2")  << "device can't be closed" << std::endl;		
		}
	}
	bOpen = false;
}
