////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#include "Watershed.h"

void Watershed::WatershedFilterType(const char * m_inputFileName,const char * m_outputFileName, float threshold, float level){


constexpr unsigned int Dimension = 2;

  using InputImageType = itk::Image< unsigned char, Dimension >;
  using FloatImageType = itk::Image< float, Dimension >;
  using RGBPixelType = itk::RGBPixel< unsigned char >;
  using RGBImageType = itk::Image< RGBPixelType, Dimension >;
  using LabeledImageType = itk::Image< itk::IdentifierType, Dimension >;

  using FileReaderType = itk::ImageFileReader< InputImageType >;
  FileReaderType::Pointer reader = FileReaderType::New();
  reader->SetFileName( m_inputFileName );

  using GradientMagnitudeImageFilterType = itk::GradientMagnitudeImageFilter< InputImageType, FloatImageType >;
  GradientMagnitudeImageFilterType::Pointer gradientMagnitudeImageFilter =
    GradientMagnitudeImageFilterType::New();

  gradientMagnitudeImageFilter->SetInput( reader->GetOutput() );
  gradientMagnitudeImageFilter->Update();

  using WatershedFilterType = itk::WatershedImageFilter< FloatImageType >;
  WatershedFilterType::Pointer watershed = WatershedFilterType::New();



  watershed->SetThreshold( threshold );
  watershed->SetLevel( level );
 // watershed->SetFullyConnected (false);
  watershed->SetInput( gradientMagnitudeImageFilter->GetOutput() );
  watershed->Update();

  using RGBFilterType = itk::ScalarToRGBColormapImageFilter< LabeledImageType, RGBImageType>;
  RGBFilterType::Pointer colormapImageFilter = RGBFilterType::New();
  colormapImageFilter->SetColormap( RGBFilterType::Jet );
  colormapImageFilter->SetInput( watershed->GetOutput() );
  colormapImageFilter->Update();

  using OutputPixelType = unsigned char;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;

  using FilterType = itk::RGBToLuminanceImageFilter< RGBImageType, OutputImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( colormapImageFilter->GetOutput());

  using WriterType = itk::ImageFileWriter< OutputImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName );
  writer->SetInput( filter->GetOutput() );
 writer->Update();
 


}


void Watershed::SeedWatershedFilterType(const char * m_inputFileName,const char * m_marherimage, const char * m_outputFileName){


constexpr unsigned int Dimension = 2;

  using InputImageType = itk::Image< unsigned char, Dimension >;
  using FloatImageType = itk::Image< float, Dimension >;
  using RGBPixelType = itk::RGBPixel< unsigned char >;
  using RGBImageType = itk::Image< RGBPixelType, Dimension >;
  using LabeledImageType = itk::Image< itk::IdentifierType, Dimension >;

  using FileReaderType = itk::ImageFileReader< InputImageType >;
  FileReaderType::Pointer reader = FileReaderType::New();
  FileReaderType::Pointer reader2 = FileReaderType::New();
  reader->SetFileName( m_inputFileName );
  reader2->SetFileName( m_marherimage );
  reader->Update();
  reader2->Update();

using FilterType2 = itk::CastImageFilter< InputImageType, FloatImageType  >;

FilterType2::Pointer filter2 = FilterType2::New();
filter2->SetInput( reader2->GetOutput() );

  using GradientMagnitudeImageFilterType = itk::GradientMagnitudeImageFilter< InputImageType, FloatImageType >;
  GradientMagnitudeImageFilterType::Pointer gradientMagnitudeImageFilter = GradientMagnitudeImageFilterType::New();

  gradientMagnitudeImageFilter->SetInput( reader->GetOutput() );
  gradientMagnitudeImageFilter->Update();

  using WatershedFilterType = itk::MorphologicalWatershedFromMarkersImageFilter< FloatImageType,FloatImageType >;
  WatershedFilterType::Pointer watershed = WatershedFilterType::New();


  watershed->SetMarkWatershedLine(true);
  watershed->SetFullyConnected (true);
 // watershed->SetThreshold( 1 );
//  watershed->SetLevel( level );
  watershed->SetMarkerImage(filter2->GetOutput());
  watershed->SetInput( gradientMagnitudeImageFilter->GetOutput() );
  watershed->Update();

  using RGBFilterType = itk::ScalarToRGBColormapImageFilter< FloatImageType, RGBImageType>;
  RGBFilterType::Pointer colormapImageFilter = RGBFilterType::New();
  colormapImageFilter->SetColormap( RGBFilterType::Jet );
  colormapImageFilter->SetInput( watershed->GetOutput() );
  colormapImageFilter->Update();

  using OutputPixelType = unsigned char;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;

  using FilterType = itk::RGBToLuminanceImageFilter< RGBImageType, OutputImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( colormapImageFilter->GetOutput());

  using WriterType = itk::ImageFileWriter< OutputImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName );
  writer->SetInput( filter->GetOutput() );
 writer->Update();

}

