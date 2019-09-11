cmake_minimum_required(VERSION 3.10)
project(external_eval)

set(CMAKE_CXX_STANDARD 11)

find_package(catkin REQUIRED)

catkin_package(
        INCLUDE_DIRS
        #  LIBRARIES srg-base
        CATKIN_DEPENDS
        #  DEPENDS system_lib
)

include_directories(
        ${catkin_INCLUDE_DIRS}
)

add_executable(external_eval main.cpp)

target_link_libraries(${PROJECT_NAME}
        ${catkin_LIBRARIES}
        clingo
        )