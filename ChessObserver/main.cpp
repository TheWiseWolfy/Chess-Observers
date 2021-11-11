#include "MainWindow.h"
#include <opencv2\opencv.hpp>


using namespace cv; 

int main(int argc, char *argv[]) {
   
   // MainWindow interface(argc, argv);


    VideoCapture cap(0);
    if (!cap.isOpened()) {
        return -1;
    }

    for (;;) {

        Mat frame;
        cap >> frame;

        imshow("Amogus Detector", frame);

        if (waitKey(30) >= 0)
            break;
    }


    return 0;

}
