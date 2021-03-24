////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef Region_grown_h
#define Region_grown_h
/*
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

*/
#include <bits/stdc++.h> 
#include <stdio.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <exception>
#include <fstream>
#include <sstream>
 
#include <dirent.h>
#include <array>
#include <iomanip>
#include <vector>
#include <boost/filesystem.hpp>

#include <stdlib.h> 
#include <string.h> 
#include <iostream>
#include <memory>  
#include <algorithm>

#include "itkKLMRegionGrowImageFilter.h"
#include "itkScalarImageToHistogramGenerator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkRegionGrowImageFilter.h"
#include "itkImage.h"
#include "itkVector.h"


#include "itkVectorConfidenceConnectedImageFilter.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkNeighborhoodConnectedImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkImage.h"
#include "itkCastImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkRGBPixel.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

class Region_grown
{
public:

using MeasurementVectorType = itk::Vector< float, 2 >;
using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;

const char * m_outputImage_RG;

Region_grown(){};
~Region_grown(){};

void Connected_threshold(const char * m_inputFileName,const char * m_outputFileName, float low, float high, int seed[2]);
void Neighboor_Connected_threshold(const char * m_inputFileName,const char * m_outputFileName, float low, float high, int seed[2], int rad[2]);
void Confident_Connected(const char * m_inputFileName, const char * m_outputFileName, int seed[2], float multiplier, int radius);
void Vector_Confident_Connected(const char * m_inputFileName, const char * m_outputFileName, float low, float high, int seed[2], float multiplier, int radius);

void KLMRGFilterType(const char * m_inputFileName,const char * m_outputFileName, std::vector<int> seedpoints);
SampleType::Pointer determine_gridmask(const char * m_inputFileName, const char * m_outputFileName, double TH[2]);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_RG;}
void compute_entropy_of_label();
MeasurementVectorType determine_nearest_point(SampleType::Pointer  map_points, int case_heap);

private:

typedef itk::Vector< double, 2 > THRES;
THRES THRS;

};

#endif
