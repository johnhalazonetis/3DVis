#include <opencv2/opencv.hpp>
#include <stdint.h>

using namespace cv;
using namespace std;

const int fps = 60;

int main(int argv, char** argc) {

    Mat frame;                                      // Store the current frame

    VideoCapture vid("../videos/test-40.mov");      // VideoCapture object - vid can load in stream data, webcam, or a video file

    // if (!vid.isOpened()) {                       // If no video is detected, exit the program (for live feeds only)
    //     return -1;
    // }

    while (vid.read(frame)) {
        
        imshow("Video Feed", frame);

        if(waitKey( 1000/fps ) >= 0) {
            break;
        }
    }
    
    return 1;
}
