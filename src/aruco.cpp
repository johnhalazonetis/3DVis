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

const float calibrationSquareDimension = 0.0905f;   // Dimension of side of one square [m]
const float arucoSquareDimenstion = 0.0382f;        // Dimension of side of one aruco square [m]
const Size chessboardDimensions = Size(9, 6);       // Number of square on Chessboard calibration page

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

        bool found = findChessboardCorners(*iter, Size(9,6), pointBuf, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);

        if (found)
        {
            allFoundCorners.push_back(pointBuf);
        }

        if (showResults)
        {
            drawChessboardCorners(*iter, Size(9,6), pointBuf, found);
            imshow("Looking for corners", *iter);
            waitKey(0);
        }
    }
}

int main(int argv, char** argc)
{
    Mat frame;
    Mat drawToFrame;

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);

    Mat distanceCoefficients;

    vector<vector<Point2f>> markerCorners, rejectedCandidates;

    VideoCapture vid(0);

    if (!vid.isOpened())
    {
        return -1;
    }

    int framesPerSecond = 60;

    namedWindow("Video Input", WINDOW_AUTOSIZE);

    while(true)
    {
        if (!vid.read(frame))
        {
            break;
        }

        vector<Vec2f> foundPoints;
        bool found = false;


        found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);
        frame.copyTo(drawToFrame);
        drawChessboardCorners(drawToFrame, chessboardDimensions, foundPoints, found);


        if (found)
        {
            imshow("Webcam", drawToFrame);
        } else
        {
            imshow("Webcam", frame);
        }
        char  character = waitKey(1000 / framesPerSecond);
    }

    return 0;
}