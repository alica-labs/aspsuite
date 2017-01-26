cmake_minimum_required(VERSION 2.8.3)
project(asp_solver)

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
	set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
	set(CMAKE_ECLIPSE_VERSION "4.5" CACHE STRING "Eclipse version" FORCE)
	add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")

find_package(catkin REQUIRED COMPONENTS
  clingo_cpp
  system_config
  system_util
  fsystem
  asp_commons
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES asp_solver
  CATKIN_DEPENDS clingo_cpp system_config fsystem asp_commons
  #DEPENDS lua python
)

add_definitions(-DWITH_THREADS=1) # for libclasp...
add_definitions(-DWITH_LUA) # for libclasp...
add_definitions(-DWITH_PYTHON) # for libclasp...

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
    src/ASPSolver.cpp
    src/AnnotatedExternal.cpp
    src/ASPFactsQuery.cpp
    src/ASPVariableQuery.cpp
)

target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})

