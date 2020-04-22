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

const float arucoSquareDimension = 0.0382f;         // Dimension of side of one aruco square [m]
const Size chessboardDimensions = Size(4, 8);       // Number of square on Chessboard calibration page

void createArucoMarkers() {                                             // Function to create the Aruco markers for us and put them in the "markers" directory

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

void inverseOfCameraMatrix(const Mat& cameraMatrix, Mat inverseCameraMatrix) {  // Function to calculate inverse of camera matrix (specific to 3x3 upper traignel matrices)
    inverseCameraMatrix = Mat::zeros(3, 3, CV_64F);

    double a = cameraMatrix.at<double>(0, 0);
    double b = cameraMatrix.at<double>(0, 1);
    double c = cameraMatrix.at<double>(0, 2);
    double d = cameraMatrix.at<double>(1, 1);
    double e = cameraMatrix.at<double>(1, 2);
    double f = cameraMatrix.at<double>(2, 2);

    inverseCameraMatrix.at<double>(0, 0) = 1/a;
    inverseCameraMatrix.at<double>(0, 1) = -b/(a*d);
    inverseCameraMatrix.at<double>(0, 2) = (b*e - c*d)/(a*f*d);
    inverseCameraMatrix.at<double>(1, 1) = 1/d;
    inverseCameraMatrix.at<double>(1, 2) = -e/(f*d);
    inverseCameraMatrix.at<double>(2, 2) = 1/f;

}

void drawDetectedMarkerAxis(InputOutputArray _image, InputArrayOfArrays _corners, InputArray _ids, const Mat& cameraMatrix, bool showID = false) {

    CV_Assert(_image.getMat().total() != 0 && (_image.getMat().channels() == 1 || _image.getMat().channels() == 3));
    CV_Assert((_corners.total() == _ids.total()) || _ids.total() == 0);

    int nMarkers = (int)_corners.total();
    for(int i = 0; i < nMarkers; i++) {
        Mat currentMarker = _corners.getMat(i);
        CV_Assert(currentMarker.total() == 4 && currentMarker.type() == CV_32FC2);

        // Draw marker axis
        Point2f originPoint, xPoint, yPoint, zPoint;
        originPoint = currentMarker.ptr< Point2f >(0)[2];
        xPoint = currentMarker.ptr< Point2f >(0)[1];
        yPoint = currentMarker.ptr< Point2f >(0)[3];
        Point2f translation;
        translation = 0.5*(xPoint - originPoint) + 0.5*(yPoint - originPoint);

        line(_image, originPoint + translation, xPoint + translation, cv::Scalar(0, 0, 255), 1, 8, 0);
        line(_image, originPoint + translation, yPoint + translation, cv::Scalar(0, 255, 0), 1, 8, 0);

        Mat inverseCameraMatrix;
        inverseOfCameraMatrix(cameraMatrix, inverseCameraMatrix);

        // Draw first corner mark
        rectangle(_image, currentMarker.ptr< Point2f >(0)[2] - Point2f(3, 3) + translation, currentMarker.ptr< Point2f >(0)[2] + Point2f(3, 3) + translation, cv::Scalar(255, 255, 255), 1, LINE_AA);

        // Show ID number (if requested)
        if (showID) {
            
            if(_ids.total() != 0) {
                
                Point2f cent(0, 0);
                for(int p = 0; p < 4; p++)
                    cent += currentMarker.ptr< Point2f >(0)[p];
                cent = cent / 4.;
                stringstream s;
                s << "id=" << _ids.getMat().ptr< int >(0)[i];
                putText(_image, s.str(), cent, FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 2);
            }
        }
    }
}

int startCameraMonitoring(const Mat& cameraMatrix, const Mat& distortionCoefficients, float arucoSquareDimension) { // Function find aruco codes in video

    Mat frame;                                                                                                      // Define a matrix as the current frame

    vector<int> markerIDs;                                                                                          // Make a vector of integers with the marker IDs
    vector<vector<Point2f>> markerCorners, rejectedCandidates;                                                      // Make a vector of vectors with the markerCorners, and the rejected candidates

    aruco::DetectorParameters parameters;                                                                           // Detect the parameters of the aruco codes
    Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);   // Define the aruco dictionary as the standard 4x4 dictionary

    VideoCapture vid("../videos/aruco-40.mov");                                                                     // Define video capturing element

    if (!vid.isOpened()) {                                                                                          // If we cannot open the video, exit the program
        return -1;
    }

    cv::namedWindow("Video", WINDOW_AUTOSIZE);                                                                     // Create a named window

    vector<Vec3d> rotationVectors, translationVectors;                                                              // Create a vector of 3d vectors containing the rotation and translation vectors
    Mat rotationMatrix;

    int nFrame = 0;

    while (true) {                                                                                                  // Loop
        if (!vid.read(frame)) {                                                                                     // If we cannot read the frame, exit the loop
            break;
        }

        aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIDs);                                    // Run the detect marker function (built into OpenCV Aruco)
        
        putText(frame, "Number of marker detected: " + to_string(markerIDs.size()), Point(20, 40), FONT_HERSHEY_SIMPLEX, 1, Scalar::all(255), 1, 8);    // Display how many aruco codes are found in the frame

        if (markerIDs.size() > 0)
        {
            drawDetectedMarkerAxis(frame, markerCorners, markerIDs, cameraMatrix, true);
        }

        cv::imshow("Video", frame);
        cv::waitKey(1);
    }
    return 1;

}

bool loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distortionCoefficients, bool showResults = false) {     // Function to load camera calibration matrix

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

        distortionCoefficients = Mat::zeros(rows, columns, CV_64F);

        for (int r = 0; r < rows; r++){
            for(int c = 0; c < columns; c++){
                double read = 0.0f;

                inStream >> read;

                distortionCoefficients.at<double>(r, c) = read;
                
                if (showResults)
                {
                    cout << distortionCoefficients.at<double>(r, c) << endl;
                }
            }
        }
        inStream.close();
        return true;

    }

    return false;
}


int main(int argv, char** argc) {

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);                          // Define camera calibration matrix

    Mat distortionCoefficients;                                         // Define distance coefficients matrix

    cout << "Loading camera calibration matrix..." << endl;
    loadCameraCalibration("../cameraCalibration", cameraMatrix, distortionCoefficients);

    cout << "Camera parameters loaded! Starting monitoring for aruco codes..." << endl << endl;

    startCameraMonitoring(cameraMatrix, distortionCoefficients, arucoSquareDimension);

    return 0;
}
