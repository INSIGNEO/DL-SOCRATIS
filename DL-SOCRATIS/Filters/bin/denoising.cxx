#include "denoising.h"

void denoising::Initialize(){
default_kerner_image(); 
noise_kerner_image_comp();
}



void denoising::Median_smoothing(const char * m_inputFileName, const char * m_outputFileName, double x, double y)
{
WriterType::Pointer writer = WriterType::New();
ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName );
  writer->SetFileName( m_outputFileName );

  using FilterType = itk::MedianImageFilter<InputImageType, OutputImageType >;
  FilterType::Pointer filter = FilterType::New();

  InputImageType::SizeType indexRadius;
  indexRadius[0] = x; // radius along x
  indexRadius[1] = y; // radius along y
  filter->SetRadius( indexRadius );
  filter->SetInput( reader->GetOutput() );
  writer->SetInput( filter->GetOutput() );
  writer->Update();


}

void denoising::Curv_anisotropicDiffusionfilter(const char * m_inputFileName,const char * m_outputFileName, double time, double conductance_parameter)
{
WriterType::Pointer writer = WriterType::New();
//  The CurvatureAnisotropicDiffusionImageFilter requires a couple of
//  parameter to be defined. The following are typical values for $2D$
//  images. However they may have to be adjusted depending on the amount of
//  noise present in the input image. 
ReaderType::Pointer reader = ReaderType::New();
reader->SetFileName( m_inputFileName );
writer->SetFileName( m_outputFileName );

using FilterType = itk::CastImageFilter< InputImageType, InternalImageType >;
using FilterType2 = itk::CastImageFilter< InternalImageType, OutputImageType >;
using SmoothingFilterType = itk::CurvatureAnisotropicDiffusionImageFilter<InternalImageType,InternalImageType >;
using CenterFilterType =itk::ChangeInformationImageFilter<InternalImageType>;

CenterFilterType::Pointer center = CenterFilterType::New();

FilterType::Pointer filter = FilterType::New();
filter->SetInput( reader->GetOutput() );
center->CenterImageOn();
center->SetInput( filter->GetOutput() );

SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
smoothing->SetInput( center->GetOutput() );
smoothing->SetTimeStep( time );
smoothing->SetNumberOfIterations(  10 );
smoothing->SetConductanceParameter( conductance_parameter );

FilterType2::Pointer filter2 = FilterType2::New();
filter2->SetInput( smoothing->GetOutput()  );

  writer->SetInput( filter2->GetOutput() );
  writer->Update();

}
/////////////////////////blurring filters////////////////////////////////////////////////////

void denoising::Tikhonov_deconvolution(const char * m_inputFileName, const char * m_outputFileName, double reg  )
{
ReaderType::Pointer reader = ReaderType::New();
ReaderType::Pointer reader2 = ReaderType::New();
//https://itk.org/Doxygen/html/classitk_1_1TikhonovDeconvolutionImageFilter.html
WriterType::Pointer writer = WriterType::New();
reader->SetFileName( m_inputFileName );
writer->SetFileName( m_outputFileName );
reader2->SetFileName(m_noise_kerner_image);

using FilterType = itk::CastImageFilter< InputImageType, InternalImageType >;
using FilterType2 = itk::CastImageFilter< InternalImageType, OutputImageType >;

FilterType::Pointer filter = FilterType::New();
filter->SetInput( reader->GetOutput() );

//define kerner
//define kerner
FilterType::Pointer filter2 = FilterType::New();
filter2->SetInput( reader2->GetOutput() );


itk::ConstantBoundaryCondition< InternalImageType > cbc;
cbc.SetConstant( 0.0 );

  using ConvolutionFilterType = itk::FFTConvolutionImageFilter< InternalImageType >;
  ConvolutionFilterType::Pointer convolutionFilter= ConvolutionFilterType::New();
  convolutionFilter->SetInput( filter->GetOutput() );
  convolutionFilter->SetKernelImage( filter2->GetOutput() );
convolutionFilter->SetBoundaryCondition( &cbc );
convolutionFilter->SetSizeGreatestPrimeFactor( 5 );

typedef itk::TikhonovDeconvolutionImageFilter<InternalImageType, InternalImageType, InputImageType,  double> TikhonovDeconvolutionImageFilter;
TikhonovDeconvolutionImageFilter::Pointer Tikhonov = TikhonovDeconvolutionImageFilter::New();
  Tikhonov->SetInput(convolutionFilter->GetOutput());
  Tikhonov->SetKernelImage(filter2->GetOutput());
  Tikhonov->SetRegularizationConstant(reg);
  writer->SetInput(Tikhonov->GetOutput());
  writer->Update();

}


void denoising::Landweber_deconvolution(const char * m_inputFileName, const char * m_outputFileName,  const char * rep  )
{
ReaderType::Pointer reader = ReaderType::New();
ReaderType::Pointer reader2 = ReaderType::New();
//https://itk.org/Doxygen/html/classitk_1_1ProjectedLandweberDeconvolutionImageFilter.html
WriterType::Pointer writer = WriterType::New();
reader->SetFileName( m_inputFileName );
writer->SetFileName( m_outputFileName );
reader2->SetFileName(m_noise_kerner_image);
using FilterType = itk::CastImageFilter< InputImageType, InternalImageType >;
using FilterType2 = itk::CastImageFilter< InternalImageType, OutputImageType >;

FilterType::Pointer filter = FilterType::New();
filter->SetInput( reader->GetOutput() );

//define kerner
//define kerner
FilterType::Pointer filter2 = FilterType::New();
filter2->SetInput( reader2->GetOutput() );



typedef itk::ProjectedLandweberDeconvolutionImageFilter<InternalImageType, InternalImageType, InputImageType,  double> ProjectedLandweberDeconvolutionImageFilter;
ProjectedLandweberDeconvolutionImageFilter::Pointer landweber = ProjectedLandweberDeconvolutionImageFilter::New();
  landweber->SetInput(filter->GetOutput());
  landweber->SetKernelImage(filter2->GetOutput());
  landweber->SetNumberOfIterations(atoi(rep));
  writer->SetInput(landweber->GetOutput());
  writer->Update();

}




void denoising::Richardson_Lucy_deconvolution(const char * m_inputFileName, const char * m_outputFileName,  const char * rep  )
{
WriterType::Pointer writer = WriterType::New();
//https://itk.org/Doxygen/html/classitk_1_1RichardsonLucyDeconvolutionImageFilter.html
ReaderType::Pointer reader = ReaderType::New();
ReaderType::Pointer reader2 = ReaderType::New();
reader->SetFileName( m_inputFileName );
writer->SetFileName( m_outputFileName );
reader2->SetFileName(m_noise_kerner_image.c_str());

using FilterType = itk::CastImageFilter< InputImageType, InternalImageType >;
using FilterType2 = itk::CastImageFilter< InternalImageType, OutputImageType >;

FilterType::Pointer filter = FilterType::New();
filter->SetInput( reader->GetOutput() );

//define kerner
//define kerner
FilterType::Pointer filter2 = FilterType::New();
filter2->SetInput( reader2->GetOutput() );

itk::ConstantBoundaryCondition< InternalImageType > cbc;
cbc.SetConstant( 0.0 );

  using ConvolutionFilterType = itk::FFTConvolutionImageFilter< InternalImageType >;
  ConvolutionFilterType::Pointer convolutionFilter= ConvolutionFilterType::New();
  convolutionFilter->SetInput( filter->GetOutput() );
  convolutionFilter->SetKernelImage( filter2->GetOutput() );
convolutionFilter->SetBoundaryCondition( &cbc );
convolutionFilter->SetSizeGreatestPrimeFactor( 5 );

typedef itk::RichardsonLucyDeconvolutionImageFilter<InternalImageType, InternalImageType, InputImageType,  double> RichardsonLucyDeconvolutionImageFilter;
RichardsonLucyDeconvolutionImageFilter::Pointer RL = RichardsonLucyDeconvolutionImageFilter::New();
  RL->SetInput(filter->GetOutput());
  RL->SetKernelImage(filter2->GetOutput());
  RL->SetNumberOfIterations(atoi(rep));
  writer->SetInput(RL->GetOutput());
  writer->Update();

}



void denoising::Binomial_deconvolution(const char * m_inputFileName,const char * m_outputFileName, const char * rep  ){
ReaderType::Pointer reader = ReaderType::New();
//https://itk.org/Wiki/ITK/Examples/Broken/Functions/GaussianBlurImageFunctionFilter
WriterType::Pointer writer = WriterType::New();
reader->SetFileName( m_inputFileName );
writer->SetFileName( m_outputFileName );

using FilterType = itk::CastImageFilter< InputImageType, InternalImageType >;
using FilterType2 = itk::CastImageFilter< InternalImageType, OutputImageType >;

FilterType::Pointer filter = FilterType::New();
filter->SetInput( reader->GetOutput() );

//
using FilterType3 = itk::BinomialBlurImageFilter<InternalImageType, InternalImageType >;
  FilterType3::Pointer filter1 = FilterType3::New();

  using RescaleFilterType = itk::RescaleIntensityImageFilter<InternalImageType, InternalImageType >;
  const unsigned int repetitions = atoi( rep );
  filter1->SetInput( filter->GetOutput() );
  filter1->SetRepetitions( repetitions );
  filter1->Update();

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetOutputMinimum(   0 );
  rescaler->SetOutputMaximum( 255 );
  rescaler->SetInput( filter1->GetOutput()) ;
FilterType2::Pointer filter2 = FilterType2::New();
filter2->SetInput( rescaler->GetOutput() );


  writer->SetInput(filter2->GetOutput());
  writer->Update();

}

void denoising::Wiener_deconvolution(const char * m_inputFileName, const char * m_outputFileName, double  var  )
{
ReaderType::Pointer reader = ReaderType::New();
ReaderType::Pointer reader2 = ReaderType::New();
//https://itk.org/Doxygen/html/classitk_1_1WienerDeconvolutionImageFilter.html
WriterType::Pointer writer = WriterType::New();
reader->SetFileName( m_inputFileName );
writer->SetFileName( m_outputFileName );
reader2->SetFileName(m_noise_kerner_image.c_str());

using FilterType = itk::CastImageFilter< InputImageType, InternalImageType >;
using FilterType2 = itk::CastImageFilter< InternalImageType, OutputImageType >;

FilterType::Pointer filter = FilterType::New();
filter->SetInput( reader->GetOutput() );

//define kerner
FilterType::Pointer filter2 = FilterType::New();
filter2->SetInput( reader2->GetOutput() );

itk::ConstantBoundaryCondition< InternalImageType > cbc;
cbc.SetConstant( 0.0 );

  using ConvolutionFilterType = itk::FFTConvolutionImageFilter< InternalImageType >;
  ConvolutionFilterType::Pointer convolutionFilter= ConvolutionFilterType::New();
  convolutionFilter->SetInput( filter->GetOutput() );
  convolutionFilter->SetKernelImage( filter2->GetOutput() );
convolutionFilter->SetBoundaryCondition( &cbc );
convolutionFilter->SetSizeGreatestPrimeFactor( 5 );



typedef itk::WienerDeconvolutionImageFilter<InternalImageType, InternalImageType, InputImageType,  double> WienerDeconvolutionImageFilter;
WienerDeconvolutionImageFilter::Pointer wiener = WienerDeconvolutionImageFilter::New();
  wiener->SetInput(convolutionFilter->GetOutput());
  wiener->SetKernelImage(filter2->GetOutput());
  wiener->SetNoiseVariance ((var));

  wiener->SetBoundaryCondition( &cbc );
 wiener->SetSizeGreatestPrimeFactor( 5 );

  writer->SetInput(wiener->GetOutput());
  writer->Update();

}




double denoising::Compute_PSNR(const char * m_inputFileName, const char * noise) {

double PSNR;
double SNR;
double MSE;

  constexpr int dim = 2;

  using PType = unsigned char;
  using IType = itk::Image< PType, dim >;

  using ReaderType = itk::ImageFileReader< IType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName );
  reader->Update();

  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( noise );
  reader2->Update();
InputImageType::Pointer m_Image = InputImageType::New();
InputImageType::Pointer m_NoisyImage = InputImageType::New();

m_Image=reader->GetOutput();
m_NoisyImage=reader2->GetOutput();

 myIteratorType iIt( m_Image,m_Image->GetRequestedRegion() );
  iIt.GoToBegin();
  myIteratorType nIt( m_NoisyImage,m_NoisyImage->GetRequestedRegion() );
  nIt.GoToBegin();

  // init the values
  double         mse = 0;
  InputPixelType max = itk::NumericTraits<InputPixelType>::NonpositiveMin();

  while( !iIt.IsAtEnd() )
    {
    mse += pow( (double)nIt.Get() - (double)iIt.Get(), 2 );
    max = std::max( iIt.Get(), max );
    ++iIt;
    ++nIt;
    }
  mse /= m_Image->GetRequestedRegion().GetNumberOfPixels();

PSNR=10 * std::log10( max * max / mse );

//std::cout<<mse<<"::MSE"<<std::endl;
//std::cout<<PSNR<<" PSNR"<<std::endl;
//std::cout<<max<<" max"<<std::endl;


return PSNR;
}

void denoising::noise_kerner_image_comp() {
ReaderType::Pointer reader = ReaderType::New();

WriterType::Pointer writer2 = WriterType::New();
writer2->SetFileName((root+"noise_kerner.jpg").c_str()  );
using FilterType = itk::CastImageFilter< InputImageType, InternalImageType >;

using FilterType2 = itk::CastImageFilter<  InternalImageType, InputImageType >;

std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
Resample1->resample(m_noise_kerner_image.c_str(),512,1);
reader->SetFileName( m_noise_kerner_image.c_str());
FilterType::Pointer filter2 = FilterType::New();
filter2->SetInput( reader->GetOutput() );
FilterType2::Pointer filter = FilterType2::New();
filter->SetInput(filter2->GetOutput() );
writer2->SetInput( filter->GetOutput()); 
writer2->Update();

if (Gaussian_noise==true){
std::cout<<" Additive Gaussian Noise Kerner compute ..."<<std::endl;
using AdditiveGaussianNoiseFilterType = itk::AdditiveGaussianNoiseImageFilter< InternalImageType, InternalImageType >;
AdditiveGaussianNoiseFilterType::Pointer additiveGaussianNoiseFilter =AdditiveGaussianNoiseFilterType::New();
additiveGaussianNoiseFilter->SetStandardDeviation(5 );
additiveGaussianNoiseFilter->SetMean( 210);
additiveGaussianNoiseFilter->SetInput( filter2->GetOutput() );
filter->SetInput(additiveGaussianNoiseFilter->GetOutput()  );
writer2->SetInput( filter->GetOutput()); 

writer2->Update();
}

if (SandP_noise==true){
std::cout<<" Salt And Pepper Noise Kerner compute ..."<<std::endl;
using SaltAndPepperNoiseImageFilterType = itk::SaltAndPepperNoiseImageFilter< InternalImageType, InternalImageType >;
  SaltAndPepperNoiseImageFilterType::Pointer saltAndPepperNoiseImageFilter = SaltAndPepperNoiseImageFilterType::New();

  double probability = 0.3;

  saltAndPepperNoiseImageFilter->SetProbability( probability );

  // change the default values and then set back to defaults so that
  // the original test image is still valid.
  InternalPixelType saltValue = m_mean;
  saltAndPepperNoiseImageFilter->SetSaltValue( saltValue );

  InternalPixelType pepperValue = 10;
  saltAndPepperNoiseImageFilter->SetPepperValue( pepperValue );

  saltAndPepperNoiseImageFilter->SetSaltValue( itk::NumericTraits<InternalPixelType>::max() );
  saltAndPepperNoiseImageFilter->SetPepperValue( itk::NumericTraits<InternalPixelType>::NonpositiveMin() );

saltAndPepperNoiseImageFilter->SetInput( filter2->GetOutput() );
filter->SetInput(saltAndPepperNoiseImageFilter->GetOutput());
writer2->SetInput( filter->GetOutput() ); 
writer2->Update();
}

if (Shot_noise==true){
std::cout<<" Shot Noise Kerner compute ..."<<std::endl;
  using ShotNoiseImageFilterType =itk::ShotNoiseImageFilter< InternalImageType, InternalImageType >;
ShotNoiseImageFilterType::Pointer shotNoiseImageFilter = ShotNoiseImageFilterType::New();
shotNoiseImageFilter->SetScale( m_scale );
shotNoiseImageFilter->SetInput(filter2->GetOutput() );
filter->SetInput(shotNoiseImageFilter->GetOutput());
writer2->SetInput( filter->GetOutput() ); 
writer2->Update();

}


if (Speckle_noise==true){
std::cout<<" Speckle Noise Kerner compute ..."<<std::endl;
using SpeckleNoiseImageFilterType =itk::SpeckleNoiseImageFilter< InternalImageType, InternalImageType >;
  SpeckleNoiseImageFilterType::Pointer speckleNoiseImageFilter = SpeckleNoiseImageFilterType::New();

  double stdDev = m_std;

  speckleNoiseImageFilter->SetStandardDeviation( stdDev );
  speckleNoiseImageFilter->SetInput( filter2->GetOutput() );
filter->SetInput(speckleNoiseImageFilter->GetOutput());
 writer2->SetInput(filter->GetOutput()  ); 

writer2->Update();
}



}


void denoising::default_kerner_image(){

 using ImageType = itk::Image< unsigned char, 2 >;
  ImageType::Pointer image = ImageType::New();
using WriterType = itk::ImageFileWriter< ImageType >;
WriterType::Pointer writer3 = WriterType::New();
writer3->SetFileName( (root+"noise_kerner.jpg").c_str() );


  ImageType::IndexType start;
  start[0] = 0;  // first index on X
  start[1] = 0;  // first index on Y
  //start[2] = 0;  // first index on Z

  ImageType::SizeType  size;
  size[0] = 512;  // size along X
  size[1] = 512;  // size along Y
  //size[2] = 0;  // size along Z

  ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );
  image->SetRegions( region );
  image->Allocate();
 for(unsigned int r = 0; r < 512; r++)
  {
      for(unsigned int c = 0; c < 512; c++)
      {
          ImageType::IndexType pixelIndex;
          pixelIndex[0] = r;
          pixelIndex[1] = c;

          image->SetPixel(pixelIndex, 0);
      }
  }

  writer3->SetInput(image);
  writer3->Update();
SetnoiseKerner_image((root+"noise_kerner.jpg"));
}


void denoising::Normalize_all_images(std::string m_ROIFolder  )
{
int sum_SD=0;
int sum_mean=0;
std::shared_ptr< unbiased_ground_trouth >UGT2=std::make_shared< unbiased_ground_trouth >(root);

std:: vector<string> patients =UGT2->read_images_folder(m_ROIFolder,1,true);
int loops=patients.size()-1;
for ( unsigned int k = 0; k < loops; k++ )
		{
std::cout<< "normalized patient:  " << k<<"/"<<patients.size()<<std::endl;
			std::string roi_path2=m_ROIFolder;			
			std::string roi_path_final=(roi_path2+"ROI/"+patients[k])+'/';
		
			std::string store_roi= roi_path_final;
			std::shared_ptr< unbiased_ground_trouth >UGT4=std::make_shared< unbiased_ground_trouth >(root);
			std:: vector<string> store_roi_images =UGT4->read_images_folder(store_roi,0,false);

			Normalize_group_images(store_roi_images);
			int mean_group=Get_mean_group_images();
			int SD_group=Get_SD_group_images();
			sum_SD=sum_SD+mean_group;
			sum_mean=sum_mean+SD_group;
	}
		int total_SD=(int)sum_SD/loops;
		int total_mean=(int)sum_mean/loops;

		Set_SD_total_images(total_SD);
		Set_mean_total_images(total_mean);

		std::cout<< "Total-> Mean: " <<total_mean<< " Variance: " << total_SD<<std::endl;
}

void denoising::Normalize_group_images(std::vector<string> m_inputFile  )
{
int sum_SD=0;
int sum_mean=0;
for ( unsigned int k = 0; k < m_inputFile.size(); k++ )
			{

			Normalize_image(m_inputFile[k].c_str(), (root+"normalize_ROI.jpg").c_str(),100,10 );
			double mean=Getmean();
			double SD=GetSD();
		sum_SD=sum_SD+SD;
		sum_mean=sum_mean+mean;

}
int group_SD=(int)sum_SD/m_inputFile.size();
int group_mean=(int)sum_mean/m_inputFile.size();

Set_SD_group_images(group_SD);
Set_mean_group_images(group_mean);

std::cout<< "Group-> Mean: " << group_mean<< " Variance: " << group_SD<<std::endl;

}

void denoising::Normalize_image(const char * m_inputFileName, const char * m_outputFileName, int norm_const, int norm_sd  )
{

  std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  Resample1->resample(m_inputFileName,512,1);

  using FloatImageType = itk::Image<double, 2>;
  using FilterType = itk::CastImageFilter< InputImageType, FloatImageType >;
  using FilterType2 = itk::CastImageFilter<  FloatImageType, InputImageType >;
  using NormalizeFilterType = itk::NormalizeImageFilter< FloatImageType,FloatImageType >;
  using StatisticsFilterType = itk::StatisticsImageFilter< FloatImageType >;
  using ShiftScaleFilterType = itk::ShiftScaleImageFilter<FloatImageType, FloatImageType >;
  using ProgressAccumulator = itk::ProgressAccumulator;

NormalizeFilterType::Pointer normalizeFilter2 = NormalizeFilterType::New();
ShiftScaleFilterType::Pointer m_ShiftScaleFilter      = ShiftScaleFilterType::New();
ShiftScaleFilterType::Pointer m_ShiftScaleFilter2      = ShiftScaleFilterType::New();

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(m_inputFileName);
  reader->Update();

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->Update();


  StatisticsFilterType::Pointer statistics1 = StatisticsFilterType::New();
  StatisticsFilterType::Pointer statistics3 = StatisticsFilterType::New();
  statistics1->SetInput(filter->GetOutput());
  statistics1->Update();


if (norm_const>=0){

 m_ShiftScaleFilter2->SetShift( -statistics1->GetMean()+norm_const);
  m_ShiftScaleFilter2->SetScale(  itk::NumericTraits< StatisticsFilterType::RealType>::OneValue()/statistics1->GetSigma() );
  m_ShiftScaleFilter2->SetInput( filter->GetOutput() );
  m_ShiftScaleFilter2->Update();
  statistics3->SetInput(m_ShiftScaleFilter2->GetOutput());
  statistics3->Update();
 m_ShiftScaleFilter->SetShift( 0);
  m_ShiftScaleFilter->SetScale(  itk::NumericTraits< StatisticsFilterType::RealType>::OneValue()*norm_sd/(statistics3->GetSigma()) );
  m_ShiftScaleFilter->SetInput( m_ShiftScaleFilter2->GetOutput() );
  m_ShiftScaleFilter->Update();
}

if (norm_const==0){
   normalizeFilter2->SetInput(filter->GetOutput());
}

  StatisticsFilterType::Pointer statistics2 = StatisticsFilterType::New();
if (norm_const==0){  statistics2->SetInput( normalizeFilter2->GetOutput() );  }
if (norm_const!=0){   // FilterType::Pointer filter3 = FilterType::New();
 // filter3->SetInput( m_ShiftScaleFilter->GetOutput() ); filter3->Update(); 
statistics2->SetInput( m_ShiftScaleFilter->GetOutput() );}
statistics2->Update();
  std::cout<< "Mean: " << statistics1->GetMean()<< " SD: " << std::sqrt(statistics1->GetVariance())<<std::endl;
//  std::cout<< "newMean: " << statistics2->GetMean()<< " newVariance: " << statistics2->GetVariance()<<std::endl;
mean_image=statistics1->GetMean();
var_image=statistics1->GetVariance();
  FilterType2::Pointer filter2 = FilterType2::New();
if (norm_const==0){  filter2->SetInput( normalizeFilter2->GetOutput() );}
if (norm_const!=0){  filter2->SetInput( m_ShiftScaleFilter->GetOutput() );}
  filter->Update();

//rescale
  using RescaleType = itk::RescaleIntensityImageFilter<InputImageType, InputImageType>;
  RescaleType::Pointer rescaler = RescaleType::New();
  rescaler->SetInput(filter2->GetOutput());
  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(255);

  //using InvertIntensityImageFilterType = itk::InvertIntensityImageFilter<InputImageType>;
 
  //InvertIntensityImageFilterType::Pointer invertIntensityFilter = InvertIntensityImageFilterType::New();
 // invertIntensityFilter->SetInput(rescaler->GetOutput());
 // invertIntensityFilter->Update();

  WriterType::Pointer writer4 = WriterType::New();
  writer4->SetFileName( m_outputFileName );
//if (norm_const!=0){    writer4->SetInput( m_ShiftScaleFilter->GetOutput() );}
//if (norm_const==0){   writer4->SetInput( filter2->GetOutput() );}
writer4->SetInput( rescaler->GetOutput() );
  writer4->Update();

}

