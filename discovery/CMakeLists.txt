cmake_minimum_required(VERSION 2.8.3)
project(discovery)

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11  ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
        set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
        set(CMAKE_ECLIPSE_VERSION "4.5" CACHE STRING "Eclipse version" FORCE)
        add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")

# Define where to find modules for ZeroMQ
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules CACHE INTERNAL "" FORCE)
add_definitions(-DZMQ_BUILD_DRAFT_API=1)
find_package(ZeroMQ)
#message("#### ZMQ-Includes: ${ZeroMQ_INCLUDE_DIRS}") 
#message("#### ZMQ-Libraries: ${ZeroMQ_LIBRARIES}") 

# Catkin Stuff
find_package(catkin QUIET)
if (catkin_FOUND)
  find_package(catkin REQUIRED COMPONENTS)
else (catkin_FOUND)
  set(catkin_LIBRARIES)
endif(catkin_FOUND)
if(catkin_FOUND)
  catkin_package(
    INCLUDE_DIRS include
  # LIBRARIES 
  # CATKIN_DEPENDS
    DEPENDS ZeroMQ
  )
endif(catkin_FOUND)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(include ${catkin_INCLUDE_DIRS} ${ZeroMQ_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
  src/Discovery.cpp
)

target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} ${ZeroMQ_LIBRARIES})

if (NOT catkin_FOUND)
 target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
endif (NOT catkin_FOUND)

