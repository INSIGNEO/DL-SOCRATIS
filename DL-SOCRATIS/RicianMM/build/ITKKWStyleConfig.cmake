set(ITK_SOURCE_DIR "")
set(CMAKE_SOURCE_DIR "/home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM")
set(KWSTYLE_EXECUTABLE "/home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/build/KWStyle-build/KWStyle")

set(WORKING_DIR "")
if(ITK_SOURCE_DIR)
  set(WORKING_DIR "${ITK_SOURCE_DIR}")
else()
  set(WORKING_DIR "${CMAKE_SOURCE_DIR}")
endif()

find_package( Git )
if(GIT_FOUND AND EXISTS "${ITK_SOURCE_DIR}/.git/config")
  execute_process( COMMAND ${GIT_EXECUTABLE} config hooks.KWStyle.path
    "${KWSTYLE_EXECUTABLE}"
    WORKING_DIRECTORY ${WORKING_DIR} )
endif()
