# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image

# Include any dependencies generated for this target.
include CMakeFiles/parser.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/parser.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/parser.dir/flags.make

CMakeFiles/parser.dir/parser.cpp.o: CMakeFiles/parser.dir/flags.make
CMakeFiles/parser.dir/parser.cpp.o: parser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/parser.dir/parser.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/parser.dir/parser.cpp.o -c /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image/parser.cpp

CMakeFiles/parser.dir/parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parser.dir/parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image/parser.cpp > CMakeFiles/parser.dir/parser.cpp.i

CMakeFiles/parser.dir/parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parser.dir/parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image/parser.cpp -o CMakeFiles/parser.dir/parser.cpp.s

# Object files for target parser
parser_OBJECTS = \
"CMakeFiles/parser.dir/parser.cpp.o"

# External object files for target parser
parser_EXTERNAL_OBJECTS =

parser: CMakeFiles/parser.dir/parser.cpp.o
parser: CMakeFiles/parser.dir/build.make
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkdouble-conversion-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitksys-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkvnl_algo-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkvnl-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkv3p_netlib-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitknetlib-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkvcl-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKCommon-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOImageBase-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKEXPAT-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOXML-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkNetlibSlatec-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKStatistics-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKTransform-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKMesh-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkzlib-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKMetaIO-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKSpatialObjects-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOSpatialObjects-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKPath-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKQuadEdgeMesh-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKLabelMap-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKOptimizers-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKPolynomials-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKBiasCorrection-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKDICOMParser-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmDICT-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmMSFF-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKznz-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKniftiio-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKgiftiio-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkhdf5_cpp.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkhdf5.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOBMP-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOBioRad-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOBruker-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOCSV-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOGDCM-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOIPL-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOGE-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOGIPL-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOHDF5-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkjpeg-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOJPEG-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitktiff-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTIFF-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOLSM-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkminc2-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMINC-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMRC-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMeshBase-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMeshBYU-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMeshVTK-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMesh-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMeta-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIONIFTI-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKNrrdIO-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIONRRD-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkpng-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOPNG-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOSiemens-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOStimulate-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKTransformFactory-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTransformBase-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTransformHDF5-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTransformInsightLegacy-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTransformMatlab-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOVTK-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKKLMRegionGrowing-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitklbfgs-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKVTK-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKVtkGlue-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKOptimizersv4-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkopenjpeg-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKWatersheds-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKReview-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKVideoCore-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKVideoIO-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkIsotropicWavelets-5.0.a
parser: /usr/local/lib/libvtkDomainsChemistryOpenGL2-8.1.so.1
parser: /usr/local/lib/libvtkFiltersFlowPaths-8.1.so.1
parser: /usr/local/lib/libvtkFiltersGeneric-8.1.so.1
parser: /usr/local/lib/libvtkFiltersHyperTree-8.1.so.1
parser: /usr/local/lib/libvtkFiltersParallelImaging-8.1.so.1
parser: /usr/local/lib/libvtkFiltersPoints-8.1.so.1
parser: /usr/local/lib/libvtkFiltersProgrammable-8.1.so.1
parser: /usr/local/lib/libvtkFiltersSMP-8.1.so.1
parser: /usr/local/lib/libvtkFiltersSelection-8.1.so.1
parser: /usr/local/lib/libvtkFiltersTexture-8.1.so.1
parser: /usr/local/lib/libvtkFiltersTopology-8.1.so.1
parser: /usr/local/lib/libvtkFiltersVerdict-8.1.so.1
parser: /usr/local/lib/libvtkGeovisCore-8.1.so.1
parser: /usr/local/lib/libvtkIOAMR-8.1.so.1
parser: /usr/local/lib/libvtkIOEnSight-8.1.so.1
parser: /usr/local/lib/libvtkIOExodus-8.1.so.1
parser: /usr/local/lib/libvtkIOExportOpenGL2-8.1.so.1
parser: /usr/local/lib/libvtkIOImport-8.1.so.1
parser: /usr/local/lib/libvtkIOInfovis-8.1.so.1
parser: /usr/local/lib/libvtkIOLSDyna-8.1.so.1
parser: /usr/local/lib/libvtkIOMINC-8.1.so.1
parser: /usr/local/lib/libvtkIOMovie-8.1.so.1
parser: /usr/local/lib/libvtkIOPLY-8.1.so.1
parser: /usr/local/lib/libvtkIOParallel-8.1.so.1
parser: /usr/local/lib/libvtkIOParallelXML-8.1.so.1
parser: /usr/local/lib/libvtkIOSQL-8.1.so.1
parser: /usr/local/lib/libvtkIOTecplotTable-8.1.so.1
parser: /usr/local/lib/libvtkIOVideo-8.1.so.1
parser: /usr/local/lib/libvtkImagingMorphological-8.1.so.1
parser: /usr/local/lib/libvtkImagingStatistics-8.1.so.1
parser: /usr/local/lib/libvtkImagingStencil-8.1.so.1
parser: /usr/local/lib/libvtkInteractionImage-8.1.so.1
parser: /usr/local/lib/libvtkRenderingContextOpenGL2-8.1.so.1
parser: /usr/local/lib/libvtkRenderingImage-8.1.so.1
parser: /usr/local/lib/libvtkRenderingLOD-8.1.so.1
parser: /usr/local/lib/libvtkRenderingVolumeOpenGL2-8.1.so.1
parser: /usr/local/lib/libvtkViewsContext2D-8.1.so.1
parser: /usr/local/lib/libvtkViewsInfovis-8.1.so.1
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKCommon-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkminc2-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKgiftiio-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMeshBYU-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMeshVTK-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMeshBase-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitklbfgs-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOSpatialObjects-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOXML-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKQuadEdgeMesh-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKLabelMap-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKPolynomials-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKBiasCorrection-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOBMP-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOBioRad-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOGDCM-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmMSFF-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmDICT-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmIOD-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKEXPAT-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmDSED-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmCommon-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmjpeg8-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmjpeg12-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmjpeg16-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmopenjp2-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmcharls-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkgdcmuuid-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOGE-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOGIPL-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOJPEG-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTIFF-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitktiff-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkjpeg-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOMeta-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKMetaIO-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIONIFTI-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKniftiio-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKznz-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIONRRD-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKNrrdIO-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOPNG-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkpng-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOSiemens-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOIPL-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOStimulate-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTransformHDF5-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkhdf5_cpp.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkhdf5.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkzlib-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTransformInsightLegacy-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTransformMatlab-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOTransformBase-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKTransformFactory-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOVTK-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKKLMRegionGrowing-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKVTK-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkopenjpeg-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKWatersheds-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKIOImageBase-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKVideoCore-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKSpatialObjects-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKMesh-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKTransform-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKPath-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKOptimizers-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKStatistics-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKCommon-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkdouble-conversion-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitksys-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libITKVNLInstantiation-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkvnl_algo-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkvnl-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitknetlib-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkvcl-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkNetlibSlatec-5.0.a
parser: /home/michail/software/ITK-5.0a01/build/lib/libitkv3p_netlib-5.0.a
parser: /usr/local/lib/libvtkDomainsChemistry-8.1.so.1
parser: /usr/local/lib/libvtkverdict-8.1.so.1
parser: /usr/local/lib/libvtkproj4-8.1.so.1
parser: /usr/local/lib/libvtkFiltersAMR-8.1.so.1
parser: /usr/local/lib/libvtkIOExport-8.1.so.1
parser: /usr/local/lib/libvtkRenderingGL2PSOpenGL2-8.1.so.1
parser: /usr/local/lib/libvtkgl2ps-8.1.so.1
parser: /usr/local/lib/libvtklibharu-8.1.so.1
parser: /usr/local/lib/libvtklibxml2-8.1.so.1
parser: /usr/local/lib/libvtkoggtheora-8.1.so.1
parser: /usr/local/lib/libvtkFiltersParallel-8.1.so.1
parser: /usr/local/lib/libvtkexoIIc-8.1.so.1
parser: /usr/local/lib/libvtkIOGeometry-8.1.so.1
parser: /usr/local/lib/libvtkIONetCDF-8.1.so.1
parser: /usr/local/lib/libvtknetcdfcpp-8.1.so.1
parser: /usr/local/lib/libvtkNetCDF-8.1.so.1
parser: /usr/local/lib/libvtkhdf5_hl-8.1.so.1
parser: /usr/local/lib/libvtkhdf5-8.1.so.1
parser: /usr/local/lib/libvtkjsoncpp-8.1.so.1
parser: /usr/local/lib/libvtkParallelCore-8.1.so.1
parser: /usr/local/lib/libvtkIOLegacy-8.1.so.1
parser: /usr/local/lib/libvtksqlite-8.1.so.1
parser: /usr/local/lib/libvtkRenderingOpenGL2-8.1.so.1
parser: /usr/lib/x86_64-linux-gnu/libSM.so
parser: /usr/lib/x86_64-linux-gnu/libICE.so
parser: /usr/lib/x86_64-linux-gnu/libX11.so
parser: /usr/lib/x86_64-linux-gnu/libXext.so
parser: /usr/lib/x86_64-linux-gnu/libXt.so
parser: /usr/local/lib/libvtkglew-8.1.so.1
parser: /usr/local/lib/libvtkImagingMath-8.1.so.1
parser: /usr/local/lib/libvtkChartsCore-8.1.so.1
parser: /usr/local/lib/libvtkRenderingContext2D-8.1.so.1
parser: /usr/local/lib/libvtkFiltersImaging-8.1.so.1
parser: /usr/local/lib/libvtkInfovisLayout-8.1.so.1
parser: /usr/local/lib/libvtkInfovisCore-8.1.so.1
parser: /usr/local/lib/libvtkViewsCore-8.1.so.1
parser: /usr/local/lib/libvtkInteractionWidgets-8.1.so.1
parser: /usr/local/lib/libvtkRenderingAnnotation-8.1.so.1
parser: /usr/local/lib/libvtkImagingColor-8.1.so.1
parser: /usr/local/lib/libvtkInteractionStyle-8.1.so.1
parser: /usr/local/lib/libvtkFiltersExtraction-8.1.so.1
parser: /usr/local/lib/libvtkFiltersStatistics-8.1.so.1
parser: /usr/local/lib/libvtkImagingFourier-8.1.so.1
parser: /usr/local/lib/libvtkalglib-8.1.so.1
parser: /usr/local/lib/libvtkFiltersHybrid-8.1.so.1
parser: /usr/local/lib/libvtkImagingGeneral-8.1.so.1
parser: /usr/local/lib/libvtkImagingSources-8.1.so.1
parser: /usr/local/lib/libvtkFiltersModeling-8.1.so.1
parser: /usr/local/lib/libvtkImagingHybrid-8.1.so.1
parser: /usr/local/lib/libvtkIOImage-8.1.so.1
parser: /usr/local/lib/libvtkDICOMParser-8.1.so.1
parser: /usr/local/lib/libvtkmetaio-8.1.so.1
parser: /usr/local/lib/libvtkpng-8.1.so.1
parser: /usr/local/lib/libvtktiff-8.1.so.1
parser: /usr/local/lib/libvtkjpeg-8.1.so.1
parser: /usr/lib/x86_64-linux-gnu/libm.so
parser: /usr/local/lib/libvtkRenderingVolume-8.1.so.1
parser: /usr/local/lib/libvtkImagingCore-8.1.so.1
parser: /usr/local/lib/libvtkIOXML-8.1.so.1
parser: /usr/local/lib/libvtkIOXMLParser-8.1.so.1
parser: /usr/local/lib/libvtkIOCore-8.1.so.1
parser: /usr/local/lib/libvtklz4-8.1.so.1
parser: /usr/local/lib/libvtkexpat-8.1.so.1
parser: /usr/local/lib/libvtkRenderingLabel-8.1.so.1
parser: /usr/local/lib/libvtkRenderingFreeType-8.1.so.1
parser: /usr/local/lib/libvtkRenderingCore-8.1.so.1
parser: /usr/local/lib/libvtkFiltersSources-8.1.so.1
parser: /usr/local/lib/libvtkFiltersGeneral-8.1.so.1
parser: /usr/local/lib/libvtkCommonComputationalGeometry-8.1.so.1
parser: /usr/local/lib/libvtkCommonColor-8.1.so.1
parser: /usr/local/lib/libvtkFiltersGeometry-8.1.so.1
parser: /usr/local/lib/libvtkFiltersCore-8.1.so.1
parser: /usr/local/lib/libvtkCommonExecutionModel-8.1.so.1
parser: /usr/local/lib/libvtkCommonDataModel-8.1.so.1
parser: /usr/local/lib/libvtkCommonTransforms-8.1.so.1
parser: /usr/local/lib/libvtkCommonMisc-8.1.so.1
parser: /usr/local/lib/libvtkCommonMath-8.1.so.1
parser: /usr/local/lib/libvtkCommonSystem-8.1.so.1
parser: /usr/local/lib/libvtkCommonCore-8.1.so.1
parser: /usr/local/lib/libvtksys-8.1.so.1
parser: /usr/local/lib/libvtkfreetype-8.1.so.1
parser: /usr/local/lib/libvtkzlib-8.1.so.1
parser: CMakeFiles/parser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable parser"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/parser.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/parser.dir/build: parser

.PHONY : CMakeFiles/parser.dir/build

CMakeFiles/parser.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/parser.dir/cmake_clean.cmake
.PHONY : CMakeFiles/parser.dir/clean

CMakeFiles/parser.dir/depend:
	cd /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image /home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE_generalization/Parser_data_DL_code/med2image/CMakeFiles/parser.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/parser.dir/depend
