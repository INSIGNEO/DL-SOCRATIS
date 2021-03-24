////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef prewatershed_h
#define prewatershed_h

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itksys/SystemTools.hxx"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include <iostream>

#include "itkVector.h"
#include "itkListSample.h"
//kd-tree
#include "itkVector.h"
#include "itkListSample.h"
#include "itkKdTree.h"
#include "itkKdTreeGenerator.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkEuclideanDistanceMetric.h"
#include "itkStatisticsAlgorithm.h"

class prewatershed
{
public:

prewatershed(){};
~prewatershed(){};

typedef itk::Vector< double, 1 > threshold;

void image_create(const char * inputFileName, const char * inputFileName2, const char * outputFileName, threshold TR);

private:


};

#endif
