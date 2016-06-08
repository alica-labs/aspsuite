cmake_minimum_required(VERSION 2.8.3)
project(alica_asp_solver)
        
## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
	set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
	set(CMAKE_ECLIPSE_VERSION "4.5" CACHE STRING "Eclipse version" FORCE)
	add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages

find_package(catkin REQUIRED COMPONENTS
  alica_engine 
  clingo_cpp
  system_config
  system_util
  fsystem
)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES alica_asp_solver
  CATKIN_DEPENDS alica_engine clingo_cpp system_config fsystem
  #DEPENDS lua python
)

###########
## Build ##
###########
add_definitions(-DWITH_THREADS=1) # for libclasp...
add_definitions(-DWITH_LUA) # for libclasp...
add_definitions(-DWITH_PYTHON) # for libclasp...


## Specify additional locations of header files
## Your package locations should be listed before other locations

#message("[alica-asp-solver] catkin_INCLUDE_DIRS: ${catkin_INCLUDE_DIRS}")
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
    src/ASPSolver.cpp
    src/ASPAlicaPlanIntegrator.cpp
    src/ASPGenerator.cpp
    src/AspQuery.cpp
)

#message("[alica-asp-solver] catkin_LIBRARIES: ${catkin_LIBRARIES}")
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})
add_dependencies(${PROJECT_NAME} ${catkin_LIBRARIES})# clingo_shared program_opts_shared clasp_shared gringo_shared)
