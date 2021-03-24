////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef Watershed_h
#define Watershed_h

#include "itkRGBToLuminanceImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkVectorCastImageFilter.h"
#include "itkVectorGradientAnisotropicDiffusionImageFilter.h"
#include "itkWatershedImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include <itkMorphologicalWatershedFromMarkersImageFilter.h>
#include "itkCastImageFilter.h"

class Watershed
{
public:

const char * m_outputImage_SE;

Watershed(){};
~Watershed(){};

void WatershedFilterType(const char * m_inputFileName,const char * m_outputFileName, float threshold, float Level);
void SeedWatershedFilterType(const char * m_inputFileName,const char * m_marherimage,const char * m_outputFileName);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_SE;}

};

#endif
