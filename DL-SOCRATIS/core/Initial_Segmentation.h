/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.2

//////////////////////////////////////////////////////////////////////////////////
#ifndef Initial_Segmentation_h
#define Initial_Segmentation_h

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

#include "Atlas_segm.h"
#include "BinaryImage.h"
#include "Segmentation_Pipeline.h"
#include "Region_grown.h"
#include "remove_image_object.h"
#include "Watershed.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <itkBinaryShapeOpeningImageFilter.h>
#include <itkInvertIntensityImageFilter.h>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
class Initial_Segmentation
{
public:
//Segmentation_Pipeline(const char * myoFolder,const char * ROIFolder,string SCARFolder,int nb_train );
Initial_Segmentation(std::string m_root="./1/"){root=m_root;};
~Initial_Segmentation(){};


void Run(std::string python_path,std::string conf_path, int num_classes, std::string m_heart_phase);
void Copy(std::string store_data_path, std::string store_myo_path,std::string store_roi_path, int classes);

inline void Set_patients(std::vector<std::string>  m_patients){patients=m_patients;};
inline void Set_images_per_patient(std::vector<int>  m_nb_images){nb_images=m_nb_images;};
inline void Set_heart_phase(std::string m_heart_phase){heart_phase=m_heart_phase;};	
inline void Store(std::string m_store_data_path, std::string m_store_myo_path, std::string m_store_roi_path)
{
store_data_path=m_store_data_path; 
store_myo_path=m_store_myo_path; 
store_roi_path=m_store_roi_path;
		std::cout << "store data: " << store_data_path<<std::endl;
		std::cout << "store myocardium data: " << store_myo_path<<std::endl;
		std::cout << "store roi data: " << store_roi_path<<std::endl;
if (mkdir(store_data_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
else{
		cout << "Directory created" << std::endl;
}

};

protected:
std::string root="./1/";
using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 2 >;
using WriterType = itk::ImageFileWriter< ImageType >;
using ReaderType = itk::ImageFileReader< ImageType >;
using BinaryShape = itk::BinaryShapeOpeningImageFilter<ImageType>;
using InvertIntensityImageFilterType = itk::InvertIntensityImageFilter <ImageType>;

std::vector<std::string>  patients;
std::vector<int>  nb_images;
std::string store_data_path;
std::string store_myo_path;
std::string store_roi_path;
std::string heart_phase;


};

#endif
