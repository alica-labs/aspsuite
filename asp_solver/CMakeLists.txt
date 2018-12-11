cmake_minimum_required(VERSION 2.8.3)
project(asp_solver)

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
	set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
	set(CMAKE_ECLIPSE_VERSION "4.5" CACHE STRING "Eclipse version" FORCE)
	add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")

## CLINGO
set(clingo_DIR "/usr/local/lib")
set(clingo_INCLUDE_DIRS "/usr/local/include")

find_package(catkin REQUIRED COMPONENTS
  system_config
  system_util
  fsystem
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES asp_solver
  CATKIN_DEPENDS system_config fsystem system_util
  DEPENDS clingo
)

add_definitions(-DWITH_THREADS=1) # for libclasp...
add_definitions(-DWITH_LUA) # for libclasp...
add_definitions(-DWITH_PYTHON) # for libclasp...

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
	src/Solver.cpp
    src/AnnotatedExternal.cpp
	src/FilterQuery.cpp
	src/ExtensionQuery.cpp
    src/GrdProgramObserver.cpp
	src/Term.cpp
	src/Variable.cpp
	src/AnnotatedValVec.cpp
	src/Query.cpp
)

target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} clingo)
