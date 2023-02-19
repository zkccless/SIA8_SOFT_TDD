#ifndef DATAH
#define DATAH

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

namespace utils {
    /*class Box {
    public:
        float left, top, right, bottom;
        Box() {}
        Box(float left, float top, float right, float bottom) :
            left(left), top(top), right(right), bottom(bottom) {}
    };*/

    // ɾ
    /*class Detection {
    public:
        Box box;
        float score, label;
        Detection() {}
        Detection(Box const& box, float score, float label) :
            box(box), score(score), label(label) {}
    };*/
    class Centerlabel {
    public:
        float x, y, label, area;
        Centerlabel(float x, float y, float label, float area) :
            x(x), y(y), label(label), area(area) {}
    };
    class Centerlabel2 {
    public:
        float label;
        Centerlabel2(float label) :
            label(label) {}
    };
    class Centerlabel3 {
    public:
        float x, y, label;
        Centerlabel3(float x, float y, float label) :
            x(x), y(y), label(label) {}
    };
    class Centerlabel4 {
    public:
        float x, y;
        Centerlabel4(float x, float y) :
            x(x), y(y) {}
    };
    class Detection2 {
    public:
        float label;
        Detection2(float label) :
            label(label) {}
    };
    class BBox_list {
    public:
        cv::Rect rect;
        cv::Point p;
        float max_value;
        BBox_list(cv::Rect rect, cv::Point p, float max_value) :
            rect(rect), p(p), max_value(max_value) {}
    };
    class ClusterRectangle {
    public:
        cv::Point p1, p2;
        float confidence;
        ClusterRectangle(cv::Point p1, cv::Point p2, float confidence) :
            p1(p1), p2(p2), confidence(confidence) {}
    };

    class FindRoi {
    public:
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        FindRoi(std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec4i> hierarchy) :
            contours(contours), hierarchy(hierarchy) {}
    };

    class GetBbox {
    public:
        std::vector<float> res1;
        cv::Point p;
        GetBbox(std::vector<float> res1, cv::Point p) :
            res1(res1), p(p) {}
    };

    class CropRect {
    public:
        cv::Mat img_crop, img_rot;
        CropRect(cv::Mat img_crop, cv::Mat img_rot) :
            img_crop(img_crop), img_rot(img_rot) {}
    };

    class RectTuple {
    public:
        Centerlabel4 cl1, cl2;
        float degree;
        RectTuple(Centerlabel4 cl1, Centerlabel4 cl2, float degree) :
            cl1(cl1), cl2(cl2), degree(degree) {}
    };

    class Rect {
    public:
        RectTuple rt;
        std::vector<std::vector<cv::Point>> contours;
        Rect(RectTuple rt, std::vector<std::vector<cv::Point>> contours) :
            rt(rt), contours(contours) {}
    };

    class SegmentMin {
    public:
        RectTuple rt;
        std::vector<cv::Point> rect;
        cv::Mat mask;
        cv::Mat img_copy;
        SegmentMin(RectTuple rt, std::vector<cv::Point> rect, cv::Mat mask, cv::Mat img_copy) :
            rt(rt), rect(rect), mask(mask), img_copy(img_copy) {}
    };

    class Coords {
    public:
        int x, y, p_w, p_h;
        Coords(int x, int y, int p_w, int p_h) :
            x(x), y(y), p_w(p_w), p_h(p_h) {}
    };

    //class DrawResult {
    //  public:
    //    cv::Mat img_cis, img_ccd, sampleimg;
    //    std::vector<std::string> text;
    //    std::vector<cv::Rect> r, r1;
    //    std::string dir;
    //    DrawResult() {}
    //    DrawResult(cv::Mat img, std::vector<std::string> text, std::vector<cv::Rect> r, 
    //        std::vector<cv::Rect> r1, std::string dir, cv::Mat sampleimg) :
    //        img_cis(img_cis), img_ccd(img_ccd), text(text), r(r), r1(r1), dir(dir), sampleimg(sampleimg) {}
    //};
    class DrawResult {
    public:
        cv::Mat img, sampleimg;
        std::vector<std::string> text;
        std::vector<cv::Rect> r, r1;
        std::string dir, pictype;
        DrawResult() {}
        DrawResult(cv::Mat img, std::vector<std::string> text, std::vector<cv::Rect> r,
            std::vector<cv::Rect> r1, std::string dir, cv::Mat sampleimg, std::string pictype) :
            img(img), text(text), r(r), r1(r1), dir(dir), sampleimg(sampleimg), pictype(pictype) {}
    };

    class DrawResult_2Camera {
    public:
        cv::Mat img_cis, img_ccd, sampleimg_cis, sampleimg_ccd;
        std::vector<std::string> text_cis, text_ccd, text_all, label_all;
        std::vector<cv::Rect> r_all, r1_all, r_cis, r_ccd;
        std::string dir, img_type;
        DrawResult_2Camera() {}
        DrawResult_2Camera(cv::Mat img_cis, cv::Mat img_ccd, std::vector<std::string> text_cis, std::vector<std::string> text_ccd, std::vector<std::string> text_all, std::vector<std::string> label_all,
            std::vector<cv::Rect> r, std::vector<cv::Rect> r1, std::vector<cv::Rect> r_cis, std::vector<cv::Rect> r_ccd, std::string dir, cv::Mat sampleimg_cis, cv::Mat sampleimg_ccd, std::string img_type) :
            img_cis(img_cis), img_ccd(img_ccd), text_cis(text_cis), text_ccd(text_ccd), text_all(text_all), label_all(label_all), r_all(r_all), r1_all(r1_all), r_cis(r_cis),
            r_ccd(r_ccd), dir(dir), sampleimg_cis(sampleimg_cis), sampleimg_ccd(sampleimg_ccd), img_type(img_type) {}
    };

    class DrawOriginImg {
    public:
        cv::Mat img;
        std::string dir;
        DrawOriginImg() {}
        DrawOriginImg(cv::Mat img, std::string dir) :
            img(img), dir(dir) {}
    };

    class SettingsInfor {
    public:
        std::string th1, th2, show_image_flag;
        SettingsInfor() {}
        SettingsInfor(std::string th1, std::string th2, std::string show_image_flag) :
            th1(th1), th2(th2), show_image_flag(show_image_flag) {}
    };

};

#endif

