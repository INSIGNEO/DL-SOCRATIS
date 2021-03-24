#include "Thresholding.h"


void Thresholding::Image_based_Thresholding(const char * m_inputFileName, const char * m_outputFileName, float m_lowerThreshold, float m_upperThreshold)
{

  using InputPixelType = unsigned char;
  using OutputPixelType = unsigned char;
  constexpr unsigned int Dimension = 2;
  using InputImageType = itk::Image< InputPixelType,  Dimension >;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;
  using FilterType = itk::ThresholdImageFilter<InputImageType >;
  using ReaderType = itk::ImageFileReader< InputImageType >;
  using WriterType = itk::ImageFileWriter< OutputImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  FilterType::Pointer filter = FilterType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName(m_inputFileName );
  filter->SetInput( reader->GetOutput() );
  const OutputPixelType outsideValue = m_upperThreshold ;
  const OutputPixelType insideValue  = m_lowerThreshold ;
  filter->SetOutsideValue( 0 );
  filter->ThresholdOutside(  m_lowerThreshold,m_upperThreshold );

  filter->Update();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( m_outputFileName );
  writer->Update();
}

