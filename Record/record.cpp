#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

void Record_Pic(string &Output_Path)
{
    cout<<Output_Path<<endl;
    cv::VideoCapture cap1(0);//using usb camera1
    cv::VideoCapture cap2(1);//using usb camera2
    cap1.set(CV_CAP_PROP_FPS,10);
    cap2.set(CV_CAP_PROP_FPS,10);
    if((!cap1.isOpened()) || (!cap2.isOpened()))
    {
    cerr<<"ERROR! Unable to open at least one of the cameras\n";
    return;
    }
    cv::Mat bgr1;
    cv::Mat bgr2;
    string ouput_cam1=Output_Path+"/camera1.avi";
    string ouput_cam2=Output_Path+"/camera2.avi";

    VideoWriter video1(ouput_cam1,CV_FOURCC('M','J','P','G'),10, Size(640,480));
    VideoWriter video2(ouput_cam2,CV_FOURCC('M','J','P','G'),10, Size(640,480));

    int i=0;
    while (true)
    {
        i+=1;
        if (i % 2 ==0){
            cap1>>bgr1;
            if(bgr1.empty())
            {
            printf("Camera1 contains Empty Frame!\n");	
            continue;
            }
            video1.write(bgr1);
        }else{
            
            cap2>>bgr2;
            if(bgr2.empty())
            {
                printf("Camera2 contains Empty Frame!\n");	
                continue;
            }            
            video2.write(bgr2);
        }
        
        
        if( waitKey(50) == 27 ) break; // stop capturing by pressing ESC
    }	

    cap1.release();
    cap2.release();
    video1.release();
    video2.release();
}

int main(int argc, char **argv)
{
	printf("Record Pic Mode\n");
	string out_folder = argv[1];
	Record_Pic(out_folder);
}
