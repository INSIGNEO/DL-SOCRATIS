set (ITK_DIR /home/michail/software/ITK-5.0a01/build)
set (VTK_DIR /home/michail/software/VTK-8.1.1/build)

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )


add_library(Clustering bin/kmeans.cxx)

target_link_libraries( Clustering ${ITK_LIBRARIES} )

install (TARGETS Clustering DESTINATION bin)

install (FILES kmeans.h DESTINATION include)
