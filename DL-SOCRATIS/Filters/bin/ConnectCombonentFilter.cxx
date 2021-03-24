////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#include "ConnectCombonentFilter.h"

void ConnectCombonentFilter::Image_CCF(const char * m_inputFileName, const char * m_outputFileName, const char * m_maskfilter)
{


    constexpr unsigned int Dimension = 2;
  using PixelType = unsigned char;
  using RGBPixelType = itk::RGBPixel<unsigned char>;
  using ImageType = itk::Image<PixelType, Dimension>;
  using RGBImageType = itk::Image<RGBPixelType, Dimension>;
  using MaskImageType = itk::Image<PixelType, Dimension>;
  ImageType::Pointer image;
  MaskImageType::Pointer mask;
  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName);
  reader->Update();
  image = reader->GetOutput();
  using ReaderType = itk::ImageFileReader<MaskImageType>;
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( m_maskfilter);
  reader2->Update();
  mask = reader2->GetOutput();

  using OutputImageType = itk::Image< unsigned short, Dimension >;
  using ConnectedComponentImageFilterType = itk::ConnectedComponentImageFilter <ImageType, OutputImageType >;
  ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New ();
  connected->SetInput(image);
  connected->SetMaskImage(mask);
  connected->SetFullyConnected(true);
  connected->Update();




  std::cout << "Number of objects: " << connected->GetObjectCount() << std::endl;
  using RGBFilterType = itk::LabelToRGBImageFilter<OutputImageType, RGBImageType>;
  RGBFilterType::Pointer rgbFilter = RGBFilterType::New();
  rgbFilter->SetInput( connected->GetOutput() );
  rgbFilter->Update();


// from RGB to Gray


  using FilterType = itk::RGBToLuminanceImageFilter< RGBImageType, ImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( rgbFilter->GetOutput() );


  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName );
  writer->SetInput(filter->GetOutput() );
  writer->Update();


}
