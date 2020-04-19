#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

const float calibrationSquareDimension = 0.03f;     // Dimension of side of one square [m]
const Size chessboardDimensions = Size(4, 8);       // Number of square on Chessboard calibration page

void createKnownBoardPosition(Size boardSize, float squareEdgeLength, vector<Point3f>& corners)
{
    for (int i = 0; i < boardSize.height; i++)
    {
        for (int j = 0; j < boardSize.width; j++)
        {
            corners.push_back(Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.0f));
        }
    }
}

void getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners, bool showResults = false)                           // Function to find chessboard corners from a set of images
{
    for (vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++)                                                         // Loop through the saved images
    {
        vector<Point2f> pointBuf;                                                                                                           // Define a vector of 2D points for the points detected called pointBuf

        bool found = findChessboardCorners(*iter, chessboardDimensions, pointBuf, CALIB_CB_NORMALIZE_IMAGE & CALIB_CB_FAST_CHECK);     // Execute findChessboardCorners (built into OpenCV) to find the corners of the image

        if (found)                                                                                                                          // If we have found the corners
        {
            allFoundCorners.push_back(pointBuf);                                                                                            // Put the found points into pointBuf
        }

        if (showResults)                                                                                                                    // If we have asked to show the results
        {
            drawChessboardCorners(*iter, chessboardDimensions, pointBuf, found);                                                            // Draw the chessboard
            imshow("Looking for corners", *iter);
            waitKey(0);
        }
    }
}

void cameraCalibration(vector<Mat> calibrationImages, Size boardSize, float squareEdgeLength, Mat& cameraMatrix, Mat& distanceCoefficients)     // Function to calibrate cameras
{
    vector<vector<Point2f>> chessboardImageSpacePoints;                                                                                         // Define identified points from chessboard into a vector of vectors
    getChessboardCorners(calibrationImages, chessboardImageSpacePoints, false);                                                                 // Get points from chessboard using getChessboardCorners

    vector<vector<Point3f>> worldSpaceCornerPoints(1);

    createKnownBoardPosition(boardSize, squareEdgeLength, worldSpaceCornerPoints[0]);
    worldSpaceCornerPoints.resize(chessboardImageSpacePoints.size(), worldSpaceCornerPoints[0]);

    vector<Mat> rVectors, tVectors;
    distanceCoefficients = Mat::zeros(8, 1, CV_64F);

    calibrateCamera(worldSpaceCornerPoints, chessboardImageSpacePoints, boardSize, cameraMatrix, distanceCoefficients, rVectors, tVectors);     // Function to calibrate camera from the previously cmoputed data


}

bool saveCameraCalibration(string name, Mat cameraMatrix, Mat distanceCoefficients) {
    ofstream outStream(name);
    if (outStream)
    {
        uint16_t rows = cameraMatrix.rows;
        uint16_t columns = cameraMatrix.cols;

        outStream << rows << endl;
        outStream << columns << endl;

        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < columns; c++)
            {
                double value = cameraMatrix.at<double>(r, c);
                outStream << value << endl;
            }
        }

        rows = distanceCoefficients.rows;
        columns = distanceCoefficients.cols;

        outStream << rows << endl;
        outStream << columns << endl;

        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < columns; c++)
            {
                double value = distanceCoefficients.at<double>(r, c);
                outStream << value << endl;
            }
        }

        outStream.close();
        return true;
    }

    return false;
}

int main(int argv, char** argc)
{
    Mat frame;                                                  // Define frame as matrix

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);                  // Define camera calibration matrix

    Mat distanceCoefficients;                                   // Define distance coefficients matrix

    vector<Mat> savedImages;                                    // Define vector of savedImages for camera calibration

    vector<vector<Point2f>> markerCorners, rejectedCandidates;

    vector<cv::String> fn;                                      // Automatically detect how many images there are in calibration_images directory
    glob("../calibration_images/calib_*.jpeg", fn, false);      // Automatically detect how many images there are in calibration_images directory

    vector<Mat> images;                                         // Automatically detect how many images there are in calibration_images directory
    size_t numberCalibrationImages = fn.size();                 // Automatically detect how many images there are in calibration_images directory

    for (size_t n = 0; n < numberCalibrationImages; n++)        // Loop through images in calibration_images directory
    {
        std::string calibrationImagePath = "../calibration_images/calib_" + to_string(n) + ".jpeg";     // Define name of image for current iteration as we loop through them
        frame = imread(calibrationImagePath, IMREAD_COLOR);                                             // Read image and import it into the "frame" matrix

        if(!frame.data)                                         // If there is no data in the frame, exit the program
        {
            cout << "No input image detected, exiting program..." << endl;
            return -1;
        }

        vector<Vec2f> foundPoints;                              // Define vector where the found points are on the image

        bool found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CALIB_CB_NORMALIZE_IMAGE);     // Use findChessboardCorners function (built into OpenCV) to find points on chessboard and put coordinates into foundPoint vector

        drawChessboardCorners(frame, chessboardDimensions, foundPoints, found);                                     // Use drawChessboardCorners function (built into OpenCV) to draw chessboard corners on drawToFrame frame

        if (found)                                                                                          // If the chessboard is found in the current frame...
        {
            Mat temp;                                                                                       // Create a temporary matrix
            frame.copyTo(temp);                                                                             // Copy the frame to the temporary matrix
            savedImages.push_back(temp);                                                                    // Add the temporary frame to the list of saved images
            cout << "   Image saved. Number of saved images: " << savedImages.size() << endl;               // Output message to confirm that the image was saved
        } else {
            cout << "   No chessboard found in this frame!" << endl;                           // Output message telling us that the chessboard was not found in that frame
        }

    }

    destroyAllWindows();

    cout << savedImages.size() << " chessboards found out of " << numberCalibrationImages << " images, starting calibration..." << endl;    // Output message to inform the program has successfully exited the loop and started calibrating
    cameraCalibration(savedImages, chessboardDimensions, calibrationSquareDimension, cameraMatrix, distanceCoefficients);   // Run cameraCalibration function
    cout << "Camera calibrated using " << savedImages.size() << " images!" << endl;             // Output message to confirm that the camera parameters have been found using the provided saved images
    saveCameraCalibration("../cameraCalibration", cameraMatrix, distanceCoefficients);          // Run saveCameraCalibration function to output to text file
    cout << "Camera calibration saved!" << endl;;                                               // Output message to confirm that the camera parameters have been found using the provided saved images

    return 0;
}
