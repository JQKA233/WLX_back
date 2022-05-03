#include<iostream>
#include<opencv2/opencv.hpp>
#include<string>
#include <stdio.h>
#include <vector>
#include"Mono.hpp"

using namespace std;
using namespace cv;

#define UNIT_PIXEL_W 0.000375
#define UNIT_PIXEL_H 0.000375

void Mono(Mat frame, Rect* boundRect)
{
    const double f = 0.42;  // focus distance
    const double w = 3;   // object width
    const double h = 3;   // object height
    // cal imgwidth&imgheight
    double width = (* (boundRect)).width * UNIT_PIXEL_W;
    double height = (* (boundRect)).height * UNIT_PIXEL_H;
    // cal distance
    double distanceW = w * f / width;
    double distanceH = h * f / height;

    char disW[50], disH[50];
    snprintf(disW,sizeof(disW), "DistanceW : %.2f cm", distanceW);
    snprintf(disH,sizeof(disH), "DistanceH : %.2f cm", distanceH);
    putText(frame, disW, Point(5, 20), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 255, 0), 1, 8);
    putText(frame, disH, Point(5, 40), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 255, 0), 1, 8);
    return;
}
