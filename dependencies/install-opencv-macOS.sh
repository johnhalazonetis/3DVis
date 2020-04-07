# Install OpenCV on macOS

# First we need to install the dependencies (installing XCode completely is the best bet)
xcode-select --install

# Then we download the latest version of OpenCV:
mkdir opcv_install && cd opcv_install

git clone https://github.com/Itseez/opencv.git

git clone https://github.com/Itseez/opencv_contrib.git

# Then we build the library to be used in our system
cd opencv

mkdir release

cd release

cmake -D BUILD_TIFF=ON -D WITH_CUDA=OFF -D ENABLE_AVX=OFF -D WITH_OPENGL=OFF -D WITH_OPENCL=OFF -D WITH_IPP=OFF -D WITH_TBB=ON -D BUILD_TBB=ON -D WITH_EIGEN=OFF -D WITH_V4L=OFF -D WITH_VTK=OFF -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=~/opcv_install/opencv_contrib/modules ~/opcv_install/opencv/

make -j2	# You can change the number of cores you want to use to build the library

make install
