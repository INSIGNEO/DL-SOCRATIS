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
#ifndef Segmentation_Pipeline_h
#define Segmentation_Pipeline_h

#include "Change_information.h"
#include "Thresholding.h"
#include "Watershed.h"
#include "Histogram.h"
#include "kmeans.h"
#include "Sobel.h"
#include "Thresholding.h"
#include "Grayscale.h"
#include "HistogramGaussianRician.h"
#include "image_vision.h"
#include "itkListSample.h"
#include "HistogramXYPlot.h"
#include "estportion.h"
#include "seedpoints.h"
#include "ConnectCombonentFilter.h"
#include "Equalizer.h"
#include "prewatershed.h"
#include "BinaryImage.h"
#include "Resample.h"
#include "denoising.h"
#include "Translation_registration.h"
#include "mergeimages.h"
#include "TestPipelineConfig.h"
#include "rebuild_myo_mask.h"
#include "remove_image_object.h"
#include "unbiased_ground_trouth.h"
#include "Deformation.h"

#include "itkListSample.h"
#include "itkStatisticsAlgorithm.h"
#include "itkVector.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkNumericSeriesFileNames.h"
#include <itkBinaryShapeOpeningImageFilter.h>

#include <memory>
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <algorithm>
///#include <experimental/filesystem>



#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
////TESTSSS///
//#include "RGhistogram_test.h"




class Segmentation_Pipeline
{
public:
//Segmentation_Pipeline(const char * myoFolder,const char * ROIFolder,string SCARFolder,int nb_train );
Segmentation_Pipeline(std::string m_root){root=m_root;};
Segmentation_Pipeline(){};
~Segmentation_Pipeline(){};


// using libraries ITK
using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 2 >;
using ReaderType = itk::ImageFileReader< ImageType >;
using WriterType = itk::ImageFileWriter< ImageType >;
using BinaryShape = itk::BinaryShapeOpeningImageFilter<ImageType>;
using MeasurementType = double;
using MeasurementVectorType = itk::Vector< MeasurementType, 1 >;
using SampleType = itk::Statistics::ListSample<MeasurementVectorType>;
using SubsampleType = itk::Statistics::Subsample<SampleType>;
typedef itk::Vector<double, 1 > meanTH;
meanTH TG;
meanTH TR;

//consts
int scar_rebuild_phase=0;
int threshold_of_scar_intensity;
bool border_zone_value=true;
bool myo_new=true;
std::string image_detect;
const char * bins = "255";
double time= atof("0.014");
double conductance_parameter= atof("6.0");
const char * uNCL = "1";
//const char * outputFileName = (root+"output.jpg").c_str();
float threshold = 0.005;
float level = threshold*100;
std::shared_ptr<image_vision > IV =std::make_shared<image_vision>();
const char * inputFileName;
const char * inputFileName2;
const char * inputFileName_ROI;


//member functions
void Run(const char * m_myoFolder, const char * m_ROIFolder,std::string m_SCARFolder,int m_nb_train);
void preprocessing();
void reduce_noise();
void restimate_myo();
void threshold_define();
void segmentation();
void align_images(const char * m_predictFileName, double mean_endo_center[2], double angle, double scale);
bool check_black_image(const char * image, int element_number, int threshold_value_intens);

//set functions
inline void SetSensitivity2_param( bool m_sensitivity2, float m_endo){sensitivity2_param=m_sensitivity2; endo=m_endo;};
inline void SetSensitivity3_param( bool m_sensitivity3, float m_epi){sensitivity3_param=m_sensitivity3; epi=m_epi;};
inline void SetSensitivity1_param( bool m_sensitivity1, float m_kmean){sensitivity1_param=m_sensitivity1; kmean=m_kmean;};
inline void SetSensitivity4_param( bool m_sensitivity4, float m_rgmm){sensitivity4_param=m_sensitivity4; rgmm=m_rgmm;};
inline void SetSensitivity5_param( bool m_sensitivity5, float m_rgmm_kmean){sensitivity5_param=m_sensitivity5; rgmm_kmean=m_rgmm_kmean;};
inline void SetBorderZone(bool bz){border_zone_value=bz;};
inline void SetThreshold_of_scar_intens(int tosi){threshold_of_scar_intensity=tosi;};
inline void Set_restimate_myo(bool myo){myo_new= myo;};
inline void Set_root_path(std::string path_root){root= path_root;};
inline void Set_segmentation_case(std::string seg_case){segmentation_part=seg_case;};
inline void Set_need_dl_case(bool nd_dl){need_dl=nd_dl;};
//get functions
inline bool Get_need_dl_case(){return need_dl;};
std::string root="./1/";

private:
void copy_to_directory(const char * SCARFolder, std::string case_copy); 
void match_myo_ROI_mask(std::string mask,std::string ROIName,int mean_group, int SD_group);
void dcm_converter(std::string mask,std::string ROIName);
void delete_images_localy(std::string path); 
bool sensitivity1_param=false;
bool sensitivity2_param=false;
bool sensitivity3_param=false;
bool sensitivity4_param=false;
bool sensitivity5_param=false;
bool need_dl=true;

std::string segmentation_part="both";

std::string mask_input="jpg";
float rgmm=0;
float rgmm_kmean=0;
float endo=0;
float epi=0;
float kmean=0;

std::string binaryepi_endo_mask;
std::string binary_myo_mask;
inline bool exists_file (const std::string& name) {struct stat buffer;   return (stat(name.c_str(), &buffer) == 0); }
inline bool higherorequal(ImageType::SizeType size){ if (size[0]>=511 && size[1]>=511){ return true;} if (size[0]<511 && size[1]<511){return false;}};
};

#endif
