# CMake generated Testfile for 
# Source directory: /home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM
# Build directory: /home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(RicianMMKWStyleTest "/home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/build/KWStyle-build/KWStyle" "-xml" "/home/michail/software/ITK-5.0a01/CMake/../Utilities/KWStyle/ITK.kws.xml" "-v" "-o" "/home/michail/software/ITK-5.0a01/CMake/../Utilities/KWStyle/ITKOverwrite.txt" "-D" "/home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/build/ITKKWStyleFiles.txt" "-gcc")
set_tests_properties(RicianMMKWStyleTest PROPERTIES  LABELS "RicianMM" WORKING_DIRECTORY "/home/michail/software/ITK-5.0a01/CMake/..")
add_test(RicianMMInDoxygenGroup "/usr/bin/python" "/home/michail/software/ITK-5.0a01/CMake/../Utilities/Doxygen/mcdoc.py" "check" "RicianMM" "/home/michail/Desktop/PHD_INSIGNEO/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/RicianMM/include")
set_tests_properties(RicianMMInDoxygenGroup PROPERTIES  LABELS "RicianMM")
