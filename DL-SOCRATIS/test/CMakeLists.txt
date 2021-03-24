set (ITK_DIR /home/michail/software/ITK-5.0a01/build)
set (VTK_DIR /home/michail/software/VTK-8.1.1/build)

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
include(${VTK_USE_FILE})

#add_library(test )

target_link_libraries( Filters )

#install (TARGETS test DESTINATION bin)

install (FILES RGhistogram_test.h DESTINATION include)
