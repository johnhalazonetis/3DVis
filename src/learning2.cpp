#include <opencv2/opencv.hpp>
#include <stdint.h>

using namespace std;
using namespace cv;

int main(int argv, char** argc) {

    Mat original = imread("../images/bug.jpg", IMREAD_COLOR);
    Mat modified = imread("../images/bug.jpg", IMREAD_COLOR);

    if ( !original.data || !modified.data)  {
        printf("No image data \n");
        return -1;
    }

    for (int r = 0; r < modified.rows; r++) {
        for (int c = 0; c < modified.cols; c++) {

            modified.at<cv::Vec3b>(r, c)[2] = modified.at<cv::Vec3b>(r, c)[2] * 0;
        }
    }

    imshow("Original", original);
    imshow("Modified", modified);

    waitKey(0);

    return 0;
    
}