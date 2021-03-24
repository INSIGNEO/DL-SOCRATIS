

set(command "/usr/local/bin/cmake;-P;/home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/build/KWStyle-prefix/tmp/KWStyle-gitclone.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/build/KWStyle-prefix/src/KWStyle-stamp/KWStyle-download-out.log"
  ERROR_FILE "/home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/build/KWStyle-prefix/src/KWStyle-stamp/KWStyle-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/build/KWStyle-prefix/src/KWStyle-stamp/KWStyle-download-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "KWStyle download command succeeded.  See also /home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/build/KWStyle-prefix/src/KWStyle-stamp/KWStyle-download-*.log")
  message(STATUS "${msg}")
endif()
