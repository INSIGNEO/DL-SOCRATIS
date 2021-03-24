////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1
//Issues: modification of TH of max/min rate
//////////////////////////////////////////////////////////////////////////////////
#include "prewatershed.h"

void prewatershed::image_create(const char * inputFileName, const char * inputFileName2, const char * outputFileName, threshold TR){

 std::cout << "Preprocessing the watershed segmentation input image ..." << '\n' ;
  constexpr unsigned int Dimension = 2;
  using PixelType = unsigned char;
  using ImageType = itk::Image< PixelType, Dimension >;
//read first image
  using ReaderType = itk::ImageFileReader< ImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );
//read the second image
  using ReaderType2 = itk::ImageFileReader< ImageType >;
  ReaderType2::Pointer reader2 = ReaderType2::New();
  reader2->SetFileName( inputFileName2 );

typedef itk::Vector< float, 2 > MeasurementVectorType;
typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

////////////////////convert to double image for itteration/////////////////
// 1st image
  using Out2putPixelType = float;
  using Out2putImageType = itk::Image< Out2putPixelType, Dimension >;

  using FilterType2 = itk::CastImageFilter< ImageType, Out2putImageType >;
  FilterType2::Pointer filter2 = FilterType2::New();
  filter2->SetInput( reader->GetOutput() );

  using WriterType2 = itk::ImageFileWriter< Out2putImageType >;
  WriterType2::Pointer writer2 = WriterType2::New();
  writer2->SetFileName("draft_image_watershed.nrrd" );
  writer2->SetInput( filter2->GetOutput() );
  writer2->Update();

//2nd image
  FilterType2::Pointer filter3 = FilterType2::New();
  filter3->SetInput( reader2->GetOutput() );

  WriterType2::Pointer writer3 = WriterType2::New();
  writer3->SetFileName("draft_image_watershed2.nrrd" );
  writer3->SetInput( filter3->GetOutput() );
  writer3->Update();

//////////////////start the process/////////////
Out2putImageType::Pointer inputImage = filter2->GetOutput();
Out2putImageType::Pointer inputImage2 = filter3->GetOutput();
ImageType::Pointer outputImage = ImageType::New();
/////////////////copy allocate output image///////////////
outputImage->SetRegions( inputImage->GetRequestedRegion() );
outputImage->CopyInformation( inputImage );
outputImage->Allocate();
///////////////////////define itterators///////////////////
typedef itk::ImageRegionConstIterator< Out2putImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< ImageType > IteratorType;

ConstIteratorType inputIt( inputImage, inputImage->GetRequestedRegion() );
IteratorType outputIt( outputImage, outputImage->GetRequestedRegion() );

for ( inputIt.GoToBegin(), outputIt.GoToBegin(); !inputIt.IsAtEnd(); ++inputIt,  ++outputIt)
{

outputIt.Set(inputIt.Get());
if(inputIt.Get()<TR[0]){
outputIt.Set(0);
//std::cout << "index of pixel1: " << inputIt.Get() << '\n' ;
}//end if

}//end for

ConstIteratorType inputIt2( inputImage2, inputImage2->GetRequestedRegion() );
IteratorType outputIt2( outputImage, outputImage->GetRequestedRegion() );
for ( inputIt2.GoToBegin(), outputIt2.GoToBegin(); !inputIt2.IsAtEnd(); ++inputIt2, ++outputIt2)
{
 if (inputIt2.Get()==255){
outputIt2.Set(255);
//std::cout << "index of pixel2: " << inputIt2.Get() << '\n' ;
 }//end if
}


//Write new image

  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFileName );
  writer->SetInput( outputImage );
  writer->Update();


}





