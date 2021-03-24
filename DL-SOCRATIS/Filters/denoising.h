#ifndef denoising_h
#define denoising_h

#include <cstdio>
#include "Resample.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMedianImageFilter.h"
//#include "itkGaussianBlurImageFunction.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBinomialBlurImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"
#include <itkWienerDeconvolutionImageFilter.h>
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkCastImageFilter.h"
//#include "itkParametricBlindLeastSquaresDeconvolutionImageFilter.h"
#include <itkRichardsonLucyDeconvolutionImageFilter.h>
#include <itkProjectedLandweberDeconvolutionImageFilter.h>
#include <itkTikhonovDeconvolutionImageFilter.h>
#include <itkNoiseImageFilter.h>
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"
#include <itkNumericTraits.h>
#include <itkConvolutionImageFilter.h>
#include "itkConstantBoundaryCondition.h"
#include <itkImageKernelOperator.h>
#include <iostream>
#include <cmath>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkTranslationTransform.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkNormalizeImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkImageRegionIterator.h"
#include "unbiased_ground_trouth.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkBinaryContourImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include <memory>

#include "itkSpeckleNoiseImageFilter.h"
#include "itkShotNoiseImageFilter.h"
#include "itkSaltAndPepperNoiseImageFilter.h"
#include "itkAdditiveGaussianNoiseImageFilter.h"
#include "itkTestingMacros.h"
#include "itkShiftScaleImageFilter.h"

using namespace std;

class denoising
{
public:

using InputPixelType = unsigned char;
using OutputPixelType = unsigned char;
using InputImageType = itk::Image< InputPixelType,  2 >;
using OutputImageType = itk::Image< OutputPixelType, 2 >;
const char * m_outputImage_SM;

using ReaderType = itk::ImageFileReader< InputImageType  >;
using WriterType = itk::ImageFileWriter< OutputImageType >;
using InternalPixelType = float;
using InternalImageType = itk::Image< InternalPixelType, 2 >;

using myIteratorType = itk::ImageRegionConstIteratorWithIndex<InputImageType>;

denoising(std::string m_root){
Gaussian_noise= false;
SandP_noise=false;
Speckle_noise=false;
Shot_noise=false;
m_std=10;
m_mean=120;
m_scale=1;
root=m_root;
};

~denoising(){ };

void Median_smoothing(const char * m_inputFileName,const char * m_outputFileName, double x, double y);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_SM;}

void Curv_anisotropicDiffusionfilter(const char * m_inputFileName,const char * m_outputFileName, double time, double conductance_parameter);

void noise_kerner_image_comp();
void Initialize();

void Wiener_deconvolution(const char * m_inputFileName, const char * m_outputFileName,  double var );
void Tikhonov_deconvolution(const char * m_inputFileName, const char * m_outputFileName,   double reg  );
void Landweber_deconvolution(const char * m_inputFileName, const char * m_outputFileName,  const char * rep  );
void Richardson_Lucy_deconvolution(const char * m_inputFileName, const char * m_outputFileName,  const char * rep  );
void Binomial_deconvolution(const char * m_inputFileName, const char * m_outputFileName, const char * rep  );

void Normalize_image(const char * m_inputFileName, const char * m_outputFileName, int norm_const, int norm_sd  );
void Normalize_group_images(std::vector<string>  m_inputFile  );
void Normalize_all_images(std::string m_ROIFolder  );

double Compute_PSNR(const char * m_inputFileName,const char * noise);

inline void SetnoiseKerner_image(std::string  noise_kerner_image){m_noise_kerner_image=noise_kerner_image;};

inline void SetGauss_noise(bool value){Gaussian_noise= value;};
inline void SetSaltPeper_noise(bool value){SandP_noise=value;};
inline void SetSpeckle_noise(bool value){Speckle_noise=value;};
inline void SetShot_noise(bool value){Shot_noise=value;};
inline void SetMean_noise(double value){m_std=value;};
inline void SetSTD_noise(double value){m_mean=value;};
inline void SetScale_noise(double value){m_scale=value;};
inline double Getmean(){return mean_image;};
inline double GetSD(){return std::sqrt(var_image);};
void Set_root_path(std::string path_root){root= path_root;};
inline int Get_mean_group_images(){return mean_group;};
inline int Get_SD_group_images(){return SD_group;};
inline void Set_SD_group_images(int m_SD_group){SD_group=m_SD_group;};
inline void Set_mean_group_images(int m_mean_group){mean_group=m_mean_group;};

inline int Get_mean_total_images(){return mean_total;};
inline int Get_SD_total_images(){return SD_total;};
inline void Set_SD_total_images(int m_SD_t){SD_total=m_SD_t;};
inline void Set_mean_total_images(int m_mean_t){mean_total=m_mean_t;};


private:
std::string m_noise_kerner_image="none ";
std::string root="./1/";
bool Gaussian_noise;
bool SandP_noise;
bool Speckle_noise;

int mean_group=100;
int SD_group=10;

int mean_total=100;
int SD_total=10;

bool Shot_noise;
double m_std;
double m_mean;
 double m_scale;
double mean_image;
double var_image;

void default_kerner_image();
};


#endif
