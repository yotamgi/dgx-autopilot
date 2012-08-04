#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

using namespace cv;

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;


    double ans = cap.get(CV_CAP_PROP_FOURCC);
    std::cout << (char*)&ans << std::endl;
    //cap.set(CV_CAP_PROP_FOURCC, *(double*)"PIM1");

    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
		std::cout << frame.elemSize() * frame.rows * frame.cols << " bytes." << std::endl;
        //cvtColor(frame, edges, CV_RGB2YCrCb);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        imshow("edges", frame);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
