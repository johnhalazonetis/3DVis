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

const float calibrationSquareDimension = 0.03f;     // Dimension of side of one square [m]
const float arucoSquareDimension = 0.0382f;         // Dimension of side of one aruco square [m]
const Size chessboardDimensions = Size(4, 8);       // Number of square on Chessboard calibration page

void createArucoMarkers()                                               // Function to create the Aruco markers for us and put them in the "markers" directory
{
    Mat outputMarker;                                                   // Define matrix where we have the output marker

    Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);       // Define the marker dictionary from the standard dictionary, we take the dictionary with 50 markers of 4x4 squares

    for (int i = 0; i < 50; i++)                                        // For loop to make each marker
    {
        aruco::drawMarker(markerDictionary, i, 500, outputMarker, 1);   // Load aruco marker

        ostringstream convert;

        string imageName = "../markers/4x4Marker_";                     // Directory where we put the marker file

        convert << imageName << i << ".jpg";                            // Define name and type of image we output

        imwrite(convert.str(), outputMarker);                           // Write and save the file to the desired location
    }

}

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

        bool found = findChessboardCorners(*iter, chessboardDimensions, pointBuf, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);     // Execute findChessboardCorners (built into OpenCV) to find the corners of the image

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

int startCameraMonitoring(const Mat& cameraMatrix, const Mat& distanceCoefficients, float arucoSquareDimension)     // Function find aruco codes in video
{
    Mat frame;                                                                                                      // Define a matrix as the current frame

    vector<int> markerIDs;                                                                                          // Make a vector of integers with the marker IDs
    vector<vector<Point2f>> markerCorners, rejectedCandidates;                                                      // Make a vector of vectors with the markerCorners, and the rejected candidates

    aruco::DetectorParameters parameters;                                                                           // Detect the parameters of the aruco codes
    Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);   // Define the aruco dictionary as the standard 4x4 dictionary

    VideoCapture vid("../videos/aruco.mov");                                                                        // Define video capturing element

    if (!vid.isOpened()) {                                                                                          // If we cannot open the video, exit the program
        return -1;
    }

    namedWindow("Video", WINDOW_AUTOSIZE);                                                                          // Create a named window

    vector<Vec3d> rotationVectors, translationVectors;                                                              // Create a vector of 3d vectors containing the rotation and translation vectors

    while (true) {                                                                                                  // Loop
        if (!vid.read(frame)) {                                                                                     // If we cannot read the frame, exit the loop
          break;
        }

        aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIDs);                                    // Run the detect marker function (built into OpenCV Aruco)
        aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDimension, cameraMatrix, distanceCoefficients, rotationVectors, translationVectors);     // Estimate the pose of the Aruco markers (built into OpenCV Aruco)

        for (int i = 0; i < markerIDs.size(); i++)                                                                  // Loop over all of the aruco markers in the frame
        {
            aruco::drawAxis(frame, cameraMatrix, distanceCoefficients, rotationVectors, translationVectors, 0.1f);  // Draw the axis on each of the aruco corners
        }

        imshow("Video", frame);                                                                                     // If it takes more than 30 milliseconds to perform this task, skip and move onto the next frame
        if (waitKey(30) >= 0) {
          break;
        }
    }
    return 1;

}

void cameraCalibration(vector<Mat> calibrationImages, Size boardSize, float squareEdgeLength, Mat& cameraMatrix, Mat& distanceCoefficients)     // Function to calibrate cameras
{
    vector<vector<Point2f>> chessboardImageSpacePoints;                                                                                         // Define identified points from chessboard into a vector of vectors
    getChessboardCorners(calibrationImages, chessboardImageSpacePoints, false);                                                                 // Get points from chessboard using getChessboardCorners (function built into OpenCV)

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

bool loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients) {     // Function to load camera calibration matrix

    ifstream inStream(name);

    if (inStream){
        uint16_t rows;
        uint16_t columns;

        // Camera Matrix
        inStream >> rows;
        inStream >> columns;

        cameraMatrix = Mat(Size(rows, columns), CV_64F);

        for (int r = 0; r < rows; r++){
            for(int c = 0; c < columns; c++){
                double read = 0.0f;
                inStream >> read;
                cameraMatrix.at<double>(r, c) = read;
                cout << cameraMatrix.at<double>(r, c) << endl;
            }
        }

        // Distance Coefficients
        inStream >> rows;
        inStream >> columns;

        distanceCoefficients = Mat::zeros(rows, columns, CV_64F);

        for (int r = 0; r < rows; r++){
            for(int c = 0; c < columns; c++){
                double read = 0.0f;

                inStream >> read;

                distanceCoefficients.at<double>(r, c) = read;
                cout << distanceCoefficients.at<double>(r, c) << endl;
            }
        }
        inStream.close();
        return true;

    }

    return false;
}


int main(int argv, char** argc)
{
    Mat frame;                                                      // Define frame as matrix

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);                      // Define camera calibration matrix

    Mat distanceCoefficients;                                       // Define distance coefficients matrix

    vector<Mat> savedImages;

    vector<vector<Point2f>> markerCorners, rejectedCandidates;

    VideoCapture vid("../videos/chessboard-4.mov");                 // Define VideoCapture element (where we define the input source of the video)

    if (!vid.isOpened())                                            // If no video is detected, exit the program
    {
        return -1;
    }


    namedWindow("Video Input", WINDOW_AUTOSIZE);                    // Make window with video input


    while(true)                                                     // Loop until exit
    {

        if (!vid.read(frame))                                       // If we can't read the frame, break the loop
        {
            break;
        }

        vector<Vec2f> foundPoints;                                  // Define vector where the found points are on the image


        bool found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CALIB_CB_NORMALIZE_IMAGE);     // Use findChessboardCorners function (built into OpenCV) to find points on chessboard and put coordinates into foundPoint vector

        drawChessboardCorners(frame, chessboardDimensions, foundPoints, found);                                     // Use drawChessboardCorners function (built into OpenCV) to draw chessboard corners on drawToFrame frame


        imshow("Video", frame);                                                                                     // Show the frame

        char character = waitKey(1);

        switch(character)                                                                                           // Depending on which character we press on the keyboard...
        {
            case ' ':                                                                                               // Space Bar: Save Image
                cout << "Input <SPACE> recieved" << endl;                                                           // Output message
                if (found)                                                                                          // If the chessboard is found in the current frame...
                {
                    Mat temp;                                                                                       // Create a temporary matrix
                    frame.copyTo(temp);                                                                             // Copy the frame to the temporary matrix
                    savedImages.push_back(temp);                                                                    // Add the temporary frame to the list of saved images
                    cout << "   Image saved. Number of saved images: " << savedImages.size() << endl;               // Output message to confirm that the image was saved
                } else {
                    cout << "   No chessboard found in this frame, try another!" << endl;                           // Output message telling us that the chessboard was not found in that frame
                }
                break;
            case 13:                                                                                                // Enter Key: Start calibration
                cout << "Input <ENTER> recieved!" << endl;;                                                         // Output message
                if (savedImages.size() > 15)                                                                        // If there are more than 15 images saved...
                {
                    cout << "   Enough images saved! Starting calibration...     ";                                 // Output message to confirm that we have more than 15 saved images
                    cameraCalibration(savedImages, chessboardDimensions, calibrationSquareDimension, cameraMatrix, distanceCoefficients);   // Run cameraCalibration function
                    saveCameraCalibration("cameraCalibration", cameraMatrix, distanceCoefficients);                 // Run saveCameraCalibration function to output to text file
                    cout << "Camera calibrated using " << savedImages.size() << " images!" << endl;                 // Output message to confirm that the camera parameters have been found using the provided saved images
                } else {
                    cout << "   Not enough saved images! Press space a few more times..." << endl;                  // Output message warning us that not enough images have been saved
                }
                break;
            case 27:                                                                                                // Escape Key: Exit
                cout << "Input <ESC> recieved! Exiting..." << endl;                                                 // Output message to confirm that we have pressed the escape key
                return 0;                                                                                           // End program
        }

    }

    return 0;
}
