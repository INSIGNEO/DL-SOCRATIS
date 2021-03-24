////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef HistogramGaussianRician_h
#define HistogramGaussianRician_h

#include <stdlib.h>    
#include "itkVector.h"
#include "itkListSample.h"
#include "itkGaussianMixtureModelComponent.h"
#include "itkRicianMixtureModelComponent.h"
#include "itkRicianMembershipFunction.h"
#include "itkGaussianMembershipFunction.h"

#include "itkExpectationMaximizationMixtureModelEstimator.h"
#include "itkNormalVariateGenerator.h"
// for histogram compute
#include <vector>
#include <fstream>
#include <itkVector.h>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToHistogramFilter.h"
#include "itkImageRandomIteratorWithIndex.h"

//for the vision of example samples
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPen.h> 
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>


class HistogramGaussianRician
{
public:
  /**Standard class type aliases. */

const char * m_inputFileName;
const char * bins ;
bool detect_max=true;

bool RG=false;
bool RRG=false;
bool RGG=false;
bool RRGG=false;
bool print=true;



HistogramGaussianRician(const char * inputFileName, const char * m_bins);
~HistogramGaussianRician(){};

typedef itk::Vector< double, 1 > meanTH;
meanTH mnR;
meanTH mnR2;
meanTH mnG;
meanTH mnG2;

//Set Get functions


void Update();

inline itk::Vector< double, 1 > GetmeanR(){return mnR;}
inline itk::Vector< double, 1 > GetmeanR2(){return mnR2;}
inline itk::Vector< double, 1 > GetmeanG(){return mnG;}
inline itk::Vector< double, 1 > GetmeanG2(){return mnG2;}

void Reset();


protected:
  double normalize;
  typedef itk::Vector< double, 1 > MeasurementVectorType;
//typedef itk::Vector< double, 2 > MeasurementVectorType2D;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;
//typedef itk::Statistics::ListSample< MeasurementVectorType2D > SampleType2d;
  using PixelType = unsigned char;
  static constexpr unsigned int Dimension_v = 2;
  using ImageType = itk::Image< PixelType, Dimension_v >;
  static constexpr unsigned int MeasurementVectorSize = 1; // Grayscale
  using ReaderType = itk::ImageFileReader< ImageType >;
  using ImageToHistogramFilterType = itk::Statistics::ImageToHistogramFilter< ImageType >;
 using AbsoluteFrequencyType =ImageToHistogramFilterType::HistogramType::AbsoluteFrequencyType;
  typedef itk::Statistics::ExpectationMaximizationMixtureModelEstimator<SampleType > EstimatorType;
  typedef itk::Statistics::GaussianMixtureModelComponent<SampleType > ComponentType;
  typedef itk::Statistics::RicianMixtureModelComponent< SampleType > ComponentType2;
  //typedef itk::Statistics::GaussianMixtureModelComponent<SampleType2d > ComponentTypeG2;
 // typedef itk::Statistics::RicianMixtureModelComponent< SampleType2d > ComponentTypeR2;
  typedef itk::Array< double > ParametersType;


  ReaderType::Pointer reader = ReaderType::New();
  EstimatorType::Pointer estimator = EstimatorType::New();
  SampleType::Pointer sample = SampleType::New();
  ImageToHistogramFilterType::Pointer imageToHistogramFilter = ImageToHistogramFilterType::New();
SampleType::Pointer frequency_measure = SampleType::New();
  MeasurementVectorType mv;
  const MeasurementVectorType mv1;

  std::vector<double> freq;
  std::vector<double> observ;

  void compute_histogram();
private:
double ar;
double sr;
double ar2;
double sr2;
double xs;
double mg;
double mr;
double mr2;
double ag;
double sg;
double ag2;
double sg2;
double mg2;
double xmax;
double argmaxh;

void Image_HistogramRicianGaussian();
void Image_HistogramRician_R_Gaussian();
void Image_HistogramRician_G_Gaussian();
void Image_HistogramRician_R_G_Gaussian();

};

#endif
