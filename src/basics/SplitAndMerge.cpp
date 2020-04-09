#include <opencv2/opencv.hpp>
#include <stdint.h>

using namespace std;
using namespace cv;

int main(int argv, char** argc) {
    
    Mat original = imread("../images/bug.jpg", IMREAD_COLOR);       // Import image in color

    Mat splitChannels[3];   // Define new matrix with 3 elements

    split(original, splitChannels);     // Split the original image into the the matrix splitChannels (each color goes in each element)

    imshow("B", splitChannels[0]);      // Show the blue channel
    imshow("G", splitChannels[1]);      // Show the green channel
    imshow("R", splitChannels[2]);      // Show the red channel

    int channel = 2;        // Define a chosen channel

    splitChannels[channel] = Mat::zeros(splitChannels[channel].size(), CV_8UC1);    // Set the chosen channel to zero

    Mat output;     // Define an output matrix of values

    merge(splitChannels, 3, output);    // Merge the 3 channels we have (including the modified channel)
    imshow("Merged", output);           // Show the merged channel in a window

    waitKey(0);

    return 0;

}