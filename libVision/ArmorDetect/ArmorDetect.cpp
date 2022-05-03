#include<iostream>
#include<opencv2/opencv.hpp>
#include<string>
#include"../MonoDistance/Mono.hpp"
#include"ArmorDetect.hpp"

using namespace std;
using namespace cv;
namespace wlx{
	vector<vector<int>>myColors
	{
	{90,120,120,150,255,255},//GREEN
	};
	Point2i center;
	Point2i calRectcenter(Rect rt)
	{
		center.x = rt.tl().x + rt.width / 2;
		center.y = rt.tl().y + rt.height / 2;
		return center;
	}
	void ArmorDetect::findColorPoint(Mat img,int color,bool apearRect)
	{
		Mat imgHSV;
		cvtColor(img, imgHSV, COLOR_BGR2HSV);
		Scalar lower(myColors[color][0], myColors[color][1], myColors[color][2]);
		Scalar upper(myColors[color][3], myColors[color][4], myColors[color][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		imshow("mask", mask);
		Mat kernel = getStructuringElement(0, Size(3, 3));
		morphologyEx(mask, mask, MORPH_OPEN, kernel);
////////////////////////findContours///////////////////////////
		vector<vector<Point>>contours;
		vector<Vec4i>hierarchy;
		findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		for (int i = 0; i < contours.size(); i++)
		{
			int area = contourArea(contours[i]);
			if (area < areaFilter) 
			{
				continue;
			}
			vector<vector<Point>>conPoly(contours.size());
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02* peri, true);
			vector<Rect>boundRect(contours.size());
			boundRect[i] = boundingRect(conPoly[i]);//输入点
			drawContours(img, conPoly, i, Scalar(255, 0, 255),2);
			center = calRectcenter(boundRect[i]);
			//Mono(img, boundRect.data());//测距
			//print
			if(apearRect)
			rectangle(img, Point(center.x - 50, center.y + 50), Point(center.x + 50, center.y - 50), Scalar(0, 0, 255), 2);
			/*char buf[256];
			snprintf(buf, 256, "predicted position:(%3d,%3d)", center.x, center.y);
			cout<<center<<endl;
			putText(img, buf, Point(5, 60), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 255, 0), 1, 8);
			snprintf(buf, 256, "current position :(%3d,%3d)", center.x, center.y);
			putText(img, buf, Point(5, 80), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 255, 0), 1, 8);
			*/
			imshow("kalman", img);
			waitKey(1);
			FoundColorCenter.push_back(center);
		}
		return;
	}
}