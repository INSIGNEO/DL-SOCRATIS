set (ITK_DIR /home/michail/software/ITK-5.0a01/build)
set (VTK_DIR /home/michail/software/VTK-8.1.1/build)

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )


add_library(Segmentation bin/Watershed.cxx bin/estportion.cxx bin/seedpoints.cxx bin/prewatershed.cxx bin/boundarysegm.cxx bin/Translation_registration.cxx bin/rebuild_myo_mask.cxx bin/Region_grown.cxx)

target_link_libraries( Segmentation ${ITK_LIBRARIES} Filters Clustering IO -lboost_system -lboost_filesystem)

install (TARGETS Segmentation DESTINATION bin)

install (FILES Watershed.h estportion.h seedpoints.h prewatershed.h boundarysegm.h Translation_registration.h rebuild_myo_mask.h Region_grown.h DESTINATION include)
