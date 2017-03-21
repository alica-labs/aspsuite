cmake_minimum_required(VERSION 2.8.3)
project(capnproto_cmake_macros)

find_package(catkin REQUIRED)

catkin_package()

set(macro_file Find${PROJECT_NAME}.cmake)
set(macro_file_absolute_source ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${macro_file})

set(destination_folder ${CATKIN_DEVEL_PREFIX}/lib/${PROJECT_NAME})
set(macro_file_absolute_destination ${destination_folder}/${macro_file}) 
 
add_custom_command(
    OUTPUT  ${macro_file_absolute_destination}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CATKIN_DEVEL_PREFIX}/lib
    COMMAND ${CMAKE_COMMAND} -E copy ${macro_file_absolute_source} ${destination_folder}}
    DEPENDS ${macro_file_absolute_source}
    COMMENT "${PROJECT_NAME}: Copying ${macro_file} into ${destination_folder} folder."
)

add_custom_target(${PROJECT_NAME} ALL 
    DEPENDS ${macro_file_absolute_destination}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "${PROJECT_NAME}: Copying ${macro_file}"
)

