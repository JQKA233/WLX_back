#pragma once
#include<opencv2/opencv.hpp>
#include<vector>
#include<queue>
#include"../ArmorDetect/ArmorDetect.hpp"
#include"../../libHardware/Transport/Serial.hpp"

using namespace std;
using namespace cv;

struct contourFound{
    vector<Point2i> x;
    friend bool operator < (contourFound a , contourFound b)
    {
        return contourArea(a.x) > contourArea(b.x);   //在结构体中，根据x的优先级高的来出队
    }
}Equeue;//声明优先队列

class Energy
{
private:
    priority_queue <contourFound> EnerArm;
    Point2i rectCenter;
public:
    void EnergyFilter(vector<vector<Point2i>> pre_EnerArmor);
};


