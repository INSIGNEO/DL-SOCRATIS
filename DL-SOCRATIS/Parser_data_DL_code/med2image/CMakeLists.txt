cmake_minimum_required(VERSION 3.10.2)
project (parser)
set(CMAKE_CPP_COMPILER gcc) #nvcc)#gcc)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
SET(GCC_COVERAGE_LINK_FLAGS    "-lboost_system" "-lboost_filesystem")
# The version number.
set (TestPipeline_VERSION_MAJOR 1)
set (TestPipeline_VERSION_MINOR 0)
set (ITK_DIR /home/michail/software/ITK-5.0a01/build)
set (VTK_DIR /home/michail/software/VTK-8.1.1/build)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})


#find ITK path of build
find_package(ITK REQUIRED)
include( ${ITK_USE_FILE} )
#find VTK path of build 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(parser parser.cpp)

target_link_libraries( parser ${ITK_LIBRARIES}  ${VTK_LIBRARIES} ${ITKCommon_LIBRARIES} ${GCC_COVERAGE_LINK_FLAGS})

install (TARGETS parser DESTINATION /)
