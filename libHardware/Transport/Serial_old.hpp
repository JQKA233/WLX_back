#pragma once
#include<opencv2/opencv.hpp>
#include<string>

namespace wlx{
    class Serial
    {
    public:
        void Serial_Inti();
        void Serial_Send(std::string Cmd);
        std::string Serial_Recev();
    private:

    };
}