//#include "lightBoLDetect.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <Eigen/Dense>
using namespace std;
using namespace cv;



/*
 *  kalmanFilter
 */
class kalmanFilter
{
public:

    // kalmanFilter();
    // ~kalmanFilter();

    void Init(float s, float v){
        x_ << s, v;
        F << 1,1,0,1;
        P << 1,0,0,1;
        Q << 1,0,0,1;
        H << 1,0,0,1;

    }

    void predict(){
        // X' = FX + U
        x_ = F * x_;
        // P' = FPFT+Q
        P = F * P * F.transpose() + Q;
    }

    float update(float m_s,float m_v){
        // Y = Z - HX'
        Eigen::Matrix<float,2,1> Z ;
        Z << m_s, m_v;
        Eigen::Matrix<float,2,1> Y = Z - H * x_;
        // S = HP'HT + R
        Eigen::Matrix<float,2,2> S = H * P * H.transpose();
        // K = P'HTS-1
        K = P * H.transpose() * S.inverse();
        // X = X' + KY
        x_ = x_ + K * Y;
        // P = (I - KH)P'
        Eigen::Matrix<float,2,2> I ;
        I << 1, 1, 1, 1;
        P = (I - K * H) * P;
        
        return x_(0, 0);

    }

private:

    Eigen::Matrix<float,2,1> x_;
    Eigen::Matrix<float,2,2> F;
    Eigen::Matrix<float,2,2> P;
    Eigen::Matrix<float,2,2> Q;
    Eigen::Matrix<float,2,2> H;
    Eigen::Matrix<float,2,2> K;
};


/*
 * light detection init
 */


Mat Init(Mat Input){
    static Mat Output;

    resize(Input, Input, Size(), 0.8, 0.8, INTER_AREA);
    //Input.copyTo(Ori);
    cvtColor(Input, Input, COLOR_BGR2GRAY);
    threshold(Input, Output, 35, 35, THRESH_BINARY);

    Mat kernel = getStructuringElement(0, Size(3, 3));
    morphologyEx(Output, Output, MORPH_OPEN, kernel);

    return Output;

}

Mat color_detect(Mat src){
   Mat imgHSV;
   vector<Mat> hsvSplit;
   cvtColor(src, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

   split(imgHSV, hsvSplit);

   equalizeHist(hsvSplit[2],hsvSplit[2]);
   merge(hsvSplit,imgHSV);


   Mat imgThresholded;

   inRange(imgHSV, Scalar(100, 150, 150), Scalar(140, 255, 255), imgThresholded); //Threshold the image
   
   Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
   morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);

   morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);

   return imgThresholded;
}

Point2i calRectcenter(Rect rt){
    Point2i center;
    center.x = rt.tl().x + rt.width/2;
    center.y = rt.tl().y + rt.height/2;
    return center;
}

void detect() {
   
    VideoCapture vc;
  

    vc.open("/home/hoyard/2Ddetection/armor-detect-master/linear_move.mp4");

    float x_pos,x_lastpos;
    float x_speed;
    bool detect_flag = false;
    Point center;

    kalmanFilter KF;

    while (vc.isOpened()) {

        KF.Init(x_lastpos, x_speed);

        cout << "speed" << x_speed << endl;
        cout << "current s" << x_lastpos << endl;

        Mat img;
        Mat oriimg;

        vector<vector<Point>> lightCol;
        vector<Rect> rtr;
        vector<Rect> truertr;
        
        detect_flag = false;
        vc.read(img);
        img.copyTo(oriimg);
        resize(oriimg, oriimg, Size(), 0.8, 0.8, INTER_AREA);
        Mat img_color = color_detect(oriimg);
        img = Init(img);
        vector<vector<Point>> contours;
        findContours(img_color, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for (auto &contour : contours) {
    
            if (contourArea(contour) < 50) {
                
                continue;
            }
            
            Rect rrect = boundingRect(contour);
           
                lightCol.push_back(contour);
                rtr.push_back(rrect);

            for (auto rtr0 : rtr) {
                //cout << rtr0.center << endl;
                rectangle(img_color, Rect((calRectcenter(rtr0).x - rtr0.width / 2), (calRectcenter(rtr0).y - rtr0.height / 2),
                                       rtr0.width, rtr0.height), Scalar(100), 3);
            }

        }
        drawContours(img_color, lightCol, -1, Scalar(100), 2, 0);
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
            rectangle(oriimg, Rect((calRectcenter(rtr0).x - rtr0.width / 2), (calRectcenter(rtr0).y - rtr0.height / 2),
                                   rtr0.width, rtr0.height), Scalar(255, 0, 255), 4);
        }
        for (int i = 0; i < truertr.size(); i += 2) {
            line(oriimg, Point((calRectcenter(truertr[i]).x - truertr[i].width / 2),
                               (calRectcenter(truertr[i]).y - truertr[i].height / 2)),
                 Point((calRectcenter(truertr[i+1]).x + truertr[i + 1].width / 2),
                       (calRectcenter(truertr[i+1]).y + truertr[i + 1].height / 2)), Scalar(255, 0, 255), 1);
            line(oriimg, Point((calRectcenter(truertr[i]).x - truertr[i].width / 2),
                               (calRectcenter(truertr[i]).y + truertr[i].height / 2)),
                 Point((calRectcenter(truertr[i+1]).x + truertr[i + 1].width / 2),
                       (calRectcenter(truertr[i+1]).y - truertr[i + 1].height / 2)), Scalar(255, 0, 255), 1);
            center.x = (calRectcenter(truertr[i]).x + calRectcenter(truertr[i+1]).x) / 2;
            center.y = (calRectcenter(truertr[i]).y + calRectcenter(truertr[i+1]).y) / 2;
            circle(oriimg, center, truertr[i].width, Scalar(255, 255, 100), 4);
            detect_flag = true;
        }
        
        float pre_s;
        if (detect_flag == true){
            x_speed = (center.x - x_lastpos);
            x_lastpos = center.x;
            detect_flag == false;
            KF.predict();
            pre_s = KF.update(center.x + x_speed*50, x_speed);
        }
        else{
            x_lastpos = x_lastpos + x_speed;
            KF.predict();
            pre_s = KF.update(x_lastpos + x_speed*50, x_speed);
        }

        putText(oriimg,std::to_string(x_speed),Point(center.x+50,center.y+50),cv::FONT_HERSHEY_PLAIN,2,(255,0,0),2);    

        


        cout << "predict s" << pre_s << endl;
        cout << "----------------------------" << endl;
        circle(oriimg, Point(pre_s, center.y), 10, Scalar(255, 255, 255), 1);

        //while (true) {
        //imshow("Init", img);
        imshow("origin", oriimg);
        imshow("color",img_color);

        waitKey(0);

    }
}