#include "ofApp.h"

double img_dpi = 4800;						//解像度
double um2pix = img_dpi / 25.4 / 1000.0;	//um->pixel変換係数
double mm2pix = img_dpi / 25.4;				//mm->pixel変換係数
double pix2um = 1.0 / um2pix;				//pixel->mm変換係数
double pix2mm = 1.0 / mm2pix;				//pixel->mm変換係数

double img_width_mm = 5;					//画像幅
double img_height_mm = 5;					//画像高さ

double  HT_width_mm = img_width_mm;		//
double  HT_height_mm = img_width_mm;	//

double gray_white = 0;						//dotない箇所の濃度
double gray_black_ave = 228;				//dotの濃度ave
double gray_black_std = 0;				//dotの濃度σ

double dot_circle_r_ave = 80 * um2pix;			//dot径ave:dotを円で描画する。
double dot_circle_r_std = 0.0*um2pix;			//dot径σ:

double dot_r_latent_um = 0;						//
double dot_r_latent_thresh_gain = 0;			//
double dot_tc_num = 0;							//
double dot_toner_r = 5.0* um2pix;				//

double xx_sin_amp = 0.0*um2pix;					//
double xx_sin_pitch = 10.0;						//
double yy_sin_amp = 3.0*um2pix;					//
double yy_sin_pitch = 1.0;						//

double yy_noise_std = 1.0*um2pix;			//プロセス・ラテラル方向にY方向ノイズを印加する。
double xx_noise_std = 1.0*um2pix;			//プロセス・ラテラル方向にX方向ノイズを印加する。
double yy_noise_std_pro = 0.0;				//プロセス方向にY方向ノイズを印加する。
double xx_noise_std_pro=0.0;				//プロセス方向にX方向ノイズを印加する。

const double screen_grid_um = 25.4  * 1000.0 * 16.0 / 2400.0;





void image_prosess(Mat &img_proc) {			//ドットを作像する処理
	
	cout << "img_dpi" << img_dpi << endl;
	cout << "img_width_mm" << img_width_mm << endl;
	cout << "img_height_mm" << img_height_mm << endl;
	cout << "HT_width_mm" << HT_width_mm << endl;
	cout << "HT_height_mm" << HT_height_mm << endl;
	cout << "gray_white" << gray_white << endl;
	cout << "gray_black_ave" << gray_black_ave << endl;
	cout << "gray_black_std" << gray_black_std << endl;
	cout << "dot_circle_r_ave" << dot_circle_r_ave << endl;
	cout << "dot_circle_r_std" << dot_circle_r_std << endl;
	cout << "dot_r_latent_um" << dot_r_latent_um << endl;
	cout << "dot_r_latent_thresh_gain" << dot_r_latent_thresh_gain << endl;
	cout << "dot_tc_num" << dot_tc_num << endl;
	cout << "dot_toner_r" << dot_toner_r << endl;
	cout << "xx_sin_amp" << xx_sin_amp << endl;
	cout << "xx_sin_pitch" << xx_sin_pitch << endl;
	cout << "yy_sin_amp" << yy_sin_amp << endl;
	cout << "yy_sin_pitch" << yy_sin_pitch << endl;
	cout << "yy_noise_std" << yy_noise_std << endl;
	cout << "xx_noise_std" << xx_noise_std << endl;
	cout << "yy_noise_std_pro" << yy_noise_std_pro << endl;
	cout << "xx_noise_std_pro" << xx_noise_std_pro << endl;

	

	um2pix = img_dpi / 25.4 / 1000.0;		//係数の作成
	mm2pix = img_dpi / 25.4;
	pix2um = 1.0 / um2pix;
	pix2mm = 1.0 / mm2pix;

	img_proc = 255 - gray_white;			//画像の初期化
	img_info(img_proc);						//画像情報の表示

	//メモリ領域の確保
	long screen_num_x = HT_width_mm* mm2pix / screen_grid_um / um2pix;
	long screen_num_y = HT_height_mm* mm2pix / screen_grid_um / um2pix;

	//cout << "dot num x:" << screen_num_x << endl;
	//cout << "dot num y:" << screen_num_y << endl;

	vector<vector<double>> screen_x_um(screen_num_y * 2, vector<double>(screen_num_x));
	vector<vector<double>> screen_y_um(screen_num_y * 2, vector<double>(screen_num_x));
	vector<vector<double>> screen_y_um_noise(screen_num_y * 2, vector<double>(screen_num_x));
	vector<vector<double>> screen_x_um_noise(screen_num_y * 2, vector<double>(screen_num_x));
	vector<double> screen_y_um_noise_pro(screen_num_y * 2);
	vector<double> screen_x_um_noise_pro(screen_num_y * 2);
	vector<vector<double>> screen_r_um(screen_num_y * 2, vector<double>(screen_num_x));
	vector<vector<double>> screen_r_dense(screen_num_y * 2, vector<double>(screen_num_x));
	
	for (long j = 0; j < screen_num_y; j++) {//プロセス方向にノイズを印加する。
		screen_y_um_noise_pro[2 * j]		= gaussian_random(0.0, yy_noise_std_pro);
		screen_y_um_noise_pro[2 * j + 1]	= gaussian_random(0.0, yy_noise_std_pro);
		screen_x_um_noise_pro[2 * j]		= gaussian_random(0.0, xx_noise_std_pro);
		screen_x_um_noise_pro[2 * j + 1]	= gaussian_random(0.0, xx_noise_std_pro);
	}

	for (long j = 0; j < screen_num_y; j++) {
		for (long i = 0; i < screen_num_x; i++) {
			//スクリーンの中心座標を定義する。
			//K色スクリーンを模擬

			double xx = screen_grid_um*i;
			double yy = screen_grid_um*j;
			screen_x_um[2 * j][i] = xx;
			screen_y_um[2 * j][i] = yy;
			screen_x_um[2 * j + 1][i] = xx + screen_grid_um / 2.0;
			screen_y_um[2 * j + 1][i] = yy + screen_grid_um / 2.0;

			double y_um_del1 = gaussian_random(0.0, yy_noise_std);
			double y_um_del2 = gaussian_random(0.0, yy_noise_std);

			screen_y_um_noise[2 * j][i] = y_um_del1;
			screen_y_um_noise[2 * j + 1][i] = y_um_del2;

			double x_um_del1 = gaussian_random(0.0, xx_noise_std);
			double x_um_del2 = gaussian_random(0.0, xx_noise_std);

			screen_x_um_noise[2 * j][i] = x_um_del1;
			screen_x_um_noise[2 * j + 1][i] = x_um_del2;

			double dot_r1 = gaussian_random(dot_circle_r_ave / 2.0, dot_circle_r_std);
			double dot_r2 = gaussian_random(dot_circle_r_ave / 2.0, dot_circle_r_std);
			if (dot_r1 < 0) dot_r1 = 0.0;
			if (dot_r2 < 0) dot_r2 = 0.0;

			screen_r_um[2 * j][i] = dot_r1;
			screen_r_um[2 * j + 1][i] = dot_r2;

			double dot_dense1 = gaussian_random(gray_black_ave, gray_black_std);
			double dot_dense2 = gaussian_random(gray_black_ave, gray_black_std);
			if (dot_dense1 < 0)dot_dense1 = 0;
			if (dot_dense1 > 255)dot_dense1 = 255;
			if (dot_dense2 < 0)dot_dense2 = 0;
			if (dot_dense2 > 255)dot_dense2 = 255;

			screen_r_dense[2 * j][i] = dot_dense1;
			screen_r_dense[2 * j + 1][i] = dot_dense2;

			if (i == screen_num_x - 1) {
				double percent = (double)j / screen_num_y / 2.0;
				cout << "Progress :" << percent*100.0 << " %_\r";
			}
		}
	}

	LF_noise_generation(screen_r_um, screen_num_x, screen_num_y);
	LF_noise_generation(screen_r_dense, screen_num_x, screen_num_y);
	LF_noise_generation(screen_y_um_noise, screen_num_x, screen_num_y);
	LF_noise_generation(screen_x_um_noise, screen_num_x, screen_num_y);
	LF_noise_generation(screen_x_um_noise_pro, screen_num_x, screen_num_y);
	LF_noise_generation(screen_y_um_noise_pro, screen_num_x, screen_num_y);
	cout << "circle_generation" << endl;


	for (long j = 0; j < screen_num_y * 2; j++) {
		for (long i = 0; i < screen_num_x; i++) {
			double xx_um = screen_x_um[j][i] + screen_x_um_noise[j][i] + screen_x_um_noise_pro[j];
			double yy_um = screen_y_um[j][i] + screen_y_um_noise[j][i] + screen_y_um_noise_pro[j];

			double xx_delt = xx_sin_amp*sin(2.0*M_PI*yy_um / 1000.0 / xx_sin_pitch);
			xx_um += xx_delt;

			double yy_delt = yy_sin_amp*sin(2.0*M_PI*yy_um / 1000.0 / yy_sin_pitch);
			yy_um += yy_delt;

			double dot_r = screen_r_um[j][i];
			int dot_dense = 255 - screen_r_dense[j][i];

			if (dot_r != 0)	cv::circle(img_proc, cv::Point(xx_um*um2pix, yy_um*um2pix), dot_r, cv::Scalar(dot_dense), -1, CV_AA);


			for (int i = 0; i < dot_tc_num; i++) {
				double dxx = gaussian_random(0, dot_r_latent_um);
				double dyy = gaussian_random(0, dot_r_latent_um);
				double xx_um2 = xx_um + dxx;
				double yy_um2 = yy_um + dyy;
				double drr = dxx*dxx + dyy*dyy;
				double drr2 = dot_r_latent_thresh_gain*dot_r_latent_um*um2pix;
				if (drr < drr2*drr2)
					cv::circle(img_proc, cv::Point(xx_um2*um2pix, yy_um2*um2pix), dot_toner_r, cv::Scalar(dot_dense), -1, CV_AA);
			}

			if (i == screen_num_x - 1) {
				double percent = (double)j / screen_num_y / 2.0;
				cout << "Progress :" << percent*100.0 << " %_\r";
			}
		}
	}




}

//--------------------------------------------------------------
void ofApp::setup() {
	gui.setup("parametor");
	gui.add(doFullScreen.set("fullscreen (F)", false));

	gui.add(screenSize.set("screenSize", ""));
	gui.add(average_depth.set("average_depth", ""));
	gui.add(auto_dense.set("auto_dense", false));

	gui.add(p11.setup("img_dpi", 7250, 1200, 15000));

	gui.add(bHide.set("GuiHide(h)", false));
	gui.add(p1.setup("dot_circle_r_ave", 70, 0, 200));
	gui.add(p2.setup("dot_circle_r_std", 0, 0, 20));
	gui.add(p3.setup("dot_density_ave", 50, 0, 255));
	gui.add(p4.setup("dot_density_stdev", 0, 0, 100));

	gui.add(p5.setup("yy_sin_amp", 0, 0, 100));
	gui.add(p6.setup("yy_sin_pitch", 1, 0.1, 10));
	gui.add(p7.setup("xx_sin_amp", 0, 0, 100));
	gui.add(p8.setup("xx_sin_pitch", 1, 0.1, 10));

	gui.add(p9.setup("image_size", 5, 1, 15));
	//gui.add(p10.setup("dot_y_stdev", 0, 0, 50));

	gui.add(p16.setup("dot_r_latent_um", 20.0, 0.0, 50));
	gui.add(p17.setup("dot_tc_num", 0.0, 0.0, 1000));
	gui.add(p18.setup("dot_thresh_std", 1.0, 0.0, 5.0));
	gui.add(p19.setup("dot_toner_r", 1.0, 0.0, 10.0));

	gui.add(p20.setup("yy_noise_std", 0.0, 0.0, 50));
	gui.add(p21.setup("xx_noise_std", 0.0, 0.0, 50));
	gui.add(p22.setup("yy_noise_std_pro", 0.0, 0.0, 50));
	gui.add(p23.setup("xx_noise_std_pro", 0.0, 0.0, 50));


	doFullScreen.addListener(this, &ofApp::setFullScreen);
	gui.loadFromFile("settings.xml");

	img_dpi = p11;

	//画像の作成
	double img_width = p9* mm2pix;
	img_proc = Mat::zeros(img_width, img_width, CV_8U);

	image_prosess(img_proc);
	image_out.allocate(img_proc.cols, img_proc.rows, OF_IMAGE_GRAYSCALE);
	image_out.setFromPixels(img_proc.ptr(), img_proc.cols, img_proc.rows, OF_IMAGE_GRAYSCALE, false);
	event_flg = 0;



}

//--------------------------------------------------------------
void ofApp::update() {


	static double dot_circle_r_ave_old;
	static double dot_circle_r_std_old;
	static double gray_black_ave_old;
	static double gray_black_std_old;

	static double dot_circle_r_ave2_old;
	static double dot_circle_r_std2_old;
	static double gray_black_ave2_old;
	static double gray_black_std2_old;

	static double yy_sin_amp_old;
	static double yy_sin_pitch_old;
	static double xx_sin_amp_old;
	static double xx_sin_pitch_old;
	static double img_width_mm_old;
	static double dot_y_std_old;
	static double img_dpi_old;
	static double dot_r_latent_um_old;
	static double dot_r_latent_thresh_gain_old;
	static double dot_tc_num_old;
	static double dot_toner_r_old;

	static double yy_noise_std_old;
	static double xx_noise_std_old;
	static double yy_noise_std_pro_old;
	static double xx_noise_std_pro_old;


	dot_circle_r_ave = p1*um2pix;
	dot_circle_r_std = p2*um2pix;
	gray_black_ave = 255 - p3;

	gray_black_std = p4;
	dot_r_latent_um = p16;
	dot_r_latent_thresh_gain = p18;
	dot_tc_num = p17;
	yy_sin_amp = p5;
	yy_sin_pitch = p6;
	xx_sin_amp = p7;
	xx_sin_pitch = p8;
	img_width_mm = p9;
	//dot_y_stdev = p10;
	img_dpi = p11;
	dot_toner_r = p19*um2pix;

	yy_noise_std = p20*um2pix;
	xx_noise_std = p21*um2pix;
	yy_noise_std_pro = p22*um2pix;
	xx_noise_std_pro = p23*um2pix;


	if (dot_circle_r_ave != dot_circle_r_ave_old)	event_flg = 1;
	if (dot_circle_r_std != dot_circle_r_std_old)	event_flg = 1;
	if (gray_black_ave != gray_black_ave_old)	event_flg = 1;
	if (gray_black_std != gray_black_std_old)	event_flg = 1;
	if (yy_sin_amp_old != yy_sin_amp)	event_flg = 1;
	if (yy_sin_pitch_old != yy_sin_pitch)	event_flg = 1;
	if (xx_sin_amp_old != xx_sin_amp)	event_flg = 1;
	if (xx_sin_pitch_old != xx_sin_pitch)	event_flg = 1;
	if (img_width_mm_old != img_width_mm)	event_flg = 1;
	//if (dot_y_std_old != dot_y_stdev)	event_flg = 1;
	if (img_dpi_old != img_dpi)	event_flg = 1;
	if (dot_r_latent_um_old != dot_r_latent_um)	event_flg = 1;
	if (dot_r_latent_thresh_gain_old != dot_r_latent_thresh_gain)	event_flg = 1;
	if (dot_tc_num_old != dot_tc_num)	event_flg = 1;
	if (dot_toner_r_old != dot_toner_r)	event_flg = 1;

	if (yy_noise_std_old != yy_noise_std)	event_flg = 1;
	if (xx_noise_std_old != xx_noise_std)	event_flg = 1;
	if (yy_noise_std_pro_old != yy_noise_std_pro)	event_flg = 1;
	if (xx_noise_std_pro_old != xx_noise_std_pro)	event_flg = 1;
	

	int average_depth_val=0;

	if (event_flg) {
		cout << "test" << endl;
		event_flg = 0;

		double img_width = img_width_mm* mm2pix;
		img_proc = Mat::zeros(img_width, img_width, CV_8U);

		image_prosess(img_proc);
	//	image_out.resize(img_proc.cols, img_proc.rows);
		image_out.setFromPixels(img_proc.ptr(), img_proc.cols, img_proc.rows, OF_IMAGE_GRAYSCALE, false);
		average_depth_val = (int)mean(img_proc)[0];
		average_depth = ofToString(average_depth_val);

	}


	dot_circle_r_ave_old = dot_circle_r_ave;
	dot_circle_r_std_old = dot_circle_r_std;
	gray_black_ave_old = gray_black_ave;
	gray_black_std_old = gray_black_std;
	yy_sin_amp_old = yy_sin_amp;
	yy_sin_pitch_old = yy_sin_pitch;
	xx_sin_amp_old = xx_sin_amp;
	xx_sin_pitch_old = xx_sin_pitch;
	img_width_mm_old = img_width_mm;
	//dot_y_std_old = dot_y_stdev;
	img_dpi_old = img_dpi;
	dot_r_latent_um_old = dot_r_latent_um;
	dot_r_latent_thresh_gain_old = dot_r_latent_thresh_gain;
	dot_tc_num_old = dot_tc_num;
	dot_toner_r_old = dot_toner_r;
	
	yy_noise_std_old = yy_noise_std;
	xx_noise_std_old = xx_noise_std;
	yy_noise_std_pro_old = yy_noise_std_pro;
	xx_noise_std_pro_old = xx_noise_std_pro;

	if (auto_dense) {

		int target_depse = 220;

		for (int i = 0; i < 100; i++) {
			int sub = average_depth_val - target_depse;
			double gain = sub*0.5;
			if (gain < 1)gain = 1;

			if ((sub >= -1) && (sub <= 0)) {
				auto_dense = false;
				break;
			}
			else if (sub > 0) p3 = p3 - gain;
			else if (sub < 0) p3 = p3 + gain;

			if (p3 < 0) { 
				p3 = 0;
			auto_dense = false; 
			break;
			}
			else if (p3 > 255){
				p3 = 255;
				auto_dense = false;
				break;
		}

			gray_black_ave = 255 - p3;
			double img_width = img_width_mm* mm2pix;
			img_proc = Mat::zeros(img_width, img_width, CV_8U);
			
			image_prosess(img_proc);
			image_out.setFromPixels(img_proc.ptr(), img_proc.cols, img_proc.rows, OF_IMAGE_GRAYSCALE, false);
			average_depth_val = (int)mean(img_proc)[0];
			average_depth = ofToString(average_depth_val);
				
		}


	}

}

//--------------------------------------------------------------
void ofApp::draw() {
	image_out.draw(0, 0, ofGetHeight(), ofGetHeight());

	if (!bHide) {
		gui.draw();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	//event_flg = 1;


	if (key == 'h') {
		bHide = !bHide;
	}
	else if (key == 's') {
		gui.saveToFile("settings.xml");
	}
	else if (key == 'l') {
		gui.loadFromFile("settings.xml");
	}


	else if (key == 'f') {
		doFullScreen.set(!doFullScreen.get());
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	screenSize = ofToString(w) + "x" + ofToString(h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}



void img_info(Mat &img) {

	// 行数
	std::cout << "rows: " << img.rows << std::endl;
	// 列数
	std::cout << "cols: " << img.cols << std::endl;


	// 次元数（画像なので縦・横の2次元）
	std::cout << "dims: " << img.dims << std::endl;
	// サイズ（2次元の場合）
	std::cout << "size[]: " << img.size().width << "," << img.size().height << std::endl;
	// ビット深度ID
	std::cout << "depth (ID): " << img.depth() << "(=" << CV_8U << ")" << std::endl;
	// チャンネル数
	std::cout << "channels: " << img.channels() << std::endl;
	// （複数チャンネルから成る）1要素のサイズ [バイト単位]
	std::cout << "elemSize: " << img.elemSize() << "[byte]" << std::endl;
	// 1要素内の1チャンネル分のサイズ [バイト単位]
	std::cout << "elemSize1 (elemSize/channels): " << img.elemSize1() << "[byte]" << std::endl;
	// タイプ
	std::cout << "type (ID): " << img.type() << "(=" << CV_8UC3 << ")" << std::endl;
	// 要素の総数
	std::cout << "total: " << img.total() << std::endl;
	// ステップ数 [バイト単位]
	std::cout << "step: " << img.step << "[byte]" << std::endl;
	// 1ステップ内のチャンネル総数
	std::cout << "step1 (step/elemSize1): " << img.step1() << std::endl;
	// データは連続か？
	std::cout << "isContinuous: " << (img.isContinuous() ? "true" : "false") << std::endl;
	// 部分行列か？
	std::cout << "isSubmatrix: " << (img.isSubmatrix() ? "true" : "false") << std::endl;
	// データは空か？
	std::cout << "empty: " << (img.empty() ? "true" : "false") << std::endl;

}



// maxを最大とするint型の乱数を生成
int int_random(int max) {
	return (int)(max*(rand() / (RAND_MAX + 1.0)));
}

// minを最小，maxを最大とするdouble型の乱数を生成
double uniform_random(double min, double max) {
	return min + (rand() / (double)RAND_MAX) * (max - min);
}

// 平均mean，標準偏差stdのガウス分布の乱数を生成
double gaussian_random(double mean, double std) {
	const double norm = 1.0 / (RAND_MAX + 1.0);
	double u = 1.0 - rand() * norm;
	double v = rand() * norm;
	double z = sqrt(-2.0 * log(u)) * cos(2.0 * M_PI * v);
	return mean + std * z;
}




void LF_noise_generation(vector<double> &raw_data, long screen_num_x, long screen_num_y) {
	vector<double> filtered_data(screen_num_y * 2);

	for (long j = 1; j < screen_num_y * 2 - 1; j++) {
		double dot_r[3] = { 0 };
		dot_r[0] = raw_data[j - 1];
		dot_r[1] = raw_data[j];
		dot_r[2] = raw_data[j + 1];

		double dot_r_new = 0;
		dot_r_new += dot_r[0] / 4.0;
		dot_r_new += dot_r[1] * 2.0 / 4.0;
		dot_r_new += dot_r[2] / 4.0;

		filtered_data[j] = dot_r_new;
	}

	for (long j = 0; j < screen_num_y * 2; j++) {
		raw_data[j] = filtered_data[j];
	}

}


void LF_noise_generation(vector<vector<double>> &raw_data, long screen_num_x, long screen_num_y) {

	vector<vector<double>> filtered_data(screen_num_y * 2, vector<double>(screen_num_x));

	for (long j = 1; j < screen_num_y * 2 - 1; j++) {
		for (long i = 1; i < screen_num_x - 1; i++) {
			double dot_r[9] = { 0 };
			dot_r[0] = raw_data[j - 1][i - 1];
			dot_r[1] = raw_data[j - 1][i];
			dot_r[2] = raw_data[j - 1][i + 1];
			dot_r[3] = raw_data[j][i - 1];
			dot_r[4] = raw_data[j][i];
			dot_r[5] = raw_data[j][i + 1];
			dot_r[6] = raw_data[j + 1][i - 1];
			dot_r[7] = raw_data[j + 1][i];
			dot_r[8] = raw_data[j + 1][i + 1];

			double dot_r_new = 0;
			dot_r_new += dot_r[0] / 16.0;
			dot_r_new += dot_r[1] * 2.0 / 16.0;
			dot_r_new += dot_r[2] / 16.0;
			dot_r_new += dot_r[3] / 16.0;
			dot_r_new += dot_r[4] * 4.0 / 16.0;
			dot_r_new += dot_r[5] / 16.0;
			dot_r_new += dot_r[6] / 16.0;
			dot_r_new += dot_r[7] * 2.0 / 16.0;
			dot_r_new += dot_r[8] / 16.0;
			filtered_data[j][i] = dot_r_new;
		}
	}

	for (long j = 0; j < screen_num_y * 2; j++) {
		for (long i = 0; i < screen_num_x; i++) {
			raw_data[j][i] = filtered_data[j][i];
		}
	}

}





