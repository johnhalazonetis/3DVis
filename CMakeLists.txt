cmake_minimum_required(VERSION 2.8)

project( 3DVis )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( learning src/learning.cpp )
add_executable( learning2 src/learning2.cpp )

target_link_libraries( learning ${OpenCV_LIBS} )
target_link_libraries( learning2 ${OpenCV_LIBS} )
