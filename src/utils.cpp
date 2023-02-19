#include <chrono>
#include <fstream>
#include <numeric>

#include <direct.h>
#include <io.h>
#include <Windows.h>

#include "utils.h"
#include "data.h"


//移除快速编辑模式
void utils::CloseConsoleMode() {
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE; //移除快速编辑模式
	mode &= ~ENABLE_INSERT_MODE; //移除插入模式
	mode &= ~ENABLE_MOUSE_INPUT;
	SetConsoleMode(hStdin, mode);
}

// 路径分割
std::vector<std::string> utils::split(const std::string& str, const std::string& delim) {
	std::vector<std::string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型  
	char* strs = new char[str.length() + 1];
	strcpy(strs, str.c_str());

	char* d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char* p = strtok(strs, d);
	while (p) {
		std::string s = p; //分割得到的字符串转换为string类型  
		res.push_back(s); //存入结果数组  
		p = strtok(NULL, d);
	}
	return res;
}

// 将float转string
std::string utils::convert(float Num) {
	std::ostringstream oss;
	oss << Num;
	std::string str(oss.str());
	return str;
}

// 获取当前日期
const std::string utils::currentDate() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
	return buf;
}

// 获取当前具体时间
const std::string utils::currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}

// 日志输出流初始化
std::ofstream utils::createLog(std::string port) {
	std::ofstream logout;
	std::string currentdate = currentDate();
	std::string log_fold = "./log/";
	if (_access(log_fold.c_str(), 0) == -1) //如果文件夹不存在 
		_mkdir(log_fold.c_str());
	std::string logfile = log_fold + currentdate + "_" + port + ".txt";
	//std::string logfile = log_fold + currentdate + "_" + ".txt";
	logout.open(logfile, std::ios_base::app);
	return logout;
}

// 推理结果对比反误检库，返回真缺陷结果
//void utils::detect_match(std::vector< utils::Detection > detections, std::vector<utils::Centerlabel> redefections, std::vector< Detection >& detections_out, std::string port)
//{
//	// detections是检测结果,redefections是反误检库;
//	std::ofstream logout = createLog(port);
//	logout << currentDateTime() + " ****** " + "start false detection match" + " ****** " + "\n";
//	detections_out.clear();
//	cv::Point2d center;
//	std::vector<Detection> detection_remove;
//	for (size_t j = 0; j < detections.size(); j++) {
//		center.x = 0.5 * (detections[j].box.right + detections[j].box.left);
//		center.y = 0.5 * (detections[j].box.bottom + detections[j].box.top);
//		float distance = 1000;
//		float area = (detections[j].box.right - detections[j].box.left) * (detections[j].box.bottom - detections[j].box.top);
//		logout << currentDateTime() + " detections- " + std::to_string(j) + " center x = " + std::to_string(center.x) + "\n";
//		logout << currentDateTime() + " detections- " + std::to_string(j) + " center y = " + std::to_string(center.y) + "\n";
//		logout << currentDateTime() + " detections- " + std::to_string(j) + " area = " + std::to_string(area) + "\n";
//		float area_diff = 1000;
//		for (size_t k = 0; k < redefections.size(); k++) {
//			float xx = redefections[k].x;
//			float yy = redefections[k].y;
//			if (redefections[k].label == detections[j].label && distance > sqrt((xx - center.x) * (xx - center.x) + (yy - center.y) * (yy - center.y))) {
//				distance = sqrt((xx - center.x) * (xx - center.x) + (yy - center.y) * (yy - center.y));
//				area_diff = abs(redefections[k].area - area);
//				if (distance <= 300 && area_diff <= 200) {
//					logout << currentDateTime() + " ### redefections.x = " + std::to_string(redefections[k].x) + "\n";
//					logout << currentDateTime() + " ### redefections.y = " + std::to_string(redefections[k].y) + "\n";
//					logout << currentDateTime() + " ### redefections.label = " + std::to_string(redefections[k].label) + "\n";
//					logout << currentDateTime() + " ### redefections.area = " + std::to_string(redefections[k].area) + "\n";
//					logout << currentDateTime() + " ### distance =  " + std::to_string(distance) + "\n";
//					logout << currentDateTime() + " ### area_diff =  " + std::to_string(area_diff) + "\n";
//					break;
//				}
//			}
//		}
//		//if ((distance > 300 && area_diff > 200) || (distance > 300 && area_diff == 0))detections_out.push_back(detections[j]);
//		//else {
//		//	logout << currentDateTime() + " !Remove! " << " label ---- " << detections[j].label << " left : " << detections[j].box.left << ","
//		//		<< " right : " << detections[j].box.right << "," << " top : " << detections[j].box.top << ","
//		//		<< " bottom : " << detections[j].box.bottom << std::endl;
//		//}
//		if (distance < 300 && area_diff < 200) {
//			logout << currentDateTime() + " !Remove! " << " label ---- " << detections[j].label << " left : " << detections[j].box.left << ","
//				<< " right : " << detections[j].box.right << "," << " top : " << detections[j].box.top << ","
//				<< " bottom : " << detections[j].box.bottom << std::endl;
//			detection_remove.push_back(detections[j]);
//		}
//		else {
//			detections_out.push_back(detections[j]);
//		}
//	}
//	logout << currentDateTime() + " ****** " + " false detection match end " + " ****** " + "\n";
//}

// 推理结果对比反误检库，返回真缺陷结果
std::shared_ptr<Detection::DetResult> utils::detect_match2(std::shared_ptr<Detection::DetResult>& detections, std::vector<utils::Centerlabel> redefections, std::shared_ptr<Detection::DetResult>& detections_out, std::string port)
{
	// detections是检测结果,redefections是反误检库;
	std::ofstream logout = utils::createLog(port);
	logout << utils::currentDateTime() + " ****** " + "start false detection match" + " ****** " + "\n";
	
	cv::Point2d center;
	std::shared_ptr<Detection::DetResult> detection_remove;
	auto detections_bboxes = detections->immutable_defects();
	auto detection_remove_bboxes = detection_remove->immutable_defects();
	auto detections_out_bboxes = detections_out->immutable_defects();
	detections_out_bboxes.clear();

	for (size_t j = 0; j < detections_bboxes.size(); j++) {
		center.x = 0.5 * (detections_bboxes[j].right + detections_bboxes[j].left);
		center.y = 0.5 * (detections_bboxes[j].bottom + detections_bboxes[j].top);
		float distance = 1000;
		float area = (detections_bboxes[j].right - detections_bboxes[j].left) * (detections_bboxes[j].bottom - detections_bboxes[j].top);
		logout << utils::currentDateTime() + " detections- " + std::to_string(j) + " center x = " + std::to_string(center.x) + "\n";
		logout << utils::currentDateTime() + " detections- " + std::to_string(j) + " center y = " + std::to_string(center.y) + "\n";
		logout << utils::currentDateTime() + " detections- " + std::to_string(j) + " area = " + std::to_string(area) + "\n";
		float area_diff = 0;
		for (size_t k = 0; k < redefections.size(); k++) {
			float xx = redefections[k].x;
			float yy = redefections[k].y;
			if (redefections[k].label == detections_bboxes[j].label && distance > sqrt((xx - center.x) * (xx - center.x) + (yy - center.y) * (yy - center.y))) {
				distance = sqrt((xx - center.x) * (xx - center.x) + (yy - center.y) * (yy - center.y));
				area_diff = abs(redefections[k].area - area);
				if (distance <= 300 && area_diff <= 200) {
					logout << utils::currentDateTime() + " ### redefections.x = " + std::to_string(redefections[k].x) + "\n";
					logout << utils::currentDateTime() + " ### redefections.y = " + std::to_string(redefections[k].y) + "\n";
					logout << utils::currentDateTime() + " ### redefections.label = " + std::to_string(redefections[k].label) + "\n";
					logout << utils::currentDateTime() + " ### redefections.area = " + std::to_string(redefections[k].area) + "\n";
					logout << utils::currentDateTime() + " ### distance =  " + std::to_string(distance) + "\n";
					logout << utils::currentDateTime() + " ### area_diff =  " + std::to_string(area_diff) + "\n";
					break;
				}
			}
		}
		//if ((distance > 300 && area_diff > 200) || (distance > 300 && area_diff == 0))detections_out.push_back(detections[j]);
		//else {
		//	logout << currentDateTime() + " !Remove! " << " label ---- " << detections[j].label << " left : " << detections[j].box.left << ","
		//		<< " right : " << detections[j].box.right << "," << " top : " << detections[j].box.top << ","
		//		<< " bottom : " << detections[j].box.bottom << std::endl;
		//}
		if (distance < 300 && area_diff < 200) {
			logout << utils::currentDateTime() + " !Remove! " << " label ---- " << detections_bboxes[j].label << " left : " << detections_bboxes[j].left << ","
				<< " right : " << detections_bboxes[j].right << "," << " top : " << detections_bboxes[j].top << ","
				<< " bottom : " << detections_bboxes[j].bottom << std::endl;
			detection_remove_bboxes.push_back(detections_bboxes[j]);

		}
		else {
			detections_out_bboxes.push_back(detections_bboxes[j]);
		}
	}
	return detection_remove;
	logout << utils::currentDateTime() + " ****** " + " false detection match end " + " ****** " + "\n";
}

// 计算两个矩形框的重叠比率。参数：x、y为矩阵左下角坐标值，h、w为矩形的高和宽
float utils::overlapRate(int x1, int y1, int h1, int w1, int x2, int y2, int h2, int w2, float& r1, float& r2)
{
	int endx = std::max(x1 + w1, x2 + w2);
	int startx = std::min(x1, x2);
	int width = w1 + w2 - (endx - startx);  // 重叠部分宽
	int endy = std::max(y1 + h1, y2 + h2);
	int starty = std::min(y1, y2);
	int height = h1 + h2 - (endy - starty);  // 重叠部分高
	if (width > 0 && height > 0) {
		int area = width * height;  // 重叠部分面积
		int area1 = w1 * h1;
		int area2 = w2 * h2;
		r1 = (float)area / area1;
		r2 = (float)area / area2;
		float ratio = (float)area / (area1 + area2 - area);
		return ratio;
	}
	else {
		// 不重叠：算出来的width或height小于等于0
		return 0.0;
	}
}

//std::vector<Detection> detections_resize(cv::Mat image_scan_cis, cv::Mat image_scan_ccd, std::vector<Detection>detections_out_cis, std::vector<Detection>detections_out_ccd, std::string mapping_type) {
//	float cis_rows = image_scan_cis.rows;
//	float cis_cols = image_scan_cis.cols;
//	float ccd_rows = image_scan_ccd.rows;
//	float ccd_cols = image_scan_ccd.cols;
//	/*std::cout << "cis_rows" << cis_rows << std::endl;
//	std::cout << "cis_cols" << cis_cols << std::endl;
//	std::cout << "ccd_rows" << ccd_rows << std::endl;
//	std::cout << "ccd_cols" << ccd_cols << std::endl;*/
//
//	float rows_ratio = cis_rows / ccd_rows;
//	float cols_ratio = cis_cols / ccd_cols;
//	float bbox_width;
//	float bbox_height;
//
//
//	std::cout << "rows_ratio =" << rows_ratio << std::endl;
//	std::cout << "cols_ratio = " << cols_ratio << std::endl;
//
//	if (mapping_type == "cis") {
//		if (rows_ratio < 1 || cols_ratio < 1) {
//			for (int i = 0; i < detections_out_ccd.size(); i++) {
//				bbox_width = detections_out_ccd[i].box.right - detections_out_ccd[i].box.left;
//				bbox_height = detections_out_ccd[i].box.bottom - detections_out_ccd[i].box.top;
//				std::cout << "1 bbox_width" << bbox_width << std::endl;
//				std::cout << "1 bbox_height" << bbox_height << std::endl;
//
//				/*float bbox_width_resize = bbox_width * cols_ratio;
//				float bbox_height_resize = bbox_height * rows_ratio;
//				std::cout << "bbox_width_resize" << bbox_width_resize << std::endl;
//				std::cout << "bbox_height_resize" << bbox_height_resize << std::endl;*/
//
//				/*detections_out_ccd[i].box.left = detections_out_ccd[i].box.left + (bbox_width - bbox_width_resize) / 2;
//				detections_out_ccd[i].box.right = detections_out_ccd[i].box.right - (bbox_width - bbox_width_resize) / 2;
//				detections_out_ccd[i].box.top = detections_out_ccd[i].box.top + (bbox_height - bbox_height_resize) / 2;
//				detections_out_ccd[i].box.bottom = detections_out_ccd[i].box.bottom - (bbox_height - bbox_height_resize) / 2;*/
//
//				//std::cout << "1 detections_out_ccd[i].box.left" << detections_out_ccd[i].box.left << std::endl;
//				//std::cout << "1 detections_out_ccd[i].box.right" << detections_out_ccd[i].box.right << std::endl;
//				//std::cout << "1 detections_out_ccd[i].box.top" << detections_out_ccd[i].box.top << std::endl;
//				//std::cout << "1 detections_out_ccd[i].box.bottom" << detections_out_ccd[i].box.bottom << std::endl;
//				detections_out_ccd[i].box.left = detections_out_ccd[i].box.left * cols_ratio;
//				detections_out_ccd[i].box.right = detections_out_ccd[i].box.right * cols_ratio;
//				detections_out_ccd[i].box.top = detections_out_ccd[i].box.top * rows_ratio;
//				detections_out_ccd[i].box.bottom = detections_out_ccd[i].box.bottom * rows_ratio;
//				bbox_width = detections_out_ccd[i].box.right - detections_out_ccd[i].box.left;
//				bbox_height = detections_out_ccd[i].box.bottom - detections_out_ccd[i].box.top;
//				std::cout << "bbox_width" << bbox_width << std::endl;
//				std::cout << "bbox_height" << bbox_height << std::endl;
//				//std::cout << "detections_out_ccd[i].box.left" << detections_out_ccd[i].box.left << std::endl;
//				//std::cout << "detections_out_ccd[i].box.right" << detections_out_ccd[i].box.right << std::endl;
//				//std::cout << "detections_out_ccd[i].box.top" << detections_out_ccd[i].box.top << std::endl;
//				//std::cout << "detections_out_ccd[i].box.bottom" << detections_out_ccd[i].box.bottom << std::endl;
//			}
//			return detections_out_ccd;
//		}
//		else { return detections_out_ccd; }
//	}
//	else if (mapping_type == "ccd") {
//		rows_ratio = 1 / rows_ratio;
//		cols_ratio = 1 / cols_ratio;
//		if (rows_ratio < 1 || cols_ratio < 1) {
//			for (int j = 0; j < detections_out_cis.size(); j++) {
//				//bbox_width = detections_out_cis[j].box.right - detections_out_cis[j].box.left;
//				//bbox_height = detections_out_cis[j].box.bottom - detections_out_cis[j].box.top;
//
//				detections_out_cis[j].box.left = detections_out_cis[j].box.left * cols_ratio;
//				detections_out_cis[j].box.right = detections_out_cis[j].box.right * cols_ratio;
//				detections_out_cis[j].box.top = detections_out_cis[j].box.top * rows_ratio;
//				detections_out_cis[j].box.bottom = detections_out_cis[j].box.bottom * rows_ratio;
//				//bbox_width = detections_out_cis[j].box.right - detections_out_cis[j].box.left;
//				//bbox_height = detections_out_cis[j].box.bottom - detections_out_cis[j].box.top;
//			}
//			return detections_out_cis;
//		}
//		else { return detections_out_cis; }
//	}
//}

//void bbox_integration(size_t j, std::vector< Detection >& res, std::vector<int>& flag_filter_box) {
//	for (int k = 0; k < j; k++) {
//		int x1 = res[k].box.left;
//		int y1 = res[k].box.top;
//		int h1 = res[k].box.bottom - res[k].box.top;
//		int w1 = res[k].box.right - res[k].box.left;
//		int area1 = h1 * w1;
//
//		int x2 = res[j].box.left;
//		int y2 = res[j].box.top;
//		int h2 = res[j].box.bottom - res[j].box.top;
//		int w2 = res[j].box.right - res[j].box.left;
//		int area2 = h2 * w2;
//
//		float r1 = 0, r2 = 0;
//
//		float ratio = overlapRate(x1, y1, h1, w1, x2, y2, h2, w2, r1, r2);
//		if (r1 == 1) {
//			res[k].box.left = res[j].box.left;
//			res[k].box.right = res[j].box.right;
//			res[k].box.top = res[j].box.top;
//			res[k].box.bottom = res[j].box.bottom;
//			if (res[k].score < res[j].score) {
//				res[k].score = res[j].score;
//				res[k].label = res[j].label;
//			}
//			flag_filter_box[j] = 1;
//			break;
//		}
//		else if (r2 == 1) {
//			if (res[k].score < res[j].score) {
//				res[k].score = res[j].score;
//				res[k].label = res[j].label;
//			}
//			flag_filter_box[j] = 1;
//			break;
//		}
//		else if (ratio > 0.3 || r1 > 0.3 || r2 > 0.3) {
//			if (res[k].box.left > res[j].box.left) { res[k].box.left = res[j].box.left; }
//			if (res[k].box.right < res[j].box.right) { res[k].box.right = res[j].box.right; }
//			if (res[k].box.top > res[j].box.top) { res[k].box.top = res[j].box.top; }
//			if (res[k].box.bottom < res[j].box.bottom) { res[k].box.bottom = res[j].box.bottom; }
//			if (res[k].score < res[j].score) {
//				res[k].score = res[j].score;
//				res[k].label = res[j].label;
//			}
//			flag_filter_box[j] = 1;
//			break;
//		}
//		else { flag_filter_box[j] = 0; }
//	}
//
//}

//std::string bbox_expand(cv::Mat image_scan, std::vector<Detection> detections_out, float RESIZERATIO, std::vector<cv::Rect>& vector_r, std::vector<cv::Rect>& vector_r1,
//	std::vector<std::string>& vector_text, std::vector<std::string>& vector_score, std::vector<std::string> classesorigin, std::string port, std::string res_type) {
//	std::vector<int> flag_filter_box(detections_out.size());
//	flag_filter_box[0] = 0;
//	int imgcol, imgrow;
//	int img_center_x, img_center_y, img_box_area;
//	std::string text, label_sum;
//	int count = 0;
//	std::ofstream logout = createLog(port);
//
//	//std::string label_sum; // collect result label
//	std::vector<std::string> confidence; // 记录各个缺陷的名称和置信度
//
//	for (size_t j = 0; j < detections_out.size(); j++) {
//		text = classesorigin[(int)detections_out[j].label];
//		//bbox整合
//		float box_x = (detections_out[j].box.right + detections_out[j].box.left) / 2;
//		float box_y = (detections_out[j].box.bottom + detections_out[j].box.top) / 2;
//		float box_label = detections_out[j].label;
//		std::cout << text << "-score-" << j << "=" << detections_out[j].score << std::endl;
//
//		// label保留两位小数
//		int t_score = (int)(detections_out[j].score * 100);
//		float score_2f = (float)t_score / 100;
//		std::ostringstream strStream;
//		strStream << score_2f;
//		std::string t_text = text + "-" + strStream.str();
//		vector_score.push_back(t_text);
//
//		/*if (j >= 1) {
//			bbox_integration(j, detections_integration, flag_filter_box);
//		}*/
//		if ((detections_out[j].box.right - detections_out[j].box.left) != 0 &&
//			(detections_out[j].box.bottom - detections_out[j].box.top) != 0 && flag_filter_box[j] != 1) {
//			img_box_area = (detections_out[j].box.right - detections_out[j].box.left) *
//				(detections_out[j].box.bottom - detections_out[j].box.top);
//			imgcol = image_scan.cols;
//			imgrow = image_scan.rows;
//
//			if (detections_out[j].box.left > 20)detections_out[j].box.left -= 20;
//			else if (detections_out[j].box.left < 20)detections_out[j].box.left = 0;
//
//			if ((imgcol - detections_out[j].box.right) > 20)detections_out[j].box.right += 20;
//			else if ((imgcol - detections_out[j].box.right) < 20)detections_out[j].box.right = imgcol;
//
//			if (detections_out[j].box.top > 20)detections_out[j].box.top -= 20;
//			else if (detections_out[j].box.top < 20)detections_out[j].box.top = 0;
//
//			if ((imgrow - detections_out[j].box.bottom) > 20)detections_out[j].box.bottom += 20;
//			else if ((imgrow - detections_out[j].box.bottom) < 20) detections_out[j].box.bottom = imgrow;
//
//			cv::Rect r_detail = cv::Rect(detections_out[j].box.left, detections_out[j].box.top,
//				detections_out[j].box.right - detections_out[j].box.left,
//				detections_out[j].box.bottom - detections_out[j].box.top);
//			cv::Rect r_overview = cv::Rect(RESIZERATIO * detections_out[j].box.left,
//				RESIZERATIO * detections_out[j].box.top, RESIZERATIO * detections_out[j].box.right -
//				RESIZERATIO * detections_out[j].box.left, RESIZERATIO * detections_out[j].box.bottom -
//				RESIZERATIO * detections_out[j].box.top);
//			vector_r.push_back(r_detail);
//			vector_r1.push_back(r_overview);
//			img_center_x = (int)(0.5 * (detections_out[j].box.right + detections_out[j].box.left));
//			img_center_y = (int)(0.5 * (detections_out[j].box.bottom + detections_out[j].box.top));
//
//			if (count == 0) {
//				label_sum += text + "!" + std::to_string(img_box_area) + "!" + std::to_string(img_center_x) + "!" + std::to_string(img_center_y)
//					+ "!" + std::to_string((int)detections_out[j].box.left) + "!" + std::to_string((int)detections_out[j].box.right)
//					+ "!" + std::to_string((int)detections_out[j].box.top) + "!" + std::to_string((int)detections_out[j].box.bottom);
//				vector_text.push_back(text + "_" + res_type);
//			}
//			else {
//				label_sum += "+" + text + "!" + std::to_string(img_box_area) + "!" + std::to_string(img_center_x) + "!" + std::to_string(img_center_y)
//					+ "!" + std::to_string((int)detections_out[j].box.left) + "!" + std::to_string((int)detections_out[j].box.right)
//					+ "!" + std::to_string((int)detections_out[j].box.top) + "!" + std::to_string((int)detections_out[j].box.bottom);
//				vector_text.push_back(text + "_" + res_type);
//			}
//			count += 1;
//
//			//if (std::find(classesorigin.begin(), classesorigin.end(), text) != classesorigin.end()) {
//			//	if (count == 0) {
//			//		label_sum += text + "!" + std::to_string(img_box_area) + "!" + std::to_string(img_center_x) + "!" + std::to_string(img_center_y)
//			//			+ "!" + std::to_string((int)detections_out[j].box.left) + "!" + std::to_string((int)detections_out[j].box.right)
//			//			+ "!" + std::to_string((int)detections_out[j].box.top) + "!" + std::to_string((int)detections_out[j].box.bottom);
//			//		vector_text.push_back(text + "_" + "cis");
//			//	}
//			//	else if (count > 0 && j < detections_out.size()) {
//			//		label_sum += "+" + text + "!" + std::to_string(img_box_area) + "!" + std::to_string(img_center_x) + "!" + std::to_string(img_center_y)
//			//			+ "!" + std::to_string((int)detections_out[j].box.left) + "!" + std::to_string((int)detections_out[j].box.right)
//			//			+ "!" + std::to_string((int)detections_out[j].box.top) + "!" + std::to_string((int)detections_out[j].box.bottom);
//			//		vector_text.push_back(text + "_" + "cis");
//			//	}
//			//	else {
//			//		label_sum += "+" + text + "!" + std::to_string(img_box_area) + "!" + std::to_string(img_center_x) + "!" + std::to_string(img_center_y)
//			//			+ "!" + std::to_string((int)detections_out[j].box.left) + "!" + std::to_string((int)detections_out[j].box.right)
//			//			+ "!" + std::to_string((int)detections_out[j].box.top) + "!" + std::to_string((int)detections_out[j].box.bottom);
//			//		vector_text.push_back(text + "_" + "ccd");
//			//	}
//				//count = count + 1;
//				////std::cout << "--------------- " << std::endl;
//				////std::cout << "detect label is " << text << std::endl;
//				////std::cout << "detect score is " << detections_integration[j].score << "\n" << std::endl;
//				//confidence.push_back(text);
//				//confidence.push_back(convert(detections_out[j].score));
//			//}
//		}
//	}
//	//if (confidence.size() > 0) {
//	//	std::cout << "remain defect num : " << confidence.size() / 2 << std::endl;
//	//	std::string confidence_string;
//	//	for (int i = 0; i < confidence.size(); i++) {
//	//		std::string conf_str;
//	//		conf_str = confidence[i] + ',';
//	//		confidence_string += conf_str;
//	//	}
//	//	logout << currentDateTime() + " main: " + " remain defects and confidences are " + confidence_string + "\n";
//	//}
//	return label_sum;
//}

void utils::write_NG_roi(cv::Mat img, std::string dir, std::vector<std::string> text, std::vector<cv::Rect> r) {
	std::vector<std::string> dir_split = split(dir, "/\\");
	std::string time_stamp = dir_split[5];
	std::string result_dir = dir_split[0] + "/" + dir_split[1] + "/" + dir_split[2] + "/" + dir_split[3] + "/ScanResultImages/NG/" + time_stamp;
	std::vector<std::string> text_split;
	std::string savepath_imgroi;
	cv::Mat imgroi;
	for (int i = 0; i < r.size(); i++) {
		text_split = split(text[i], "-");
		savepath_imgroi = result_dir + "/" + time_stamp + "-detail-" + text_split[0] + "-" + std::to_string(i) + ".bmp";
		imgroi = img(r[i]);
		if (_access(result_dir.c_str(), 0) == -1)
			_mkdir(result_dir.c_str());
		cv::imwrite(savepath_imgroi, imgroi);
	}
}

void utils::write_NG_overview(cv::Mat pic_overview_cis, cv::Mat pic_overview_ccd, std::string mapping_type, std::string dir, std::vector<std::string> text_all,
	std::vector<std::string> label_all, std::vector<cv::Rect> r1_all) {

	std::vector<std::string> dir_split = split(dir, "/\\");
	std::string time_stamp = dir_split[5];
	std::string result_dir = dir_split[0] + "/" + dir_split[1] + "/" + dir_split[2] + "/" + dir_split[3] + "/ScanResultImages/NG/" + time_stamp;
	std::vector<std::string> text_split;
	std::string savepath_overview;
	if (mapping_type == "cis") { // 在cis原图上画
		for (int k = 0; k < r1_all.size(); k++) {
			text_split = split(text_all[k], "_");
			if (text_split[1] == "cis") {
				cv::rectangle(pic_overview_cis, r1_all[k], cv::Scalar(115, 52, 239), 3);
				cv::putText(pic_overview_cis, label_all[k], cv::Point(r1_all[k].x, r1_all[k].y - 1),
					cv::FONT_HERSHEY_PLAIN, 1.1, cv::Scalar(115, 52, 239), 2);
			}
			else if (text_split[1] == "ccd") {
				cv::rectangle(pic_overview_cis, r1_all[k], cv::Scalar(215, 52, 50), 3);
				cv::putText(pic_overview_cis, label_all[k], cv::Point(r1_all[k].x, r1_all[k].y - 1),
					cv::FONT_HERSHEY_PLAIN, 1.1, cv::Scalar(215, 52, 50), 2);
			}
		}
		savepath_overview = result_dir + "/" + time_stamp + "-overview.jpg";
		cv::imwrite(savepath_overview, pic_overview_cis);
	}
	else if (mapping_type == "ccd") {// 在ccd原图上画
		for (int k = 0; k < r1_all.size(); k++) {
			text_split = split(text_all[k], "_");
			if (text_split[1] == "cis") { // 绘制cis bbox
				cv::rectangle(pic_overview_ccd, r1_all[k], cv::Scalar(115, 52, 239), 3);
				cv::putText(pic_overview_ccd, label_all[k], cv::Point(r1_all[k].x, r1_all[k].y - 1),
					cv::FONT_HERSHEY_PLAIN, 1.1, cv::Scalar(115, 52, 239), 2);
			}
			else if (text_split[1] == "ccd") { // 绘制ccd bbox
				cv::rectangle(pic_overview_ccd, r1_all[k], cv::Scalar(215, 52, 50), 3);
				cv::putText(pic_overview_ccd, label_all[k], cv::Point(r1_all[k].x, r1_all[k].y - 1),
					cv::FONT_HERSHEY_PLAIN, 1.1, cv::Scalar(215, 52, 50), 2);
			}
		}
		savepath_overview = result_dir + "/" + time_stamp + "-overview.jpg";
		cv::imwrite(savepath_overview, pic_overview_ccd);
	}
}

// 计算欧式距离
float utils::calcuDistance(uchar* ptr, uchar* ptrCen, int cols) {
	float d = 0.0;
	for (size_t j = 0; j < cols; j++) {
		d += (double)(ptr[j] - ptrCen[j]) * (ptr[j] - ptrCen[j]);
	}
	d = sqrt(d);
	return d;
}

//创建实物模板误检库
//unsigned creat_matchlabel(std::vector< Detection > res, std::vector<Centerlabel>& outputs, std::string port)
//{
//	std::ofstream logout = createLog(port);
//	logout << currentDateTime() + " ****** " + "start creat_matchlabel" + " ****** " + "\n";
//	cv::Point2i center;
//	for (size_t j = 0; j < res.size(); j++) {
//		center.x = (int)0.5 * (res[j].box.right + res[j].box.left);
//		center.y = (int)0.5 * (res[j].box.bottom + res[j].box.top);
//		int i = res[j].label;
//		int area = (res[j].box.right - res[j].box.left) * (res[j].box.bottom - res[j].box.top);
//		logout << currentDateTime() + " creat_matchlabel center is: " + std::to_string(center.x) + "," + std::to_string(center.y) + "\n";
//		logout << currentDateTime() + " creat_matchlabel label is: " + std::to_string(i) + "\n";
//		logout << currentDateTime() + " creat_matchlabel area is: " + std::to_string(area) + "\n";
//		Centerlabel sample_dets = Centerlabel(center.x, center.y, i, area);
//		outputs.push_back(sample_dets);
//	}
//	logout << currentDateTime() + " ****** " + " creat_matchlabel end " + " ****** " + "\n";
//	return outputs.size();
//}

// 计算平均值
float utils::average(const std::vector<float>& arr) {
	float sum = 0;
	for (unsigned i = 0; i < arr.size(); i++) { sum += arr[i]; }
	return sum / arr.size();
}

// 将string转float
std::vector<float> utils::threshold_stof(std::string threshold_string, std::vector<float> threshold_float) {
	std::vector<std::string> string_threshold1 = utils::split(threshold_string, ",");
	for (int i = 0; i < string_threshold1.size(); i++) {
		threshold_float[i] = std::stof(string_threshold1[i]);
	}
	return threshold_float;
}
// 将string转int
std::vector<int> utils::threshold_stoi(std::string redefect_string, std::vector<int> redefect_int) {
	std::vector<std::string> string_threshold1 = utils::split(redefect_string, ",");
	for (int i = 0; i < string_threshold1.size(); i++) {
		redefect_int[i] = std::stoi(string_threshold1[i]);
	}
	return redefect_int;
}

// 读文件获取误检库信息（center.x, center.y, label）
std::vector<utils::Centerlabel> utils::redefections_add(std::string port) {
	//void redefections_add_thread() {
	std::ofstream logout = utils::createLog(port);
	std::cout << "redefect start" << std::endl;
	logout << utils::currentDateTime() + " redefect start " + "\n";
	std::vector<std::string> classesorigin = { "AngleEdgeBroken","DropInk","PullLine" ,"Bubble","LackInk" ,"Crack", "Mark",
		"WhitePoint", "BurstGlaze", "DropDirty","DropGlaze","PolishWhite","LackGlaze","DropWater","ColorMarks","PrintHead","Blister","Pit" };
	std::vector<utils::Centerlabel> redefectpoints_add;
	cv::Point2i center;
	std::ifstream file;
	std::string string_line;
	int count_line = 0;
	char ch;
	file.open("./redefections_add.txt", std::ios_base::in);
	file >> ch;
	if (!file.is_open()) // 文件不存在
	{
		std::cout << "redefect file is null" << std::endl;
		logout << utils::currentDateTime() + " redefect file is null " + "\n";
	}
	else if (!file.eof()) // 文件不为空
	{
		while (getline(file, string_line)) {
			//std::vector<std::string> AllStr = split(string_line, ",");
			std::vector<int> redefect_vector(5);
			redefect_vector = threshold_stoi(string_line, redefect_vector);
			center.x = redefect_vector[1];
			center.y = redefect_vector[2];
			int label = redefect_vector[3];
			int area = redefect_vector[4];
			Centerlabel each_redefect = Centerlabel(center.x, center.y, label, area);
			logout << utils::currentDateTime() + " center.x: " + std::to_string(redefect_vector[1]) + " center.y: " + std::to_string(redefect_vector[2]) +
				" label: " + std::to_string(redefect_vector[3]) + " area: " + std::to_string(redefect_vector[4]) + "-" + classesorigin[redefect_vector[3]] + "\n";
			redefectpoints_add.push_back(each_redefect);
			count_line = count_line + 1;
		}
	}
	std::cout << "redefect_add get num is:" << count_line << std::endl;
	logout << utils::currentDateTime() + " redefect_add get num is: " + std::to_string(count_line) + "\n";
	//std::cout << "redefect thread sleep..." << std::endl;
	//std::this_thread::sleep_for(std::chrono::milliseconds(6000));
	return redefectpoints_add;
}

cv::Mat utils::Sharemm_Pretreatment(std::vector<std::string> AllStr, std::string port) {
	std::ofstream logout = utils::createLog(port);
	HANDLE hMapFile1;
	LPVOID lpBase;
	const auto get_share_mm_start = std::chrono::high_resolution_clock::now();
	cv::Mat image_before_resize, image_scan;
	int temcols = atoi(AllStr[3].c_str()); // 图像宽
	//std::cout << "temcols " << temcols;
	int temrows = atoi(AllStr[4].c_str()); // 图像高
	//std::cout << "temrows " << temrows;
	int Width_l = atoi(AllStr[5].c_str()); // 左侧剪裁
	//std::cout << "Width_l " << Width_l;
	int Width_r = atoi(AllStr[6].c_str());// 右侧剪裁
	//std::cout << "Width_r " << Width_r;
	int Top = atoi(AllStr[7].c_str()); // 上侧剪裁
	//std::cout << "Top " << Top;
	int Bottom = atoi(AllStr[8].c_str()); // 下侧剪裁
	//std::cout << "Bottom " << Bottom;
	double ratio = atof(AllStr[9].c_str()); // 压缩比例
	std::string shared_name = AllStr[2].c_str();//获取共享内存
	int MMAP_SIZE = temcols * temrows * 3;
	hMapFile1 = OpenFileMapping(FILE_MAP_WRITE,
		TRUE,
		AllStr[2].c_str()		// 共享内存名称
	);
	lpBase = MapViewOfFile(     // 映射缓存区视图 , 得到指向共享内存的指针
		hMapFile1,              // 共享内存的句柄
		FILE_MAP_ALL_ACCESS,    // 可读写许可
		0, 0, MMAP_SIZE
	);
	char* imgdata = (char*)lpBase;
	cv::Mat image2(cv::Size(temcols, temrows), CV_8UC3, imgdata);
	image_before_resize = image2;
	//std::cout << "Shared Established" << std::endl;
	logout << utils::currentDateTime() + " Shared Established!\n";
	const auto get_share_mm_end = std::chrono::high_resolution_clock::now();
	const float get_share_mm_cost = std::chrono::duration<float, std::milli>(get_share_mm_end - get_share_mm_start).count();
	logout << utils::currentDateTime() + " get_share_mm_cost: " + std::to_string(get_share_mm_cost) + "\n";
	//cv::imwrite("image_before_resize.bmp", image_before_resize);


	//图像压缩
	const auto resize_share_mm_start = std::chrono::high_resolution_clock::now();
	cv::Mat image_after_resize;
	temcols = temcols * ratio;
	temrows = temrows * ratio;
	cv::resize(image_before_resize, image_after_resize, cv::Size(), ratio, ratio);
	const auto resize_share_mm_end = std::chrono::high_resolution_clock::now();
	const float resize_share_mm_cost = std::chrono::duration<float, std::milli>(resize_share_mm_end - resize_share_mm_start).count();
	logout << utils::currentDateTime() + " resize_share_mm_cost: " + std::to_string(resize_share_mm_cost) + "\n";
	//cv::imwrite("image_after_resize.bmp", image_after_resize);

	// 图像裁剪
	const auto rect_share_mm_start = std::chrono::high_resolution_clock::now();
	cv::Rect origin_rect;
	origin_rect.x = Width_l;
	origin_rect.y = Top;
	origin_rect.width = temcols - Width_l - Width_r;
	origin_rect.height = temrows - Top - Bottom;
	std::cout << "cis col after cut is " << origin_rect.width << "," << "cis rows after cut is " << origin_rect.height << std::endl;
	image_scan = image_after_resize(origin_rect);
	const auto rect_share_mm_end = std::chrono::high_resolution_clock::now();
	const float rect_share_mm_cost = std::chrono::duration<float, std::milli>(rect_share_mm_end - rect_share_mm_start).count();
	logout << utils::currentDateTime() + " rect_share_mm_cost: " + std::to_string(rect_share_mm_cost) + "\n";
	//cv::imwrite("image_scan.bmp", image_scan);

	//DrawOriginImg doi_temp;
	//doi_temp.dir = AllStr[1].c_str();
	//doi_temp.img = image_scan;
	//image_scan_vector.push_back(doi_temp);

	if (NULL != lpBase) {
		UnmapViewOfFile(lpBase);
		lpBase = NULL;
	}
	if (NULL != hMapFile1) {
		CloseHandle(hMapFile1);
		hMapFile1 = NULL;
	}
	return image_scan;
}

cv::Mat utils::Sharemm_Pretreatment_ccd(std::vector<std::string> AllStr, std::string port, float cis_rows, float cis_cols) {
	std::ofstream logout = utils::createLog(port);
	HANDLE hMapFile1;
	LPVOID lpBase;
	const auto get_share_mm_start = std::chrono::high_resolution_clock::now();
	cv::Mat image_before_resize, image_scan;
	int temcols = atoi(AllStr[11].c_str()); // 图像宽
	int temrows = atoi(AllStr[12].c_str()); // 图像高
	int Width_l = atoi(AllStr[13].c_str()); // 左侧剪裁
	int Width_r = atoi(AllStr[14].c_str());// 右侧剪裁
	int Top = atoi(AllStr[15].c_str()); // 上侧剪裁
	int Bottom = atoi(AllStr[16].c_str()); // 下侧剪裁
	double ratio = atof(AllStr[17].c_str()); // 压缩比例
	std::string shared_name = AllStr[10].c_str();//获取共享内存
	int MMAP_SIZE = temcols * temrows;

	hMapFile1 = OpenFileMapping(FILE_MAP_WRITE,
		TRUE,
		AllStr[10].c_str()		// 共享内存名称
	);
	lpBase = MapViewOfFile(     // 映射缓存区视图 , 得到指向共享内存的指针
		hMapFile1,              // 共享内存的句柄
		FILE_MAP_ALL_ACCESS,    // 可读写许可
		0, 0, MMAP_SIZE
	);
	char* imgdata = (char*)lpBase;
	cv::Mat image2(cv::Size(temcols, temrows), CV_8UC1, imgdata);
	image_before_resize = image2;
	//std::cout << "Shared Established" << std::endl;
	logout << utils::currentDateTime() + " Shared Established!\n";
	const auto get_share_mm_end = std::chrono::high_resolution_clock::now();
	const float get_share_mm_cost = std::chrono::duration<float, std::milli>(get_share_mm_end - get_share_mm_start).count();
	logout << utils::currentDateTime() + " get_share_mm_cost: " + std::to_string(get_share_mm_cost) + "\n";
	//cv::imwrite("image_before_resize.bmp", image_before_resize);


	//图像压缩
	const auto resize_share_mm_start = std::chrono::high_resolution_clock::now();
	cv::Mat image_after_resize;
	temcols = temcols * ratio;
	temrows = temrows * ratio;
	cv::resize(image_before_resize, image_after_resize, cv::Size(), ratio, ratio);
	const auto resize_share_mm_end = std::chrono::high_resolution_clock::now();
	const float resize_share_mm_cost = std::chrono::duration<float, std::milli>(resize_share_mm_end - resize_share_mm_start).count();
	logout << utils::currentDateTime() + " resize_share_mm_cost: " + std::to_string(resize_share_mm_cost) + "\n";
	//cv::imwrite("image_after_resize.bmp", image_after_resize);

	// 图像裁剪
	const auto rect_share_mm_start = std::chrono::high_resolution_clock::now();
	cv::Rect origin_rect;
	origin_rect.x = Width_l;
	origin_rect.y = Top;
	origin_rect.width = temcols - Width_l - Width_r;
	origin_rect.height = temrows - Top - Bottom;
	std::cout << "ccd col after cut is " << origin_rect.width << "," << "ccd rows after cut is " << origin_rect.height << std::endl;
	image_scan = image_after_resize(origin_rect);
	const auto rect_share_mm_end = std::chrono::high_resolution_clock::now();
	const float rect_share_mm_cost = std::chrono::duration<float, std::milli>(rect_share_mm_end - rect_share_mm_start).count();
	logout << utils::currentDateTime() + " rect_share_mm_cost: " + std::to_string(rect_share_mm_cost) + "\n";
	//cv::imwrite("image_scan.bmp", image_scan);

	// 处理ccd图片，resize到cis尺寸
	//float resize_rows;
	//float resize_cols;
	//if (image_scan.rows > cis_rows || image_scan.cols > cis_cols) {
	//	if ((image_scan.rows - cis_rows) > (image_scan.cols - cis_cols)) {
	//		resize_rows = cis_rows;
	//		resize_cols = resize_rows * (image_scan.cols / image_scan.rows);
	//		cv::Size dsize = cv::Size(resize_rows, resize_cols);
	//		cv::resize(image_scan, image_with_cis_size, dsize, 0, 0, cv::INTER_AREA);
	//	}
	//	else if ((image_scan.rows - cis_rows) <= (image_scan.cols - cis_cols)) {
	//		resize_cols = cis_cols;
	//		resize_rows = resize_cols * (resize_rows / image_scan.cols);
	//		cv::Size dsize2 = cv::Size(resize_rows, resize_cols);
	//		cv::resize(image_scan, image_with_cis_size, dsize2, 0, 0, cv::INTER_AREA);
	//	}
	//}
	//DrawOriginImg doi_temp;
	//doi_temp.dir = AllStr[1].c_str();
	//doi_temp.img = image_scan;
	//image_scan_vector.push_back(doi_temp);

	if (NULL != lpBase) {
		UnmapViewOfFile(lpBase);
		lpBase = NULL;
	}
	if (NULL != hMapFile1) {
		CloseHandle(hMapFile1);
		hMapFile1 = NULL;
	}
	return image_scan;
}

// 读取阈值文件
utils::SettingsInfor utils::read_threshold(int camera_type, int threshold_num) {
	SettingsInfor sf;
	std::string th1, th2, show_image_flag;
	std::ifstream file;

	//1.设置拣选的阈值
	std::string threshold_self1 = "0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1";
	std::string threshold_tile1 = "0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1";
	std::vector< std::string > threshold_self1_vector = split(threshold_self1, ",");
	std::vector< std::string > threshold_tile1_vector = split(threshold_tile1, ",");
	std::string show_image_flag_default = "cis";
	// 2.设置喷墨后阈值
	std::string threshold_self2 = "0.55,0.4,0.4,0.4,0.4,0.4,0.4,0.5,0.4,0.65,0.3,0.4,0.5,0.4";
	std::string threshold_tile2 = "0.65,0.5,0.5,0.5,0.5,0.5,0.5,0.6,0.5,0.75,0.4,0.5,0.6,0.5";
	std::vector< std::string > threshold_self2_vector = split(threshold_self2, ",");
	std::vector< std::string > threshold_tile2_vector = split(threshold_tile2, ",");
	// 2.设置后处理阈值
	//std::string threshold_post = "0.3";
	file.open("./threshold.txt", std::ios_base::in);
	if (!file.is_open())
	{
		std::ofstream outfile("./threshold.txt", std::ios::out);
		if (camera_type == 1) {
			if (threshold_self1_vector.size() == threshold_num) {
				outfile << threshold_self1 << std::endl;
				outfile << threshold_tile1 << std::endl;
				outfile << show_image_flag_default << std::endl;
			}
			else { std::cout << "threshold num error!" << std::endl; }
		}
		else {
			if (threshold_self2_vector.size() == threshold_num) {
				outfile << threshold_self2 << std::endl;
				outfile << threshold_tile2 << std::endl;
				outfile << show_image_flag_default << std::endl;
			}
			else { std::cout << "threshold num error!" << std::endl; }
		}
		file.open("./threshold.txt", std::ios_base::in);
	}
	getline(file, th1);
	getline(file, th2);
	getline(file, show_image_flag);

	sf.th1 = th1;
	sf.th2 = th2;
	if (show_image_flag == "") sf.show_image_flag = show_image_flag_default;
	else sf.show_image_flag = show_image_flag;


	file.close();
	return sf;
}

float utils::CountChannelColor(std::vector<float> tempmean_autoLearn) {
	float tempcolor;
	if (tempmean_autoLearn.size() == 0) { tempcolor = 0; }
	else {
		float sum = std::accumulate(std::begin(tempmean_autoLearn), std::end(tempmean_autoLearn), 0.0);
		tempcolor = sum / tempmean_autoLearn.size();
	}
	return tempcolor;
}

int utils::dectections_merge(std::vector<std::shared_ptr<Detection::DetResult>>& result, std::shared_ptr<Detection::DetResult>& detections, std::vector<cv::Size2f> scale_factors, std::vector<cv::Point> crop_points, std::vector<float> threshold, int th_area) {
	assert(result.size() == scale_factors.size());
	assert(result.size() == crop_points.size());
	auto& detections_bboxes = detections->mutable_defects();
	detections_bboxes.clear();
	for (int i = 0; i < result.size(); ++i) {
		auto& ret = result[i];
		auto bboxes = ret->immutable_defects();
		for (auto& bbox : bboxes) {
			if (bbox.confidence > threshold[bbox.label] && bbox.area > th_area) {
				int left = bbox.left / scale_factors[i].width + crop_points[i].x;
				int top = bbox.top / scale_factors[i].height + crop_points[i].y;
				int right = bbox.right / scale_factors[i].width + crop_points[i].x;
				int bottom = bbox.bottom / scale_factors[i].height + crop_points[i].y;
				detections_bboxes.emplace_back(left, top, right, bottom, bbox.confidence, bbox.label);
			}
		}
	}
	return detections_bboxes.size();
}
