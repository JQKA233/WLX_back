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

	Point2f calRectcenter(Rect rt)
		{
			Point2f center;
			center.x = rt.tl().x + rt.width / 2;
			center.y = rt.tl().y + rt.height / 2;
			return center;
		}

	vector<Point3f> setObjectPoints(double width, double height)
	{
		double half_x = width / 2.0; 
		double half_y = height / 2.0;
		vector<Point3f> POINTS_3D;
		POINTS_3D.push_back(Point3f(-half_x, half_y, 0));   //tl top left
		POINTS_3D.push_back(Point3f(half_x, half_y, 0));	//tr top right
		POINTS_3D.push_back(Point3f(half_x, -half_y, 0));   //br below right
		POINTS_3D.push_back(Point3f(-half_x, -half_y, 0));  //bl below left

		return POINTS_3D;
	}

	/**
	 *@brief: using the lightRect of two lightBar to construct the armorVertices
	*/
	vector<Point2f> setImagePoints(Rect lightbar1,Rect lightbar2) {
		//handle two lights
		Rect l_light;
		Rect r_light;
		float armorAngle;
		if(lightbar1.tl().x<lightbar2.tl().x)
		{
			l_light = lightbar1;
			r_light = lightbar2;
		}
		else
		{
			l_light = lightbar2;
			r_light = lightbar1;
		}
		cv::Size exLSize(int(l_light.width), int(l_light.height * 2));
		cv::Size exRSize(int(r_light.width), int(r_light.height * 2));
		cv::RotatedRect exLLight(calRectcenter(l_light), exLSize, armorAngle);
		cv::RotatedRect exRLight(calRectcenter(r_light), exRSize, armorAngle);

		cv::Point2f pts_l[4];
		exLLight.points(pts_l);
		cv::Point2f upper_l = pts_l[2];
		cv::Point2f lower_l = pts_l[3];

		cv::Point2f pts_r[4];
		exRLight.points(pts_r);
		cv::Point2f upper_r = pts_r[1];
		cv::Point2f lower_r = pts_r[0];
		
		vector<Point2f> armorVertices;
		armorVertices.resize(4);
		armorVertices[0] = upper_l;
		armorVertices[1] = upper_r;
		armorVertices[2] = lower_r;
		armorVertices[3] = lower_l;
		return armorVertices;
	}


	Mat color_detect(Mat src){
		Mat imgHSV;
		Mat imgThresholded;
		vector<Mat> hsvSplit;
		cvtColor(src, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		split(imgHSV, hsvSplit);
		equalizeHist(hsvSplit[2],hsvSplit[2]);
		merge(hsvSplit,imgHSV);
		inRange(imgHSV, Scalar(100, 150, 150), Scalar(140, 255, 255), imgThresholded); //Threshold the image
		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
		morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);
		morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);
		return imgThresholded;
	}

	void ArmorDetect::findArmorPoint(Mat img,bool appearRect)
	{
		vector<vector<Point>>contours;
		vector<Vec4i>hierarchy;
		vector<vector<Point>> lightCol;
        vector<Rect> rtr;
        vector<Rect> truertr;
		bool detect_flag = false;
        Point2f center;
		findContours(color_detect(img), contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		for (auto &contour : contours) {
    
            if (contourArea(contour) < areaFilter) 
			{
                
                continue;
            }
            vector<vector<Point>>conPoly(contours.size());
			float peri = arcLength(contours, true);
			approxPolyDP(contours, conPoly, 0.02* peri, true);
			vector<Rect>boundRect(contours.size());
            Rect rrect = boundingRect(conPoly);//输入点
                lightCol.push_back(contour);
                rtr.push_back(rrect);

			for (auto rtr0 : rtr) 
					{
						//cout << rtr0.center << endl;
						rectangle(color_detect(img), Rect((calRectcenter(rtr0).x - rtr0.width / 2), (calRectcenter(rtr0).y - rtr0.height / 2),
												rtr0.width, rtr0.height), Scalar(100), 3); 
					}

		}

        drawContours(color_detect(img), lightCol, -1, Scalar(100), 2, 0);

        //pi peihuang jia ban
        for (int i = 0; i < rtr.size(); i++) {
            for (int j = i + 1; j < rtr.size(); j++) {
                if (abs(calRectcenter(rtr[i]).x - calRectcenter(rtr[j]).x) < rtr[i].width * 100 
                    && abs(calRectcenter(rtr[i]).x - calRectcenter(rtr[j]).x) > rtr[i].width * 4) {
                    truertr.push_back(rtr[i]);
                    truertr.push_back(rtr[j]);
                    continue;
                }
            }
            //rectangle(oriimg,Rect ((rtr[i].center.x - rtr[i].size.width/2) ,(rtr[i].center.y - rtr[i].size.height/2), rtr[i].size.width ,rtr[i].size.height ),Scalar(255,0,255),2);
        }

        for (auto rtr0 : truertr) {
            // cout << calRectcenter(rtr0) << endl;
            rectangle(img, Rect((calRectcenter(rtr0).x - rtr0.width / 2), (calRectcenter(rtr0).y - rtr0.height / 2),
                                   rtr0.width, rtr0.height), Scalar(255, 0, 255), 4);
        }
        for (int i = 0; i < truertr.size(); i += 2) {
            line(img, Point((calRectcenter(truertr[i]).x - truertr[i].width / 2),
                               (calRectcenter(truertr[i]).y - truertr[i].height / 2)),
                 Point((calRectcenter(truertr[i+1]).x + truertr[i + 1].width / 2),
                       (calRectcenter(truertr[i+1]).y + truertr[i + 1].height / 2)), Scalar(255, 0, 255), 1);
            line(img, Point((calRectcenter(truertr[i]).x - truertr[i].width / 2),
                               (calRectcenter(truertr[i]).y + truertr[i].height / 2)),
                 Point((calRectcenter(truertr[i+1]).x + truertr[i + 1].width / 2),
                       (calRectcenter(truertr[i+1]).y - truertr[i + 1].height / 2)), Scalar(255, 0, 255), 1);

			armorImagePoints=setImagePoints(truertr[i],truertr[i+1]);

            center.x = (calRectcenter(truertr[i]).x + calRectcenter(truertr[i+1]).x) / 2;
            center.y = (calRectcenter(truertr[i]).y + calRectcenter(truertr[i+1]).y) / 2;
            circle(img, center, truertr[i].width, Scalar(255, 255, 100), 4);
            detect_flag = true;
        }
		//imshow("kalman", img);
		waitKey(1);
		ArmorCenter = center;
		ARMOR_POINTS_3D=setObjectPoints(4, 4);
		return;
	}
}
