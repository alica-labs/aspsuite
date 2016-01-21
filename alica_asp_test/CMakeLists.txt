cmake_minimum_required(VERSION 2.8.3)
project(alica_asp_test)

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
	set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
	set(CMAKE_ECLIPSE_VERSION "4.5" CACHE STRING "Eclipse version" FORCE)
	add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED clingo_cpp cmake_modules alica_engine alica_dummy_proxy alica_asp_solver)


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
  INCLUDE_DIRS src/etc/Expr/include
#  LIBRARIES alica_asp_test
  CATKIN_DEPENDS cmake_modules alica_engine alica_dummy_proxy alica_asp_solver clingo_cpp
  #DEPENDS
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations


include_directories(src/etc/Expr/include  ${catkin_INCLUDE_DIRS})

file(GLOB_RECURSE alica_asp-test_SOURCES "src/*.cpp" "src/etc/Expr/src/*.cpp")
catkin_add_gtest(alica_asp-test 
  ${alica_asp-test_SOURCES}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)


message("[alica_asp_test] catkin_LIBRARIES: ${catkin_LIBRARIES}")
target_link_libraries(alica_asp-test ${catkin_LIBRARIES})

add_dependencies(alica_asp-test ${catkin_LIBRARIES})
