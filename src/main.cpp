#include <chrono>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <iomanip>

#include <WINSOCK2.H>   
#include <stdio.h>   
#include <direct.h>
#include <io.h>
#include <Windows.h>
#include <map>
#include <thread>
#include <iomanip>

#include <detection.h>
#include <pre_processing.h>

#include "data.h"
#include "utils.h"
#include "SimpleIni.h"

// 参数初始化
#define PORT           8888    
#define MSGSIZE        1024 
#define RESIZERATIO    0.625
#pragma comment(lib, "ws2_32.lib") 
std::string label_sum; // collect result label
std::vector<std::string> confidence; // 记录各个缺陷的名称和置信度
int camera_type = 1; // 1.代表拣选相机，2.代表喷墨相机
std::vector<std::string> classesorigin;
int detect_wrong_count = 0;
std::string th3;
std::vector<utils::DrawResult> draw_result;
std::vector<utils::DrawOriginImg> image_scan_vector;
int count_tile_num = 0;
std::string port = std::to_string(PORT);
const unsigned channels = 3;
std::array<float, 3> g_mean{ 123.675f, 116.28f, 103.53f };
std::array<float, 3> g_std{ 58.395f, 57.12f, 57.375f };

void Write_NG_thread() {
	std::ofstream logout = utils::createLog(port);
	while (true) {
		// 存缺陷结果图
		if (draw_result.size() > 0) {
			std::cout << "Write_NG_thread draw_size = " << draw_result.size() << std::endl;
			logout << utils::currentDateTime() + "draw_size = " + std::to_string(draw_result.size()) + "\n";
			logout << utils::currentDateTime() + " ****** " + "start Write_NG_thread" + " ****** " + "\n";
			cv::Mat pic_overview = draw_result[0].sampleimg;
			std::vector<std::string> dir_split = utils::split(draw_result[0].dir, "/\\");
			std::string savepath_imgroi;
			std::string savepath_overview;
			std::string time_stamp = dir_split[5];
			std::string result_dir = dir_split[0] + "/" + dir_split[1] + "/" + dir_split[2] + "/" + dir_split[3] + "/ScanResultImages/NG/" + time_stamp;
			try {
				for (int i = 0; i < draw_result[0].r.size(); i++) {
					if (draw_result[0].pictype == "") {
						savepath_imgroi = result_dir + "/" + time_stamp + "-detail-" + draw_result[0].text[i] + "-" + std::to_string(i) + ".bmp";
					}
					else if (draw_result[0].pictype != "") {
						savepath_imgroi = result_dir + "/" + time_stamp + draw_result[0].pictype + "-detail-" + draw_result[0].text[i] + "-" + std::to_string(i) + ".bmp";
					}
					cv::Mat imgroi = draw_result[0].img(draw_result[0].r[i]);
					if (_access(result_dir.c_str(), 0) == -1) //如果文件夹不存在 
						_mkdir(result_dir.c_str());
					cv::imwrite(savepath_imgroi, imgroi);
					logout << utils::currentDateTime() + " write NG small pic: " + savepath_imgroi + "\n";
				}
				for (int j = 0; j < draw_result[0].r1.size(); j++) {
					if (j < draw_result[0].r.size()) {
						cv::rectangle(pic_overview, draw_result[0].r1[j], cv::Scalar(115, 52, 239), 3);
						cv::putText(pic_overview, draw_result[0].text[j], cv::Point(draw_result[0].r1[j].x, draw_result[0].r1[j].y - 1),
							cv::FONT_HERSHEY_PLAIN, 1.1, cv::Scalar(115, 52, 239), 2);
					}
					else {
						cv::rectangle(pic_overview, draw_result[0].r1[j], cv::Scalar(80, 200, 80), 1);
						cv::putText(pic_overview, draw_result[0].text[j], cv::Point(draw_result[0].r1[j].x, draw_result[0].r1[j].y - 1),
							cv::FONT_HERSHEY_PLAIN, 1.1, cv::Scalar(80, 200, 80), 1);
					}
				}
				if (draw_result[0].pictype == "") { savepath_overview = result_dir + "/" + time_stamp + "-overview.jpg"; }
				else if (draw_result[0].pictype != "") { savepath_overview = result_dir + "/" + time_stamp + draw_result[0].pictype + "-overview.jpg"; }
				cv::imwrite(savepath_overview, pic_overview);
				std::cout << "write NG overview pic:" << savepath_overview << std::endl;
				logout << utils::currentDateTime() + " write NG overview pic succeed " + "\n";
			}
			catch (...)
			{
				std::cout << "Write NG Picture YiChang!" << std::endl;
				logout << utils::currentDateTime() + " Write NG Picture YiChang! " + "\n";
			}
			std::vector<utils::DrawResult>::iterator n = draw_result.begin();
			draw_result.erase(n);
			logout << utils::currentDateTime() + " ****** " + " Write_NG_thread end once " + " ****** " + "\n";
		}

		//// 存原始图像
		//if (image_scan_vector.size() > 0) {
		//    logout << currentDateTime() + "image_scan_vector = " + std::to_string(image_scan_vector.size()) + "\n";
		//    logout << currentDateTime() + " ****** " + "start Write_origin_thread" + " ****** " + "\n";
		//    std::string savepath_originimg;
		//    std::vector<std::string> dir_split = split(image_scan_vector[0].dir, "/\\");
		//    std::string time_stamp = dir_split[5];
		//    std::string result_dir = dir_split[0] + "/" + dir_split[1] + "/" + dir_split[2] + "/" + dir_split[3] + "/ScanOriginImages/" + time_stamp;
		//    try {
		//        savepath_originimg = result_dir + "/" + time_stamp +"-origin.bmp";
		//        if (_access(result_dir.c_str(), 0) == -1) //如果文件夹不存在 
		//            _mkdir(result_dir.c_str());
		//        cv::imwrite(savepath_originimg, image_scan_vector[0].img);
		//        logout << currentDateTime() + " write Origin Img succeed " + "\n";
		//    }
		//    catch (...) {
		//        std::cout << "Write OriginImg YiChang!" << std::endl;
		//        logout << currentDateTime() + " Write OriginImg YiChang! " + "\n";
		//    }
		//    std::vector<DrawResult>::iterator n = draw_result.begin();
		//    draw_result.erase(n);
		//    logout << currentDateTime() + " ****** " + " Write_origin_thread end once " + " ****** " + "\n";
		//}
	}
}

// 缺陷画框和图像保存
std::string tileinfer(cv::Mat& img, std::shared_ptr<Detection::DetResult>& res, std::string result_dirs, cv::Mat sampleimg,
	std::string pictype, std::shared_ptr<Detection::DetResult>& detection_remove)
{
	std::ofstream logout = utils::createLog(port);
	logout << utils::currentDateTime() + " ****** " + "start tileinfer" + " ****** " + "\n";
	std::string flag;
	cv::Mat img_origin = img;
	std::string text;
	std::vector<std::string> vector_text;
	std::vector<cv::Rect> vector_r;
	std::vector<cv::Rect> vector_r1;
	std::string label_sum_null;
	label_sum = label_sum_null;

	auto& res_bboxes = res->mutable_defects();
	auto& detection_remove_bboxes = detection_remove->mutable_defects();
	if (res_bboxes.size() > 0)
	{
		confidence.clear();
		flag = "NG";
		int count = 0;
		int imgcol;
		int imgrow;
		int img_center_x;
		int img_center_y;
		int img_box_area;
		std::vector<utils::Centerlabel3> filter_box;
		std::vector<int> flag_filter_box(res_bboxes.size());
		int n = 0;
		flag_filter_box[0] = 0;
		try {
			for (size_t j = 0; j < res_bboxes.size(); j++) {
				logout << utils::currentDateTime() + " read left " + std::to_string(res_bboxes[j].left) + "\n";
				logout << utils::currentDateTime() + " read right " + std::to_string(res_bboxes[j].right) + "\n";
				logout << utils::currentDateTime() + " read top " + std::to_string(res_bboxes[j].top) + "\n";
				logout << utils::currentDateTime() + " read bottom " + std::to_string(res_bboxes[j].bottom) + "\n";
				text = classesorigin[(int)res_bboxes[j].label];
				logout << utils::currentDateTime() + " origin label is " + text + "\n";
				//bbox整合
				float box_x = (res_bboxes[j].right + res_bboxes[j].left) / 2;
				float box_y = (res_bboxes[j].bottom + res_bboxes[j].top) / 2;
				float box_label = res_bboxes[j].label;
				utils::Centerlabel3 filter_box_temp(box_x, box_y, box_label);
				filter_box.push_back(filter_box_temp);
				std::cout << text << "-score-" << j << "=" << res_bboxes[j].confidence << std::endl;
				if (j >= 1) {
					for (int k = 0; k < j; k++) {
						int x1 = res_bboxes[k].left;
						int y1 = res_bboxes[k].top;
						int h1 = res_bboxes[k].bottom - res_bboxes[k].top;
						int w1 = res_bboxes[k].right - res_bboxes[k].left;
						int area1 = h1 * w1;

						int x2 = res_bboxes[j].left;
						int y2 = res_bboxes[j].top;
						int h2 = res_bboxes[j].bottom - res_bboxes[j].top;
						int w2 = res_bboxes[j].right - res_bboxes[j].left;
						int area2 = h2 * w2;

						float r1 = 0;
						float r2 = 0;

						float ratio = utils::overlapRate(x1, y1, h1, w1, x2, y2, h2, w2, r1, r2);
						if (r1 == 1) {
							res_bboxes[k].left = res_bboxes[j].left;
							res_bboxes[k].right = res_bboxes[j].right;
							res_bboxes[k].top = res_bboxes[j].top;
							res_bboxes[k].bottom = res_bboxes[j].bottom;
							if (res_bboxes[k].confidence < res_bboxes[j].confidence) {
								res_bboxes[k].confidence = res_bboxes[j].confidence;
								res_bboxes[k].label = res_bboxes[j].label;
							}
							flag_filter_box[j] = 1;
							break;
						}
						else if (r2 == 1) {
							if (res_bboxes[k].confidence < res_bboxes[j].confidence) {
								res_bboxes[k].confidence = res_bboxes[j].confidence;
								res_bboxes[k].label = res_bboxes[j].label;
							}
							flag_filter_box[j] = 1;
							break;
						}
						else if (ratio > 0.3 || r1 > 0.3 || r2 > 0.3) {
							if (res_bboxes[k].left > res_bboxes[j].left) { res_bboxes[k].left = res_bboxes[j].left; }
							if (res_bboxes[k].right < res_bboxes[j].right) { res_bboxes[k].right = res_bboxes[j].right; }
							if (res_bboxes[k].top > res_bboxes[j].top) { res_bboxes[k].top = res_bboxes[j].top; }
							if (res_bboxes[k].bottom < res_bboxes[j].bottom) { res_bboxes[k].bottom = res_bboxes[j].bottom; }
							if (res_bboxes[k].confidence < res_bboxes[j].confidence) {
								res_bboxes[k].confidence = res_bboxes[j].confidence;
								res_bboxes[k].label = res_bboxes[j].label;
							}
							flag_filter_box[j] = 1;
							break;
						}
						else { flag_filter_box[j] = 0; }
					}
				}
				if ((res_bboxes[j].right - res_bboxes[j].left) != 0 && (res_bboxes[j].bottom - res_bboxes[j].top) != 0 && flag_filter_box[j] != 1) {
					img_box_area = (res_bboxes[j].right - res_bboxes[j].left) * (res_bboxes[j].bottom - res_bboxes[j].top);
					imgcol = img.cols;
					imgrow = img.rows;

					if (res_bboxes[j].left > 20) res_bboxes[j].left -= 20;
					else if (res_bboxes[j].left < 20) res_bboxes[j].left = 0;

					if ((imgcol - res_bboxes[j].right) > 20) res_bboxes[j].right += 20;
					else if ((imgcol - res_bboxes[j].right) < 20) res_bboxes[j].right = imgcol;

					if (res_bboxes[j].top > 20)res_bboxes[j].top -= 20;
					else if (res_bboxes[j].top < 20)res_bboxes[j].top = 0;

					if ((imgrow - res_bboxes[j].bottom) > 20)res_bboxes[j].bottom += 20;
					else if ((imgrow - res_bboxes[j].bottom) < 20) res_bboxes[j].bottom = imgrow;

					cv::Rect r_detail = cv::Rect(res_bboxes[j].left, res_bboxes[j].top, res_bboxes[j].right - res_bboxes[j].left, res_bboxes[j].bottom - res_bboxes[j].top);
					cv::Rect r_overview = cv::Rect(RESIZERATIO * res_bboxes[j].left, RESIZERATIO * res_bboxes[j].top, RESIZERATIO * res_bboxes[j].right - RESIZERATIO * res_bboxes[j].left, RESIZERATIO * res_bboxes[j].bottom - RESIZERATIO * res_bboxes[j].top);
					vector_r.push_back(r_detail);
					vector_r1.push_back(r_overview);
					img_center_x = (int)(0.5 * (res_bboxes[j].right + res_bboxes[j].left));
					img_center_y = (int)(0.5 * (res_bboxes[j].bottom + res_bboxes[j].top));
					if (std::find(classesorigin.begin(), classesorigin.end(), text) != classesorigin.end()) {
						//if (count > 4) {
						//    logout << currentDateTime() + " socket 2 long! defect sum == " + std::to_string(res.size()) + "\n";
						//    break;
						//}
						if (count == 0) {
							label_sum += text + "!" + std::to_string(img_box_area) + "!" + std::to_string(img_center_x) + "!" + std::to_string(img_center_y)
								+ "!" + std::to_string((int)res_bboxes[j].left) + "!" + std::to_string((int)res_bboxes[j].right)
								+ "!" + std::to_string((int)res_bboxes[j].top) + "!" + std::to_string((int)res_bboxes[j].bottom);
						}
						//else if(count <= 4) {
						else {
							label_sum += "+" + text + "!" + std::to_string(img_box_area) + "!" + std::to_string(img_center_x) + "!" + std::to_string(img_center_y)
								+ "!" + std::to_string((int)res_bboxes[j].left) + "!" + std::to_string((int)res_bboxes[j].right)
								+ "!" + std::to_string((int)res_bboxes[j].top) + "!" + std::to_string((int)res_bboxes[j].bottom);
						}
						// 保留两位小数
						int t_score = (int)(res_bboxes[j].confidence * 100);
						float score_2f = (float)t_score / 100;
						std::ostringstream strStream;
						strStream << score_2f;
						std::string t_text = text + "-" + strStream.str();

						vector_text.push_back(t_text);
						count = count + 1;
						std::cout << "--------------- " << std::endl;
						std::cout << "detect label is " << text << std::endl;
						std::cout << "detect score is " << res_bboxes[j].confidence << "\n" << std::endl;
						confidence.push_back(t_text);
						//confidence.push_back(convert(res[j].score));
					}
				}
			}
			for (int n = 0; n < detection_remove_bboxes.size(); n++) {
				cv::Rect r_remove = cv::Rect(RESIZERATIO * detection_remove_bboxes[n].left - 10, RESIZERATIO * detection_remove_bboxes[n].top - 10,
					RESIZERATIO * detection_remove_bboxes[n].right - RESIZERATIO * detection_remove_bboxes[n].left + 20,
					RESIZERATIO * detection_remove_bboxes[n].bottom - RESIZERATIO * detection_remove_bboxes[n].top + 20);

				std::cout << "$$$$$$" << RESIZERATIO * detection_remove_bboxes[n].left << "," << RESIZERATIO * detection_remove_bboxes[n].top << "," <<
					RESIZERATIO * detection_remove_bboxes[n].right - RESIZERATIO * detection_remove_bboxes[n].left << "," <<
					RESIZERATIO * detection_remove_bboxes[n].bottom - RESIZERATIO * detection_remove_bboxes[n].top << std::endl;
				vector_r1.push_back(r_remove);

				std::string text_rm = classesorigin[(int)detection_remove_bboxes[n].label];
				int remove_score = (int)(detection_remove_bboxes[n].confidence * 100);
				float remove_score_2f = (float)remove_score / 100;
				std::ostringstream strStream_rm;
				strStream_rm << remove_score_2f;
				std::string t_text_rm = text_rm + "-" + strStream_rm.str();
				vector_text.push_back(t_text_rm);
			}
			utils::DrawResult dr;
			dr.img = img_origin;
			dr.sampleimg = sampleimg;
			dr.r = vector_r;
			dr.r1 = vector_r1;
			dr.text = vector_text;
			dr.dir = result_dirs;
			dr.pictype = pictype;
			draw_result.push_back(dr);
		}
		catch (...)
		{
			std::cout << "Get Result YiChang!" << std::endl;
			logout << utils::currentDateTime() + " Get Result YiChang! " + "\n";
		}
		if (count == 0) {
			logout << utils::currentDateTime() + " ****** " + "tileinfer end" + " ****** " + "\n";
			return "1";
		}
		else {
			logout << utils::currentDateTime() + " ****** " + "tileinfer end" + " ****** " + "\n";
			return "2";
		}
	}
	else {
		logout << utils::currentDateTime() + " ****** " + "tileinfer end" + " ****** " + "\n";
		return "1";
	}
}

// main entrance
int main()
{
	utils::CloseConsoleMode();
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile("conf/Config.ini");
	std::string th1 = ini.GetValue("threshold", "th1");
	int th_area = (int)ini.GetLongValue("threshold", "th_area", 1);
	//std::cout << th1;
	std::cout << th_area;

	// tensorRT引擎初始化
	//const std::string& model_path = "test_model_high_resolution.trt";
	//const std::string& model_path = "test_model_0211_cis_16.trt"; 
	const std::string& model_path = "test_model_20230213_ccd_bs4.trt";

	auto engine = App::create_infer<Detection::DetResult>(model_path, Detection::faster_rcnn_parser);

	//初始化检测阈值
	// 1.拣选第三版分类，18类
	if (camera_type == 1) {
		classesorigin = { "AngleEdgeBroken","DropInk","PullLine" ,"Bubble","LackInk" ,"Crack", "Mark","WhitePoint",
			"BurstGlaze", "DropDirty","DropGlaze","PolishWhite","LackGlaze","DropWater","ColorMarks","PrintHead","Blister","Pit" };
	}
	// 2.喷墨后分类
	else {
		classesorigin = { "AngleEdgeBroken","DropInk","PullLine" ,"PrintHead","Break" ,"Line", "Huahen","DropGlaze", "Zhenkong",
			"Guopao","LackGlaze","Paohen","Lanzhuan","Jihao" };
	}
	int threshold_num = classesorigin.size();
	std::vector<float> threshold(threshold_num);
	std::vector<float> threshold1(threshold_num);
	std::vector<float> threshold2(threshold_num);
	//std::pair<std::string, std::string> th_temp = read_threshold(camera_type, threshold_num);// 从threshold.txt中读取阈值
	//std::cout << "th1 : " << th_temp.first << std::endl;
	//std::cout << "th2 : " << th_temp.second << std::endl;
	//threshold1 = threshold_stof(th_temp.first, threshold1);
	//threshold2 = threshold_stof(th_temp.second, threshold2);
	//threshold = threshold2;
	//std::ofstream logout_th = createLog(port);
	//logout_th << currentDateTime() + " threshold1: " + th_temp.first + "\n";
	//logout_th << currentDateTime() + " threshold2: " + th_temp.second + "\n";

	utils::SettingsInfor settings_infor = utils::read_threshold(camera_type, threshold_num);// 从threshold.txt中读取阈值
	std::cout << "th1 : " << settings_infor.th1 << std::endl;
	std::cout << "th2 : " << settings_infor.th2 << std::endl;
	std::cout << "show_image_flag : " << settings_infor.show_image_flag << std::endl;
	threshold1 = utils::threshold_stof(settings_infor.th1, threshold1);
	threshold2 = utils::threshold_stof(settings_infor.th2, threshold2);
	threshold = threshold2;
	std::string show_image_flag = settings_infor.show_image_flag;
	std::ofstream logout = utils::createLog(port);
	logout << utils::currentDateTime() + " threshold1: " + settings_infor.th1 + "\n";
	logout << utils::currentDateTime() + " threshold2: " + settings_infor.th2 + "\n";
	logout << utils::currentDateTime() + " show_image_flag changed to: " + settings_infor.show_image_flag + "\n";

	//初始化反误检库
	std::vector<utils::Centerlabel> redefectpoints;   //防误检点击库
	std::vector<utils::Centerlabel> redefectpoints_sum; // 汇总防误检库，作为总防误检库
	//std::vector<utils::Centerlabel> redefectpoints_sum2; // 初始化实时添加模板后的反误检库
	int current_add_flag = 0;
	//std::thread redfect_thread(redefections_add_thread);

	// 初始化色差参数
	std::vector<float> tempcolor(channels);
	int count_color;
	std::string pictype0 = "color";
	std::string pictype1 = "gray";
	std::string pictype;
	std::vector<std::vector<float>> tempmean_autoLearn_vector(channels);
	std::vector<float> tempmean_autoLearn0;
	std::vector<float> tempmean_autoLearn1;
	std::vector<float> tempmean_autoLearn2;

	// 初始化其他参数
	int temcols;
	int temrows;
	int avg_color = 0;
	int color_diff_flag = 0;

	//socket通信部分代码，广播模式，可接收任意ip发来的通信
	WSADATA wsaData; // 加载网络库
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;
	SOCKADDR clntAddr;
	int ret;
	char szMessage[MSGSIZE];
	int iaddrSize = sizeof(SOCKADDR);
	WSAStartup(0x0202, &wsaData); // 设置版本号
	/*  创建服务器端的socket
		第一个是协议族的类型
		第二个是STREAM类型的套接字
		第三个是TCP  */
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	local.sin_family = AF_INET;// 设置使用的协议簇
	local.sin_port = htons(PORT);// 设置使用的端口，htons 把本地字节序转为网络字节序
	local.sin_addr.s_addr = htonl(INADDR_ANY);// INADDR_ANY就是0.0.0.0的地址，表示为任意地址
	bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR_IN));
	listen(sListen, 1);//设置监听服务器端口,第二个参数指连接的最大数量
	printf("Start listening\n");

	// 开启存图线程
	std::thread th_write(Write_NG_thread); //子线程1
	th_write.detach();

	// 开始执行推理任务
	while (TRUE) {
		sClient = accept(sListen, (SOCKADDR*)&clntAddr, &iaddrSize);
		printf("Connected to the client please load template first\n");
		try {
			while (TRUE)
			{
				const auto t_whole_time_with_wait_start = std::chrono::high_resolution_clock::now();
				std::ofstream logout = utils::createLog(port);
				std::cout << "wait for send...\n" << std::endl;
				logout << "\n" << utils::currentDateTime() + " wait for send...\n";
				ret = recv(sClient, szMessage, MSGSIZE, 0);
				if (ret == -1) break;
				szMessage[ret] = '\0';
				count_tile_num += 1;
				std::vector<std::string> AllStr = utils::split(szMessage, "#"); //分割路径
				std::string socketType = AllStr[0].c_str();              //获取图片类型
				std::string picAddress = AllStr[1].c_str();              //获取路径
				std::cout << "from software recevied:" << szMessage << std::endl;
				logout << utils::currentDateTime() + " from software recevied: " + szMessage + "\n";

				// 判断socket
				if (size(socketType) == 0 || AllStr.size() > 10 ||
					(socketType.compare("TemplateImage") != 0
						&& socketType.compare("ScanResultImage") != 0)
					&& socketType.compare("ScanTemplate") != 0
					&& socketType.compare("AScanResultImage") != 0
					&& socketType.compare("PScanResultImage") != 0
					&& socketType.compare("AutoLearn") != 0
					&& socketType.compare("SetScanResultImage") != 0)
				{
					char sendbuf1[] = "The format is not standardized, please resend.";
					std::cout << sendbuf1 << std::endl;
					logout << utils::currentDateTime() + " socket error !" + sendbuf1 + "\n";
					continue;
				}
				/*
				* 抛光线缺陷推理算法
				*/

				// 抛光线模板初始化
				if (socketType.compare("ScanTemplate") == 0) {
					redefectpoints.clear();
					redefectpoints_sum.clear();
					avg_color = 0;
					color_diff_flag = 0;
					tempcolor.clear();
					std::string defect_result = "ScanTemplateSucceed#";
					send(sClient, defect_result.c_str(), defect_result.size() + 1, 0);
				}
				// 抛光线自学习+算法推理
				else if (socketType.compare("ScanResultImage") == 0 || socketType.compare("AScanResultImage") == 0
					|| socketType.compare("AutoLearn") == 0 || socketType.compare("SetScanResultImage") == 0)
				{
					std::vector<utils::Centerlabel> redefectpoints_add;
					redefectpoints_sum.clear();
					redefectpoints_add = utils::redefections_add(port);
					redefectpoints_sum.insert(redefectpoints_sum.end(), redefectpoints_add.begin(), redefectpoints_add.end()); //将实时反误检库加到总反误检库中
					std::cout << "redefectpoints_sum : " << redefectpoints_sum.size() << std::endl;
					// 实时添加反误检库
					if (socketType.compare("AScanResultImage") == 0) {
						current_add_flag = 1;
						std::vector<utils::Centerlabel> redefectpoints_add;
						redefectpoints_sum.clear();
						redefectpoints_add = utils::redefections_add(port);
						redefectpoints_sum.insert(redefectpoints_sum.end(), redefectpoints_add.begin(), redefectpoints_add.end()); //将实时反误检库加到总反误检库中
						std::cout << "redefectpoints_sum : " << redefectpoints_sum.size() << std::endl;
						logout << utils::currentDateTime() + " false detection library num: " + std::to_string(redefectpoints_add.size()) + "\n";
						logout << utils::currentDateTime() + " redefect sum2: " + std::to_string(redefectpoints_sum.size()) + "\n";
					}
					// 实时修改检测阈值
					if (socketType.compare("SetScanResultImage") == 0) {
						logout << utils::currentDateTime() + " threshold start change !" + "\n";
						settings_infor = utils::read_threshold(camera_type, threshold_num);
						std::cout << "th1 : " << settings_infor.th1 << std::endl;
						std::cout << "th2 : " << settings_infor.th2 << std::endl;
						threshold1 = utils::threshold_stof(settings_infor.th1, threshold1);
						threshold2 = utils::threshold_stof(settings_infor.th2, threshold2);
						logout << utils::currentDateTime() + " threshold1 changed to: " + settings_infor.th1 + "\n";
						logout << utils::currentDateTime() + " threshold2 changed to: " + settings_infor.th2 + "\n";
					}
					if (socketType.compare("AutoLearn") == 0) {
						threshold = threshold1;
						std::cout << " self-study start!" << std::endl;
						logout << utils::currentDateTime() + " self-study start!\n";
					}
					if (socketType.compare("ScanResultImage") == 0) {
						threshold = threshold2;
					}
					// 开始推理
					const auto t_inference_total_time_start = std::chrono::high_resolution_clock::now();
					std::cout << "current_add_flag is: " << current_add_flag << std::endl; // 判断当前是否发生手动添加模板
					
					cv::Mat image_scan;
					cv::Mat sampleimg;
					std::string shared_name = AllStr[2].c_str();//获取共享内存
					std::string colorflag = "1";

					std::cout << "\n--------------------------------------------------" << std::endl;
					std::cout << "Obtain samples, inferencing....." << std::endl;
					logout << utils::currentDateTime() + " Obtain samples, inferencing.....\n";
					temcols = atoi(AllStr[3].c_str()); // 图像宽
					temrows = atoi(AllStr[4].c_str()); // 图像高

					/*const auto get_share_mm_start = std::chrono::high_resolution_clock::now();  */
					//int Width_l = atoi(AllStr[5].c_str()); // 左侧剪裁
					//int Width_r = atoi(AllStr[6].c_str());// 右侧剪裁
					//int Top = atoi(AllStr[7].c_str()); // 上侧剪裁
					//int Bottom = atoi(AllStr[8].c_str()); // 下侧剪裁
					//double ratio = atof(AllStr[9].c_str()); // 压缩比例

					if (shared_name.compare("") != 0) {
						image_scan = utils::Sharemm_Pretreatment(AllStr, port);

						//    /*float total1 = 0;
						//    float total2 = 0;*/
						//    std::string colorflag = "1";
						//    //std::cout << "shared name:" << shared_name << std::endl;
						//    HANDLE hMapFile1;
						//    LPVOID lpBase;
						//    try {
						//        int MMAP_SIZE = temcols * temrows * 3;
						//        hMapFile1 = OpenFileMapping(FILE_MAP_WRITE,
						//            TRUE,
						//            AllStr[2].c_str()		// 共享内存名称
						//        );
						//        lpBase = MapViewOfFile(     // 映射缓存区视图 , 得到指向共享内存的指针
						//            hMapFile1,              // 共享内存的句柄
						//            FILE_MAP_ALL_ACCESS,    // 可读写许可
						//            0,0, MMAP_SIZE
						//        );
						//        char* imgdata = (char*)lpBase;
						//        cv::Mat image2(cv::Size(temcols, temrows), CV_8UC3, imgdata);
						//        image_before_resize = image2;
						//    }
						//    catch (...) {
						//        std::string error_result = "0";
						//        send(sClient, error_result.c_str(), error_result.size() + 1, 0);
						//        UnmapViewOfFile(lpBase);
						//        CloseHandle(hMapFile1);
						//        continue;
						//    }
						//    std::cout << "Shared Established" << std::endl;
						//    logout << currentDateTime() + " Shared Established!\n";
						//    const auto get_share_mm_end = std::chrono::high_resolution_clock::now();
						//    const float get_share_mm_cost = std::chrono::duration<float, std::milli>(get_share_mm_end - get_share_mm_start).count();
						//    
						//    logout << currentDateTime() + " get_share_mm_cost: " + std::to_string(get_share_mm_cost) + "\n";
						//    //cv::imwrite("image_before_resize.bmp", image_before_resize);

						//    const auto resize_share_mm_start = std::chrono::high_resolution_clock::now();
						//    //图像压缩
						//    cv::Mat image_after_resize;
						//    temcols = temcols * ratio;
						//    temrows = temrows * ratio;
						//    cv::resize(image_before_resize, image_after_resize, cv::Size(), ratio, ratio);
						//    const auto resize_share_mm_end = std::chrono::high_resolution_clock::now();
						//    const float resize_share_mm_cost = std::chrono::duration<float, std::milli>(resize_share_mm_end - resize_share_mm_start).count();
						//    
						//    logout << currentDateTime() + " resize_share_mm_cost: " + std::to_string(resize_share_mm_cost) + "\n";
						//    //cv::imwrite("image_after_resize.bmp", image_after_resize);

						//    // 图像裁剪
						//    const auto rect_share_mm_start = std::chrono::high_resolution_clock::now();
						//    cv::Rect origin_rect;
						//    origin_rect.x = Width_l;
						//    origin_rect.y = Top;
						//    origin_rect.width = temcols - Width_l - Width_r;
						//    origin_rect.height = temrows - Top - Bottom;
						//    std::cout << "col after cut is " << origin_rect.width << "\n" << "rows after cut is " << origin_rect.height << std::endl;
						//    image_scan = image_after_resize(origin_rect);
							//const auto rect_share_mm_end = std::chrono::high_resolution_clock::now();
							//const float rect_share_mm_cost = std::chrono::duration<float, std::milli>(rect_share_mm_end - rect_share_mm_start).count();
							//logout << currentDateTime() + " rect_share_mm_cost: " + std::to_string(rect_share_mm_cost) + "\n";
						//    //cv::imwrite("image_scan.bmp", image_scan);
						//    image_scan_vector.push_back(image_scan);

						// 计算前五张瓷砖颜色均值
						const auto t_colordiff_start = std::chrono::high_resolution_clock::now();
						if (color_diff_flag == 0 && avg_color < 5)
						{
							avg_color += 1;
							// 计算当前图像三通道颜色
							tempmean_autoLearn0.push_back(cv::mean(image_scan).val[0]);
							tempmean_autoLearn1.push_back(cv::mean(image_scan).val[1]);
							tempmean_autoLearn2.push_back(cv::mean(image_scan).val[2]);
							tempmean_autoLearn_vector[0] = tempmean_autoLearn0;
							tempmean_autoLearn_vector[1] = tempmean_autoLearn1;
							tempmean_autoLearn_vector[2] = tempmean_autoLearn2;
							logout << utils::currentDateTime() + " img color: " + std::to_string(cv::mean(image_scan).val[0]) + "," +
								std::to_string(cv::mean(image_scan).val[1]) + "," + std::to_string(cv::mean(image_scan).val[2]) + "\n";
							// 计算当前扫描多张图像均值
							//if (tempmean_autoLearn0.size() == 0) { tempcolor[0] = 0; }
							//else {
							//    float sum0 = std::accumulate(std::begin(tempmean_autoLearn0), std::end(tempmean_autoLearn0), 0.0);
							//    tempcolor[0] = sum0 / tempmean_autoLearn0.size();
							//}
							//if (tempmean_autoLearn1.size() == 0) { tempcolor[1] = 0; }
							//else {
							//    float sum1 = std::accumulate(std::begin(tempmean_autoLearn1), std::end(tempmean_autoLearn1), 0.0);
							//    tempcolor[1] = sum1 / tempmean_autoLearn1.size();
							//}
							//if (tempmean_autoLearn2.size() == 0) { tempcolor[2] = 0; }
							//else {
							//    float sum2 = std::accumulate(std::begin(tempmean_autoLearn2), std::end(tempmean_autoLearn2), 0.0);
							//    tempcolor[2] = sum2 / tempmean_autoLearn2.size();
							//}
							//tempcolor[0] = CountChannelColor(tempmean_autoLearn0, 0);
							//tempcolor[1] = CountChannelColor(tempmean_autoLearn1, 1);
							//tempcolor[2] = CountChannelColor(tempmean_autoLearn2, 2);
							for (int i = 0; i < channels; i++) {
								tempcolor[i] = utils::CountChannelColor(tempmean_autoLearn_vector[i]);
								std::cout << "average color0 is:" << tempcolor[i] << std::endl;
							}
							logout << utils::currentDateTime() + " average color is: " + std::to_string(tempcolor[0]) + "," +
								std::to_string(tempcolor[1]) + "," + std::to_string(tempcolor[2]) + "\n";
						}
						// 计算当前图像的色差
						else
						{
							float image_scan_color0 = cv::mean(image_scan).val[0];
							float image_scan_color1 = cv::mean(image_scan).val[1];
							float image_scan_color2 = cv::mean(image_scan).val[2];
							float color_diff0 = abs(tempcolor[0] - image_scan_color0);
							float color_diff1 = abs(tempcolor[1] - image_scan_color1);
							float color_diff2 = abs(tempcolor[2] - image_scan_color2);
							if (color_diff0 > 20 || color_diff1 > 20 || color_diff2 > 20) {
								colorflag = "2";
							}
							std::cout << "image color is:" << image_scan_color0 << " " << image_scan_color1 << " " << image_scan_color2 << std::endl;
							std::cout << "color diff is: " << color_diff0 << " " << color_diff1 << " " << color_diff1 << std::endl;
							logout << utils::currentDateTime() + " image color is: " + std::to_string(image_scan_color0) + " "
								+ std::to_string(image_scan_color0) + " " + std::to_string(image_scan_color0) + "\n";
							logout << utils::currentDateTime() + " color diff is: " + std::to_string(color_diff0) + " "
								+ std::to_string(color_diff1) + " " + std::to_string(color_diff2) + "\n";
						}
						const auto t_colordiff_end = std::chrono::high_resolution_clock::now();
						const float t_colordiff_cost = std::chrono::duration<float, std::milli>(t_colordiff_end - t_colordiff_start).count();

						const auto t_imgcrop_start = std::chrono::high_resolution_clock::now();
						// 图像开始分块 
						float crop_h = 0.5;
						float crop_w = 0.5;
						temcols = image_scan.cols;
						temrows = image_scan.rows;
						crop_h = int(temrows * crop_h + 0.5);
						crop_w = int(temcols * crop_w + 0.5);
						/*std::cout << "col is " << temcols << "\n" << "rows is " << temrows << std::endl;
						std::cout << "crop_h is " << crop_h << "\n" << "crop_w is " << crop_w << std::endl;*/
						std::vector<cv::Mat> crop_imgs;
						//将image按crop_h,crop_w分块，并记录左上角坐标至vector
						std::vector<cv::Point> crop_points;
						for (int i = 0; i < temrows; i += crop_h)
						{
							for (int j = 0; j < temcols; j += crop_w)
							{
								int crop_w_ = temcols - j < crop_w ? temcols - j : crop_w;
								int crop_h_ = temrows - i < crop_h ? temrows - i : crop_h;
								cv::Mat img_crop = image_scan(cv::Rect(j, i, crop_w_, crop_h_));
								crop_points.push_back(cv::Point(j, i));   //记录左上角坐标
								crop_imgs.push_back(img_crop);
							}
						}

						//// 图像滑窗切块 
						//crop_h = 640;
						//crop_w = 640;
						//temcols = image_scan.cols;
						//temrows = image_scan.rows;
						///*std::cout << "col is " << temcols << "\n" << "rows is " << temrows << std::endl;
						//std::cout << "crop_h is " << crop_h << "\n" << "crop_w is " << crop_w << std::endl;*/
						//std::vector<cv::Mat> crop_imgs;
						////将image按crop_h,crop_w分块，并记录左上角坐标至vector
						//std::vector<cv::Point2d> crop_points;
						//for (int i = 0; i < temrows; i += crop_h)
						//{
						//	if (i + crop_h > temrows) {
						//		i = temrows - crop_h;
						//	}
						//	for (int j = 0; j < temcols; j += crop_w)
						//	{
						//		if (j + crop_w > temcols) {
						//			j = temcols - crop_w;
						//		}
						//		cv::Mat img_crop = image_scan(cv::Rect(j, i, crop_w, crop_h));
						//		std::string file_name = std::to_string(i) + "_" + std::to_string(j) + ".bmp";
						//		cv::imwrite(file_name, img_crop);
						//		crop_points.push_back(cv::Point2d(j, i));   //记录左上角坐标
						//		crop_imgs.push_back(img_crop);
						//	}
						//}

						const auto t_imgcrop_end = std::chrono::high_resolution_clock::now();
						const float t_imgcrop_cost = std::chrono::duration<float, std::milli>(t_imgcrop_end - t_imgcrop_start).count();

						const auto t_imgprocess_start = std::chrono::high_resolution_clock::now();
						cv::resize(image_scan, sampleimg, cv::Size(), RESIZERATIO, RESIZERATIO);
						//cv::Size2d scale_factor = processInput(image_scan, input_buffer_pointers[0], cpu_transfer_buffer, input_dims[0]);
						std::vector<cv::Size2f> scale_factors;
						scale_factors.reserve(crop_imgs.size());
						auto input_height = engine->mutable_infer()->input(0)->height();
						auto input_width = engine->mutable_infer()->input(0)->width();
						for (auto& crop_img : crop_imgs) {
							scale_factors.emplace_back(preprocessing::resize_keep_aspect_ratio(crop_img, cv::Size{ input_height, input_width }, crop_img));
						}
						const auto t_imgprocess_end = std::chrono::high_resolution_clock::now();
						const float t_imgprocess_cost = std::chrono::duration<float, std::milli>(t_imgprocess_end - t_imgprocess_start).count();

						//const auto t_start2 = std::chrono::high_resolution_clock::now();
						// inference
						const auto t_enqueueV2_start = std::chrono::high_resolution_clock::now();
						auto result = engine->run(crop_imgs, g_mean, g_std);
						const auto t_enqueueV2_end = std::chrono::high_resolution_clock::now();
						const float t_enqueueV2_cost = std::chrono::duration<float, std::milli>(t_enqueueV2_end - t_enqueueV2_start).count();

						// extract to cpu
						const auto t_batched_parse_detections_start = std::chrono::high_resolution_clock::now();
						std::shared_ptr<Detection::DetResult> detections(std::make_shared<Detection::DetResult>());
						int detectnum = utils::dectections_merge(result, detections, scale_factors, crop_points, threshold, th_area); // utils里支持
						const auto t_batched_parse_detections_end = std::chrono::high_resolution_clock::now();
						const float t_batched_parse_detections_cost = std::chrono::duration<float, std::milli>(t_batched_parse_detections_end - t_batched_parse_detections_start).count();
						std::cout << "engine detect num: " << detectnum << std::endl;

						// 反误检比对
						const auto t_detect_match_start = std::chrono::high_resolution_clock::now();
						//if (detections.size() > 0) {
						//	for (size_t k = 0; k < detections.size(); k++) {
						//		logout << currentDateTime() + " origin label " + std::to_string(detections[k].label) + "\n";
						//		logout << currentDateTime() + " origin score " + std::to_string(detections[k].score) + "\n";
						//		logout << currentDateTime() + " origin left " + std::to_string(detections[k].box.left) + "\n";
						//		logout << currentDateTime() + " origin right " + std::to_string(detections[k].box.right) + "\n";
						//		logout << currentDateTime() + " origin top " + std::to_string(detections[k].box.top) + "\n";
						//		logout << currentDateTime() + " origin bottom " + std::to_string(detections[k].box.bottom) + "\n";
						//	}
						//}
						std::shared_ptr<Detection::DetResult> detections_out(std::make_shared<Detection::DetResult>());
						std::shared_ptr<Detection::DetResult> detection_remove(std::make_shared<Detection::DetResult>());
						if (redefectpoints_sum.size() > 0) {
							detection_remove = utils::detect_match2(detections, redefectpoints_sum, detections_out, port);
							logout << utils::currentDateTime() + " false detection library sum = " + std::to_string(redefectpoints_sum.size()) + "\n";
						} else { 
							detections_out = detections;
						}
						const auto t_detect_match_end = std::chrono::high_resolution_clock::now();
						const float t_detect_match_cost = std::chrono::duration<float, std::milli>(t_detect_match_end - t_detect_match_start).count();

						//const auto t_end2 = std::chrono::high_resolution_clock::now();
						//const float inference_cost = std::chrono::duration<float, std::milli>(t_end2 - t_start2).count();
						const auto t_save_pic_time_start = std::chrono::high_resolution_clock::now();
						std::cout << "after match defects: " << detections_out->defect_num() << std::endl;
						std::string defect_label = tileinfer(image_scan, detections_out, picAddress, sampleimg, pictype, detection_remove);
						logout << utils::currentDateTime() + " engine detect num: " + std::to_string(detectnum) + "\n";
						logout << utils::currentDateTime() + " after match defects: " + std::to_string(detections_out->defect_num()) + "\n";

						// 将各个缺陷名称和置信度写到log
						if (confidence.size() > 0) {
							std::cout << "remain defect num : " << confidence.size() << std::endl;
							std::string confidence_string;
							for (int i = 0; i < confidence.size(); i++) {
								std::string conf_str;
								conf_str = confidence[i] + ',';
								confidence_string += conf_str;
							}
							std::cout << "remain defects and confidences are : " << confidence_string << std::endl;
							logout << utils::currentDateTime() + " remain defects and confidences are " + confidence_string + "\n";
						}
						const auto t_save_pic_time_end = std::chrono::high_resolution_clock::now();
						const float save_pic_time = std::chrono::duration<float, std::milli>(t_save_pic_time_end - t_save_pic_time_start).count();
						const auto t_inference_total_time_end = std::chrono::high_resolution_clock::now();
						float inference_total_time = std::chrono::duration<float, std::milli>(t_inference_total_time_end - t_inference_total_time_start).count();
						float whole_time_with_wait = std::chrono::duration<float, std::milli>(t_inference_total_time_end - t_whole_time_with_wait_start).count();
						std::cout << "colordiff_cost: " << t_colordiff_cost << " ms" << std::endl;
						std::cout << "imgcrop_cost: " << t_imgcrop_cost << " ms" << std::endl;
						std::cout << "imgprocess_cost: " << t_imgprocess_cost << " ms" << std::endl;
						std::cout << "enqueueV2_cost: " << t_enqueueV2_cost << " ms" << std::endl;
						std::cout << "batched_parse_detections_cost: " << t_batched_parse_detections_cost << " ms" << std::endl;
						std::cout << "detect_match_cost: " << t_detect_match_cost << " ms" << std::endl;
						std::cout << "save pic time: " << save_pic_time << " ms" << std::endl;
						std::cout << "total engine cost: " << inference_total_time << " ms" << std::endl;
						std::cout << "whole_time_with_wait cost: " << whole_time_with_wait << " ms" << std::endl;
						std::string defect_result = "ScanResultImage#" + defect_label + colorflag + "#" + label_sum;
						std::cout << defect_result << std::endl;
						std::cout << "count_num = " << count_tile_num << std::endl;
						logout << utils::currentDateTime() + " count_num =  " + std::to_string(count_tile_num) + "\n";
						std::cout << "--------------------------------------------------\n" << std::endl;
						send(sClient, defect_result.c_str(), defect_result.size() + 1, 0);
						logout << utils::currentDateTime() + " colordiff_cost : " + std::to_string(t_colordiff_cost) + "\n";
						logout << utils::currentDateTime() + " imgcrop_cost : " + std::to_string(t_imgcrop_cost) + "\n";
						logout << utils::currentDateTime() + " imgprocess_cost : " + std::to_string(t_imgprocess_cost) + "\n"; \
							logout << utils::currentDateTime() + " enqueueV2_cost : " + std::to_string(t_enqueueV2_cost) + "\n";
						logout << utils::currentDateTime() + " batched_parse_detections_cost : " + std::to_string(t_batched_parse_detections_cost) + "\n";
						logout << utils::currentDateTime() + " detect_match_cost_cost : " + std::to_string(t_detect_match_cost) + "\n";
						logout << utils::currentDateTime() + " save pic time: " + std::to_string(save_pic_time) + "\n";
						logout << utils::currentDateTime() + " total engine cost: " + std::to_string(inference_total_time) + "\n";
						logout << utils::currentDateTime() + " whole_time_with_wait cost: " + std::to_string(whole_time_with_wait) + "\n";
						logout << utils::currentDateTime() + " defect_result is : " + defect_result + "\n";
					}
				}
			}
		}
		catch (...) {
			std::ofstream logout = utils::createLog(port);
			closesocket(sClient);
			std::cout << "Main YiChang!" << std::endl;
			logout << utils::currentDateTime() + " Main YiChang!\n";
		}
	}
}
