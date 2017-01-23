cmake_minimum_required(VERSION 2.8.3)
project(concept_net_gui)

find_package(catkin REQUIRED qt_gui system_config)

find_package(Qt5Core REQUIRED)
get_target_property(Qt5Core_location Qt5::Core LOCATION)
find_package(Qt5Gui REQUIRED)
get_target_property(Qt5Gui_location Qt5::Gui LOCATION)
find_package(Qt5Network REQUIRED)
get_target_property(Qt5Network_location Qt5::Network LOCATION)
find_package(Qt5Widgets REQUIRED)
get_target_property(Qt5Widgets_location Qt5::Widgets LOCATION)
find_package(Qt5WebKitWidgets REQUIRED)
get_target_property(Qt5WebKitWidgets_location Qt5::WebKitWidgets LOCATION)
SET( QT_USE_QTXML TRUE )
SET( QT_WRAP_CPP TRUE )

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
	set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
	set(CMAKE_ECLIPSE_VERSION "4.5" CACHE STRING "Eclipse version" FORCE)
	add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")

set(conceptNetGui_SRCS
  src/main.cpp
  src/gui/ConceptNetGui.cpp
  src/gui/SettingsDialog.cpp
  src/containers/SolverSettings.cpp
  src/commands/NewSolverCommand.cpp
  src/commands/ChangeSolverSettingsCommand.cpp
  src/commands/ConceptNetQueryCommand.cpp
  src/commands/FactsQueryCommand.cpp
  src/commands/GroundCommand.cpp
  src/commands/LoadSavedProgramCommand.cpp
  src/commands/SolveCommand.cpp
  src/commands/VariableQueryCommand.cpp
  src/commands/LoadBackgroundKnowledgeCommand.cpp
  src/handler/SaveLoadHandler.cpp
  src/handler/CommandHistoryHandler.cpp
  
)

set(conceptNetGui_HDRS
  include/gui/ConceptNetGui.h
  include/gui/SettingsDialog.h
  include/containers/SolverSettings.h
  include/commands/Command.h
  include/commands/NewSolverCommand.h
  include/commands/ChangeSolverSettingsCommand.h
  include/commands/ConceptNetQueryCommand.h
  include/commands/FactsQueryCommand.h
  include/commands/GroundCommand.h
  include/commands/LoadSavedProgramCommand.h
  include/commands/SolveCommand.h
  include/commands/VariableQueryCommand.h
  include/commands/LoadBackgroundKnowledgeCommand.h
  include/handler/SaveLoadHandler.h
  include/handler/CommandHistoryHandler.h
)

set(conceptNetGui_UIS
  ui/conceptnetgui.ui
  ui/settingsdialog.ui
)

set(conceptNetGui_INCLUDE_DIRECTORIES
    include
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5WebKitWidgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

include_directories(${conceptNetGui_INCLUDE_DIRECTORIES} ${catkin_INCLUDE_DIRS})

catkin_package(
  INCLUDE_DIRS ${conceptNetGui_INCLUDE_DIRECTORIES}
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS qt_gui system_config
)

#QT5_ADD_RESOURCES(QT_RESOURCES_CPP ${QT_RESOURCES})
QT5_WRAP_CPP(conceptNetGui_MOCS ${conceptNetGui_HDRS})
QT5_WRAP_UI(conceptNetGui_UIS_H ${conceptNetGui_UIS})

#add_library(${PROJECT_NAME} ${conceptNetGui_SRCS} ${conceptNetGui_MOCS} ${conceptNetGui_UIS_H} ${QT_RESOURCES_CPP})
add_executable (ConceptNetGui ${conceptNetGui_SRCS} ${conceptNetGui_MOCS} ${conceptNetGui_UIS_H} ${QT_RESOURCES_CPP})
target_link_libraries(ConceptNetGui ${catkin_LIBRARIES} ${Qt5Widgets_location} ${Qt5Core_location} ${Qt5Gui_location} ${Qt5Network_location} ${Qt5WebKitWidgets_location})

install(TARGETS ConceptNetGui
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(DIRECTORY include/ConceptNetGui/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)
