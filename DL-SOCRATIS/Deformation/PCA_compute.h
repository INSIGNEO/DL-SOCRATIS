////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef PCA_compute_h
#define PCA_compute_h

#include <stdlib.h> 
#include <string.h> 
#include <Python.h>
#include <iostream>
#include "mergeimages.h"
#include <memory>  
#include <algorithm>

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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"
#include "itkListSample.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"

//#include "denoising.h"
//#include "Sobel.h"
//#include "BinaryImage.h"
#include "mergeimages.h"
//#include "Initial_Segmentation.h"
#include "deformable_registration.h"
//#include "kmeans.h"
//#include "Region_grown.h"
//#include "boundarysegm.h"
#include "interparameters.h"
#include "unbiased_ground_trouth.h"

#include <itkBinaryShapeOpeningImageFilter.h>
#include "itkImagePCAShapeModelEstimator.h"
#include "itkMultiplyImageFilter.h"
#include "itkPCAShapeSignedDistanceFunction.h"
#include "Segmentation_Pipeline.h"

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


class PCA_compute
{
public:


const char * m_outputImage_pca;

PCA_compute(std::string m_root){root=m_root;};
~PCA_compute(){};

void compute_PCA(const char *inputfolder, int number_train, int modes_train);

void compute_3DPCA(std::string inputpatientroot, std::vector<string> patient_folders);

//Set Get functions
inline const char * GetOutput(){return m_outputImage_pca;}
inline void SetMax_images_number(int max_capacity){max=max_capacity;}
inline void SetMax_images_location(int  max_images_location){max_location=max_images_location;}
private:
std::string root="./1/";
using PixelType = unsigned char;
using MeasurementVectorType = itk::Vector< float, 2 >;
using MeasurementVectorType2 = itk::Vector< int, 2 >;
using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;
using ImageType = itk::Image< PixelType, 2 >;
using OutputImageType = itk::Image< float, 2 >;

void path_create(std::string store_data_path);
vector<string> folders_name;
vector<int> patient_number;
std::string outED;
std::string patient_store_path;
void create_max_length_image_number( std::vector<string> images,int patient_number );
int max=max_register_capacity;
int max_location=0;

};

#endif
