cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
set(ROS_BUILD_TYPE Release)

rosbuild_init()
rosbuild_add_boost_directories()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

# BUT rviz display
set( DISPLAY_SOURCE_FILES src/arm_navigation_controls.cpp
                          src/arm_navigation_display.cpp
                          src/init.cpp
                          src/bb_estimation_controls.cpp
                          src/bb_estimation_display.cpp )
                          
set( BUT_DISPLAY_PROJECT_NAME but_assisted_plugins )

rosbuild_add_library( ${BUT_DISPLAY_PROJECT_NAME} ${DISPLAY_SOURCE_FILES} )

# Add wx widgets
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})
include_directories( ${wxWidgets_INCLUDE_DIRS} )

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)
#rosbuild_add_executable(example examples/example.cpp)
#target_link_libraries(example ${PROJECT_NAME})

rosbuild_add_roslaunch_check(launch)
