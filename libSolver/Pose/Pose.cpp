#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include"Pose.hpp"

void Pose::calAngle(int x,int y)
{
    x*=UNIT_PIXEL_W;
    y*=UNIT_PIXEL_H;
    cap.Cam_Init();
    double fx=cap.cameraMatrix.at<double>(0,0);
    double fy=cap.cameraMatrix.at<double>(1,1);
    double cx=cap.cameraMatrix.at<double>(0,2);
    double cy=cap.cameraMatrix.at<double>(1,2);
    double k1=cap.distCoeffs.at<double>(0);
    double k2=cap.distCoeffs.at<double>(1);
    double p1=cap.distCoeffs.at<double>(2);
    double p2=cap.distCoeffs.at<double>(3);
    cv::Point2f pnt;
    std::vector<cv::Point2f>in;
    std::vector<cv::Point2f>out;
    in.push_back(cv::Point2f(x,y));
    //openCV对像素点去畸变
    cv::undistortPoints(in,out,cap.cameraMatrix,cap.distCoeffs,cv::noArray(),cap.cameraMatrix);
    pnt=out.front();
    rx=(pnt.x-cx)/fx;
    ry=(pnt.y-cy)/fy;

    double tanx=(rx);
    double tany=(ry);
    std::cout <<out<<std::endl;
    FrameSolutions = pnt;
    AngleSolutions.x = atan(rx)/CV_PI*180;
    AngleSolutions.y = atan(ry)/CV_PI*180;
/*
    New is out of distCoeffs
    "xscreen:"<<x
    "xNew:"<<pnt.x 
    "yscreen:"<<y
    "yNew:"<<pnt.y
    "angx:"<<atan((x-cx)/fx)/CV_PI*180
    "angleNew:"<<atan(rx)/CV_PI*180
    "angy:"<<atan((y-cy)/fy)/CV_PI*180
    "angleNew:"<<atan(ry)/CV_PI*180
*/
}
double Pose::Distance_InCorrect(double center_x,double center_y,double ArmorDelta_x, double ArmorDelta_y){
    calAngle(center_x,center_y);
    double distance;
    if(ArmorDelta_x!=0){
        distance=ArmorDelta_x/rx;
    }
    else
        if(ArmorDelta_y!=0){
            distance=ArmorDelta_y/ry;
        }
        else distance = -1.0;
    return distance;
}
