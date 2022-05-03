#include"FuwenDetect.hpp"

void Energy::EnergyFilter(vector<vector<Point2i>> pre_EnerArmor){
    for(int i=0;i<pre_EnerArmor.size();i++){
        Equeue.x = pre_EnerArmor[i];
        EnerArm.push(Equeue);
    }
    if(EnerArm.size()>=3){
        for(int i=1;i<=3;i++){
            //拟合矩形，并取矩形中心点
            rectCenter=wlx::calRectcenter(boundingRect(EnerArm.top().x));
            UartSendData(NULL,64);//向串口写入数据
            while(!UartRecvData)break;//等待MCU反馈
            EnerArm.pop();
        }
    }
    while(!EnerArm.empty()) EnerArm.pop();//清空队列
    return;
}