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
#ifndef Atlas_segm_h
#define Atlas_segm_h

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
#include <string.h>

#include <dirent.h>
#include <array>
#include <iomanip>
#include <vector>
#include <boost/filesystem.hpp>

//ITK thresholder
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkThresholdImageFilter.h"
#include "itkImage.h"


#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

class Atlas_segm
{
public:
//Segmentation_Pipeline(const char * myoFolder,const char * ROIFolder,string SCARFolder,int nb_train );
Atlas_segm(std::string m_root){

root=m_root;

if (root=="./"){


json_roi_store_path="../Initial_Segmentation/Model/";
store_roi_image_path="../data_internal/Data/Atlas/";
store_image_path="../data_internal/Atlas/";
atlas_roi_run="../Initial_Segmentation/atlas/";
path_of_med2jpg="../Initial_Segmentation/multi_atlas/CIMAS/med2image-master/bin/";
path_of_atlas="../Initial_Segmentation/multi_atlas/CIMAS/";

}


};

~Atlas_segm(){};


void Run(std::string data_path, std::string landmarks_path);
void slice2dimages(std::string store_data, std::string imagenii);
void preprocessing(std::string images_path);
void write_json_roi_file(std::vector<std::string> filename, std::string jsonname);
void run_roiDL_model();
void Image_segment_Thresholding(const char * m_inputFileName, const char * m_outputFileName, float m_lowerThreshold, float m_upperThreshold, bool inverse);
void create_binary_Image(const char * m_inputFileName, const char * m_outputFileName);
inline void Set_heart_phase(std::string m_heart_phase){heart_phase=m_heart_phase;};
inline void Set_type_of_input_data(std::string m_type_of_input_data){type_of_input_data=m_type_of_input_data;};	
inline void Store(std::string m_store_myo_path,std::string m_store_roi_path){store_myo_path=m_store_myo_path; store_roi_path=m_store_roi_path;};


private:
std::string root="./1/";
std::string store_myo_path;
std::string store_roi_path;
std::string type_of_input_data;
std::string heart_phase;
std::vector<std::string> folders_name;
std::vector<int> patient_number;
std::string json_roi_store_path="../../Initial_Segmentation/Model/";
std::string store_roi_image_path="../../data_internal/Data/Atlas/";
std::string store_image_path="../../data_internal/Atlas/";
std::string atlas_roi_run="../../Initial_Segmentation/atlas/";
std::string path_of_med2jpg="../../Initial_Segmentation/multi_atlas/CIMAS/med2image-master/bin/";
std::string path_of_atlas="../../Initial_Segmentation/multi_atlas/CIMAS/";
};

#endif
