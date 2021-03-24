set (ITK_DIR /home/michail/software/ITK-5.0a01/build)
set (VTK_DIR /home/michail/software/VTK-8.1.1/build)
find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
include(${VTK_USE_FILE})


add_library(Deformation bin/deformable_registration.cxx bin/strain_compute.cxx bin/PCA_compute.cxx)

target_link_libraries( Deformation IO ${ITK_LIBRARIES} ${VTK_LIBRARIES}  )

install (TARGETS Deformation DESTINATION bin)

install (FILES deformable_registration.h strain_compute.h  PCA_compute.h DESTINATION include )
