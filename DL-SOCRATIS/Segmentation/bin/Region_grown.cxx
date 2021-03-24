////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#include "Region_grown.h"

#define   NUMBANDS1           1
#define   NUMBANDS2           2
#define   NUMBANDS3           3
#define   NUMDIM1D            1
#define   NUMDIM2D            2
#define   NUMDIM3D            3
#define   NUMDIM4D            4
#define   NUMDIM5D            5

using MeasurementVectorType = itk::Vector< float, 2 >;
using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;


void Region_grown::KLMRGFilterType(const char * m_inputFileName,const char * m_outputFileName, std::vector<int> seedpoints){

typedef itk::Image<itk::Vector<int, NUMBANDS2>, NUMDIM2D>    ImageType;
typedef itk::Image<unsigned int, NUMBANDS2>    ImageType2;
typedef itk::Image<itk::Vector<double, NUMBANDS2>, NUMDIM2D> OutputImageType;
typedef itk::Image<itk::Vector<unsigned char, NUMBANDS2>, NUMDIM2D> OutputImageType2;
using OutputImageType3 = itk::Image<unsigned char, 2 >;



using ReaderType = itk::ImageFileReader< OutputImageType2 >;
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(m_inputFileName);
	reader->Update();

using FilterType2 = itk::CastImageFilter<  OutputImageType2, ImageType >;
FilterType2::Pointer filter2 = FilterType2::New();
	filter2->SetInput(reader->GetOutput() ); 
	filter2->Update();


  // Set the filter with valid inputs

typedef itk::KLMRegionGrowImageFilter<ImageType, OutputImageType> KLMRegionGrowImageFilterType;
typedef KLMRegionGrowImageFilterType::RegionLabelType LabelType;
typedef itk::Image<LabelType, NUMDIM2D> LabelledImageType;
  KLMRegionGrowImageFilterType::Pointer KLMFilter = KLMRegionGrowImageFilterType::New();

  KLMRegionGrowImageFilterType::GridSizeType gridSize;

  gridSize.Fill(1);
  //gridSize[0]=1;
  //gridSize[1]=2;
//for ( unsigned int i = 0 ; i < seedpoints.size() ; ++i ){
//gridSize[i] =seedpoints[i] ;
//	std::cout << " point "<<seedpoints.size()<<std::endl;
//gridSize[i+1] = seedpoints[i+1];
//}

  KLMFilter->SetInput(  filter2->GetOutput());
  KLMFilter->SetMaximumLambda( 0);//1e45 );

  KLMFilter->SetMaximumNumberOfRegions( 1);//seedpoints.size()/2 );
  KLMFilter->SetGridSize(gridSize);

KLMFilter->PrintAlgorithmBorderStats();
KLMFilter->PrintAlgorithmRegionStats();

OutputImageType::Pointer outImage = KLMFilter->GetOutput();
LabelledImageType::Pointer labelledImage2 = KLMFilter->GetLabelledImage();
	using FilterType = itk::CastImageFilter<  ImageType2, OutputImageType3 >;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput( labelledImage2 );
	filter->Update();

  using WriterType = itk::ImageFileWriter< OutputImageType3 >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName );
  writer->SetInput( filter->GetOutput() );
 writer->Update();


 


}

SampleType::Pointer Region_grown::determine_gridmask(const char * m_inputFileName, const char * m_outputFileName, double TH[2])
{
SampleType::Pointer grid = SampleType::New(); 

THRS[0]=TH[0];
THRS[1]=TH[1];
using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 2 >;
using OutputImageType = itk::Image< double, 2 >;
using ReaderType = itk::ImageFileReader< ImageType >;
using WriterType = itk::ImageFileWriter< ImageType >;
typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< OutputImageType > IteratorType;

	//Write the seed point image
	MeasurementVectorType test;
	ReaderType::Pointer reader = ReaderType::New();
	using FilterType = itk::CastImageFilter< ImageType, OutputImageType >;
	ImageType::Pointer  inputImage = ImageType::New();
	OutputImageType::Pointer outputImage = OutputImageType::New();
	reader->SetFileName( m_inputFileName);
	reader->Update();
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput( reader->GetOutput() );
	filter->Update();
	outputImage = filter->GetOutput();
	/////////////////copy allocate output image///////////////
	inputImage->SetRegions( outputImage->GetRequestedRegion() );
	inputImage->CopyInformation( outputImage );
	inputImage->Allocate();

	ConstIteratorType imageit( inputImage, inputImage->GetRequestedRegion()); 
	IteratorType imagefit( outputImage, outputImage->GetRequestedRegion()); 
	std::cout <<" Check threshold: ["<<THRS[0]<<", "<<THRS[1]<<"] "<<std::endl;

	for ( imageit.GoToBegin(), imagefit.GoToBegin(); !imageit.IsAtEnd(); ++imageit, ++imagefit)
	{
		int terminate=0;
		ImageType::IndexType idx2 = imageit.GetIndex();
		test[0]=idx2[0];
		test[1]=idx2[1];
		
		if ((imagefit.Get()<(THRS[1])) && (imagefit.Get()>(THRS[0])) ){
		std::cout << " point intensity: "<<imagefit.Get()<<std::endl;
			imagefit.Set(255 );
			grid->PushBack(test);

		}
		else { imagefit.Set(0);}

	}//end for

	//from float to char
	using FilterType_c2f = itk::CastImageFilter<OutputImageType,ImageType>;
	FilterType_c2f::Pointer filter_c2f = FilterType_c2f::New();
	filter_c2f->SetInput(   outputImage );
	filter_c2f->Update();
	//Write new image
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( m_outputFileName );
	writer->SetInput( filter_c2f->GetOutput() );
	writer->Update();

	return grid;

}


void Region_grown::Connected_threshold(const char * m_inputFileName,const char * m_outputFileName, float low, float high, int seed[2])
{
 typedef   float           InternalPixelType;
  const     unsigned int    Dimension = 2;
  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;
  typedef unsigned char                            OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  typedef itk::CastImageFilter< InternalImageType, OutputImageType > CastingFilterType;
  CastingFilterType::Pointer caster = CastingFilterType::New();

  typedef  itk::ImageFileReader< InternalImageType > ReaderType;
  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( m_inputFileName );
  writer->SetFileName( m_outputFileName );


  typedef itk::CurvatureFlowImageFilter< InternalImageType, InternalImageType > CurvatureFlowImageFilterType;

  CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();

  typedef itk::ConnectedThresholdImageFilter< InternalImageType,  InternalImageType > ConnectedFilterType;

  ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
 
  smoothing->SetInput( reader->GetOutput() );
  connectedThreshold->SetInput( smoothing->GetOutput() );
  caster->SetInput( connectedThreshold->GetOutput() );
  writer->SetInput( caster->GetOutput() );

  smoothing->SetNumberOfIterations( 5 );
  smoothing->SetTimeStep( 0.125 );
 

  const InternalPixelType lowerThreshold = ( low );
  const InternalPixelType upperThreshold = ( high );


  connectedThreshold->SetLower(  lowerThreshold  );
  connectedThreshold->SetUpper(  upperThreshold  );
  connectedThreshold->SetReplaceValue( 255 );  
  InternalImageType::IndexType  index;
  index[0] =  seed[0] ;
  index[1] =  seed[1] ;
  connectedThreshold->SetSeed( index );
  writer->Update();


}


void Region_grown::Neighboor_Connected_threshold(const char * m_inputFileName,const char * m_outputFileName, float low, float high, int seed[2], int rad[2])
{


 typedef   float           InternalPixelType;
  const     unsigned int    Dimension = 2;
  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;
  typedef unsigned char                            OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  typedef itk::CastImageFilter< InternalImageType, OutputImageType > CastingFilterType;
  CastingFilterType::Pointer caster = CastingFilterType::New();


  typedef  itk::ImageFileReader< InternalImageType > ReaderType;
  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( m_inputFileName);
  writer->SetFileName( m_outputFileName );


  typedef   itk::CurvatureFlowImageFilter<InternalImageType, InternalImageType> CurvatureFlowImageFilterType;

  CurvatureFlowImageFilterType::Pointer smoothing =          CurvatureFlowImageFilterType::New();

  typedef itk::NeighborhoodConnectedImageFilter<InternalImageType,    InternalImageType > ConnectedFilterType;

  ConnectedFilterType::Pointer neighborhoodConnected = ConnectedFilterType::New();

  smoothing->SetInput( reader->GetOutput() );
  neighborhoodConnected->SetInput( smoothing->GetOutput() );
  caster->SetInput( neighborhoodConnected->GetOutput() );
  writer->SetInput( caster->GetOutput() );
  smoothing->SetNumberOfIterations( 5 );
  smoothing->SetTimeStep( 0.125 );

  const InternalPixelType lowerThreshold = ( low );
  const InternalPixelType upperThreshold = ( high );

  neighborhoodConnected->SetLower(  lowerThreshold  );
  neighborhoodConnected->SetUpper(  upperThreshold  );

  InternalImageType::SizeType   radius;

  radius[0] = rad[0];   // two pixels along X
  radius[1] = rad[1];   // two pixels along Y

  neighborhoodConnected->SetRadius( radius );


  InternalImageType::IndexType  index;

  index[0] = seed[0];
  index[1] = seed[1];


  neighborhoodConnected->SetSeed( index );
  neighborhoodConnected->SetReplaceValue( 255 );
 
    writer->Update();

}

void Region_grown::Confident_Connected(const char * m_inputFileName, const char * m_outputFileName, int seed[2], float multiplier, int radius)
{
 typedef   float           InternalPixelType;
  const     unsigned int    Dimension = 2;
  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;
  typedef unsigned char                            OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

  typedef itk::CastImageFilter< InternalImageType, OutputImageType > CastingFilterType;
  CastingFilterType::Pointer caster = CastingFilterType::New();


  typedef  itk::ImageFileReader< InternalImageType > ReaderType;
  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( m_inputFileName );
  writer->SetFileName( m_outputFileName );

  typedef itk::CurvatureFlowImageFilter< InternalImageType, InternalImageType > CurvatureFlowImageFilterType;

  CurvatureFlowImageFilterType::Pointer smoothing =   CurvatureFlowImageFilterType::New();
  typedef itk::ConfidenceConnectedImageFilter<InternalImageType, InternalImageType> ConnectedFilterType;

  ConnectedFilterType::Pointer confidenceConnected = ConnectedFilterType::New();
  smoothing->SetInput( reader->GetOutput() );
  confidenceConnected->SetInput( smoothing->GetOutput() );
  caster->SetInput( confidenceConnected->GetOutput() );
  writer->SetInput( caster->GetOutput() );
  smoothing->SetNumberOfIterations( 10 );
  smoothing->SetTimeStep( 0.125 );

  confidenceConnected->SetMultiplier( multiplier );

  confidenceConnected->SetNumberOfIterations( 2000 );

  confidenceConnected->SetReplaceValue( 255 );


  InternalImageType::IndexType  index;

  index[0] = seed[0];
  index[1] = seed[1];



  confidenceConnected->SetSeed( index );

  confidenceConnected->SetInitialNeighborhoodRadius( radius );

    writer->Update();

}

void Region_grown::Vector_Confident_Connected(const char * m_inputFileName, const char * m_outputFileName, float low, float high, int seed[2], float multiplier, int radius)
{
  typedef   unsigned char                         PixelComponentType;
  typedef   itk::RGBPixel< PixelComponentType >   InputPixelType;
  const     unsigned int    Dimension = 2;
  typedef itk::Image< InputPixelType, Dimension >  InputImageType;
  typedef unsigned char                            OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;


  typedef  itk::ImageFileReader<  InputImageType   > ReaderType;
  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( m_inputFileName );
  writer->SetFileName( m_outputFileName);


  typedef  itk::VectorConfidenceConnectedImageFilter< InputImageType, OutputImageType > ConnectedFilterType;

  ConnectedFilterType::Pointer confidenceConnected = ConnectedFilterType::New();
  confidenceConnected->SetInput( reader->GetOutput() );
  writer->SetInput( confidenceConnected->GetOutput() );
 

  const double multiplier2 = multiplier;


  confidenceConnected->SetMultiplier( multiplier2 );


  const unsigned int iterations = ( 100 );


  confidenceConnected->SetNumberOfIterations( iterations );

  confidenceConnected->SetReplaceValue( 255 );


  InputImageType::IndexType  index;

  index[0] = ( seed[0] );
  index[1] = ( seed[1] );


  // Software Guide : BeginCodeSnippet
  confidenceConnected->SetSeed( index );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The size of the initial neighborhood around the seed is defined with the
  //  method \code{SetInitialNeighborhoodRadius()}. The neighborhood will be
  //  defined as an $N$-Dimensional rectangular region with $2r+1$ pixels on
  //  the side, where $r$ is the value passed as initial neighborhood radius.
  //
  //  Software Guide : EndLatex

  confidenceConnected->SetInitialNeighborhoodRadius( radius );

    writer->Update();

}


MeasurementVectorType Region_grown::determine_nearest_point(SampleType::Pointer  map_points, int case_heap)
{

MeasurementVectorType point;
	using SubsampleType = itk::Statistics::Subsample<SampleType>;

	SubsampleType::Pointer sample_dist_sort = SubsampleType::New();

	if (case_heap==0){
		sample_dist_sort->SetSample(map_points);
		sample_dist_sort->InitializeWithAllInstances();
		itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_dist_sort,0, 0, sample_dist_sort->Size() );
	}
	if (case_heap==1){
		sample_dist_sort->SetSample(map_points);
		sample_dist_sort->InitializeWithAllInstances();
		itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_dist_sort,1, 0, sample_dist_sort->Size() );
	}

int median=(map_points->Size()*0.5)-1;
//if sample_dist_sort->GetMeasurementVector(0)[0]<
	point[0]=sample_dist_sort->GetMeasurementVector(median)[0];
	point[1]=sample_dist_sort->GetMeasurementVector(median)[1];
return point;

}



void Region_grown::compute_entropy_of_label()
{
typedef itk::Image<itk::Vector<int, NUMBANDS2>, NUMDIM2D>    ImageType;
typedef itk::Image<itk::Vector<double, NUMBANDS2>, NUMDIM2D> OutputImageType;
typedef itk::KLMRegionGrowImageFilter<ImageType, OutputImageType>
  KLMRegionGrowImageFilterType;

  KLMRegionGrowImageFilterType::Pointer KLMFilter =
    KLMRegionGrowImageFilterType::New();
typedef KLMRegionGrowImageFilterType::RegionLabelType LabelType;
typedef itk::Image<LabelType, NUMDIM2D> LabelledImageType;
  std::cout << "Extracting and checking label image" << std::endl;

  typedef itk::Statistics::ScalarImageToHistogramGenerator<LabelledImageType>
  HistogramGeneratorType;

  HistogramGeneratorType::Pointer histogramGenerator =
    HistogramGeneratorType::New();

  histogramGenerator->SetInput( KLMFilter->GetLabelledImage() );
  histogramGenerator->SetNumberOfBins( KLMFilter->GetNumberOfRegions() );
  histogramGenerator->SetMarginalScale( 1.0 );
  histogramGenerator->Compute();

  typedef HistogramGeneratorType::HistogramType HistogramType;

  const HistogramType * histogram = histogramGenerator->GetOutput();

  const unsigned int histogramSize = histogram->Size();
  if( histogramSize != KLMFilter->GetNumberOfRegions() )
    {
    std::cout << "Test FAILED" << std::endl;
    }

  HistogramType::ConstIterator histIt = histogram->Begin();
  HistogramType::ConstIterator histItEnd = histogram->End();

  double Sum = histogram->GetTotalFrequency();
  double labelEntropy = 0.0;

  while( histIt != histItEnd )
    {

    double probability = histIt.GetFrequency() / Sum;

    if( probability == 0 )
      {
      std::cout << "Test FAILED" << std::endl;

      }

    labelEntropy -= probability * vcl_log( probability );

    ++histIt;
    }
unsigned int numPixels = 200;
//numPixels != KLMFilter->GetNumberOfRegions() 
  labelEntropy /= vcl_log( 2.0 );
  double idealEntropy = -vcl_log( 8.0 / numPixels ) / vcl_log( 2.0 );


  std::cout << "Label entropy = " << labelEntropy << " bits " << std::endl;
  std::cout << "Ideal entropy = " << idealEntropy << " bits " << std::endl;

//https://github.com/hinerm/ITK/blob/master/Modules/Segmentation/KLMRegionGrowing/test/itkRegionGrow2DTest.cxx

}
