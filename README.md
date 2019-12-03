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

## Appendix
### Dependencies:
3DVis uses Python 3.6 as its programming language, and uses the following libraries listed below:

* NumPy
* OpenCV (Python)
* Yaml-Python
* MatlPlotLib
