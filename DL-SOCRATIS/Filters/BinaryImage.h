////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef BinaryImage_h
#define BinaryImage_h

#include "itkImage.h"
#include "itkBinaryThresholdImageFilter.h"
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

class BinaryImage
{
public:
const char * m_outputImage_BTHR;

BinaryImage(){};
~BinaryImage(){};

void Image_binary(const char * m_inputFileName, const char * m_outputFileName, int lowerThreshold, int upperThreshold);
void Image_binary_intensity_handle(const char * m_inputFileName, const char * m_outputFileName, int lowerThreshold, int upperThreshold, int value_forward, int value_backward);

//Set Get functions
inline const char * GetOutput(){return m_outputImage_BTHR;}
};

#endif
