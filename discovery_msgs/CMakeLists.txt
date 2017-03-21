cmake_minimum_required(VERSION 2.8.3)
project(discovery_msgs)

add_definitions(-std=c++11)

# Define where to find macros for generating capnproto 
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake CACHE INTERNAL "" FORCE)
find_package(CapnProto REQUIRED)

find_package(catkin REQUIRED)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES discovery_msgs
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

# make src a include directory, because everything is generated into the src folder
include_directories(src)

## Autogenerate the c++ capnproto files

set(capnp_message_folder ${CMAKE_CURRENT_SOURCE_DIR}/msg)
file(GLOB_RECURSE capnp_messages ${capnp_message_folder} *.capnp)

set(CAPNPC_SRC_PREFIX ${capnp_message_folder})
set(CAPNPC_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS ${capnp_messages})
add_custom_target(${PROJECT_NAME}_generate_messages_cpp ALL
  COMMENT "${PROJECT_NAME}_generate_messages_cpp: Triggering generation."
  SOURCES ${CAPNP_SRCS} ${CAPNP_HDRS}
)


## Some debug output

message("${PROJECT_NAME}: CAPNP_SRCS: ${CAPNP_SRCS}")
message("${PROJECT_NAME}: CAPNP_HDRS: ${CAPNP_HDRS}")
message("${PROJECT_NAME}: CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message("${PROJECT_NAME}: CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")


## Declare the C++ capnproto library

add_library(${PROJECT_NAME}
  ${CAPNP_SRCS}
)

add_dependencies(${PROJECT_NAME} capnproto_cmake_macros)
