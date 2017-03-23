cmake_minimum_required(VERSION 2.8.3)
project(discovery_msgs)

add_definitions(-std=c++11)

find_package(catkin REQUIRED)

find_package(CapnProto)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES discovery_msgs
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

# make src an include directory, because everything is generated into the src folder
include_directories(src)
 
## Autogenerate the c++ capnproto files
set(capnp_message_folder "${CMAKE_CURRENT_SOURCE_DIR}/msg")
file(GLOB_RECURSE capnp_messages ${capnp_message_folder} *.capnp)

set(CAPNPC_SRC_PREFIX msg)
set(CAPNPC_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS ${capnp_messages})

## Declare the C++ capnproto message library
add_library(${PROJECT_NAME}
  ${CAPNP_SRCS}
)
