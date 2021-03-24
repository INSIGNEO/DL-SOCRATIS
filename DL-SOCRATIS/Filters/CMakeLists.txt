set (ITK_DIR /home/michail/software/ITK-5.0a01/build)
set (VTK_DIR /home/michail/software/VTK-8.1.1/build)
find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
include(${VTK_USE_FILE})


add_library(Filters bin/denoising.cxx bin/Thresholding.cxx  bin/Histogram.cxx bin/Grayscale.cxx bin/HistogramGaussianRician.cxx  bin/HistogramXYPlot.cxx bin/Sobel.cxx bin/ConnectCombonentFilter.cxx bin/Median_Filter.cxx bin/mergeimages.cxx bin/remove_image_object.cxx bin/GradientRecursiveGaussian.cxx bin/BinaryImage.cxx bin/Change_information.cxx )

target_link_libraries( Filters IO ${ITK_LIBRARIES} ${VTK_LIBRARIES}  )

install (TARGETS Filters DESTINATION bin)

install (FILES Thresholding.h denoising.h Histogram.h Grayscale.h HistogramGaussianRician.h HistogramXYPlot.h Sobel.h ConnectCombonentFilter.h Median_Filter.h mergeimages.h remove_image_object.h GradientRecursiveGaussian.h BinaryImage.h Change_information.h  DESTINATION include )
