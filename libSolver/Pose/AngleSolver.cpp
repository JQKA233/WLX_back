#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include "AngleSolver.hpp"
#include"../../libCamera/Camera.hpp"
//#include"../../libVision/ArmorDetect/ArmorDetect.hpp"
//#include"../../libVision/FuwenDetect/FuwenDetect.hpp"

AngleSolver::AngleSolver()
{
}

AngleSolver::~AngleSolver()
{
}

void AngleSolver::setBulletSpeed(int bulletSpeed)
{
	BULLET_SPEED = bulletSpeed;
}

void AngleSolver::solveAngles(vector<Point3f> POINTS_3D,vector<Point2f> POINTS_2D,Point2f center)
{
	Cam p1;
	p1.Cam_Init();
	Mat _rvec;
	cout<<POINTS_3D<<POINTS_2D<<endl;
	solvePnP(POINTS_3D, POINTS_2D, p1.cameraMatrix, p1.distCoeffs, _rvec, tVec, false, SOLVEPNP_ITERATIVE);

	GUN_CAM_DISTANCE_Y = 0;
	tVec.at<double>(1, 0) -= GUN_CAM_DISTANCE_Y;
	double x_pos = tVec.at<double>(0, 0);
	double y_pos = tVec.at<double>(1, 0);
	double z_pos = tVec.at<double>(2, 0);
	distance = sqrt(x_pos * x_pos + y_pos * y_pos + z_pos * z_pos);
	PNP.x = x_pos;
	PNP.y = y_pos;
	PNP.z = x_pos;
	

	// Target is too far, using PinHole solver
    if (distance > 5000)
	{
		PinHole_solver(p1.cameraMatrix,p1.distCoeffs,center);
	}
	// Target is moderate, using PnP solver
	else
	{
		wlx::MotionPredict MP;
		Point3f newPNP = MP.predict(PNP);
		P4P_solver(newPNP);
	}
}

void AngleSolver::P4P_solver(Point3f PNP)
{
	double tan_pitch = PNP.y / sqrt(PNP.x*PNP.x + PNP.z * PNP.z);
	double tan_yaw = PNP.x / PNP.z;
	x_pitch = -atan(tan_pitch) * 180 / CV_PI;//arc
    y_yaw = atan(tan_yaw) * 180 / CV_PI;
}

void AngleSolver::PinHole_solver(Mat CM,Mat DC,Point2f targetCenter)
{
	double fx = CM.at<double>(0, 0);
	double fy = CM.at<double>(1, 1);
	double cx = CM.at<double>(0, 2);
	double cy = CM.at<double>(1, 2);
	Point2f pnt;
	vector<cv::Point2f> in;
	vector<cv::Point2f> out;
	in.push_back(targetCenter);
	
	//对像素点去畸变
	undistortPoints(in, out, CM, DC, noArray(), CM);
	pnt = out.front();

	//去畸变后的比值
	double rxNew = (pnt.x - cx) / fx;
	double ryNew = (pnt.y - cy) / fy;

	y_yaw = atan(rxNew) / CV_PI * 180;
	x_pitch = -atan(ryNew) / CV_PI * 180;
}

void AngleSolver::compensateAngle()
{
    compensateOffset();
    compensateGravity();
}

void AngleSolver::compensateOffset()
{
    float camera_target_height = distance * sin(x_pitch / 180 * CV_PI);
    float gun_target_height = camera_target_height + GUN_CAM_DISTANCE_Y;
    float gun_pitch_tan = gun_target_height / (distance * cos(x_pitch / 180 * CV_PI));
    x_pitch = atan(gun_pitch_tan) / CV_PI * 180;
}

void AngleSolver::compensateGravity()
{
    float compensateGravity_pitch_tan = tan(x_pitch/180*CV_PI) + (0.5*9.8*(distance / BULLET_SPEED)*(distance / BULLET_SPEED)) / cos(x_pitch/180*CV_PI);
    x_pitch = atan(compensateGravity_pitch_tan)/CV_PI*180;
}

void AngleSolver::getAngle(vector<Point3f> POINTS_3D,vector<Point2f> POINTS_2D,Point2f center)
{
	solveAngles(POINTS_3D,POINTS_2D,center);
    compensateAngle();
}

void AngleSolver::showDebugInfo(bool showCurrentResult)
{
    if(showCurrentResult)
    {
        Mat angleImage = Mat::zeros(250,600,CV_8UC3);
        putText(angleImage, "Yaw: " + to_string(y_yaw), Point(100, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 1, 8, false);
        putText(angleImage, "Pitch: " + to_string(x_pitch), Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 1, 8, false);
        putText(angleImage, "Distance: " + to_string(distance), Point(100, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 1, 8, false);
        putText(angleImage, "X:" + to_string((int)(tVec.at<double>(0))), Point(50, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 1, 8, false);
        putText(angleImage, "Y:" + to_string((int)(tVec.at<double>(1))), Point(250, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 1, 8, false);
        putText(angleImage, "Z:" + to_string((int)(tVec.at<double>(2))), Point(400, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 1, 8, false);
        imshow("AngleSolver",angleImage);
    }
}