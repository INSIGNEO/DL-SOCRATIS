////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef estportion_h
#define estportion_h

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itksys/SystemTools.hxx"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include <iostream>

class estportion
{
public:
typedef itk::Vector< double, 1 > THRS;

const char * m_outputImage;

estportion(){};
~estportion(){};

void estportion_compute(const char * m_inputFileName, const char * m_outputFileName, THRS TG, THRS TR);
//Set Get functions
inline const char * GetOutput(){return m_outputImage;}

};

#endif
