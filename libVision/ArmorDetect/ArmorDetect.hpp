#pragma once
#include<opencv2/opencv.hpp>
namespace wlx{
    class ArmorDetect{
        public:
        /*
        *brief:
        *pram:
        */
        void findColorPoint(cv::Mat img,int color,bool apearRect = false);
        std::vector<cv::Point2i> FoundColorCenter;
        private:
        int areaFilter = 200;
    };
    cv::Point2i calRectcenter(cv::Rect rt);
    enum
    {
        BLUE,
        GREEN,
        ARMOR_RED,
        ENERGY_RED,
        ARMOR_BLUE,
        ENERGY_BLUE,
    };
}
