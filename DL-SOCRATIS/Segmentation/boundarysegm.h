////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef boundarysegm_h
#define boundarysegm_h
#include "itkImagePCADecompositionCalculator.h"

#include "itkSigmoidImageFilter.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include <itkAbsoluteValueDifferenceImageFilter.h>
#include <memory>
#include "itkStatisticsAlgorithm.h"
#include "itkScalarChanAndVeseSparseLevelSetImageFilter.h"
#include "itkScalarChanAndVeseLevelSetFunction.h"
#include "itkScalarChanAndVeseLevelSetFunctionData.h"
#include "itkConstrainedRegionBasedLevelSetFunctionSharedData.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "Resample.h"
#include "itkAtanRegularizedHeavisideStepFunction.h"
#include "itkCastImageFilter.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "itkGeodesicActiveContourShapePriorLevelSetImageFilter.h"
#include "itkChangeInformationImageFilter.h"
#include "itkBoundedReciprocalImageFilter.h"
#include "itkEuler2DTransform.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkNormalVariateGenerator.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSpatialFunctionImageEvaluatorFilter.h"
#include "itkCommand.h"
#include "seedpoints.h"
#include "Segmentation_Pipeline.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBoundedReciprocalImageFilter.h"
#include "itkChangeInformationImageFilter.h"
#include "itkCommand.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkEuler2DTransform.h"
#include "itkFastMarchingImageFilter.h"
#include "itkGeodesicActiveContourShapePriorLevelSetImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImagePCAShapeModelEstimator.h"
#include "itkMultiplyImageFilter.h"
#include "itkNumericSeriesFileNames.h"
#include "itkNormalVariateGenerator.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkPCAShapeSignedDistanceFunction.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkShapePriorMAPCostFunction.h"
#include "itkSpatialFunctionImageEvaluatorFilter.h"

class boundarysegm {
public:


void level_set(const char * inputFileName, const char * outputFileName,const char * couputFileName, int iteration, float RMSerror, float mu, int nu, int vol);
boundarysegm(std::string m_root){root=m_root;};
~boundarysegm(){};

using PixelType = unsigned char;
using MeasurementVectorType = itk::Vector< float, 2 >;
using MeasurementVectorType2 = itk::Vector< int, 2 >;
using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;
using ImageType = itk::Image< PixelType, 2 >;
using OutputImageType = itk::Image< float, 2 >;
void Set_root_path(std::string path_root){root= path_root;};
void geodesic_active_contour(const char * inputFileName, const char * outputFileName, const char * couputFileName_endo, const char * couputFileName_epi, double prob, double prio, int number);
//void compute_PCA(const char *inputfolder, int number_train, int modes_train);
void determine_mean_shape(const char * inputFileName1, const char * inputFileName2, const char * outputFileName);
MeasurementVectorType mean_endo;
MeasurementVectorType mean_epi;
MeasurementVectorType mean;
MeasurementVectorType Getmean(){return mean;};
int maxdif;
int mindif;
int Getmax_radius(){return maxdif;}
int Getmin_radius(){return mindif;}
void create_geodestic_seed_points(const char * inputFileName1, const char * inputFileName2);
inline void SetSensitivity1_param( bool m_sensitivity3, float m_endo_portion){sensitivity1_param=m_sensitivity3; endo_portion=m_endo_portion;}
inline void SetSensitivity2_param( bool m_sensitivity4, float m_epi_portion){sensitivity2_param=m_sensitivity4; epi_portion=m_epi_portion;}
inline void Set_equal_portion_endo_epi( bool m_equal_portion){equal_portion=m_equal_portion;}

private:

std::string root="./1/";
bool sensitivity1_param=false;
bool sensitivity2_param=false;
bool equal_portion=false;
float endo_portion=0;
float epi_portion=0;
typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< OutputImageType > IteratorType;
using SubsampleType = itk::Statistics::Subsample<SampleType>;
using DistanceMetricType = itk::Statistics::EuclideanDistanceMetric<MeasurementVectorType>;

MeasurementVectorType miny_epi;
MeasurementVectorType maxy_epi;
MeasurementVectorType miny_endo;
MeasurementVectorType maxy_endo;

MeasurementVectorType first_epi;
MeasurementVectorType second_epi;
MeasurementVectorType third_epi;
MeasurementVectorType forth_epi;
MeasurementVectorType fifth_epi;
MeasurementVectorType mirror_first_epi;
MeasurementVectorType mirror_second_epi;
MeasurementVectorType mirror_third_epi;
MeasurementVectorType mirror_forth_epi;
MeasurementVectorType mirror_fifth_epi;

DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New();
SampleType::Pointer sample_endo = SampleType::New();
SampleType::Pointer sample_high_intens = SampleType::New();
SampleType::Pointer samplemaximum = SampleType::New();
SampleType::Pointer sample_min_epi_end = SampleType::New();
ImageType::Pointer  inputImage = ImageType::New();
OutputImageType::Pointer outputImage = OutputImageType::New();




};

template<class TFilter>
class CommandIterationUpdate : public itk::Command
{
public:
  using Self = CommandIterationUpdate;
  using Superclass = itk::Command;
  using Pointer = itk::SmartPointer<Self>;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() = default;
public:
  void Execute(itk::Object *caller,
               const itk::EventObject & event) override
    {
    Execute( (const itk::Object *) caller, event);
    }
  void Execute(const itk::Object * object,
               const itk::EventObject & event) override
    {
    const auto * filter = static_cast< const TFilter * >( object );
    if( typeid( event ) != typeid( itk::IterationEvent ) )
      { return; }
    std::cout << filter->GetElapsedIterations() << ": ";
    std::cout << filter->GetRMSChange() << " ";
    std::cout << filter->GetCurrentParameters() << std::endl;
    }
};

#endif
