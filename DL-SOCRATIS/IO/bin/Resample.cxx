#include "Resample.h"

 
void Resample::resample(const char * m_inputImageFileName, int size, int interpolation_type)
{

  typedef itk::Image<unsigned char, 2> ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputImageFileName);
  reader->Update();
  ImageType::Pointer input = reader->GetOutput();
  ImageType::RegionType region = input->GetLargestPossibleRegion();
  ImageType::SizeType inputSize = region.GetSize();
  //std::cout << "Input size: " << inputSize <<" name "<<m_inputImageFileName<< std::endl;
  ImageType::SizeType outputSize;
  outputSize.Fill(size);
  ImageType::SpacingType outputSpacing;
  outputSpacing[0] = input->GetSpacing()[0] * (static_cast<double>(inputSize[0]) / static_cast<double>(outputSize[0]));
  outputSpacing[1] = input->GetSpacing()[1] * (static_cast<double>(inputSize[1]) / static_cast<double>(outputSize[1]));
   using InterpolatorType = itk::NearestNeighborInterpolateImageFunction <ImageType, double >;
   using InterpolatorType2 = itk::BSplineInterpolateImageFunction <ImageType, double >;
   InterpolatorType2::Pointer interpolator = InterpolatorType2::New();
   if (interpolation_type==0){
     InterpolatorType::Pointer interpolator = InterpolatorType::New();
    }
   if (interpolation_type==1){
     InterpolatorType2::Pointer interpolator = InterpolatorType2::New();
   }
  typedef itk::IdentityTransform<double, 2> TransformType;
  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleImageFilterType;
  ResampleImageFilterType::Pointer resample = ResampleImageFilterType::New();
  resample->SetInput(input);
  resample->SetSize(outputSize);
  resample->SetInterpolator( interpolator );
  resample->SetOutputSpacing(outputSpacing);
  resample->SetTransform(TransformType::New());
  resample->UpdateLargestPossibleRegion();
 
  ImageType::Pointer output = resample->GetOutput();
 
  //std::cout << "Output size: " << output->GetLargestPossibleRegion().GetSize() << std::endl;
  typedef  itk::ImageFileWriter<ImageType> WriterType;
  //std::cout << "Writing output... " << std::endl;
  WriterType::Pointer outputWriter = WriterType::New();
  outputWriter->SetFileName(m_inputImageFileName);
  outputWriter->SetInput(output);
  outputWriter->Update();
 }
