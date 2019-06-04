cmake_minimum_required(VERSION 2.8.3)
project(knowledge_manager)

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
 
## Enable exception handling for segfaults using SigFault.h
set(CMAKE_CXX_FLAGS "-rdynamic -fnon-call-exceptions -ggdb ${CMAKE_CXX_FLAGS}")

find_package(catkin REQUIRED
	asp_solver
)

catkin_package(
	INCLUDE_DIRS include
	LIBRARIES ${PROJECT_NAME}
	CATKIN_DEPENDS asp_solver
)

include_directories(
	include
	${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
	src/ASPKnowledgeManager.cpp
)

target_link_libraries(${PROJECT_NAME}
	${catkin_LIBRARIES} 
)
