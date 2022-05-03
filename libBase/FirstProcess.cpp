#include"FirstProcess.hpp"
#include<iostream>

using namespace wlx;
using namespace std;
//Detect 
void* func1(void* args){
        Cam pp;
        pp.Cam_Init();
        while (1)
        {
            pp.getImage();
            pp.imgAdjust();
            ArmorDetect dd;
            dd.findColorPoint(pp.AdjustedImg,BLUE);
            cout<<pp.AdjustedImg.size()<<endl;
            Pose pos;
            for(int i=0;i<dd.FoundColorCenter.size();i++){
                pos.calAngle(dd.FoundColorCenter[i].x+320,dd.FoundColorCenter[i].y+120);
                cout<<pos.AngleSolutions<<endl;
            }
            dd.FoundColorCenter.clear();
            cv::waitKey(100);
        }
    return 0;
}