#include <detection.h>

int main() {
	App::use_amirstan_plugin();
	auto image_NG = cv::imread("workspace/NG_origin.jpg");
	auto engine = App::create_infer<Detection::DetResult>("workspace/faster_rcnn_batch=1_trt8.trt", Detection::faster_rcnn_parser);
	auto result = engine->run(image_NG);
	result->format();
}