cmake_minimum_required(VERSION 2.8.3)
project(alica_asp_test)
    
## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
	set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
	set(CMAKE_ECLIPSE_VERSION "4.5" CACHE STRING "Eclipse version" FORCE)
	add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")


find_package(catkin REQUIRED clingo cmake_modules alica_engine alica_dummy_proxy asp_commons asp_solver_wrapper asp_solver)

catkin_package(
  INCLUDE_DIRS src/etc/Expr/include
  CATKIN_DEPENDS cmake_modules alica_engine alica_dummy_proxy asp_commons asp_solver_wrapper clingo asp_solver
)

include_directories(src/etc/Expr/include  ${catkin_INCLUDE_DIRS})

file(GLOB_RECURSE alica_asp-test_SOURCES "src/*.cpp" "src/etc/Expr/src/*.cpp")
catkin_add_gtest(alica_asp-test 
  ${alica_asp-test_SOURCES}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

target_link_libraries(alica_asp-test ${catkin_LIBRARIES})
