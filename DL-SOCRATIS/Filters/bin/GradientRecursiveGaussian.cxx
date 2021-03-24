////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#include "GradientRecursiveGaussian.h"

void GradientRecursiveGaussian::Image_GRG(const char * m_inputFileName, const char * m_outputFileName,  double sig)
{

  constexpr unsigned int Dimension = 3;
  using InputPixelType = float;
  using OutputComponentPixelType = float;
  using OutputPixelType = itk::CovariantVector< OutputComponentPixelType, Dimension >;
  using InputImageType = itk::Image< InputPixelType,  Dimension >;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;
  using ReaderType = itk::ImageFileReader< InputImageType >;
  using FilterType = itk::GradientRecursiveGaussianImageFilter< InputImageType, OutputImageType >;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName );
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetSigma( sig );
  filter->Update();
  using WriterType = itk::ImageFileWriter< OutputImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

 


}
