#ifndef Grayscale_h
#define Grayscale_h

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRGBToLuminanceImageFilter.h"


class Grayscale
{
public:
const char * m_outputImage_GRA;

Grayscale(){};
~Grayscale(){};

void Image_Grayscale(const char * m_inputFileName, const char * m_outputFileName);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_GRA;}
};

#endif
