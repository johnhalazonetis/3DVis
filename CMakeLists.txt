cmake_minimum_required(VERSION 3.0)

project( 3DVis )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( OpenAndMoveWindows src/OpenAndMoveWindows.cpp )
add_executable( PixelPerfect src/PixelPerfect.cpp )
add_executable( SplitAndMerge src/SplitAndMerge.cpp )
add_executable( DFT1 src/DFT1.cpp )

target_link_libraries( OpenAndMoveWindows ${OpenCV_LIBS} )
target_link_libraries( PixelPerfect ${OpenCV_LIBS} )
target_link_libraries( SplitAndMerge ${OpenCV_LIBS} )
target_link_libraries( DFT1 ${OpenCV_LIBS} )
