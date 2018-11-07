cmake_minimum_required(VERSION 2.8.3)
project(capnzero)

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11  ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
        set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
        set(CMAKE_ECLIPSE_VERSION "4.7" CACHE STRING "Eclipse version" FORCE)
        add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")

# Define where to find modules for ZeroMQ and UUID
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake CACHE INTERNAL "" FORCE)

# this enables the draft part of the api
add_definitions(-DZMQ_BUILD_DRAFT_API=1)
find_package(ZeroMQ)

# Introduce CapnProto
#find_package(CapnProto REQUIRED)

find_package(catkin QUIET)
if(catkin_FOUND)
  catkin_package(
    INCLUDE_DIRS include
    LIBRARIES capnzero
    DEPENDS ZeroMQ
  )
endif(catkin_FOUND)

include_directories(
  include 
  ${ZeroMQ_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
  src/Publisher.cpp
  src/Subscriber.cpp
)

target_link_libraries(${PROJECT_NAME} 
  ${ZeroMQ_LIBRARIES}
)

if (NOT catkin_FOUND)
 target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
endif (NOT catkin_FOUND)
