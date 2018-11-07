cmake_minimum_required(VERSION 2.8.3)
project(cpptests)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES cpptests
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

include_directories(
# include
# ${catkin_INCLUDE_DIRS}
)

add_executable(unordered_set_test
  src/unordered_set_test.cpp
)
