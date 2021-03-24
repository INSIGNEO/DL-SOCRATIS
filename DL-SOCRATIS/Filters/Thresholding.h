////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef Thresholding_h
#define Thresholding_h

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkThresholdImageFilter.h"
#include "itkImage.h"

class Thresholding
{
public:
const char * m_outputImage_THR;

Thresholding(){};
~Thresholding(){};

void Image_based_Thresholding(const char * m_inputFileName, const char * m_outputFileName, float lowerThreshold, float upperThreshold);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_THR;}
};

#endif
