  cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
  cmake_policy(VERSION 3.10.2)


  project(ITKRicianMM)
  set (ITK_DIR /home/michail/software/ITK-5.0a01/build2)
  set(ITKRicianMM_LIBRARIES ITKRicianMM)

  if(NOT ITK_SOURCE_DIR)
    find_package(ITK REQUIRED)
    include( ${ITK_USE_FILE} )
    list(APPEND CMAKE_MODULE_PATH ${ITK_CMAKE_DIR})
   include(ITKModuleExternal)
 else()
    itk_module_impl()
  endif()

