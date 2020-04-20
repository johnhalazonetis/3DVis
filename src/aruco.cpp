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

int startCameraMonitoring(const Mat& cameraMatrix, const Mat& distanceCoefficients, float arucoSquareDimension)     // Function find aruco codes in video
{
    Mat frame;                                                                                                      // Define a matrix as the current frame

    vector<int> markerIDs;                                                                                          // Make a vector of integers with the marker IDs
    vector<vector<Point2f>> markerCorners, rejectedCandidates;                                                      // Make a vector of vectors with the markerCorners, and the rejected candidates

    aruco::DetectorParameters parameters;                                                                           // Detect the parameters of the aruco codes
    Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);   // Define the aruco dictionary as the standard 4x4 dictionary

    VideoCapture vid("../videos/aruco-40.mov");                                                                     // Define video capturing element

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
        
        string displayText = "Number of marker detected: " + to_string(markerIDs.size());
        Point textOrg(20, 40);
        putText(frame, displayText, textOrg, FONT_HERSHEY_SIMPLEX, 1, Scalar::all(255), 1, 8);


        if (markerIDs.size() > 0)
        {
            // aruco::drawDetectedMarkers(frame, markerCorners, markerIDs);

            vector<Vec3d> rvec, tvec;
            aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDimension, cameraMatrix, distanceCoefficients, rvec, tvec);     // Estimate the pose of the Aruco markers (built into OpenCV Aruco)

            for (int i = 0; i != markerIDs.size(); i++)
            {
                aruco::drawAxis(frame, cameraMatrix, distanceCoefficients, rvec[i], tvec[i], 0.1f);                 // Draw the axis on each of the aruco corners (built into OpenCV Aruco)
            }
            
        }

        imshow("Video", frame);
        waitKey(1);
    }
    return 1;

}

bool loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients, bool showResults = false) {     // Function to load camera calibration matrix

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
                
                if (showResults)
                {
                    cout << cameraMatrix.at<double>(r, c) << endl;
                }
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
                
                if (showResults)
                {
                    cout << distanceCoefficients.at<double>(r, c) << endl;
                }
            }
        }
        inStream.close();
        return true;

    }

    return false;
}


int main(int argv, char** argc)
{
    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);                      // Define camera calibration matrix

    Mat distanceCoefficients;                                       // Define distance coefficients matrix

    namedWindow("Video Input", WINDOW_AUTOSIZE);                    // Make window with video input

    cout << "Loading camera calibration matrix..." << endl;
    loadCameraCalibration("../cameraCalibration", cameraMatrix, distanceCoefficients);
    
    // cout << "Camera matrix:" << endl << cameraMatrix << endl << endl;

    cout << "Camera parameters loaded! Starting monitoring for aruco codes..." << endl << endl;

    startCameraMonitoring(cameraMatrix, distanceCoefficients, arucoSquareDimension);

    return 0;
}
