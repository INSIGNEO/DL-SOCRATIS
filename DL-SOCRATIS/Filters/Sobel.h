////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef Sobel_h
#define Sobel_h

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSobelEdgeDetectionImageFilter.h"
 
#include "itksys/SystemTools.hxx"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include "QuickView.h"
class Sobel
{
public:
const char * m_outputImage_Sobel;

Sobel(){};
~Sobel(){};

void Image_Sobel(const char * m_inputFileName, const char * m_outputFileName);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_Sobel;}
};

#endif
