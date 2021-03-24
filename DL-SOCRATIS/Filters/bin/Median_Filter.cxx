////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#include "Median_Filter.h"

void Median_Filter::Image_Median_Filter(const char * m_inputFileName, const char * m_outputFileName)
{


  using InputPixelType = unsigned char;
  using OutputPixelType = unsigned char;
  using InputImageType = itk::Image< InputPixelType,  2 >;
  using OutputImageType = itk::Image< OutputPixelType, 2 >;
  using ReaderType = itk::ImageFileReader< InputImageType  >;
  using WriterType = itk::ImageFileWriter< OutputImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  reader->SetFileName( m_inputFileName );
  writer->SetFileName( m_outputFileName );
  using FilterType = itk::MedianImageFilter<InputImageType, OutputImageType >;
  FilterType::Pointer filter = FilterType::New();
  InputImageType::SizeType indexRadius;
  indexRadius[0] = 1; // radius along x
  indexRadius[1] = 1; // radius along y
  filter->SetRadius( indexRadius );
  filter->SetInput( reader->GetOutput() );
  writer->SetInput( filter->GetOutput() );
  writer->Update();
  

}
