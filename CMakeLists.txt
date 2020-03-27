cmake_minimum_required(VERSION 2.8)

project( 3DVis )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( 3DVis src/learning.cpp )

target_link_libraries( 3DVis ${OpenCV_LIBS} )
