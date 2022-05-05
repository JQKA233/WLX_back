#pragma once
#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include"../../libCamera/Camera.hpp"
#include"../../libSolver/Predict/Predict.hpp"

#define UNIT_PIXEL_W 0.000375
#define UNIT_PIXEL_H 0.000375
using namespace cv;
using namespace std;

class AngleSolver
{
public:
	AngleSolver();
	~AngleSolver();

	/**
	* @brief Set bullet speed
	* @param bulletSpeed: the speed of bullet(mm/s)
	*/
	void setBulletSpeed(int bulletSpeed);

	/**
	* @brief solve the angles using P4P or PinHole according to the distance
	*/
	void solveAngles(vector<Point3f> POINTS_3D,vector<Point2f> POINTS_2D,Point2f center);

	/**
	* @brief solve the angles using P4P method
	*/
	void P4P_solver(Point3f PNP);

	/**
	* @brief solve the angles using PinHole method
	*/
	void PinHole_solver(Mat CM,Mat DC,Point2f targetCenter);

	/**
	* @brief compensation of pitch
	*/
	void compensateAngle();

	/**
	* @brief compensation of pitch for y_offset between barrel and camera
	*/
	void compensateOffset();

	/**
	* @brief compensation of pitch for gravity
	*/
	void compensateGravity();

	/**
	* @brief according to the target2D points to get the yaw and pitch and distance towards the certain type target using solvePnP
	* @param inputArrayOfPoints contourPoints, the vertices of target armor
	* @param inputPoint centerPoint the center point of target armor
	* @param input type the type of armor BIG_ARMOR or SMALL_ARMOR
	* @param output y_yaw angle     the angle that yaw revolve     '<-' left is minus-       '->' right is positive+
	* @param output x_pitch angle   the angle that pitch revolve   '下' down is minus-       '上' up is positive+ 
	* @param output distance  unit is mm
	*/
	void getAngle(vector<Point3f> POINTS_3D,vector<Point2f> POINTS_2D,Point2f center);

	/**
    * @brief show debug information
    */
    void showDebugInfo(bool showCurrentResult);
	// calculated by solvePnP
	//s[R|t]=s'  s->world coordinate;s`->camera coordinate
	Mat rVec;    //rot rotation between camera and target center
	Mat tVec;  //trans tanslation between camera and target center

	//Results
	float y_yaw;
	float x_pitch;
	double distance;
	Point3d PNP;

private:

	//speed of bullet (compensation for gravity and air fru)
	double BULLET_SPEED;

	//distance between camera and barrel in y axis(positive when camera is under barrel)  barrel_y = camera_y + barrel_camera_y
	double GUN_CAM_DISTANCE_Y=6;

	
};