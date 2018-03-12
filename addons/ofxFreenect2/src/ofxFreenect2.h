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

namespace ofxFreenect2{
	class Device;
}

class ofxFreenect2::Device{
public:

/// [context]
  libfreenect2::Freenect2 freenect2;
  libfreenect2::Freenect2Device *dev = 0;
  libfreenect2::PacketPipeline *pipeline = 0;
  std::string serial = "";
/// [context]

	Device();
	~Device();
	bool setup();

	void exit();

	void update();

	bool isOpen() const{
		return bOpen;
	}
private:
	bool bOpen;
};
#endif //OFX_FREENECT2_H