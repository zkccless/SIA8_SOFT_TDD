#ifndef UTILS
#define UTILS

#include <vector>
#include <detection.h>
#include "data.h"

namespace utils {
	std::vector<std::string> split(const std::string& str, const std::string& delim);

	std::string convert(float Num);

	// void detect_match(std::vector< Detection > detections, std::vector<Centerlabel> redefections, std::vector< Detection >& detections_out, std::string port);

	std::shared_ptr<Detection::DetResult> detect_match2(std::shared_ptr<Detection::DetResult>& detections, std::vector<Centerlabel> redefections, std::shared_ptr<Detection::DetResult>& detections_out, std::string port);

	std::ofstream createLog(std::string port);

	const std::string currentDate();

	const std::string currentDateTime();

	float overlapRate(int x1, int y1, int h1, int w1, int x2, int y2, int h2, int w2, float& r1, float& r2);

	// std::vector<Detection> detections_resize(cv::Mat image_scan_cis, cv::Mat image_scan_ccd, std::vector<Detection>detections_out_cis, std::vector<Detection>detections_out_ccd, std::string show_image_flag);

	//void bbox_integration(size_t j, std::vector< Detection >& res, std::vector<int>& flag_filter_box);

	/*std::string bbox_expand(cv::Mat image_scan, std::vector<Detection> detections_out, float RESIZERATIO, std::vector<cv::Rect>& vector_r, std::vector<cv::Rect>& vector_r1,
		std::vector<std::string>& vector_text, std::vector<std::string>& vector_score, std::vector<std::string> classesorigin, std::string port, std::string res_type = "all");*/

	void write_NG_roi(cv::Mat img, std::string dir, std::vector<std::string> text, std::vector<cv::Rect> r);

	void write_NG_overview(cv::Mat pic_overview_cis, cv::Mat pic_overview_ccd, std::string mapping_type, std::string dir, std::vector<std::string> text_all,
		std::vector<std::string> label_all, std::vector<cv::Rect> r1_all);

	float calcuDistance(uchar* ptr, uchar* ptrCen, int cols);

	//unsigned creat_matchlabel(std::vector< Detection > res, std::vector<Centerlabel>& outputs, std::string port);

	float average(const std::vector<float>& arr);

	std::vector<float> threshold_stof(std::string threshold_string, std::vector<float> threshold_float);

	std::vector<int> threshold_stoi(std::string redefect_string, std::vector<int> redefect_int);

	std::vector<Centerlabel> redefections_add(std::string port);

	cv::Mat Sharemm_Pretreatment(std::vector<std::string> AllStr, std::string port);

	cv::Mat Sharemm_Pretreatment_ccd(std::vector<std::string> AllStr, std::string port, float cis_rows, float cis_cols);

	SettingsInfor read_threshold(int camera_type, int threshold_num);

	float CountChannelColor(std::vector<float> tempmean_autoLearn);

	void CloseConsoleMode();

	int dectections_merge(std::vector<std::shared_ptr<Detection::DetResult>>& result, std::shared_ptr<Detection::DetResult>& detections, std::vector<cv::Size2f> scale_factors, std::vector<cv::Point> crop_points, std::vector<float> threshold, int th_area);

}; // namespace utils

#endif