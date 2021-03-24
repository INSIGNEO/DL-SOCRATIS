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
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef Deformation_h
#define Deformation_h

#include "deformable_registration.h"
#include <stdlib.h> 
#include <string.h> 
#include <Python.h>
#include <iostream>
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
#include "itkCastImageFilter.h"
#include "itkWarpImageFilter.h"
#include "BinaryImage.h"
#include "mergeimages.h"
#include "Initial_Segmentation.h"
#include "Resample.h"
#include "Atlas_segm.h"
#include "Region_grown.h"
#include <denoising.h>
#include "itkResampleImageFilter.h"
#include <configuration.h>
#include "PCA_compute.h"

#include "strain_compute.h"


#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
class Deformation
{
public:


//functions
Deformation(std::string m_root){
root=m_root;

if (root=="./"){

store_roi_image_path="../data_internal/Data/Deform/";
store_myo_path="/home/michail/Desktop/test/";
def_roi_run="../Initial_Segmentation/deform/";

}


};
~Deformation(){};
void Run(std::string ES_path, std::string ED_path, std::string store_path , bool dl  );
void each_patient_compute(std::vector<string> input12, std:: vector<string> input22, std::string output);
void run_roiDL_model(std::string store_ES_path, std::string store_ED_path, std::string roi_path);
void run_unet_DL_model(std::string cardiac_phase);
void postprocessing_ROI( std::string cardiac_phase, bool dl );
inline void  Setconfig(const char * m_confg_file){confg_file=m_confg_file;}
inline void  Set_images_per_patient(vector<int> m_patient_number){patient_number=m_patient_number;}
inline void  Set_patients(vector<string> m_folders_name){folders_name=m_folders_name;}
inline void  Set_store_myo_path(std::string m_store_myo_path){store_myo_path=m_store_myo_path;}

private:
std::string root="./1/";
vector<string> folders_name;
vector<string> dl_store_myo_path;
std::string  sstore_path;
vector<int> patient_number;
std::string store_roi_image_path="../../data_internal/Data/Deform/";
std::string store_myo_path="/home/michail/Desktop/test/";
std::string def_roi_run="../..Initial_Segmentation/deform/";
const char * confg_file;

void clean_data()
{
// clear the unused computing data
std::string commandwr1;
commandwr1 += "rm -rf ";
commandwr1 += "*.jpg";
int wrf1= system(commandwr1.c_str());

std::string commandwr2;
commandwr2 += "rm -rf ";
commandwr2 += "*.mha";
int wrf2= system(commandwr2.c_str());}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

#endif
