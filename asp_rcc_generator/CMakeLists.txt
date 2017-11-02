cmake_minimum_required(VERSION 2.8.3)
project(asp_rcc_generator)

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")


## Enable exception handling for segfaults
set(CMAKE_CXX_FLAGS "-rdynamic -g -fnon-call-exceptions -ggdb ${CMAKE_CXX_FLAGS}")

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES 
  CATKIN_DEPENDS roscpp rospy std_msgs
  # DEPENDS 
)
include_directories(include
  ${catkin_INCLUDE_DIRS}
)

## Declare a cpp executable
add_executable(asp_rcc_generator src/ASPRCCGenerator.cpp)

target_link_libraries(asp_rcc_generator ${catkin_LIBRARIES} )
