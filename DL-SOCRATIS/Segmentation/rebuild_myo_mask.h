////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef rebuild_myo_mask_h
#define rebuild_myo_mask_h

#include "itkImage.h"
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkExtractImageFilter.h>
#include "itkFlipImageFilter.h"
#include "itkPasteImageFilter.h"
#include "itkEuclideanDistanceMetric.h"
#include "itkStatisticsAlgorithm.h"
#include "seedpoints.h"
#include "mergeimages.h"
#include "itkVector.h"
#include "itkListSample.h"

class rebuild_myo_mask {



private:
std::string root="./1/";
using MeasurementVectorType = itk::Vector< float, 2 >;
using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;
using DistanceMetricType = itk::Statistics::EuclideanDistanceMetric<MeasurementVectorType>;
DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New();

public:
void Set_root_path(std::string path_root){root= path_root;};
rebuild_myo_mask(std::string m_root){root=m_root;};
~rebuild_myo_mask(){}; 
bool max_boundary_case=true;
void Run(const char * inputFileName);
void cut_symetric_images(const char * inputFileName, const char * outputFileName1, const char * outputFileName2, const char * outputFileName3, const char * outputFileName4);
int compute_volume(const char * image);
void create_new_symmetric_myo(int case_sym);
void add_images(const char * inputFileName1, const char * inputFileName2, const char * inputFileName3, const char * inputFileName4,const char * outputimage);
int compute_median_thickness(const char * epicont, const char * endocont);
int compute_median_distance_radius(SampleType::Pointer contour, MeasurementVectorType center);
void draw_median_cycle(int median_dist, int radius_epi, const char * outputFileName,int cycle[2] );
double compute_median(SampleType::Pointer contour);
inline void Set_max_median_compute(bool value){max_boundary_case=value;};





















};

#endif
