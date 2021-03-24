set (ITK_DIR /home/michail/software/ITK-5.0a01/build)
set (VTK_DIR /home/michail/software/VTK-8.1.1/build)


find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

#find ITK path of build
find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
#find VTK path of build 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
else()
  find_package(ItkVtkGlue REQUIRED)
  include(${ItkVtkGlue_USE_FILE})
  set(Glue ItkVtkGlue)
endif()

add_library(IO bin/image_vision.cxx bin/LabelObject.cxx bin/Resample.cxx bin/threeDvtk2Dslice.cxx bin/evaluate.cxx bin/configuration.cxx bin/unbiased_ground_trouth.cxx)

target_link_libraries( IO ${Glue}  ${VTK_LIBRARIES} ${ITK_LIBRARIES} ${ITKCommon_LIBRARIES})

install (TARGETS IO DESTINATION bin)

install (FILES image_vision.h LabelObject.h Resample.h threeDvtk2Dslice.h evaluate.h read_writedata.h  configuration.h  interparameters.h unbiased_ground_trouth.h DESTINATION include)
