#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

int main(int argc, char** argv ) {

    Mat image = imread( "../images/bug.jpg", IMREAD_UNCHANGED );

    if ( !image.data ) {
        printf("No image data \n");
        return -1;
    }

    namedWindow("Display Image", WINDOW_KEEPRATIO );
    imshow("Display Image", image);

    resizeWindow("Display Image", image.cols, image.rows);

    moveWindow("Display Image", 700, 500);

    waitKey(0);

    return 0;
}