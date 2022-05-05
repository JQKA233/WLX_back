#pragma once
#include<opencv2/opencv.hpp>

namespace wlx{
    class PID{
        public:
            void PID_Init();
            void PID_Calc(double tar,double sen);
        private:
            int choose_model;        //使用哪个模式调节
            
            float target;     //用户设定值Sv
            float sensor;        //当前值，实际值Pv
        
            float K_p;        //比例系数
            float clock;      //PID计算周期--采样周期T
            int Tdata;    //判断PID周期到没到
            float K_i;        //积分时间常数Ti
            float K_d;       //微分系数Td

            float Ek;          //本次偏差
            float Ek_1;        //上次偏差
            float SumEk;         //历史偏差之和
            
            float I_out;        //积分输出
            float P_out;        //比例输出    
            float D_out;        //微分输出
            
            float OUT0;        //一个维持的输出，防止失控
            float OUT;        //计数最终得到的值
            
            int pwmcycle;//pwm周期
            
            float err;
            float p_err;
            float inti;
    };
    enum{
        MODLE_P,
        MODLE_PI,
        MODLE_PID,
    };
}