#ifndef image_vision_h
#define image_vision_h


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRescaleIntensityImageFilter.h"
 
#include "QuickView.h"

class image_vision
{
public:

image_vision(){};
~image_vision(){};

void vision(const char * m_inputImageFileName, const char * text);

};

#endif
