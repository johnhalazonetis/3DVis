# 3D Vision Project

This repository is to serve the purpose of creating 3DVis, where we will attempt to track objects in 3D using deep learning and computer vision algorithms.

## Goals of Algorithm

Our algorithm's goal is to identify the blue, yellow and orange cones that make up the track in Formula Student Driverless Events.

Of course, we want our algorithm to be able to identify these cones correctly, be robust to moving cones, as well as occlusions. Finally we want our algorithm to be executable on a small development computer (where there are limited computing resources), and be fast enough to be used with a 60 Hz camera. To put that in perspective:

* A camera at 30 Hz has a frame time of 0.0333 seconds
* A camera at 60 Hz has a frame time of 0.0166 seconds

Meaning that our algorithm has to be able to be completed in under 16 milliseconds.

More time can be gained by using the GPUs computing power to leverage the high resolution frames that we can obtain from the camera, however for now, we will be using OpenCV without CUDA acceleration.

We also want to be able to position these cones in 3D space whilst only using one or (at maximum) two cameras. To do so, we will use our known knowledge of camera calibration and 3D vision to have an accurate estimation of where the cone is and its orientation in 3D space.

## Use of Neural Networks
For now we are not using neural networks in the system, we are just looking to see how much we can filter our data using OpenCV's tools while maintaining a high speed calculation (to be able to perform at 60 fps).

## Alternative Goals
* Learning more C++
* Learning OpenCV in C++

# Setting Things Up

Start by cloning the repo onto your computer: ```git clone https://github.com/johnhalazonetis/3DVis.git```.

Go into the folder: ```cd 3DVis```

Make build directory and go cd into it: ```mkdir build && cd build```

After installing the required dependencies (see Appendix) you can build the project:
	```cmake ..```
	```make```

You can then execute any one of the executables in the build directory (they have the same as the scripts in the ```src``` directory), e.g. ```./PixelPerfect```.

# Appendix

## Dependencies

*Note:* All dependencies can be install using the shell scripts included in the ```dependencies``` directory included with the 3DVis repo. *It is however recommended that users running anything other than a debian-based system should either modify the scripts or type in the commands one by one to install dependencies without any errors*.

If you do have a debian-based system, you can follow the instruction below to install all dependencies needed to run 3DVis (you will need ```sudo``` permissions):

### Instructions to running the installation scripts to install dependencies
1. Start by going into the ```dependencies``` directory by opening a terminal and typing in the command:
    ```cd dependencies```

2. Change the mode of the scripts to that you can run them on your system:
    ```chmod +x *.sh```

3. Run all scripts (or one by one) by typing:
    ```sudo ./*.sh```   (respectively ```sudo ./<name-of-script>.sh``` if you only want to run one script at a time)

#### Documentation of Used Libraries

* [OpenCV 4.3.0 Documentation](https://docs.opencv.org/4.3.0/index.html)
