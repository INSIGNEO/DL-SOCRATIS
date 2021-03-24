////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef GradientRecursiveGaussian_h
#define GradientRecursiveGaussian_h


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGradientRecursiveGaussianImageFilter.h"


class GradientRecursiveGaussian
{
public:
const char * m_outputImage_GRG;

GradientRecursiveGaussian(){};
~GradientRecursiveGaussian(){};

void Image_GRG(const char * m_inputFileName, const char * m_outputFileName, double sig);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_GRG;}
};

#endif
