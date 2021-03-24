////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#include "mergeimages.h"
#include <iostream>




void mergeimages::Image_overlap(const char * m_inputFileName1, const char * m_inputFileName2, const char * m_outputFileName)
{


std::shared_ptr<BinaryImage> B =std::make_shared<BinaryImage>();

B->Image_binary_intensity_handle(m_inputFileName2, m_inputFileName2, 120 ,255, 115,0);
B->Image_binary_intensity_handle(m_inputFileName1, m_inputFileName1, 120 ,255, 115,0);


Image_add( m_inputFileName1,  m_inputFileName2,  m_outputFileName);
for ( unsigned int i = 0 ; i < 20 ; ++i ){
									std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
									BI->Image_binary(m_outputFileName, m_outputFileName, 200 ,255);
									std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
									SM1->Median_smoothing(m_outputFileName,m_outputFileName,1,1);
}





}


void mergeimages::Image_merge(const char * m_inputFileName1, const char * m_inputFileName2, const char * m_outputFileName)
{


  constexpr unsigned int Dimension = 2;

  using InputPixelType = unsigned char;
  using InputImageType = itk::Image< InputPixelType, Dimension >;

  using ReaderType = itk::ImageFileReader< InputImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName1 );
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( m_inputFileName2 );

  using SubtractImageFilterType = itk::SubtractImageFilter <InputImageType, InputImageType >;
  SubtractImageFilterType::Pointer SubFilter = SubtractImageFilterType::New ();
  SubFilter->SetInput1(reader->GetOutput());
  SubFilter->SetInput2(reader2->GetOutput());
  SubFilter->Update();

  using WriterType2 = itk::ImageFileWriter<  InputImageType >;
  WriterType2::Pointer writer2 = WriterType2::New();
  writer2->SetFileName( m_outputFileName );
  writer2->SetInput( SubFilter->GetOutput() );
  writer2->Update();


 


}

void mergeimages::Image_add(const char * m_inputFileName1, const char * m_inputFileName2, const char * m_outputFileName)
{


  constexpr unsigned int Dimension = 2;

  using InputPixelType = unsigned char;
  using InputImageType = itk::Image< InputPixelType, Dimension >;

  using ReaderType = itk::ImageFileReader< InputImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName1 );
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( m_inputFileName2 );

  using AddImageFilter = itk::AddImageFilter <InputImageType, InputImageType >;
  AddImageFilter::Pointer addFilter = AddImageFilter::New ();
  addFilter->SetInput1(reader->GetOutput());
  addFilter->SetInput2(reader2->GetOutput());
  addFilter->Update();

  using WriterType2 = itk::ImageFileWriter<  InputImageType >;
  WriterType2::Pointer writer2 = WriterType2::New();
  writer2->SetFileName( m_outputFileName );
  writer2->SetInput( addFilter->GetOutput() );
  writer2->Update();

}


void mergeimages::Image_add_const(const char * m_inputFileName1,int constant, const char * m_outputFileName)
{


  constexpr unsigned int Dimension = 2;

  using InputPixelType = unsigned char;
  using InputImageType = itk::Image< InputPixelType, Dimension >;

  using ReaderType = itk::ImageFileReader< InputImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName1 );


  using AddImageFilter = itk::AddImageFilter <InputImageType, InputImageType >;
  AddImageFilter::Pointer addFilter = AddImageFilter::New ();
  addFilter->SetInput1(reader->GetOutput());
  addFilter->SetConstant2(constant);
  addFilter->Update();

  using WriterType2 = itk::ImageFileWriter<  InputImageType >;
  WriterType2::Pointer writer2 = WriterType2::New();
  writer2->SetFileName( m_outputFileName );
  writer2->SetInput( addFilter->GetOutput() );
  writer2->Update();

}


void mergeimages::Crop_binary(const char * inputFileName1, const char * inputFileName2, const char * outputFileName)
{
using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 2 >;
using ReaderType = itk::ImageFileReader< ImageType >;
using WriterType = itk::ImageFileWriter< ImageType >;
using MaskFilterType = itk::MaskImageFilter< ImageType, ImageType >;
MaskFilterType::Pointer maskFilter = MaskFilterType::New();
WriterType::Pointer writer = WriterType::New();
WriterType::Pointer writer2 = WriterType::New();
ReaderType::Pointer reader = ReaderType::New();
ReaderType::Pointer reader2 = ReaderType::New();
ReaderType::Pointer reader3 = ReaderType::New();
const char * m_angle="0";
std::shared_ptr<Change_information> CI =std::make_shared<Change_information>();
CI ->Change_information_run(inputFileName1,  inputFileName2);
reader->SetFileName( inputFileName1);
reader->Update();
ImageType::Pointer mask = ImageType::New();
ImageType::Pointer mask2 = ImageType::New();

if (inverse==false){
	reader2->SetFileName( inputFileName2);
	reader2->Update();
	mask=reader2->GetOutput();
	writer2->SetFileName( (root+"mask.jpg").c_str() );
	writer2->SetInput( mask );
	writer2->Update();
}

if (inverse==true){
	using InvertIntensityImageFilterType = itk::InvertIntensityImageFilter <ImageType>;
	InvertIntensityImageFilterType::Pointer invertIntensityFilter= InvertIntensityImageFilterType::New();
	reader2->SetFileName( inputFileName2);
	reader2->Update();
	invertIntensityFilter->SetInput(reader2->GetOutput());
	invertIntensityFilter->SetMaximum(254);
	mask=invertIntensityFilter->GetOutput();
	writer2->SetFileName(  (root+"mask.jpg").c_str() );
	writer2->SetInput( mask );
	writer2->Update();
}

//Write new image
CI ->Change_information_run(inputFileName1,   (root+"mask.jpg").c_str() );
reader3->SetFileName(   (root+"mask.jpg").c_str() );
reader3->Update();
mask2=reader3->GetOutput();
maskFilter->SetInput(reader->GetOutput());
maskFilter->SetMaskImage(mask2);
//maskFilter->Update();
writer->SetFileName( outputFileName );
writer->SetInput( maskFilter->GetOutput() );
writer->Update();
}

void mergeimages::Crop_cicle(const char * inputFileName1, int centerx, int centey, int radius, const char * outputFileName)
{
using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 2 >;
using ReaderType = itk::ImageFileReader< ImageType >;
using WriterType = itk::ImageFileWriter< ImageType >;
WriterType::Pointer writer = WriterType::New();
 ImageType::Pointer image = ImageType::New();
ReaderType::Pointer reader = ReaderType::New();
  // Create an image cicle
  ImageType::RegionType region;
  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  ImageType::SizeType size;
  unsigned int NumRows = 512;
  unsigned int NumCols = 512;
  size[0] = NumRows;
  size[1] = NumCols;
  region.SetSize(size);
  region.SetIndex(start);
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer( itk::NumericTraits<ImageType::PixelType>::Zero);


 std::shared_ptr<Change_information> CI =std::make_shared<Change_information>();
  writer->SetFileName( (root+"cicle_mask.jpg").c_str() );
  writer->SetInput( image );
  writer->Update();

 CI ->Change_information_run(inputFileName1,  (root+"cicle_mask.jpg").c_str());
 reader->SetFileName( (root+"cicle_mask.jpg").c_str());
reader->Update();
image=(reader->GetOutput());

image->Allocate();
//std::cout <<radius<<" r"<<std::endl; 
//std::cout <<centerx<<" center"<<std::endl;
//std::cout <<centey<<" centery"<<std::endl;
//constrains x
unsigned int c=0;
 int start_r;
 int end_r;
end_r=centerx+radius;
start_r=centerx-radius;
if (end_r>511){end_r=511;}
//if (start_r<(-511)){start_r=0;}
if (start_r<0){start_r=0;}//{start_r=std::abs(start_r); if (start_r>centerx){start_r=0;}}

  for(unsigned int r = start_r; r < end_r; r++)
    {

		//constrains y
		int end=0;
		int start=0;
		double ro=std::pow(radius,2);
		int update_x=std::abs(r-centerx);
		int update_x1=std::abs(r-centerx);
		if (start_r==0){update_x1=std::abs(r);}
		if (end_r==511){update_x=std::abs(r);}
		double xnew=std::pow(update_x,2);
		double xnew1=std::pow(update_x1,2);
		end= (int) std::sqrt(std::abs(ro-xnew))+centey;
		start=(int) std::sqrt(std::abs(ro-xnew1))-centey;
		//std::cout << start<<std::endl;
		if (end>511){end=511;}
		if (start<(-511)){start=0;}
		if (start<0){start=std::abs(start);}
		if ((centey-radius)<0){start=0;}
		if ((centey+radius)>511){end=511;}		
		//std::cout << ro<<std::endl;
		//std::cout << xnew<<std::endl; 
		//std::cout << update_x <<std::endl;
		//std::cout << end<<std::endl; 
		//std::cout << start<<std::endl; 
		  for( c = start; c < end; c++)
		  {
			      ImageType::IndexType pixelIndex;
			      pixelIndex[0] = r;
			      pixelIndex[1] = c; 

			      image->SetPixel(pixelIndex, 254);



		   }

    }
  writer->SetFileName( (root+"cicle_mask.jpg").c_str() );
  //reader->SetInput( image );
 // reader->Update();
  writer->SetInput( image );
  writer->Update();




mergeimages::Crop_binary(inputFileName1, (root+"cicle_mask.jpg").c_str(), outputFileName);





}

double mergeimages::Compare_images(const char * m_inputFileNameoriginal, const char * m_inputFileNametest){

  using ImageType = itk::Image<double,2>;
  using OutputType = itk::Image<unsigned char,2>;
  using DiffOutputType = itk::Image<unsigned char,2>;
  using ReaderType = itk::ImageFileReader<ImageType>;

  ReaderType::Pointer baselineReader = ReaderType::New();
  baselineReader->SetFileName(m_inputFileNameoriginal);
  baselineReader->Update();
  // Read the file generated by the test
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName(m_inputFileNametest);
  testReader->Update();

  using DiffType = itk::Testing::ComparisonImageFilter<ImageType,ImageType>;
  DiffType::Pointer diff = DiffType::New();
    diff->SetValidInput(baselineReader->GetOutput());
    diff->SetTestInput(testReader->GetOutput());
    diff->SetDifferenceThreshold( 100 );
   // diff->SetToleranceRadius( radiusTolerance );
    diff->UpdateLargestPossibleRegion();
  bool differenceFailed = false;
  const double averageIntensityDifference = diff->GetNumberOfPixelsWithDifferences();

return averageIntensityDifference;







}







