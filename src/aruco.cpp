#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

const float calibrationSquareDimension = 0.03f;   // Dimension of side of one square [m]
const float arucoSquareDimenstion = 0.0382f;        // Dimension of side of one aruco square [m]
const Size chessboardDimensions = Size(7, 7);       // Number of square on Chessboard calibration page

void createArucoMarkers()       // Function to create the Aruco markers for us and put them in the "markers" directory
{
    Mat outputMarker;           // Define matrix where we have the output marker

    Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);       // Define the marker dictionary from the standard dictionary, we take the dictionary with 50 markers of 4x4 squares

    for (int i = 0; i < 50; i++)        // For loop to make each marker
    {
        aruco::drawMarker(markerDictionary, i, 500, outputMarker, 1);       // Load aruco marker 

        ostringstream convert;

        string imageName = "../markers/4x4Marker_";     // Directory where we put the marker file

        convert << imageName << i << ".jpg";            // Define name and type of image we output

        imwrite(convert.str(), outputMarker);           // Write and save the file to the desired location
    }
    
}

void createKnownBoardPosition(Size boardSize, float squareEdgeLength, vector<Point3f>& corners)     // Function to
{
    for (int i = 0; i < boardSize.height; i++)
    {
        for (int j = 0; j < boardSize.width; j++)
        {
            corners.push_back(Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.0f));
        }
    }
}

void getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners, bool showResults = false)
{
    for (vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++)
    {
        vector<Point2f> pointBuf;

        bool found = findChessboardCorners(*iter, chessboardDimensions, pointBuf, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

        if (found)
        {
            allFoundCorners.push_back(pointBuf);
        }

        if (showResults)
        {
            drawChessboardCorners(*iter, chessboardDimensions, pointBuf, found);
            imshow("Looking for corners", *iter);
            waitKey(0);
        }
    }
}

int main(int argv, char** argc)
{
    Mat frame;                                                      // Define frame as matrix

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);                      // Define camera calibration matrix

    Mat distanceCoefficients;                                       // Define distance coefficients matrix

    vector<vector<Point2f>> markerCorners, rejectedCandidates;

    VideoCapture vid("../videos/chessboard-2.mov");                 // Define VideoCapture element (where we define the input source of the video)

    if (!vid.isOpened())                                            // If no video is detected, exit the program
    {
        return -1;
    }

    int framesPerSecond = 60;                                       // Define the framerate of the videos we are dealing with

    namedWindow("Video Input", WINDOW_AUTOSIZE);                    // Make window with video input

    while(true)                                                     // Loop until exit
    {
        if (!vid.read(frame))                                       // If we can't read the frame, break the loop
        {
            break;
        }

        vector<Vec2f> foundPoints;                                  // Define vector where the found points are on the image


        bool found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CALIB_CB_FAST_CHECK);  // Use findChessboardCorners function (built into OpenCV) to find points on checker board and put the coordinates into foundPoint vector

        drawChessboardCorners(frame, chessboardDimensions, foundPoints, found);                             // Use drawChessboardCorners function (built into OpenCV) to draw the chessboard corners on the drawToFrame frame

        for (auto i = foundPoints.begin(); i != foundPoints.end(); ++i)
            {cout << *i << endl;}
        cout << endl;

        imshow("Video", frame);

        waitKey(1);

    }

    return 0;
}