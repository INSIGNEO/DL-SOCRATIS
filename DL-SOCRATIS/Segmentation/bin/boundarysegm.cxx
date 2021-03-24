////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1
//Issues: modification of TH of max/min rate
//////////////////////////////////////////////////////////////////////////////////
#include "boundarysegm.h"
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <interparameters.h> 
void boundarysegm::level_set(const char * inputFileName, const char * outputFileName, const char * couputFileName, int iteration, float RMSerror, float mu, int nu, int vol)
{


  unsigned int nb_iteration = iteration;
  double rms = RMSerror;
  double curvature_weight = mu;
  double area_weight =nu ;
  double volume_weight = vol;
  double volume = 0.;
  double overlap_weight = 0.;
  double l1 = 1;
  double l2 = 1;
  double epsilon = 1;

  const unsigned int Dimension = 2;
  typedef float ScalarPixelType;
  typedef itk::Image< ScalarPixelType, Dimension > LevelSetImageType;
  typedef itk::Image< unsigned char, Dimension > FeatureImageType;
  typedef itk::Image< ScalarPixelType, Dimension > OutputImageType;
 
  typedef itk::ScalarChanAndVeseLevelSetFunctionData< LevelSetImageType,
    FeatureImageType > DataHelperType;
 
  typedef itk::ConstrainedRegionBasedLevelSetFunctionSharedData<
    LevelSetImageType, FeatureImageType, DataHelperType > SharedDataHelperType;
 
  typedef itk::ScalarChanAndVeseLevelSetFunction< LevelSetImageType,
    FeatureImageType, SharedDataHelperType > LevelSetFunctionType;
 
  typedef itk::ScalarChanAndVeseSparseLevelSetImageFilter< LevelSetImageType,
    FeatureImageType, OutputImageType, LevelSetFunctionType,
    SharedDataHelperType > MultiLevelSetType;
 
  typedef itk::ImageFileReader< LevelSetImageType > LevelSetReaderType;
  typedef itk::ImageFileReader< FeatureImageType > FeatureReaderType;
  typedef itk::ImageFileWriter< OutputImageType > WriterType;

  typedef itk::AtanRegularizedHeavisideStepFunction< ScalarPixelType,ScalarPixelType >  DomainFunctionType;
 
  DomainFunctionType::Pointer domainFunction = DomainFunctionType::New();
  domainFunction->SetEpsilon( epsilon );

   //resize the input image to 512x512
  std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  LevelSetReaderType::Pointer contourReader1 = LevelSetReaderType::New();
  Resample1->resample(couputFileName,512,0);
  contourReader1->SetFileName( couputFileName );
  contourReader1->Update();

 std::shared_ptr<Resample> Resample2 =std::make_shared<Resample>();
  Resample2->resample(inputFileName,512,1);


  FeatureReaderType::Pointer featureReader = FeatureReaderType::New();
  featureReader->SetFileName( inputFileName );
  featureReader->Update();





  FeatureImageType::Pointer featureImage = featureReader->GetOutput();
  LevelSetImageType::Pointer contourImage1 = contourReader1->GetOutput();

 
  MultiLevelSetType::Pointer levelSetFilter = MultiLevelSetType::New();
  levelSetFilter->SetFunctionCount( 1 );
  levelSetFilter->SetFeatureImage( featureImage );
  levelSetFilter->SetLevelSet( 0, contourImage1 );
  levelSetFilter->SetNumberOfIterations( nb_iteration );
  levelSetFilter->SetMaximumRMSError( rms );
  levelSetFilter->SetUseImageSpacing( 0 );
  levelSetFilter->SetInPlace( false );
 

    levelSetFilter->GetDifferenceFunction(0)->SetDomainFunction( domainFunction );
    levelSetFilter->GetDifferenceFunction(0)->SetCurvatureWeight( curvature_weight );
    levelSetFilter->GetDifferenceFunction(0)->SetAreaWeight( area_weight );
    levelSetFilter->GetDifferenceFunction(0)->SetOverlapPenaltyWeight( overlap_weight );
    levelSetFilter->GetDifferenceFunction(0)->SetVolumeMatchingWeight( volume_weight );
    levelSetFilter->GetDifferenceFunction(0)->SetVolume( volume );
    levelSetFilter->GetDifferenceFunction(0)->SetLambda1( l1 );
    levelSetFilter->GetDifferenceFunction(0)->SetLambda2( l2 );

  #ifndef NDEBUG
  //std::cout << levelSetFilter << std::endl;
#endif
 
  levelSetFilter->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( levelSetFilter->GetOutput() );
  writer->SetFileName(outputFileName );
  writer->Update();

}

/*
void boundarysegm::compute_PCA(const char *inputfolder, int number_train, int modes_train){

  const   unsigned int        Dimension = 2;
  typedef float    my_PixelType;
  typedef itk::Image< my_PixelType, Dimension >    ImageType2;

  typedef  itk::ImageFileReader< ImageType2 > ReaderType;
  typedef  itk::ImageFileWriter<  ImageType  > WriterType;
  typedef itk::MultiplyImageFilter < ImageType2 , ImageType2 ,ImageType2 > ScaleType;

  unsigned int nb_train= number_train;

using FilterType = itk::CastImageFilter< ImageType, ImageType2  >;
FilterType::Pointer filter2 = FilterType::New();

using FilterType2 = itk::CastImageFilter< ImageType2, ImageType  >;
FilterType2::Pointer filter3 = FilterType2::New();
std::vector<ImageType2::Pointer> trainingImages( nb_train );




  itk::NumericSeriesFileNames::Pointer fileNamesCreator =itk::NumericSeriesFileNames::New();

  fileNamesCreator->SetStartIndex( 0 );
  fileNamesCreator->SetEndIndex( nb_train - 1 );
  fileNamesCreator->SetSeriesFormat( inputfolder);
  const std::vector<std::string> & shapeModeFileNames = fileNamesCreator->GetFileNames();



  for ( unsigned int k = 0; k < nb_train; k++ )
    {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( shapeModeFileNames[k].c_str() );
    reader->Update();
    trainingImages[k] = reader->GetOutput();

    }
  typedef itk::ImagePCAShapeModelEstimator<ImageType2,   ImageType2 >  my_Estimatortype;
  my_Estimatortype::Pointer filter = my_Estimatortype::New();
  filter->SetNumberOfTrainingImages(nb_train);
  filter->SetNumberOfPrincipalComponentsRequired(modes_train);
for ( unsigned int k = 0; k < nb_train; k++ )
    {
    filter->SetInput(k, trainingImages[k] );
    }


  unsigned int nb_modes=modes_train;

  itk::NumericSeriesFileNames::Pointer fileNamesOutCreator = itk::NumericSeriesFileNames::New();

  fileNamesOutCreator->SetStartIndex( 0 );
  fileNamesOutCreator->SetEndIndex( nb_modes-1  );
//////////////////////////////
    // Creating a directory 
    if (mkdir("PCA", 0777) == -1) 
        cerr << "Error :  " << strerror(errno) << endl; 
  
    else
        cout << "Directory created" << std::endl;
/////////////////////////////////////////
  fileNamesOutCreator->SetSeriesFormat( "PCA/PCA_combonent_%d.jpg" );
  const std::vector<std::string> & outFileNames = fileNamesOutCreator->GetFileNames();

  ScaleType::Pointer scaler = ScaleType::New();

  filter->Update();
  my_Estimatortype::VectorOfDoubleType v=filter->GetEigenValues();
  double sv_mean=sqrt(v[0]);
//   std::cout << "v: " << v[2] << std::endl;
//std::cout << "v: " << v[1] << std::endl;
//std::cout << "v: " << v[0] << std::endl;
//v[modes_train-1]=v[modes_train-1]*10000000000000;
  for ( unsigned int k = 0; k < nb_modes; k++ )
    {
    double sv=sqrt(std::abs(v[k]));
    double sv_n=(sv/sv_mean)*255;
    //double sv_n=sv;
    std::cout << "writing: " << outFileNames[k] << std::endl;

    std::cout << "svd[" << k << "]: " << sv << " norm: " << sv_n << std::endl;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outFileNames[k].c_str() );
    scaler->SetInput(filter->GetOutput(k));
    scaler->SetConstant(sv_n);
     filter3->SetInput( scaler->GetOutput() );
    writer->SetInput( filter3->GetOutput() );
    writer->Update();
    }


}

*/

void boundarysegm::geodesic_active_contour(const char * inputFileName, const char * outputFileName, const char * couputFileName_endo, const char * couputFileName_epi, double prob, double prio, int number)
{


// In \cite{Leventon2000}, Leventon \emph{et al.} extended the
// geodesic active contours method with an additional shape-influenced term in
// the driving PDE. The \doxygen{GeodesicActiveContourShapePriorLevelSetFilter}
// is a generalization of Leventon's approach and its use is illustrated
// in the following example.
// https://itk.org/Doxygen/html/Examples_2Segmentation_2GeodesicActiveContourShapePriorLevelSetImageFilter_8cxx-example.html#_a15





std::cout<<" "<<std::endl;
std::cout<<"Start the geodestic active contour combute..."<<std::endl;
 
  using InternalPixelType = float;
  constexpr unsigned int Dimension = 2;
  using InternalImageType = itk::Image< InternalPixelType, Dimension >;
  using ThresholdingFilterType = itk::BinaryThresholdImageFilter<InternalImageType, ImageType   >;
  ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
  thresholder->SetLowerThreshold( -1000.0 );
  thresholder->SetUpperThreshold(     0.0 );
  thresholder->SetOutsideValue(  255  );
  thresholder->SetInsideValue(  0 );
  // We instantiate reader and writer types in the following lines.
  //
  using ReaderType = itk::ImageFileReader< InternalImageType >;
  using ReaderType2 = itk::ImageFileReader< ImageType >;
  using WriterType = itk::ImageFileWriter<  ImageType  >;
  ReaderType::Pointer reader = ReaderType::New();
ReaderType2::Pointer reader1 = ReaderType2::New();
ReaderType2::Pointer reader2 = ReaderType2::New();
ReaderType2::Pointer reader3 = ReaderType2::New();
  WriterType::Pointer writer = WriterType::New();
  reader->SetFileName(inputFileName );
  writer->SetFileName( outputFileName);
  //  The RescaleIntensityImageFilter type is declared below. This filter will
  //  renormalize image before sending them to writers.
  //
  using CastFilterType = itk::RescaleIntensityImageFilter<InternalImageType, ImageType >;
  using SmoothingFilterType = itk::CurvatureAnisotropicDiffusionImageFilter<InternalImageType, InternalImageType >;
  using GradientFilterType = itk::GradientMagnitudeRecursiveGaussianImageFilter<InternalImageType,InternalImageType >;
  using FastMarchingFilterType = itk::FastMarchingImageFilter<InternalImageType,InternalImageType >;
  using GeodesicActiveContourFilterType = itk::GeodesicActiveContourShapePriorLevelSetImageFilter<InternalImageType, InternalImageType >;
//  using GeodesicActiveContourFilterType = itk::GeodesicActiveContourLevelSetImageFilter< InternalImageType,InternalImageType >;
using CenterFilterType =itk::ChangeInformationImageFilter<InternalImageType>;
  using ReciprocalFilterType = itk::BoundedReciprocalImageFilter<InternalImageType,InternalImageType >;
  using NodeContainer = FastMarchingFilterType::NodeContainer;
  using NodeType = FastMarchingFilterType::NodeType;
  using SigmoidFilterType = itk::SigmoidImageFilter<InternalImageType,InternalImageType >;
typedef  itk::ImageFileWriter<  ImageType  > WriterType2;

  SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
  GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
  FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();
  GeodesicActiveContourFilterType::Pointer geodesicActiveContour =GeodesicActiveContourFilterType::New();
  CenterFilterType::Pointer center = CenterFilterType::New();
  CenterFilterType::Pointer center1 = CenterFilterType::New();
  CenterFilterType::Pointer center2 = CenterFilterType::New();
  ReciprocalFilterType::Pointer reciprocal = ReciprocalFilterType::New();
//compute mean endo epi contour
  determine_mean_shape(couputFileName_epi,couputFileName_endo,(root+"mean.jpg").c_str());
  ReaderType::Pointer meanShapeReader = ReaderType::New();
  meanShapeReader->SetFileName( (root+"mean.jpg").c_str() );
  meanShapeReader->Update();

  center->CenterImageOn();

  center->SetInput( reader->GetOutput() );
  smoothing->SetInput( center->GetOutput() );
  gradientMagnitude->SetInput( smoothing->GetOutput() );
 reciprocal->SetInput( gradientMagnitude->GetOutput() );


 
  smoothing->SetTimeStep( 0.02 );
  smoothing->SetNumberOfIterations(  10 );
  smoothing->SetConductanceParameter( 6.0 );
 
  const double sigma = 1; /// CHANGE PARAMETER!!!
  gradientMagnitude->SetSigma(  sigma  );
  


int offset_center=20;
create_geodestic_seed_points(couputFileName_epi,couputFileName_endo);
  NodeContainer::Pointer seeds = NodeContainer::New();
  InternalImageType::IndexType  seedPosition_endo;

  seedPosition_endo[0] = mean_endo[0]+offset_center;
  seedPosition_endo[1] = mean_endo[1]+offset_center;
  const double initialDistance_endo = 1;/// CHANGE PARAMETER!!!
  NodeType node_endo;
  const double seedValue_endo =  -initialDistance_endo;
  node_endo.SetValue( seedValue_endo );
  seeds->Initialize();

 // seeds->InsertElement( 0, node );

  node_endo.SetIndex( seedPosition_endo );
  seeds->InsertElement( 0, node_endo );

  seedPosition_endo[0] = mean_endo[0]-offset_center;
  seedPosition_endo[1] = mean_endo[1]+offset_center;
  node_endo.SetIndex( seedPosition_endo );
  seeds->InsertElement( 1, node_endo );

  seedPosition_endo[0] = mean_endo[0]+offset_center;
  seedPosition_endo[1] = mean_endo[1]-offset_center;
  node_endo.SetIndex( seedPosition_endo );
  seeds->InsertElement( 2, node_endo );
  seedPosition_endo[0] = mean_endo[0]-offset_center;
  seedPosition_endo[1] = mean_endo[1]-offset_center;
  node_endo.SetIndex( seedPosition_endo );
  seeds->InsertElement( 3, node_endo );

///////////////////////////////////////////////


  const double initialDistance_epi = 1;/// CHANGE PARAMETER!!!
  NodeType node_epi;
  const double seedValue_epi =  -initialDistance_epi;
  node_epi.SetValue( seedValue_endo );
 InternalImageType::IndexType  seedPosition_epi;
  seedPosition_epi[0] = maxy_epi[0];
  seedPosition_epi[1] = maxy_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 4, node_epi );


  seedPosition_epi[0] = miny_epi[0];
  seedPosition_epi[1] = miny_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 5, node_epi );

  seedPosition_epi[0] = first_epi[0];
  seedPosition_epi[1] = first_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 6, node_epi );

  seedPosition_epi[0] = second_epi[0];
  seedPosition_epi[1] = second_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 7, node_epi );

  seedPosition_epi[0] = third_epi[0];
  seedPosition_epi[1] = third_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 8, node_epi );

  seedPosition_epi[0] = forth_epi[0];
  seedPosition_epi[1] = forth_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 9, node_epi );

  seedPosition_epi[0] = fifth_epi[0];
  seedPosition_epi[1] = fifth_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 10, node_epi );

  seedPosition_epi[0] = mirror_first_epi[0];
  seedPosition_epi[1] = mirror_first_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 11, node_epi );

  seedPosition_epi[0] = mirror_second_epi[0];
  seedPosition_epi[1] = mirror_second_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 12, node_epi );

  seedPosition_epi[0] = mirror_third_epi[0];
  seedPosition_epi[1] = mirror_third_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 13, node_epi );

  seedPosition_epi[0] = mirror_forth_epi[0];
  seedPosition_epi[1] = mirror_forth_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 14, node_epi );

  seedPosition_epi[0] = mirror_fifth_epi[0];
  seedPosition_epi[1] = mirror_fifth_epi[1];
  node_epi.SetIndex( seedPosition_epi );
  seeds->InsertElement( 15, node_epi );

///////////////////////////////////////////////
if (number<2){
  const double initialDistance = 1;/// CHANGE PARAMETER!!!
  NodeType node;
  const double seedValue =  -initialDistance;
  node.SetValue( seedValue );
  const double initialDistance2 = 1;/// CHANGE PARAMETER!!!
  NodeType node2;
  const double seedValue2 =  -initialDistance2;
  node2.SetValue( seedValue2 );
 InternalImageType::IndexType  seedPosition;

if (mean_endo[0]==256 && mean_endo[1]==256){
  seedPosition[0] = 0;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 16, node );

  seedPosition[0] = 150;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 17, node );


  seedPosition[0] = 0;
  seedPosition[1] = 150;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 18, node );

  seedPosition[0] = 360;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 19, node );


  seedPosition[0] = 511;
  seedPosition[1] = 360;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 20, node );

  seedPosition[0] = 50;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 21, node );


  seedPosition[0] = 511;
  seedPosition[1] = 150;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 22, node );

  seedPosition[0] = 360;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 23, node );


  seedPosition[0] = 511;
  seedPosition[1] = 360;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 24, node );

  seedPosition[0] = 0;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 25, node );

  seedPosition[0] = 511;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 26, node );

  seedPosition[0] = 511;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 27, node );
}

if (mean_endo[0]<256 && mean_endo[1]>256){
  seedPosition[0] = 0;
  seedPosition[1] = 0;
  node2.SetIndex( seedPosition );
  seeds->InsertElement( 16, node2 );

  seedPosition[0] = 511;
  seedPosition[1] = 300;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 17, node );


  seedPosition[0] = 511;
  seedPosition[1] = 150;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 18, node );

  seedPosition[0] = 511;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 19, node );

  seedPosition[0] = 361;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 20, node );

  seedPosition[0] = 511;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 21, node );

  seedPosition[0] = 361;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 22, node );

}

if (mean_endo[0]>256 && mean_endo[1]<256){
  seedPosition[0] = 0;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 16, node );

  seedPosition[0] = 0;
  seedPosition[1] = 300;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 17, node );


  seedPosition[0] = 0;
  seedPosition[1] = 150;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 18, node );

  seedPosition[0] = 0;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 19, node );


  seedPosition[0] = 150;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 20, node );

  seedPosition[0] = 511;
  seedPosition[1] = 511;
  node2.SetIndex( seedPosition );
  seeds->InsertElement( 21, node2 );

  seedPosition[0] = 150;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 22, node );


}

if (mean_endo[0]<256 && mean_endo[1]<256){

  seedPosition[0] = 0;
  seedPosition[1] = 511;
  node2.SetIndex( seedPosition );
  seeds->InsertElement( 16, node2 );

  seedPosition[0] = 511;
  seedPosition[1] = 300;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 17, node );


  seedPosition[0] = 511;
  seedPosition[1] = 150;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 18, node );

  seedPosition[0] = 511;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 22, node );

  seedPosition[0] = 361;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 19, node );

  seedPosition[0] = 361;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 21, node );

  seedPosition[0] = 511;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 20, node );
}

if (mean_endo[0]>256 && mean_endo[1]>256){
  seedPosition[0] = 0;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 16, node );

  seedPosition[0] = 0;
  seedPosition[1] = 300;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 17, node );


  seedPosition[0] = 0;
  seedPosition[1] = 150;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 18, node );


  seedPosition[0] = 0;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 19, node );

  seedPosition[0] = 150;
  seedPosition[1] = 0;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 20, node );

  seedPosition[0] = 511;
  seedPosition[1] = 0;
  node2.SetIndex( seedPosition );
  seeds->InsertElement( 21, node2 );

  seedPosition[0] = 150;
  seedPosition[1] = 511;
  node.SetIndex( seedPosition );
  seeds->InsertElement( 22, node );

}
}
/////////////////////////////////////////////////////////////////
  fastMarching->SetTrialPoints(  seeds  );

  fastMarching->SetSpeedConstant( 1.0 );

using FilterType = itk::CastImageFilter< ImageType, InternalImageType  >;
reader1->SetFileName(couputFileName_endo );
reader2->SetFileName( couputFileName_epi);
FilterType::Pointer filter = FilterType::New();
FilterType::Pointer filter2 = FilterType::New();
filter->SetInput( reader1->GetOutput() );
filter2->SetInput( reader2->GetOutput() );

center1->SetInput( filter->GetOutput() );
center2->SetInput( filter2->GetOutput() );

// fastMarching->SetInput(center1->GetOutput());
 

  using SigmoidFilterType = itk::SigmoidImageFilter<InternalImageType,InternalImageType >;

  SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
  sigmoid->SetOutputMinimum(  0.0  );
  sigmoid->SetOutputMaximum(  1.0  );
  sigmoid->SetInput( gradientMagnitude->GetOutput() );
  sigmoid->SetAlpha( 10 );
  sigmoid->SetBeta(  150  );




  CastFilterType::Pointer caster1 = CastFilterType::New();
  CastFilterType::Pointer caster2 = CastFilterType::New();
  CastFilterType::Pointer caster3 = CastFilterType::New();
  CastFilterType::Pointer caster4 = CastFilterType::New();
  WriterType::Pointer writer1 = WriterType::New();
  WriterType::Pointer writer2 = WriterType::New();
  WriterType::Pointer writer3 = WriterType::New();
  WriterType::Pointer writer4 = WriterType::New();
  caster1->SetInput( smoothing->GetOutput() );
  writer1->SetInput( caster1->GetOutput() );
  writer1->SetFileName((root+"GeodesicActiveContourShapePriorImageFilterOutput1.jpg").c_str());
  caster1->SetOutputMinimum(   0 );
  caster1->SetOutputMaximum( 255 );
  writer1->Update();
  caster2->SetInput( gradientMagnitude->GetOutput() );
  writer2->SetInput( caster2->GetOutput() );
  writer2->SetFileName((root+"GeodesicActiveContourShapePriorImageFilterOutput2.jpg").c_str());
  caster2->SetOutputMinimum(   0 );
  caster2->SetOutputMaximum( 255 );
  writer2->Update();
  caster3->SetInput( reciprocal->GetOutput() );
  writer3->SetInput( caster3->GetOutput() );
  writer3->SetFileName((root+"GeodesicActiveContourShapePriorImageFilterOutput3.jpg").c_str());
  caster3->SetOutputMinimum(   0 );
  caster3->SetOutputMaximum( 255 );
  writer3->Update();
  caster4->SetInput( fastMarching->GetOutput() );
  writer4->SetInput( caster4->GetOutput() );
  writer4->SetFileName((root+"GeodesicActiveContourShapePriorImageFilterOutput4.jpg").c_str());
  caster4->SetOutputMinimum(   0 );
  caster4->SetOutputMaximum( 255 );
writer4->Update();


 fastMarching->SetOutputRegion( center->GetOutput()->GetBufferedRegion() );
  fastMarching->SetOutputSpacing(center->GetOutput()->GetSpacing() );
  fastMarching->SetOutputOrigin(center->GetOutput()->GetOrigin() );



  const double propagationScaling = ( prob );
  const double shapePriorScaling  = ( prio );
  //  Software Guide : BeginCodeSnippet
  geodesicActiveContour->SetPropagationScaling( propagationScaling ); //lambda_1 x c []
  geodesicActiveContour->SetShapePriorScaling( shapePriorScaling ); // lambda_2 [0,1]
  geodesicActiveContour->SetCurvatureScaling( 1.0 );//lambda_1 [0,1]
  geodesicActiveContour->SetAdvectionScaling( 1.0 );//lambda_1 [0,1]
  geodesicActiveContour->SetMaximumRMSError( 0.005 );
  geodesicActiveContour->SetNumberOfIterations( 100 );
  geodesicActiveContour->SetNumberOfLayers( 4 );
  geodesicActiveContour->SetInput(  fastMarching->GetOutput() );
  geodesicActiveContour->SetFeatureImage( reciprocal->GetOutput() );



   unsigned int numberOfPCAModes = 1; //only one mode we have the spaecific image


 numberOfPCAModes = std::abs(number);  
if (number==0){ numberOfPCAModes=1;}
 std::vector<InternalImageType::Pointer> shapeModeImages( numberOfPCAModes );


if (number==0){
  reader->SetFileName( couputFileName_endo );
    shapeModeImages[0] =  reader->GetOutput();

}
if (number==1){
  reader->SetFileName( couputFileName_epi );
    shapeModeImages[0] =  reader->GetOutput();
}
if (number==-1){
  reader->SetFileName( (root+"mean.jpg").c_str());
    shapeModeImages[0] =  reader->GetOutput();
}

if (number>=2){

  itk::NumericSeriesFileNames::Pointer fileNamesOutCreator = itk::NumericSeriesFileNames::New();
  fileNamesOutCreator->SetStartIndex( 0 );
  fileNamesOutCreator->SetEndIndex( number-1  );
  fileNamesOutCreator->SetSeriesFormat( (root+"PCA/PCA_combonent_%d.jpg").c_str() );
  const std::vector<std::string> & FileNames = fileNamesOutCreator->GetFileNames();

int o=0;
  for (unsigned int k = 0; k < number; ++k )
    {

    ReaderType::Pointer shapeModeReader = ReaderType::New();
    shapeModeReader->SetFileName( FileNames[k].c_str() );
    std::cout << "reading: " << FileNames[k].c_str() << std::endl;
    shapeModeReader->Update();
  shapeModeImages[k] = shapeModeReader->GetOutput();

}
}

  using ShapeFunctionType = itk::PCAShapeSignedDistanceFunction<double,Dimension,InternalImageType >;
  ShapeFunctionType::Pointer shape = ShapeFunctionType::New();
  shape->SetNumberOfPrincipalComponents(numberOfPCAModes );

  shape->SetMeanImage( shapeModeImages[0] );
  shape->SetPrincipalComponentImages( shapeModeImages ); //only the mean value

  ShapeFunctionType::ParametersType pcaStandardDeviations( numberOfPCAModes );
  pcaStandardDeviations.Fill( 1.0 );
  shape->SetPrincipalComponentStandardDeviations( pcaStandardDeviations );

  using TransformType = itk::Euler2DTransform<double>;
  TransformType::Pointer transform = TransformType::New();
  shape->SetTransform( transform );
//without PCA combonents there is no variation do the sd is not change the shape variation fo the \alpha is fixed!! 
//TODO better use of PCA!!

  using CostFunctionType = itk::ShapePriorMAPCostFunction<InternalImageType, InternalPixelType >;
  


CostFunctionType::Pointer costFunction = CostFunctionType::New();
  CostFunctionType::WeightsType weights;
  weights[0] =  1.0;  // weight for contour fit term (beta)
  weights[1] =  20.0; // weight for image fit term  (lambda)
  weights[2] =  1.0;  // weight for shape prior term (alpha)
  weights[3] =  1.0;  // weight for pose prior term  (p)
  costFunction->SetWeights( weights );

  CostFunctionType::ArrayType mean(     shape->GetNumberOfShapeParameters() );
  CostFunctionType::ArrayType stddev(   shape->GetNumberOfShapeParameters() );
  mean.Fill( 0.0 );
  stddev.Fill( 1.0 );
  costFunction->SetShapeParameterMeans( mean );
  costFunction->SetShapeParameterStandardDeviations( stddev );
//gaussian distribution of P(p)=exp(U(p))
 
  using OptimizerType = itk::OnePlusOneEvolutionaryOptimizer;
  OptimizerType::Pointer optimizer = OptimizerType::New();
 
  using GeneratorType = itk::Statistics::NormalVariateGenerator;
  GeneratorType::Pointer generator = GeneratorType::New();
  generator->Initialize( 20020702 );
  optimizer->SetNormalVariateGenerator( generator );
  
  OptimizerType::ScalesType scales( shape->GetNumberOfParameters() );
  scales.Fill( 1.0 );
  for( unsigned int k = 0; k < numberOfPCAModes; k++ )
    {
    scales[k] = 1.0;  // scales for the pca mode multiplier
    }
  scales[numberOfPCAModes] = 1;  // scale for 2D rotation
  optimizer->SetScales( scales );

//(translation and rotation and scale of parameter distribution U(p))

  double initRadius = 1;//1.05;
  double grow = 2;//1.1;
  double shrink = 0.5;//pow(grow, -0.25);
  optimizer->Initialize(initRadius, grow, shrink);
  optimizer->SetEpsilon(1.0e-6); // minimal search radius
  optimizer->SetMaximumIteration(20);
if (number>=2){  optimizer->SetMaximumIteration(20);}

 // with the initial x- and y- translation specified through command-line
  ShapeFunctionType::ParametersType parameters( shape->GetNumberOfParameters() );
  parameters.Fill( 0.0 );
  parameters[numberOfPCAModes + 1] = mean_endo[0]; 
  parameters[numberOfPCAModes + 2] = mean_endo[1]; 

//fit from the 0,0 point to the center of the mean contour 

  geodesicActiveContour->SetShapeFunction(  shape );
  geodesicActiveContour->SetCostFunction( costFunction );
  geodesicActiveContour->SetOptimizer( optimizer );
  geodesicActiveContour->SetInitialParameters( parameters );
  using CommandType = CommandIterationUpdate<GeodesicActiveContourFilterType>;
  CommandType::Pointer observer = CommandType::New();
  geodesicActiveContour->AddObserver( itk::IterationEvent(), observer );



  thresholder->SetInput( geodesicActiveContour->GetOutput() );
  writer->SetInput( thresholder->GetOutput() );
  writer->Update();

  std::cout << std::endl;
  std::cout << "Max. no. iterations: " << geodesicActiveContour->GetNumberOfIterations() << std::endl;
  std::cout << "Max. RMS error: " << geodesicActiveContour->GetMaximumRMSError() << std::endl;
  std::cout << std::endl;
  std::cout << "No. elpased iterations: " << geodesicActiveContour->GetElapsedIterations() << std::endl;
  std::cout << "RMS change: " << geodesicActiveContour->GetRMSChange() << std::endl;
  std::cout << "Parameters: " << geodesicActiveContour->GetCurrentParameters() << std::endl;

  writer4->Update();

  using InternalWriterType = itk::ImageFileWriter< InternalImageType >;
  InternalWriterType::Pointer mapWriter = InternalWriterType::New();
  mapWriter->SetInput( fastMarching->GetOutput() );
  mapWriter->SetFileName((root+"GeodesicActiveContourShapePriorImageFilterOutput4.mha").c_str());
  mapWriter->Update();
  InternalWriterType::Pointer speedWriter = InternalWriterType::New();
  speedWriter->SetInput( reciprocal->GetOutput() );
  speedWriter->SetFileName((root+"GeodesicActiveContourShapePriorImageFilterOutput3.mha").c_str());
  speedWriter->Update();
  InternalWriterType::Pointer gradientWriter = InternalWriterType::New();
  gradientWriter->SetInput( gradientMagnitude->GetOutput() );
  gradientWriter->SetFileName((root+"GeodesicActiveContourShapePriorImageFilterOutput2.mha").c_str());
  gradientWriter->Update();

 // Also write out the initial and final best fit shape
  using EvaluatorFilterType = itk::SpatialFunctionImageEvaluatorFilter<ShapeFunctionType,InternalImageType,InternalImageType >;
  EvaluatorFilterType::Pointer evaluator = EvaluatorFilterType::New();
  evaluator->SetInput( geodesicActiveContour->GetOutput() );

  shape->SetParameters( geodesicActiveContour->GetInitialParameters() );
  evaluator->SetFunction( shape );
  thresholder->SetInput( evaluator->GetOutput() );
  writer->SetFileName( (root+"GeodesicActiveContourShapePriorImageFilterOutput5.jpg").c_str() );
  writer->SetInput( thresholder->GetOutput() );
  writer->Update();
  shape->SetParameters( geodesicActiveContour->GetCurrentParameters() );
 // evaluator->SetFunction( shape );
  evaluator->Modified();
  thresholder->SetInput( evaluator->GetOutput() );
  writer->SetInput( thresholder->GetOutput() );
  writer->SetFileName( (root+"GeodesicActiveContourShapePriorImageFilterOutput6.jpg").c_str() );
  writer->Update();


 
}

void boundarysegm::determine_mean_shape(const char * inputFileName1, const char * inputFileName2, const char * outputFileName){



 std::cout << "Compute mean contour points..." << '\n' ;

std::shared_ptr<seedpoints> SP_distance_comp =std::make_shared<seedpoints>(root);

SP_distance_comp->create_vector_of_images(inputFileName2,inputFileName1);

SP_distance_comp->compute_distance_of_two_contour();

std::shared_ptr<Segmentation_Pipeline > SP1=std::make_shared<Segmentation_Pipeline >(root);

SampleType::Pointer sample_endo_copy = SampleType::New();
SampleType::Pointer sample_epi_copy = SampleType::New();
SampleType::Pointer sample_mean_copy = SampleType::New();
SampleType::Pointer offset = SampleType::New();

if(SP1->check_black_image((inputFileName2 ),50,40)==false )

{
	sample_endo_copy= SP_distance_comp->GetSample_endo();
	SubsampleType::Pointer sample_endox = SubsampleType::New();
 //std::cout << "size.." <<sample_endo_copy->Size()<< '\n' ;
	// heapSort area

	 sample_endox->SetSample(sample_endo_copy);
	 sample_endox->InitializeWithAllInstances();
	 itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_endox, 0, 0, sample_endox->Size() );

	int median= (int)sample_endox->Size()*0.5 -1 ;
	int bias=(int) sample_endox->Size()*0.001;
	int max=sample_endox->Size()-1-bias;
	int min=bias;

	mean_endo[1]=(int) sample_endo_copy->GetMeasurementVector(min)[1]+ (std::abs(sample_endo_copy->GetMeasurementVector(max)[1]-(int)sample_endo_copy->GetMeasurementVector(min)[1]))*0.5;
	mean_endo[0]=(int) sample_endox->GetMeasurementVector(min)[0] + (sample_endox->GetMeasurementVector(max)[0]-sample_endox->GetMeasurementVector(min)[0])*0.5;



}

std::shared_ptr<Segmentation_Pipeline > SP2=std::make_shared<Segmentation_Pipeline >(root);
if(SP2->check_black_image((inputFileName1 ),50,40)==false )
{
	sample_epi_copy= SP_distance_comp->GetSample_max();

	SubsampleType::Pointer sample_epix = SubsampleType::New();
 //std::cout << "size.." <<sample_epi_copy->Size()<< '\n' ;
	// heapSort area

	 sample_epix->SetSample(sample_epi_copy);
	 sample_epix->InitializeWithAllInstances();
	 itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_epix, 0, 0, sample_epix->Size() );

	int median= (int)sample_epix->Size()*0.5 -1 ;
	int bias=(int) sample_epix->Size()*0.001;
	int max=sample_epix->Size()-1-bias;
	int min=bias;

	mean_epi[1]=(int) sample_epi_copy->GetMeasurementVector(min)[1]+ (std::abs(sample_epi_copy->GetMeasurementVector(max)[1]-(int)sample_epi_copy->GetMeasurementVector(min)[1]))*0.5;
	mean_epi[0]=(int) sample_epix->GetMeasurementVector(min)[0] + (sample_epix->GetMeasurementVector(max)[0]-sample_epix->GetMeasurementVector(min)[0])*0.5;

}


std::shared_ptr<Segmentation_Pipeline > SP12=std::make_shared<Segmentation_Pipeline >(root);
std::shared_ptr<Segmentation_Pipeline > SP13=std::make_shared<Segmentation_Pipeline >(root);
std::shared_ptr<Segmentation_Pipeline > SP14=std::make_shared<Segmentation_Pipeline >(root);

if(SP12->check_black_image((inputFileName2 ),50,40)==false && SP12->check_black_image((inputFileName1 ),50,40)==true)
{
	mean[0]=mean_endo[0];
	mean[1]=mean_endo[1];
}


else if(SP13->check_black_image((inputFileName1 ),50,40)==false && SP13->check_black_image((inputFileName2 ),50,40)==true)
{
	mean[0]=mean_epi[0];
	mean[1]=mean_epi[1];
}

else if(SP14->check_black_image((inputFileName1 ),50,40)==false && SP14->check_black_image((inputFileName2 ),50,40)==false)
{
	int distance_endo=std::abs(mean_endo[0]-255)+std::abs(mean_endo[1]-255);
	int distance_epi=std::abs(mean_epi[0]-255)+std::abs(mean_epi[1]-255);

	if (distance_endo<=distance_epi)
	{

		mean[0]=mean_endo[0];
		mean[1]=mean_endo[1];
	}

	if (distance_endo>distance_epi)
	{

		mean[0]=mean_epi[0];
		mean[1]=mean_epi[1];
	}
}


else
{
	std::cout << "No input images from initial step so center will be in the center of image default" << '\n' ;
	mean[0]=0;
	mean[1]=0;
}







std::cout << "................................" << '\n' ;
}


void boundarysegm::create_geodestic_seed_points(const char * inputFileName1, const char * inputFileName2){

float epi=PORTION_CYCLE_MASK_epi;
float endo =PORTION_CYCLE_MASK_endo;	
if (sensitivity1_param==true){ endo=endo_portion;}
if (sensitivity2_param==true){ epi=epi_portion;}
if (equal_portion==true){ epi=std::min(endo,epi); endo=epi;}


 std::cout << "   Define seed points for geodestic active contour..." << '\n' ;
///Compute the seedpoints
std::shared_ptr<seedpoints> SP_distance_comp2 =std::make_shared<seedpoints>(root);

SP_distance_comp2->create_vector_of_images(inputFileName2,inputFileName1);
SP_distance_comp2->compute_distance_of_two_contour();
SampleType::Pointer sample_epi_copy = SampleType::New();
SampleType::Pointer sample_endo_copy = SampleType::New();
sample_epi_copy= SP_distance_comp2->GetSample_max();
sample_endo_copy= SP_distance_comp2->GetSample_endo();

 SubsampleType::Pointer sample_epix = SubsampleType::New();

// heapSort area

int activeDimension=0;
 sample_epix->SetSample(sample_epi_copy);
 sample_epix->InitializeWithAllInstances();
 itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_epix, activeDimension, 0, sample_epix->Size() );

SubsampleType::Pointer sample_endox = SubsampleType::New();

// heapSort area

 sample_endox->SetSample(sample_endo_copy);
 sample_endox->InitializeWithAllInstances();
 itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_endox, activeDimension, 0, sample_endox->Size() );


maxy_epi=sample_epi_copy->GetMeasurementVector(sample_epi_copy->Size()-1);
miny_epi=sample_epi_copy->GetMeasurementVector(0);
maxy_endo=sample_endo_copy->GetMeasurementVector(sample_endo_copy->Size()-1);
miny_endo=sample_endo_copy->GetMeasurementVector(0);

MeasurementVectorType minx_epi;
MeasurementVectorType maxx_epi;
MeasurementVectorType minx_endo;
MeasurementVectorType maxx_endo;
MeasurementVectorType difepi;
MeasurementVectorType difepix;
MeasurementVectorType difendo;
MeasurementVectorType difendox;
maxx_epi=(sample_epix->GetMeasurementVector(sample_epix->Size()-1));
minx_epi=(sample_epix->GetMeasurementVector(0));

maxx_endo=(sample_endox->GetMeasurementVector(sample_epix->Size()-1));
minx_endo=(sample_endox->GetMeasurementVector(0));

difepix[1]=std::abs(maxx_epi[1]-minx_epi[1]);
difepix[0]=std::abs(maxx_epi[0]-minx_epi[0]);

difepi[1]=std::abs(maxy_epi[1]-miny_epi[1]);
difepi[0]=std::abs(maxy_epi[0]-miny_epi[0]);

difendox[1]=std::abs(maxx_endo[1]-minx_endo[1]);
difendox[0]=std::abs(maxx_endo[0]-minx_endo[0]);

difendo[1]=std::abs(maxy_endo[1]-miny_endo[1]);
difendo[0]=std::abs(maxy_endo[0]-miny_endo[0]);


int maxx=(int)std::abs(difepix[0]);
int maxy=(int)std::abs(difepi[1]);

int minx=(int)std::abs(difendox[0]);
int miny=(int)std::abs(difendo[1]);
int OFFSET=0;

std::cout<<" max, : "<<sample_epi_copy->GetMeasurementVector(0)<<" , "<<sample_epi_copy->GetMeasurementVector(10)<<std::endl;
std::cout<<" ,min : "<<sample_epi_copy->GetMeasurementVector(100)<<" , "<<maxy_epi<<std::endl;

std::cout<<" max, : "<<difepi[1]<<" , "<<difepix[0]<<std::endl;
std::cout<<" ,min : "<<difendo[1]<<" , "<<difendox[0]<<std::endl;

int maxradius= (int)(std::sqrt(std::pow(maxx,2)+std::pow(maxy,2)))*0.5;
int minradius= (int)(std::sqrt(std::pow(minx,2)+std::pow(miny,2)))*0.5;



maxdif=(maxradius)+(minradius)*epi-OFFSET;
mindif=(minradius)+(minradius)*endo-OFFSET; 

std::cout<<" RADIUS ARE: "<<maxdif<<" , "<<mindif<<std::endl;
//strategy for find if the amount of offset to increase or dicrease the cycle
//int distance_max=std::abs(maxx-maxy);
//if (distance_max< 10){maxdif=std::max(maxx,maxy);}
//if (distance_max> 10){maxdif=std::min(maxx,maxy)+10;}
//int distance_min=std::abs(minx-miny);
//if (distance_min< 10){mindif=std::max(minx,miny);}
//if (distance_min> 10){mindif=std::min(minx,miny)+10;}


maxy_epi[1]=maxdif*0.625+mean_endo[1];
miny_epi[1]=-maxdif*0.625+mean_endo[1];
maxy_epi[0]=mean_endo[0];
miny_epi[0]=mean_endo[0];


first_epi[0]=(int) std::abs(-maxdif*0.125 + mean_endo[0] );
first_epi[1]=(int) std::abs(std::sqrt(-std::pow(maxdif*0.125,2)+std::pow(maxdif*0.625,2)) + mean_endo[1] );

second_epi[0]=(int) std::abs(-maxdif*0.625 + mean_endo[0]);
second_epi[1]=(int) std::abs( mean_endo[1] );

third_epi[0]=(int) std::abs(-maxdif*0.5 + mean_endo[0] );
third_epi[1]=(int) std::abs(-std::sqrt(-std::pow(maxdif*0.5,2)+std::pow(maxdif*0.625,2)) + mean_endo[1] );

forth_epi[0]=(int) std::abs(-maxdif*0.375 + mean_endo[0] );
forth_epi[1]=(int) std::abs(std::sqrt(-std::pow(maxdif*0.375,2)+std::pow(maxdif*0.625,2)) + mean_endo[1] );

fifth_epi[0]=(int) std::abs(-maxdif*0.25 + mean_endo[0] );
fifth_epi[1]=(int) std::abs(-std::sqrt(-std::pow(maxdif*0.25,2)+std::pow(maxdif*0.625,2)) + mean_endo[1] );

mirror_first_epi[0]=(int) (+maxdif*0.125 + mean_endo[0] );
mirror_first_epi[1]=(int) (std::sqrt(-std::pow(maxdif*0.125,2)+std::pow(maxdif*0.625,2)) + mean_endo[1] );

mirror_second_epi[0]=(int) (+maxdif*0.625 + mean_endo[0] );
mirror_second_epi[1]=(int) ( mean_endo[1] );

mirror_third_epi[0]=(int) (+maxdif*0.5 + mean_endo[0] );
mirror_third_epi[1]=(int) std::abs(-std::sqrt(-std::pow(maxdif*0.5,2)+std::pow(maxdif*0.625,2)) + mean_endo[1]);

mirror_forth_epi[0]=(int) (+maxdif*0.375 + mean_endo[0] );
mirror_forth_epi[1]=(int) (std::sqrt(-std::pow(maxdif*0.375,2)+std::pow(maxdif*0.625,2)) + mean_endo[1] );

mirror_fifth_epi[0]=(int) (+maxdif*0.25 + mean_endo[0] );
mirror_fifth_epi[1]=(int) std::abs(-std::sqrt(-std::pow(maxdif*0.25,2)+std::pow(maxdif*0.625,2)) + mean_endo[1] );


   // std::cout << "origin of ENDO: " << mean_endo << '\n' ;
   // std::cout << "max of Epi: " << maxy_epi << '\n' ;
   // std::cout << "min of Epi: " << miny_epi << '\n' ;

   // std::cout << "first_epi: " << first_epi << '\n' ;
   // std::cout << "second_epi: " << second_epi << '\n' ;
   // std::cout << "third_epi: " << third_epi << '\n' ;
   // std::cout << "forth_epi: " << forth_epi << '\n' ;
   // std::cout << "fifth_epi: " << fifth_epi << '\n' ;
   // std::cout << "mirror_first_epi: " << mirror_first_epi << '\n' ;
   // std::cout << "mirror_second_epi: " << mirror_second_epi << '\n' ;
   // std::cout << "mirror_third_epi: " << mirror_third_epi << '\n' ;
   // std::cout << "mirror_forth_epi: " << mirror_forth_epi << '\n' ;
   // std::cout << "mirror_fifth_epi: " << mirror_fifth_epi << '\n' ;

}

