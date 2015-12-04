cmake_minimum_required(VERSION 2.8.3)
project(alica_asp_test)

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall ${CMAKE_CXX_FLAGS}")

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED cling_wrap  cmake_modules alica_engine alica_dummy_proxy alica_asp_solver)


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
  CATKIN_DEPENDS cling_wrap cmake_modules alica_engine alica_dummy_proxy alica_asp_solver
  DEPENDS TBB
)

###########
## Build ##
###########
add_definitions(-DWITH_THREADS=1) # for libclasp...

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(src/etc/Expr/include  ${catkin_INCLUDE_DIRS})

file(GLOB_RECURSE alica_asp-test_SOURCES "src/*.cpp")
catkin_add_gtest(alica_asp-test 
  ${alica_asp-test_SOURCES}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

#message("<<< Catkin Libraries:  ${catkin_LIBRARIES}")
#message("<<< TBB Libraries:  ${TBB_LIBRARIES}")
find_package(TBB REQUIRED)

target_link_libraries(alica_asp-test ${catkin_LIBRARIES} ${TBB_LIBRARIES})
add_dependencies(alica_asp-test ${catkin_LIBRARIES})
