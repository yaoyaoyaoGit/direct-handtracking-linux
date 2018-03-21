#ifndef OFX_FREENECT2_H
#define OFX_FREENECT2_H

#include "ofMain.h"
/// [headers]
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
/// [headers]
#include <DoubleBuffer.h>
namespace ofxFreenect2{
	class Stream;

	template<class Interface>
	inline void safe_release(Interface *& p_release)
	{
		if(p_release){
			p_release->Release();
			p_release = NULL;
		}
	}
}

class ofxFreenect2::Stream: public ofThread{
public:
	Stream();
	~Stream();
	bool setup();

	bool open();

	void close();

	void update();

	bool isOpen() const{
		return bOpened;
	}

	bool isFrameNew() const{
		return bNewFrame;
	}
	// ofPixels& getPixels() { return pix.getFrontBuffer(); }
	// const ofPixels& getPixels() const { return pix.getFrontBuffer(); }
	// ofPixels& getDepthPixels(){return depthPixels;}
	// const ofPixels& getDepthPixels() const { return depthPixels;}
	ofShortPixels& getDepthPixels(){return depthPixels;}
	const ofShortPixels& getDepthPixels() const { return depthPixels;}
	// ofPixels& getIrPixels() { return irPixels;}
	// const ofPixels& getIrPixels() const {return irPixels;}
	ofShortPixels& getIrPixels() { return irPixels;}
	const ofShortPixels& getIrPixels() const {return irPixels;}
	ofFloatPixels& getRawIrPixels() { return rawIrPixels;}
	const ofFloatPixels& getRawIrPixels() const {return rawIrPixels;}

	void getPointXYZ(int r, int c, float dv, float &x, float &y, float &z);
	int width;
	int height;

protected:
	void threadedFunction();
private:

	bool bOpened;
	bool bNewFrame;
	bool bNewBuffer;
	
	ofShortPixels depthPixels;
	ofShortPixels irPixels;

	ofFloatPixels rawDepthPixels;
	ofFloatPixels rawIrPixels;

	DoubleBuffer<ofFloatPixels> dbDepthPixels;
	DoubleBuffer<ofFloatPixels> dbIrPixels;

	int lastFrameNo;
/// [context]
	libfreenect2::Freenect2 freenect2;
	libfreenect2::Freenect2Device *dev = 0;
	libfreenect2::PacketPipeline *pipeline = 0;
	std::string serial = "";
/// [context]

	libfreenect2::SyncMultiFrameListener *listener;
	libfreenect2::FrameMap frames;

	libfreenect2::Registration* registration;

	libfreenect2::Freenect2Device::IrCameraParams irParams;
	// libfreenect2::Frame* depth;
	// libfreenect2::Frame* ir;	
};
#endif //OFX_FREENECT2_H