cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
#SET(OpenCV_DIR /home/etcvision/installation/OpenCV-master/lib/cmake/opencv4)

find_package(OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs)
if(NOT OpenCV_FOUND)
	    find_package(OpenCV REQUIRED COMPONENTS core highgui imgproc)
endif()

add_executable(record record.cpp)
target_link_libraries(record ${OpenCV_LIBS})


