#include "ofMain.h"
#include "ofApp.h"
extern double img_dpi;
extern double img_width_mm;
extern double img_height_mm;
extern double  HT_width_mm;
extern double  HT_height_mm;
extern double gray_white;
extern double gray_black_ave;
extern double gray_black_std;
extern double dot_circle_r_ave;
extern double dot_circle_r_std;
extern double dot_r_latent_um;
extern double dot_r_latent_thresh_gain;
extern double dot_tc_num;
extern double dot_toner_r;
extern double xx_sin_amp;
extern double xx_sin_pitch;
extern double yy_sin_amp;
extern double yy_sin_pitch;
extern double yy_noise_std;
extern double xx_noise_std;
extern double yy_noise_std_pro;
extern double xx_noise_std_pro;

bool save_flg_raw = 1;
bool save_flg_200dpi = 1;
bool save_flg_400dpi = 1;
bool save_flg_600dpi = 1;
bool save_flg_1200dpi = 1;
bool save_flg_2400dpi = 1;
void image_prosess(Mat &img_proc);
double ini_file_read(string lpKeyName, string ini_file);


//csvƒtƒ@ƒCƒ‹“Ç‚İ‚İ
int read_file(string fname, vector<vector<string>> & values) {
	ifstream file(fname);
	string str;
	int p;
	if (file.fail()) {
		cerr << "failed." << endl;
		return -1;
	}
	while (getline(file, str)) {
		//ƒRƒƒ“ƒg‰ÓŠ‚Íœ‚­
		if ((p = str.find("//")) != str.npos) continue;
		vector<string> inner;

		//ƒRƒ“ƒ}‚ª‚ ‚é‚©‚ğ’T‚µA‚»‚±‚Ü‚Å‚ğvalues‚ÉŠi”[
		while ((p = str.find(",")) != str.npos) {
			inner.push_back(str.substr(0, p));
			//str‚Ì’†g‚Í", "‚Ì2•¶š‚ğ”ò‚Î‚·
			str = str.substr(p + 1);
		}
		inner.push_back(str);
		values.push_back(inner);
	}
	return 0;
}


double ini_file_read(string lpKeyName, string ini_file) {
	int cnt = 1;
	vector<vector<string>> values;
	read_file(ini_file, values);
	double val = 0;
	for (unsigned int i = 0; i < values.size(); ++i) {
		if (values[i][0] == lpKeyName) {
			 val = atof(values[i][cnt].c_str());
			cout << lpKeyName << "_" << val << endl;
		}
	}
	return val;
}
void save_point(string fname_s, string &fname_ret) {

	//‚±‚±‚Åƒtƒ@ƒCƒ‹‚É‘‚«‚ñ‚Å‚¢‚­‚±‚Æ
	static string fname;
	static  time_t start_time;
	start_time = time(NULL);
	struct tm pnow;

	errno_t error;
	error = localtime_s(&pnow, &start_time);

	char buff[128] = "";
	sprintf_s(buff, "_%.2d%.2d%.2d_%.2d%.2d_%.2d", pnow.tm_year - 100, pnow.tm_mon + 1, pnow.tm_mday, pnow.tm_hour, pnow.tm_min, pnow.tm_sec);
	string buff_s = buff;
	fname = fname_s + buff_s + ".txt";
	fname_ret = fname_s + buff_s;

	ofstream fout(fname.c_str());
	double um2pix = img_dpi / 25.4 / 1000.0;	//um->pixel•ÏŠ·ŒW”


	fout << "img_dpi," << img_dpi << endl;
	fout << "img_width_mm," << img_width_mm << endl;
	fout << "img_height_mm," << img_height_mm << endl;
	fout << "HT_width_mm," << HT_width_mm << endl;
	fout << "HT_height_mm," << HT_height_mm << endl;
	fout << "gray_white," << 255.0 - gray_white << endl;
	fout << "gray_black_ave," << 255.0 - gray_black_ave << endl;
	fout << "gray_black_std," << gray_black_std << endl;
	fout << "dot_circle_r_ave," << dot_circle_r_ave / um2pix << endl;
	fout << "dot_circle_r_std," << dot_circle_r_std / um2pix << endl;
	fout << "xx_noise_std," << xx_noise_std << endl;
	fout << "yy_sin_amp," << yy_sin_amp << endl;
	fout << "yy_sin_pitch," << yy_sin_pitch << endl;
	fout << "yy_noise_std," << yy_noise_std << endl;
	fout << "yy_noise_std_pro," << yy_noise_std_pro << endl;
	fout << "xx_noise_std_pro," << xx_noise_std_pro << endl;
	fout << "xx_sin_amp," << xx_sin_amp << endl;
	fout << "xx_sin_pitch," << xx_sin_pitch << endl;

	fout << "dot_r_latent_um," << dot_r_latent_um << endl;
	fout << "dot_r_latent_thresh_gain," << dot_r_latent_thresh_gain << endl;
	fout << "dot_tc_num," << dot_tc_num << endl;
	fout << "dot_toner_r," << dot_toner_r << endl;

	fout << "save_flg_raw," << save_flg_raw << endl;
	fout << "save_flg_200dpi," << save_flg_200dpi << endl;
	fout << "save_flg_400dpi," << save_flg_400dpi << endl;
	fout << "save_flg_600dpi," << save_flg_600dpi << endl;
	fout << "save_flg_1200dpi," << save_flg_1200dpi << endl;
	fout << "save_flg_2400dpi," << save_flg_2400dpi << endl;
	fout << endl;
	return;
}

void offline_imgproc(string ini_file) {

	cout << "setting----------" << endl;
	cout << "...reading..." << ini_file << endl;

	img_dpi = ini_file_read("img_dpi", ini_file);
	double um2pix = img_dpi / 25.4 / 1000.0;	//um->pixel•ÏŠ·ŒW”
	double mm2pix = img_dpi / 25.4;				//mm->pixel•ÏŠ·ŒW”
	double pix2um = 1.0 / um2pix;				//pixel->mm•ÏŠ·ŒW”
	double pix2mm = 1.0 / mm2pix;				//pixel->mm•ÏŠ·ŒW”

	img_width_mm = ini_file_read("img_width_mm", ini_file);
	img_height_mm = ini_file_read("img_height_mm", ini_file);

	HT_width_mm = ini_file_read("HT_width_mm", ini_file);
	HT_height_mm = ini_file_read("HT_height_mm", ini_file);
	gray_white = 255.0 - ini_file_read("gray_white", ini_file);
	gray_black_ave = 255.0 - ini_file_read("gray_black_ave", ini_file);
	gray_black_std = ini_file_read("gray_black_std", ini_file);
	dot_circle_r_ave = ini_file_read("dot_circle_r_ave", ini_file)*um2pix;
	dot_circle_r_std = ini_file_read("dot_circle_r_std", ini_file)* um2pix;

	yy_sin_amp = ini_file_read("yy_sin_amp", ini_file);
	yy_sin_pitch = ini_file_read("yy_sin_pitch", ini_file);
	xx_sin_amp = ini_file_read("xx_sin_amp", ini_file);
	xx_sin_pitch = ini_file_read("xx_sin_pitch", ini_file);

	dot_r_latent_um = ini_file_read("dot_r_latent_um", ini_file);
	dot_r_latent_thresh_gain = ini_file_read("dot_r_latent_thresh_gain", ini_file);
	dot_tc_num = ini_file_read("dot_tc_num", ini_file);
	dot_toner_r = ini_file_read("dot_toner_r", ini_file);

	xx_noise_std = ini_file_read("xx_noise_std", ini_file)*um2pix;
	yy_noise_std = ini_file_read("yy_noise_std", ini_file)*um2pix;
	xx_noise_std_pro = ini_file_read("xx_noise_std_pro", ini_file)*um2pix;
	yy_noise_std_pro = ini_file_read("yy_noise_std_pro", ini_file)*um2pix;

	save_flg_raw = ini_file_read("save_flg_raw", ini_file);
	save_flg_200dpi = ini_file_read("save_flg_200dpi", ini_file);
	save_flg_400dpi = ini_file_read("save_flg_400dpi", ini_file);
	save_flg_600dpi = ini_file_read("save_flg_600dpi", ini_file);
	save_flg_1200dpi = ini_file_read("save_flg_1200dpi", ini_file);
	save_flg_2400dpi = ini_file_read("save_flg_2400dpi", ini_file);

	cout << "----------" << endl;

	
	Mat img_proc = Mat::zeros(img_height_mm* mm2pix, img_width_mm* mm2pix, CV_8U);
	//‰æ‘œ‚Ìì¬
	long img_width = img_width_mm* mm2pix;
	long img_height = img_height_mm * mm2pix;

	Mat img_proc_200dpi = Mat::zeros(img_height*200.0 / img_dpi, img_width*200.0 / img_dpi, CV_8U);
	Mat img_proc_400dpi = Mat::zeros(img_height*400.0 / img_dpi, img_width*400.0 / img_dpi, CV_8U);
	Mat img_proc_600dpi = Mat::zeros(img_height*600.0 / img_dpi, img_width*600.0 / img_dpi, CV_8U);
	Mat img_proc_1200dpi = Mat::zeros(img_height*1200.0 / img_dpi, img_width*1200.0 / img_dpi, CV_8U);
	Mat img_proc_2400dpi = Mat::zeros(img_height*2400.0 / img_dpi, img_width*2400.0 / img_dpi, CV_8U);

	image_prosess(img_proc);

	cout << "resize img_proc_2400dpi" << endl;
	cv::resize(img_proc, img_proc_2400dpi, img_proc_2400dpi.size(), cv::INTER_AREA);
	cout << "resize img_proc_1200dpi" << endl;
	cv::resize(img_proc_2400dpi, img_proc_1200dpi, img_proc_1200dpi.size(), cv::INTER_AREA);
	cout << "resize img_proc_400dpi" << endl;
	cv::resize(img_proc_1200dpi, img_proc_400dpi, img_proc_400dpi.size(), cv::INTER_AREA);
	cout << "resize img_proc_200dpi" << endl;
	cv::resize(img_proc_400dpi, img_proc_200dpi, img_proc_200dpi.size(), cv::INTER_AREA);

	cout << "image write..." << endl;

	string fname_img;
	save_point(".\\img\\img", fname_img);
	string output = fname_img + "_raw.tif";

	cout << "fname_img"<<fname_img << endl;
	string output_200dpi = fname_img + "_200dpi.tif";
	string output_400dpi = fname_img + "_400dpi.tif";
	string output_600dpi = fname_img + "_600dpi.tif";
	string output_1200dpi = fname_img + "_1200dpi.tif";
	string output_2400dpi = fname_img + "_2400dpi.tif";

	if (save_flg_raw)		cv::imwrite(output, img_proc);
	if (save_flg_200dpi)	cv::imwrite(output_200dpi, img_proc_200dpi);
	if (save_flg_400dpi)	cv::imwrite(output_400dpi, img_proc_400dpi);
	if (save_flg_600dpi)	cv::imwrite(output_600dpi, img_proc_600dpi);
	if (save_flg_1200dpi)	cv::imwrite(output_1200dpi, img_proc_1200dpi);
	if (save_flg_2400dpi)	cv::imwrite(output_2400dpi, img_proc_2400dpi);

	cv::imwrite("test.tif", img_proc);


}


//========================================================================
int main(int argc, char* argv[]){

	bool flg_ui = 1;

	string input_fname = "test.csv";
	
	if (flg_ui == 0) {
		//string path = "";
		//ofDirectory dir(path);
		//string fname = "log.txt";
		//ofstream outputfile(fname);
		//outputfile << "test" << endl;
		//outputfile.close();

		offline_imgproc(input_fname);


		return 0;
	}


	if (flg_ui == 1) {
		ofSetupOpenGL(800, 800, OF_WINDOW);			// <-------- setup the GL context
		ofRunApp(new ofApp());
	}

}
