#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include <iostream> 
#include <cmath>
#include <string>
#include <sstream>

#include <vector>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _DEBUG
#define CV_EXT "d.lib"
#else
#define CV_EXT ".lib"
#endif


#if _WIN64 || __amd64__
#pragma comment(lib, "opencv_world320" CV_EXT) 
#else
#define CV_VERSION_STR CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)

// ライブラリのリンク（不要な物はコメントアウト）
#pragma comment(lib, "opencv_core"            CV_VERSION_STR CV_EXT)
#pragma comment(lib, "opencv_highgui"        CV_VERSION_STR CV_EXT)
#pragma comment(lib, "opencv_imgproc"  CV_VERSION_STR CV_EXT)
#endif


using namespace std;
using namespace cv;

void img_info(Mat &img);
int int_random(int max);
double uniform_random(double min, double max);
double gaussian_random(double mean, double std);
void LF_noise_generation(vector<double> &raw_data, long screen_num_x, long screen_num_y);
void LF_noise_generation(vector<vector<double>> &raw_data, long screen_num_x, long screen_num_y);

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofParameter<string> screenSize;
	ofParameter<string> average_depth;
	ofParameter<bool>	doFullScreen;
	ofImage image_out;
	ofxPanel gui;
	ofParameter<bool> bHide;
	ofxFloatSlider p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19;
	ofxFloatSlider p20,p21,p22,p23,p24,p25,p26;


	ofParameter<bool>	auto_dense;



	Mat img_proc;
	Mat img_proc_scale;

	bool event_flg;

	void				setFullScreen(bool& _value) { ofSetFullscreen(_value); }

};