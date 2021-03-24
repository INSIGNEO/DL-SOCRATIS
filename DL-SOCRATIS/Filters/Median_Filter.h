////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef Median_Filter_h
#define Median_Filter_h


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMedianImageFilter.h"

class Median_Filter
{
public:


Median_Filter(){};
~Median_Filter(){};

void Image_Median_Filter(const char * m_inputFileName, const char * m_outputFileName );

};

#endif
