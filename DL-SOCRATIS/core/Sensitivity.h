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
#ifndef Sensitivity_h
#define Sensitivity_h

#include <stdlib.h> 
#include <iostream>
#include <memory>  
#include <algorithm>
#include <omp.h>

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
#include <random>

#include "unbiased_ground_trouth.h"
#include "Segmentation_Pipeline.h"
#include "Initial_Segmentation.h"
#include "Evaluation.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "configuration.h"


#include<dirent.h>
#include "itkNumericSeriesFileNames.h"
#include <vector>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
class Sensitivity
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


//constructors
Sensitivity(){};
~Sensitivity(){};
Sensitivity(std::vector<bool> parameters, std::string sensitivity_mode );

//main member functions
void RUN( const char *confg_file, std::string m_storepath_results );

//Set functionss
inline void Set_lower_border(std::vector<float> lower)
{
lower_value_endo=lower[0];
lower_value_epi=lower[1];
lower_value_kmean=lower[2];
lower_value_rgmm=lower[3];
lower_value_rgmm_kmean=lower[4];
low=lower;
};
inline void Set_higher_border(std::vector<float> higher)
{
higher_value_endo=higher[0];
higher_value_epi=higher[1];
higher_value_kmean=higher[2];
higher_value_rgmm=higher[3];
higher_value_rgmm_kmean=higher[4];
high=higher;
};
inline void Set_Initial_SD(std::vector<float> m_SD)
{
SD_endo=m_SD[0];
SD_epi=m_SD[1];
SD_kmean=m_SD[2];
SD_rgmm=m_SD[3];
SD_rgmm_kmean=m_SD[4];
SD=m_SD;
};

// Get functions
inline std::vector<float> Get_Initial_SD(){return SD;};
inline std::vector<float> Get_higher_border(){return high;};
inline std::vector<float> Get_lower_border(){return low;};

void copy(const char *confg_file, int number_of_sensitivity, std::vector<float> parameters_value, std::string root);
void delete_folder_localy(std::string path);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:

void parameter_exacute(int parametr_number, int interval_of_active, std::vector<float> parameters_value, const char * confg_file);


std::string root="";
int num_sensitivity=1;
bool parameter_endo=false;
bool parameter_epi=false;
bool parameter_kmean=false;

bool parameter_rgmm=false;
bool parameter_rgmm_kmean=false;

// initial values default
std::vector<float> high;
std::vector<float> low;
std::vector<float> SD;

float lower_value_endo=0;
float lower_value_epi=0;
float lower_value_kmean=0;
float lower_value_rgmm=0;
float lower_value_rgmm_kmean=0;

float higher_value_endo=3;
float higher_value_epi=3;
float higher_value_kmean=1;
float higher_value_rgmm=1;
float higher_value_rgmm_kmean=1;

float SD_endo=0.5;
float SD_epi=0.5;
float SD_kmean=0.25;
float SD_rgmm=0.25;
float SD_rgmm_kmean=0.25;

float endo=0;
float epi=0;
float kmean=0;
float rgmm=0;
float rgmm_kmean=0;

bool Global_mode=false;
bool Local_mode=true;
std::string storepath_results;

float uniform_real_generator(double max, double min, double SD);
float normal_real_generator(double max, double min, double SD);


};

#endif
