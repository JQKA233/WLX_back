#pragma once
#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include"../../libCamera/Camera.hpp"

#define UNIT_PIXEL_W 0.000375
#define UNIT_PIXEL_H 0.000375

class Pose
{
private:
    double rx;
    double ry;
public:
/**
        @brief :输出角度解算结果
        @return :x方向角度与y方向角度
    */
    cv::Point2f AngleSolutions;
    /**
        @brief :世界坐标系解算
        @return :世界坐标的xy
    */
    cv::Point2f FrameSolutions;
    Cam cap;
    /**
        @brief :角度解算
        @param xy 输入点坐标x，y
        @return :无返回值
    */
    void calAngle(int x,int y);
    double Distance_InCorrect(double center_x,double center_y,double ArmorDelta_x=0, double ArmorDelta_y=0);
};