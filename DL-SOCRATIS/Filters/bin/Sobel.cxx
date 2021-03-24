////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#include "Sobel.h"

void Sobel::Image_Sobel(const char * m_inputFileName, const char * m_outputFileName)
{


  constexpr unsigned int Dimension = 2;

  using InputPixelType = unsigned char;
  using InputImageType = itk::Image< InputPixelType, Dimension >;

  using ReaderType = itk::ImageFileReader< InputImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName );

  using OutputPixelType = float;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;

  using FilterType = itk::SobelEdgeDetectionImageFilter< InputImageType, OutputImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );

 using WriterType = itk::ImageFileWriter< OutputImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "float_image.nrrd" );
  writer->SetInput( filter->GetOutput() );
  writer->Update();
//change float image to unsigned char

  using Out2putPixelType = unsigned char;
  using Out2putImageType = itk::Image< Out2putPixelType, Dimension >;

  using ReaderType2 = itk::ImageFileReader< OutputImageType >;
  ReaderType2::Pointer reader2 = ReaderType2::New();
  reader2->SetFileName(  "float_image.nrrd");

  using RescaleType = itk::RescaleIntensityImageFilter< OutputImageType, OutputImageType >;
  RescaleType::Pointer rescale = RescaleType::New();
  rescale->SetInput( reader2->GetOutput() );
  rescale->SetOutputMinimum( 0 );
  rescale->SetOutputMaximum( itk::NumericTraits< Out2putPixelType >::max() );

  using FilterType2 = itk::CastImageFilter< OutputImageType, Out2putImageType >;
  FilterType2::Pointer filter2 = FilterType2::New();
  filter2->SetInput( rescale->GetOutput() );

  using WriterType2 = itk::ImageFileWriter< Out2putImageType >;
  WriterType2::Pointer writer2 = WriterType2::New();
  writer2->SetFileName( m_outputFileName );
  writer2->SetInput( filter2->GetOutput() );
  writer2->Update();

  try
    {
    writer2->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;

    }


 


}
