//
//  geomConfig.h
//  Calibration and screen settings shared by all apps
//
//  Created by Robert Xiao on April 9, 2015.
//
//

#pragma once

#include "ofMath.h"
#include "opencv2/core.hpp"
/* Projector display is assumed to be to the left of the main display */
#define PROJW 1280
#define PROJH 800

#define DISPW 1920
#define DISPH 1080

/* Matrix calibration from calibrate.py */
// const static ofMatrix4x4 projector_projmat(
// 		56.840041f,-0.120907f,22.803011f,19.316478f,
// 		0.561777f,58.463123f,23.164536f,-6.195360f,
// 		-0.000581f,0.001136f,0.027288f,-0.029459f,
// 		-0.000475f,0.000929f,0.022327f,0.016536f);


// [ -2.8831616641030172e-01, 1.1301993723283843e+01,
//        -1.5395506653977316e+03, 4.2471822948277076e+00,
//        1.4157274316092319e-01, -4.8262148143558858e+02,
//        3.1741148447238158e-05, 2.2057945556344647e-04, 1. ]
const static ofMatrix4x4 projector_projmat(
	-2.883161e-1f, 1.130199e+01f, 0.0f, -1.539550e+03f,
	4.247182e0f, 1.415727e-01f, 0.0f, -4.8262148143558858e+02f,
	0.0f,0.0f,1.0f,0.0f,
	3.174114e-5f, 2.205794e-04f, 0.0f, 1.f);
const static ofMatrix4x4 projector_transpose = ofMatrix4x4::getTransposedOf(projector_projmat);
// const static ofMatrix4x4 projector_transpose = ofMatrix4x4::getTransposedOf(projector_projmat);

const static ofPoint homo(ofPoint p){
	cv::Mat hp = (cv::Mat_<float>(3,1) << p.x, p.y, 1);
	ofLogVerbose("geomConfig")<< hp;
	// float m[9] = {-2.8831616641030172e-01, 1.1301993723283843e+01,
 //       -1.5395506653977316e+03, 4.2471822948277076e+00,
 //       1.4157274316092319e-01, -4.8262148143558858e+02,
 //       3.1741148447238158e-05, 2.2057945556344647e-04, 1. };
float m[9] = {7.785847747884422, -0.4671508648730665, -1512.32306654907,
 0.5325922184357827, 8.182553174149763, -1302.768983768329,
 6.713830772693987e-05, 5.195805728479092e-05, 1};



	cv::Mat hm(3, 3, cv::DataType<float>::type, m);
	// ofLogVerbose("geomConfig")<< hm;
	cv::Mat ret = hm * hp;
	return ofPoint(ret.at<float>(0,0)/ret.at<float>(2,0), PROJH - ret.at<float>(1,0)/ret.at<float>(2,0));
}