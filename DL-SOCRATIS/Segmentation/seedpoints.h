////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef seedpoints_h
#define seedpoints_h

#include <memory>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itksys/SystemTools.hxx"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include <iostream>
#include <cmath>
#include "ConnectCombonentFilter.h"
#include <cstdlib>
#include <vtkMetaImageWriter.h>
#include "vtkSmartPointer.h"
#include "itkLabelShapeKeepNObjectsImageFilter.h"
#include "kmeans.h"
#include "mergeimages.h"
#include "remove_image_object.h"
#include "denoising.h"
#include "BinaryImage.h"
#include "LabelObject.h"
#include "Sobel.h"
#include "Median_Filter.h"
#include "boundarysegm.h"
#include "Thresholding.h"
#include "itkInvertIntensityImageFilter.h"
#include "Change_information.h"
#include "unbiased_ground_trouth.h"
//kd-tree
#include "itkVector.h"
#include "itkListSample.h"
#include "itkKdTree.h"
#include "itkKdTreeGenerator.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkEuclideanDistanceMetric.h"
#include "itkStatisticsAlgorithm.h"

#include <itkBinaryShapeOpeningImageFilter.h>

class seedpoints
{
public:

typedef itk::Vector< double, 1 > THRS;

seedpoints(std::string ROI, std::string inputFileName,std::string inputFileName2, std::string outputFileName, float TH, THRS TR_threshold, std::string m_root);
seedpoints(std::string m_root){root=m_root;};
~seedpoints(){};

bool scar_on=false;
bool border_zone_ellimination=true;
int offset_cycle=2;
int scar_threshold_k_means=0;
using MeasurementVectorType = itk::Vector< float, 2 >;
using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;

void Initiallize();
void preprocessing( const char * inImage_endo,const char * inImage_epi);
void preprocessing2();
void sober_epi_endo(const char * inImage_epiendo, double scale);
void preprocessing3(const char * inImage_myo);
void compute_area_seedpoints();
void compute_boundaries( const char * initial_image);

inline std::string GetOutput(){return m_outputFileName;};
inline void  SetInput( std::string inputFileName){m_inputFileName=inputFileName;};
inline void  SetInput2( std::string inputFileName2){m_inputFileName2=inputFileName2;};
inline void  SetOutput( std::string outputFileName){m_outputFileName=outputFileName;};

inline void SetSensitivity1_param( bool m_sensitivity1, float kmean_value){sensitivity1_param=m_sensitivity1; kmean_variation_value=kmean_value;};
inline void SetSensitivity2_param( bool m_sensitivity2, float m_endo){sensitivity2_param=m_sensitivity2; endo=m_endo;};
inline void SetSensitivity3_param( bool m_sensitivity3, float m_epi){sensitivity3_param=m_sensitivity3; epi=m_epi;};


inline bool Get_times_loop(){return rebuild_loop;};
inline bool Get_scar_exist(){return scar_on;};
inline int GetScar_kmean(){return scar_threshold_k_means;};
inline SampleType::Pointer GetSample_endo(){return sample_endo;};
inline SampleType::Pointer GetSample_epi(){return sample_epi;};
inline SampleType::Pointer GetSample_max(){return samplemaximum;};
inline SampleType::Pointer Getdistance_epi_endo(){return sample_min_epi_end;};

void create_vector_of_images(const char * inImage_endo,const char * inImage_epi);
void write_seed_image(SampleType::Pointer sampleseed);
void compute_distance_of_two_contour( );
void remove_object( const char * i_image, const char * out_image, int number, const char * type);
void borderzone_scars_estimate( const char * image, int radius, int radius2, int centerx, int centery );
void Update();

void remove_holles( const char * init_image);
void Set_root_path(std::string path_root){root= path_root;};

protected:
float endo=0;
float epi=0;


private:
std::string root="./1/";
using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 2 >;
using OutputImageType = itk::Image< float, 2 >;
using ReaderType = itk::ImageFileReader< ImageType >;
using WriterType = itk::ImageFileWriter< ImageType >;
using WriterType_out = itk::ImageFileWriter< OutputImageType >;
typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< OutputImageType > IteratorType;
//ImageType::Pointer  inputImage = ImageType::New();
//OutputImageType::Pointer outputImage = OutputImageType::New();
using DistanceMetricType = itk::Statistics::EuclideanDistanceMetric<MeasurementVectorType>;
DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New();
using SubsampleType = itk::Statistics::Subsample<SampleType>;
using BinaryShape = itk::BinaryShapeOpeningImageFilter<ImageType>;
SampleType::Pointer sample_endo = SampleType::New();
SampleType::Pointer sample_epi = SampleType::New();

THRS TR;
float THRESHOLD;
std::string m_inputFileName;
std::string m_inputFileName2;
std::string m_outputFileName;
std::string m_ROI;
bool rebuild_loop=false;
bool sensitivity1_param=false;
bool sensitivity2_param=false;
bool sensitivity3_param=false;


float kmean_variation_value=0;

SampleType::Pointer sample_high_intens = SampleType::New();
SampleType::Pointer samplemaximum = SampleType::New();
SampleType::Pointer sample_min_epi_end = SampleType::New();
SampleType::Pointer sample_offset = SampleType::New();

BinaryShape::Pointer binaryshape = BinaryShape::New();



//void remove_noise_of_endo(SampleType::Pointer sample_endo_initial);
void find_max_intensity_funct_portion();

};

#endif
