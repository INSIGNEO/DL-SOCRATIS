#include "BinaryImage.h"


void BinaryImage::Image_binary(const char * m_inputFileName, const char * m_outputFileName, int lowerThreshold, int upperThreshold)
{

 
  typedef itk::Image<unsigned char, 2>  ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
 
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName);
 
  typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
    BinaryThresholdImageFilterType;
 
  BinaryThresholdImageFilterType::Pointer thresholdFilter
    = BinaryThresholdImageFilterType::New();
  thresholdFilter->SetInput(reader->GetOutput());
  thresholdFilter->SetLowerThreshold(lowerThreshold);
  thresholdFilter->SetUpperThreshold(upperThreshold);
  thresholdFilter->SetInsideValue(255);
  thresholdFilter->SetOutsideValue(0);

  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName );
  writer->SetInput( thresholdFilter->GetOutput() );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    
    }

}


void BinaryImage::Image_binary_intensity_handle(const char * m_inputFileName, const char * m_outputFileName, int lowerThreshold, int upperThreshold, int value_forward, int value_backward)
{

 
  typedef itk::Image<unsigned char, 2>  ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
 
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName);
 
  typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
    BinaryThresholdImageFilterType;
 
  BinaryThresholdImageFilterType::Pointer thresholdFilter
    = BinaryThresholdImageFilterType::New();
  thresholdFilter->SetInput(reader->GetOutput());
  thresholdFilter->SetLowerThreshold(lowerThreshold);
  thresholdFilter->SetUpperThreshold(upperThreshold);
  thresholdFilter->SetInsideValue(value_forward);
  thresholdFilter->SetOutsideValue(value_backward);

  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName );
  writer->SetInput( thresholdFilter->GetOutput() );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    
    }

}
