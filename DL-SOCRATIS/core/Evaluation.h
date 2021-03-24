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
#ifndef Evaluation_h
#define Evaluation_h


#include "TestPipelineConfig.h"
#include "itkImageFileReader.h"

#include <stdlib.h> 
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
#include <string.h>
#include "deformable_registration.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkVector.h"
#include "itkListSample.h"

#include "evaluate.h"
#include "configuration.h"
#include "unbiased_ground_trouth.h"
#include "rebuild_myo_mask.h"

#include<dirent.h>
#include "itkNumericSeriesFileNames.h"
#include <vector>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
class Evaluation
{
public:
//consts
using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 2 >;
using ReaderType = itk::ImageFileReader< ImageType >;
double total_dice;
double total_total;
double total_jaccard;
const char * predictFile;
const char * manualFile;


//functions
Evaluation(std::string m_root){root=m_root;};
~Evaluation(){};



int evaluate_images_dice_metric( const char * predictFileName, const char *manualFileName,std::string storeFolderName, int slices_numb, double rotation_angle, double scale, std::vector<int> center_array );
void copy(const char *confg_file);
void compute_dice_sensitivity_specificity_metric( std::string predictpath, std::string manualpath,  std::vector<string> patient, std::string label );
void copy_pedrict_data( std::string predictpath_store, std::string path_of_code_results,  std::string image_file );
void preprocessing_align_match_images( std::string predictpath, std::string manualpath,  std::vector<string> patient_folders, std::string label,std::string ground_truth_case );

double Get_Total_Dice(){return total_dice;};
double Get_Total_Total(){return total_total;};
double Get_Total_Jaccard(){return total_jaccard;};
inline void Set_store_txt_path(std::string path_txt){txt_path_store=path_txt;};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string evaluation_file="../../IO/";

bool fexists(const std::string& filename) {
  std::ifstream ifile(filename.c_str());
  return (bool)ifile;
}
private:
std::string root="./1/";
std::string txt_path_store=root;
void smooth_images( std::string predictim,  int loop, int thres );
using MeasurementVectorType = itk::Vector< float, 2 >;
using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;


};

#endif
