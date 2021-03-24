set (ITK_DIR /home/michail/software/ITK-5.0a01/build)
set (VTK_DIR /home/michail/software/VTK-8.1.1/build)
find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
include(${VTK_USE_FILE})

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

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




add_library(core bin/Initial_Segmentation.cxx bin/Segmentation_Pipeline.cxx bin/Evaluation.cxx  bin/Interpolate_3D.cxx bin/Atlas_segm.cxx bin/Deformation.cxx bin/Sensitivity.cxx bin/Translation_space.cxx)

target_link_libraries( core IO Filters Clustering Segmentation ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${PYTHON_LIBRARIES} ${OPENMP_LIBRARY})

install (TARGETS core DESTINATION bin)

install (FILES Initial_Segmentation.h Segmentation_Pipeline.h Evaluation.h  Interpolate_3D.h  Atlas_segm.h Deformation.h Sensitivity.h Translation_space.h DESTINATION  include )
