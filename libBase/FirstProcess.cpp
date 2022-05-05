#include"FirstProcess.hpp"
#include<iostream>

//using namespace wlx;
using namespace std;
//Detect 
void* func1(void* args){
        Cam pp;
        wlx::ArmorDetect dd;
        wlx::MotionPredict MP;
        AngleSolver AS;
        Mat pre_Frame;
        Mat curr_Frame;
        
        while (1)
        {
            pp.getImage();
            pp.imgAdjust();
            dd.findArmorPoint(pp.AdjustedImg,1);
            AS.getAngle(dd.ARMOR_POINTS_3D,dd.armorImagePoints,dd.ArmorCenter);
            

            //dd.ArmorCenter.clear();
            cv::waitKey(100);
        }
    return 0;
}