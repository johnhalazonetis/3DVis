#include <opencv2/opencv.hpp>
#include <stdint.h>

using namespace std;
using namespace cv;

int main(int argv, char** argc) {

    Mat original = imread("../images/bug.jpg", IMREAD_COLOR);   // Import image from images folder
    Mat modified = imread("../images/bug.jpg", IMREAD_COLOR);   // Import image from images folder

    if ( !original.data || !modified.data)  {                   // If there is no specified image in the images folder, terminate the program
        printf("No image data \n");
        return -1;
    }

    int channel = 2;                // Set which channel we mute in the image (red, green blue)

    for (int r = 0; r < modified.rows; r++) {           // Iterate through each row of image
        for (int c = 0; c < modified.cols; c++) {       // Iterate through each column of image

            modified.at<cv::Vec3b>(r, c)[channel] = modified.at<cv::Vec3b>(r, c)[channel] * 0;  // At each pixel, we set the specified channel to zero (not fastest method!!)
        }
    }

    imshow("Original", original);   // Show the original image
    imshow("Modified", modified);   // Show the modified image

    waitKey(0);

    return 0;
    
}