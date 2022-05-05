#include<opencv2/opencv.hpp>
#include"libCamera/Camera.hpp"
#include"libVision/ArmorDetect/ArmorDetect.hpp"
#include"libBase/FirstProcess.hpp"
#include <iostream>
#include <pthread.h>
#include <mutex>

using namespace wlx;

int count = 10;


void* func2(void* args){
    
    return 0;
}
void* func3(void* args){

    return 0;
}

int main() {
    pthread_t tids[5];//一共5个线程
    //参数依次是：创建的线程id，线程参数，调用的函数，传入的函数参数
    int ret1 = pthread_create(&tids[0], NULL, func1, NULL);
    if (ret1 != 0)
    {
        std::cout << "pthread_create error: error_code=" << ret1 << std::endl;
    }
    int ret2 = pthread_create(&tids[1], NULL, func2, NULL);
    if (ret2 != 0)
    {
        std::cout << "pthread_create error: error_code=" << ret2 << std::endl;
    }
    int ret3 = pthread_create(&tids[2], NULL, func3, NULL);
    if (ret3 != 0)
    {
        std::cout << "pthread_create error: error_code=" << ret3 << std::endl;
    }
    //等各个线程退出后，进程才结束，否则进程强制结束了，线程可能还没反应过来；
    pthread_exit(NULL);
    return 0;
}