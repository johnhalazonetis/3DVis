#include <opencv2/opencv.hpp>
#include <stdint.h>

using namespace std;
using namespace cv;

void takeDFT(Mat& source, Mat& destination) {

    Mat originalComplex[2] ={source, Mat::zeros(source.size(), CV_32F)};    // Make a matrix with the float values and zeros
    Mat dftReady;                                                           // Define matrix that we will use to compute the DFT with
    merge(originalComplex, 2, dftReady);                                    // Merge the two matrices

    dft(dftReady, destination, DFT_COMPLEX_OUTPUT);                         // Compute the DFT

}

void recenterDFT (Mat& source) {                                // Recenter DFT function

    /*
        We change around the quadrants of the DFT image so that the high
        frequency data is at the edges and the low frequency data is at
        the center (this is usually how DFT images are presented).
    */

    int centerX = source.cols/2;                                // Define center column of image
    int centerY = source.rows/2;                                // Define center row of image

    Mat q1(source, Rect(0, 0, centerX, centerY));               // Define quadrant 1 from top left to center point of image
    Mat q2(source, Rect(centerX, 0, centerX, centerY));         // Define quadrant 2 from top middle to center row of image
    Mat q3(source, Rect(0, centerY, centerX, centerY));         // Define quadrant 3 from middle row to bottom center point of image
    Mat q4(source, Rect(centerX, centerY, centerX, centerY));   // Define quadrant 4 from middle point to bottom right point of image

    Mat swapMap;            // Define swapMap matrix for swapping the quadrants (temporary variable for placing the quadrant we are moving)

    q1.copyTo(swapMap);     // Copy quadrant 1 to swapMap
    q4.copyTo(q1);          // Move quadrant 4 to where quadrant 1 was
    swapMap.copyTo(q4);     // Move quadrant 1 to where quadrant 4 was

    q2.copyTo(swapMap);     // Copy quadrant 2 to swapMap
    q3.copyTo(q2);          // Move quadrant 3 to where quadrant 2 was
    swapMap.copyTo(q3);     // Move quadrant 2 to where quadrant 3 was

}

void showDFT(Mat& source) {

    Mat splitArray[2] = {Mat::zeros(source.size(), CV_32F), Mat::zeros(source.size(), CV_32F)}; 

    split(source, splitArray);

    Mat dftMagnitude;

    magnitude(splitArray[0], splitArray[1], dftMagnitude);

    dftMagnitude += Scalar::all(1);                                 // Add 1 to add values in dftMagnitude
    log(dftMagnitude, dftMagnitude);                                // Take the log of dftMagnitude

    normalize(dftMagnitude, dftMagnitude, 0, 1, NORM_MINMAX);       // Normalize dftMagnitude with 0 as minimum value and 1 as maximum value

    recenterDFT(dftMagnitude);

    imshow("DFT", dftMagnitude);                                    // Show dftMagnitude in Window
    waitKey(0);
}

void invertDFT(Mat& source, Mat& destination) {                             // Function to invert the DFT to get original image                                                        // Define inverse matrix

    dft(source, destination, DFT_INVERSE | DFT_REAL_OUTPUT | DFT_SCALE);    // call dft with the flags to invert, only retain the real ouput value, and scale the image.
}

int main(int argv, char** argc) {

    Mat original = imread("../images/bug.jpg", IMREAD_GRAYSCALE);   // Import image as greyscale (we cannot perform a DFT on a multi-channel image!!)

    Mat originalFloat;                                              // Define matrix so that we can convert our matrix to a float
    original.convertTo(originalFloat, CV_32FC1, 1.0/255.0);         // Convert original matrix to float and normalize the values in the matrix

    Mat dftOfOriginal;                                              // Define the output image of the DFT

    takeDFT(originalFloat, dftOfOriginal);                          // Call function to compute DFT
    showDFT(dftOfOriginal);                                         // Call function to show DFT

    Mat invertedDFT;                                                // Define inverted DFT data matrix

    invertDFT(dftOfOriginal, invertedDFT);                          // Call invert DFT function

    imshow("Inverted DFT Result", invertedDFT);                     // Show inverted DFT image (which will be the original image)
    waitKey(0);

    return 0;
}