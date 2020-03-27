#include <opencv2/opencv.hpp>
#include <stdint.h>

using namespace std;
using namespace cv;

int main(int argv, char** argc) {

    Mat original = imread("../images/bug.jpg", IMREAD_GRAYSCALE);   // Import image as greyscale (we cannot perform a DFT on a multi-channel image!!)

    // We need to prepare our data to perform a DFT on it:

    Mat originalFloat;  // Define matrix so that we can convert our matrix to a float
    original.convertTo(originalFloat, CV_32FC1, 1.0/255.0);                             // Convert original matrix to float and normalize the values in the matrix
    Mat originalComplex[2] ={originalFloat, Mat::zeros(originalFloat.size(), CV_32F)};  // Make a matrix with the float values and zeros
    Mat dftReady;                                                                       // Define matrix that we will use to compute the DFT with
    merge(originalComplex, 2, dftReady);                                                // Merge the two matrices
    Mat dftOfOriginal;                                                                  // Define the output image of the DFT

    dft(dftReady, dftOfOriginal, DFT_COMPLEX_OUTPUT);                                   // Compute the DFT



    return 0;
}