cmake_minimum_required(VERSION 3.10.2)
project (auto_segm)
set(CMAKE_CPP_COMPILER gcc) #nvcc)#gcc)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# The version number.
set (TestPipeline_VERSION_MAJOR 1)
set (TestPipeline_VERSION_MINOR 0)
set (ITK_DIR /home/michail/software/ITK-5.0a01/build2)
set (VTK_DIR /home/michail/software/VTK-8.1.1/build)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})


#find ITK path of build
find_package(ITK REQUIRED)
include( ${ITK_USE_FILE} )
#find VTK path of build 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
# This module defines
# LIBCONFIG_INCLUDE_DIR, where to find cppunit include files, etc.
# LIBCONFIG_LIBRARIES, the libraries to link against to use CppUnit.
# LIBCONFIG_STATIC_LIBRARIY_PATH
# LIBCONFIG_FOUND, If false, do not try to use CppUnit.

# also defined, but not for general use are
# LIBCONFIG_LIBRARY, where to find the CUnit library.

#MESSAGE("Searching for libconfig library")

FIND_PATH(LIBCONFIG_INCLUDE_DIR libconfig.h
  /usr/local/include
  /usr/include
)

FIND_PATH(LIBCONFIGPP_INCLUDE_DIR libconfig.h++
  /usr/local/include
  /usr/include
)

FIND_LIBRARY(LIBCONFIG_LIBRARY config
  /usr/local/lib
  /usr/lib
)

FIND_LIBRARY(LIBCONFIGPP_LIBRARY config++
  /usr/local/lib
  /usr/lib
)

FIND_LIBRARY(LIBCONFIG_STATIC_LIBRARY "libconfig${CMAKE_STATIC_LIBRARY_SUFFIX}"
  /usr/local/lib
  /usr/lib
)

FIND_LIBRARY(LIBCONFIGPP_STATIC_LIBRARY "libconfig++${CMAKE_STATIC_LIBRARY_SUFFIX}"
    /usr/local/lib
    /usr/lib
)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
IF(OPENMP_INCLUDE_DIR)
  IF(OPENMP_LIBRARY)
    SET(OPENMP_FOUND TRUE)
    SET(OPENMP_LIBRARIES ${OPENMP_LIBRARY})
    SET(OPENMP_STATIC_LIBRARY_PATH ${OPENMP_STATIC_LIBRARY})
  ENDIF(OPENMP_LIBRARY)
ENDIF(OPENMP_INCLUDE_DIR)



IF(LIBCONFIG_INCLUDE_DIR)
  IF(LIBCONFIG_LIBRARY)
    SET(LIBCONFIG_FOUND TRUE)
    SET(LIBCONFIG_LIBRARIES ${LIBCONFIG_LIBRARY})
    SET(LIBCONFIG_STATIC_LIBRARY_PATH ${LIBCONFIG_STATIC_LIBRARY})
  ENDIF(LIBCONFIG_LIBRARY)
ENDIF(LIBCONFIG_INCLUDE_DIR)

IF(LIBCONFIGPP_INCLUDE_DIR)
  IF(LIBCONFIGPP_LIBRARY)
    SET(LIBCONFIGPP_FOUND TRUE)
    SET(LIBCONFIGPP_LIBRARIES ${LIBCONFIGPP_LIBRARY})
    SET(LIBCONFIGPP_STATIC_LIBRARY_PATH ${LIBCONFIGPP_STATIC_LIBRARY})
  ENDIF(LIBCONFIGPP_LIBRARY)
ENDIF(LIBCONFIGPP_INCLUDE_DIR)

IF (LIBCONFIG_FOUND)
   IF (NOT LibConfig_FIND_QUIETLY)
      MESSAGE(STATUS "Found LibConfig++: ${LIBCONFIGPP_LIBRARIES}" )
      MESSAGE(STATUS "Found LibConfig: ${LIBCONFIG_LIBRARIES}")
      MESSAGE(STATUS "static LibConfig path: ${LIBCONFIG_STATIC_LIBRARY_PATH}")
   ENDIF (NOT LibConfig_FIND_QUIETLY)
ELSE (LIBCONFIG_FOUND)
   IF (LibConfig_FIND_REQUIRED)
      MESSAGE(SEND_ERROR "Could NOT find LibConfig")
   ENDIF (LibConfig_FIND_REQUIRED)
ENDIF (LIBCONFIG_FOUND)

MARK_AS_ADVANCED(LIBCONFIG_INCLUDE_DIR LIBCONFIG_LIBRARIES)




if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
else()
  find_package(ITKVtkGlue REQUIRED)
  include(${ITKVtkGlue_USE_FILE})
  set(Glue ITKVtkGlue)
endif()

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/TestPipelineConfig.h.in"
  "${PROJECT_BINARY_DIR}/TestPipelineConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# add subdirectories 
include_directories ("${PROJECT_SOURCE_DIR}/Filters")
include_directories ("${PROJECT_SOURCE_DIR}/Clustering")
include_directories ("${PROJECT_SOURCE_DIR}/Segmentation")
include_directories ("${PROJECT_SOURCE_DIR}/IO")
include_directories ("${PROJECT_SOURCE_DIR}/test")
include_directories ("${PROJECT_SOURCE_DIR}/core")
include_directories ("${PROJECT_SOURCE_DIR}/Deformation")

add_subdirectory (Filters)
add_subdirectory (Clustering) 
add_subdirectory (Segmentation)  
add_subdirectory (IO) 
add_subdirectory (test)
add_subdirectory (core)
add_subdirectory (Deformation)

# add the executable
add_executable(auto_segm auto_segm.cxx) #Evaluation.cxx Preprossesing.cxx)

#link the libraries
target_link_libraries( auto_segm ${ITK_LIBRARIES} ${Glue}  ${VTK_LIBRARIES} ${ITKCommon_LIBRARIES} ${PYTHON_LIBRARIES})
target_link_libraries (auto_segm Filters)
target_link_libraries (auto_segm Clustering)
target_link_libraries (auto_segm Segmentation)
target_link_libraries (auto_segm IO)
target_link_libraries (auto_segm core)
target_link_libraries (auto_segm Deformation)

target_link_libraries (auto_segm ${LIBCONFIGPP_LIBRARY})
target_link_libraries (auto_segm ${OPENMP_LIBRARY})
#install 

install (TARGETS auto_segm DESTINATION /)
install (FILES  TestPipelineConfig.h DESTINATION Filters/header)
install (FILES  TestPipelineConfig.h DESTINATION Clustering/header)
install (FILES  TestPipelineConfig.h DESTINATION Segmentation/header)
install (FILES  TestPipelineConfig.h DESTINATION IO/header)
install (FILES  TestPipelineConfig.h DESTINATION test/header)
install (FILES  TestPipelineConfig.h DESTINATION core/header)
install (FILES  TestPipelineConfig.h DESTINATION Deformation/header)
# add test so the correct  use of code is verified


#install the auto_segm software python

add_executable(Install_Python_project ${CMAKE_SOURCE_DIR}/install_python_software.cpp)

set (cmd3 "ls")
set (arg "./Install_Python_project")
set (CMAKE_SOURCE ${CMAKE_SOURCE_DIR}/build/)

message(STATUS "git cmd: ${arg}")

execute_process(COMMAND ${cmd3} 
  WORKING_DIRECTORY ${CMAKE_SOURCE}
  RESULT_VARIABLE git_result
  OUTPUT_VARIABLE git_ver)
message(STATUS "git ver[${git_result}]: ${git_ver}")

execute_process(COMMAND ${arg} 
  WORKING_DIRECTORY ${CMAKE_SOURCE}
  RESULT_VARIABLE exe_result
  OUTPUT_VARIABLE exe_ver)

message(STATUS "exe ver[${exe_result}]: ${exe_ver}")







