#include<opencv2/opencv.hpp>
#include"PID.hpp"
#include"../../libHardware/Transport/Serial.hpp"

namespace wlx{
        void PID::PID_Init()//PID初始化
        {
            clock=1;//采样周期，定时器使用1ms，则最小执行PID的周期为330ms
            target=0;//用户设定值
            sensor=0;
            K_p=0.5;//比例系数
            K_i=180;//积分时间
            K_d=1;//微分时间
            //pwmcycle = 330;//PWM的周期
            
            err = 0;
            p_err = 0;
            //inti = 0;

            OUT0=0;//一个维持的输出
            choose_model = MODLE_PID;
        }
        void PID::PID_Calc(double tar,double sen)  //pid计算
        {
            float DelEk;            //本次和上次偏差，最近两次偏差之差
            float ti,ki;
            float td;
            float kd;
            float out;
            if(Tdata < (clock))  //最小计算周期未到
            {
                return;
            }
            Tdata = 0;
            Ek=target-sensor;//得到当前的偏差值
            P_out=K_p*Ek;//比例输出
            SumEk+=Ek;//历史偏差总和
            DelEk=Ek-Ek_1;//最近两次偏差之差
            ti=clock/K_i;
            ki=ti*K_p;
            I_out=ki*SumEk;//积分输出
            /*注意：上面程序中多了个pid.Kp,原程序中有，请自动删除，正确的应该是pid.Iout=ki*pid.SEK */
            td=K_d/clock;
            kd=K_p*td;
            D_out=kd*DelEk;                //微分输出
            switch(choose_model)
            {
                case MODLE_P:out= P_out;
                    break;
                
                case MODLE_PI:out= P_out+ I_out;
                    break;
                        
                case MODLE_PID:out= P_out+ I_out+ D_out;
                    break;
            }
        //////////////////////////////////////////////////////////
            /*判断算出的数是否符合控制要求*/
            if(out>pwmcycle){//不能比PWM周期大，最大就是全高吗
                OUT=pwmcycle;
            }
            else if(out<0){//值不能为负数
                OUT=OUT0;
            }
            else{
                OUT=out;
            }
            //printf("实际输出使用的pid.OUT:\t%d\r\n",(int)pid.OUT) ;
            Ek_1=Ek;  //更新偏差
            /*输出PWM*/
            //Serial a;
            //a.Serial_Send("SERVO RUN");//run
            //cv::waitKey((int)OUT);//
            // a.Serial_Send("SERVO STOP");//stop
            // cv::waitKey(pwmcycle-(int)OUT);//
            //Turn_Angle();
        }

}
