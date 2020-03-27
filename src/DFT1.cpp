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

void showDFT(Mat& source) {

    Mat splitArray[2] = {Mat::zeros(source.size(), CV_32F), Mat::zeros(source.size(), CV_32F)}; 

    split(source, splitArray);

    Mat dftMagnitude;

    magnitude(splitArray[0], splitArray[1], dftMagnitude);

    dftMagnitude += Scalar::all(1);                                 // Add 1 to add values in dftMagnitude
    log(dftMagnitude, dftMagnitude);                                // Take the log of dftMagnitude

    normalize(dftMagnitude, dftMagnitude, 0, 1, NORM_MINMAX);       // Normalize dftMagnitude with 0 as minimum value and 1 as maximum value

    imshow("DFT", dftMagnitude);                                    // Show dftMagnitude in Window
    waitKey(0);
}

int main(int argv, char** argc) {

    Mat original = imread("../images/bug.jpg", IMREAD_GRAYSCALE);   // Import image as greyscale (we cannot perform a DFT on a multi-channel image!!)

    Mat originalFloat;                                              // Define matrix so that we can convert our matrix to a float
    original.convertTo(originalFloat, CV_32FC1, 1.0/255.0);         // Convert original matrix to float and normalize the values in the matrix

    Mat dftOfOriginal;                                              // Define the output image of the DFT

    takeDFT(originalFloat, dftOfOriginal);                          // Call function to compute DFT
    showDFT(dftOfOriginal);                                         // Call function to show DFT

    return 0;
}