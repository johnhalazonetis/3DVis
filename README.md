# 3D Vision Project

This repo is to serve the purpose of creating 3DVis, where we will attempt to have a tracking algorithm using predefined 3D models made in CAD applications.

## Goals of Algorithm

Our algorithm's goal is to identify the blue, yellow and orange cones that make up the track in Formula Student Driverless Events.

Of course, we want our algorithm to be able to identify these cones correctly, be robust to moving cones, as well as occlusions. Finally we want our algorithm to be executable on a small developement computer (where there are limited computing resources), and be fast enough to be used with a 60 Hz camera. To put that in perspective:

* A camera at 30 Hz has a frame time of 0.0333 seconds
* A camera at 60 Hz has a frame time of 0.0166 seconds

Meaning that our algorithm has to be able to be completed in under 16 milliseconds.

More time can be gained by using the GPUs computing power to leverage the high resolution frames that we can obtain from the camera, however for now, we will be using OpenCV without CUDA acceleration.

## Steps of Algorithm
1. Take frame from camera (for testing purposes we take an image and compute the time it takes to run the pipeline).
2. Convert images into white blobs by selecting within a certain range of color.
3. Find contours of blobs and filter out any noise that could impact performance of algorithm.

## Use of Neural Netowrks
For now we are not using neural networks in the system, we are just looking to see how much we can filter our data using openCV's tools while maintaining a high speed calculation (to be able to perform at 60 fps).

When we reach our limit of this, and when we have more experience with PyTorch NNs, we will then implement our NN to recognize cone AND be able to place them in a 3D space.

## Moving to C++
Given the speed of C++, we can obtain better speed results with C++.

## Appendix

*Note:* All dependencies can be install using the sheel scripts included in the ```dependiencies``` directory included with the 3DVis repo. *It is however recommended that users running anything other than a debian-based system should either modify the scripts or type in the commands one by one to install dependencies without any errors*.

If you do have a debian-based system, you can follow the instruction below to install all dependencies needed to run 3DVis (you will need ```sudo``` permissions):

### Instructions to running the installation scripts to install deendencies
1. Start by going into the ```dependiencies``` directory by opening a terminal and typing in the command:
    ```cd dependencies```

2. Change the mode of the scripts to that you can run them on your system:
    ```chmod +x *.sh```

3. Run all scripts (or one by one) by typing:
    ```sudo ./*.sh```   (respectively ```sudo ./<name-of-script>.sh``` if you only want to run one script at a time)

### Dependencies (for non-neural network scripts):
3DVis uses Python 3.6 as its programming language, and uses the following libraries listed below:

* NumPy
* OpenCV (Python)
* Yaml-Python
* MatlPlotLib
* tqdm (to show progress in for loop)

### Dependencies for NN scripts

* PyTorch:
    * torch
    * torchvision