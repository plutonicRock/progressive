cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)

project( Progressive )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
# set(CMAKE_BUILD_TYPE "DEBUG")

find_package( OpenCV REQUIRED )
add_executable( Progressive Progressive.cpp )
target_link_libraries( Progressive ${OpenCV_LIBS} ${CMAKE_DL_LIBS})