////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#include "estportion.h"

void estportion::estportion_compute(const char * m_inputFileName, const char * m_outputFileName, THRS TG, THRS TR ){
//read input image

  constexpr unsigned int Dimension = 2;
  using PixelType = unsigned char;
  using ImageType = itk::Image< PixelType, Dimension >;
  using ReaderType = itk::ImageFileReader< ImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName );

// convert to double image for itteration

  using Out2putPixelType = double;
  using Out2putImageType = itk::Image< Out2putPixelType, Dimension >;


  using FilterType2 = itk::CastImageFilter< ImageType, Out2putImageType >;
  FilterType2::Pointer filter2 = FilterType2::New();
  filter2->SetInput( reader->GetOutput() );

  using WriterType2 = itk::ImageFileWriter< Out2putImageType >;
  WriterType2::Pointer writer2 = WriterType2::New();
  writer2->SetFileName("draft.nrrd" );
  writer2->SetInput( filter2->GetOutput() );
  writer2->Update();

//start the process
Out2putImageType::Pointer inputImage = filter2->GetOutput();
ImageType::Pointer outputImage = ImageType::New();
//copy allocate output image
outputImage->SetRegions( inputImage->GetRequestedRegion() );
outputImage->CopyInformation( inputImage );
outputImage->Allocate();
//define itterators
typedef itk::ImageRegionConstIterator< Out2putImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< ImageType > IteratorType;

ConstIteratorType inputIt( inputImage, inputImage->GetRequestedRegion() );
IteratorType outputIt( outputImage, outputImage->GetRequestedRegion() );

ImageType::IndexType requestedIndex =outputImage->GetRequestedRegion().GetIndex();
// create the estimated portion (linear partial volume)
THRS TR_new;
THRS TG_new;
if (TR[0]>TG[0]){TR_new=TG; TG_new=TR;}
if (TR[0]<=TG[0]){TR_new=TR; TG_new=TG;}

for ( inputIt.GoToBegin(), outputIt.GoToBegin(); !inputIt.IsAtEnd();++inputIt, ++outputIt)
{
if (inputIt.Get()<=TR_new[0]){
outputIt.Set( 0 );
}//end if

if ((inputIt.Get()>TR_new[0]) && (inputIt.Get()<=TG_new[0])){
outputIt.Set( ((inputIt.Get()-TR_new[0])/(TG_new[0]-TR_new[0]))*255 );
}//end if


if (inputIt.Get()>TG_new[0]){
outputIt.Set( 255 );
}//end if

}//end for

//Write new image

  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName );
  writer->SetInput( outputImage );
  writer->Update();



}
