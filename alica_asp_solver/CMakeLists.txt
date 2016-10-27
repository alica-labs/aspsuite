cmake_minimum_required(VERSION 2.8.3)
project(alica_asp_solver)
        
## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
	set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
	set(CMAKE_ECLIPSE_VERSION "4.5" CACHE STRING "Eclipse version" FORCE)
	add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")

find_package(catkin REQUIRED COMPONENTS
  alica_engine 
  autodiff
  clingo_cpp
  system_config
  system_util
  fsystem
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES alica_asp_solver
  CATKIN_DEPENDS alica_engine clingo_cpp system_config fsystem autodiff
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
    src/ASPAlicaPlanIntegrator.cpp
    src/ASPGenerator.cpp
    src/AspQuery.cpp
    src/Variable.cpp
    src/Term.cpp
    src/AnnotatedExternal.cpp
)

target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})
