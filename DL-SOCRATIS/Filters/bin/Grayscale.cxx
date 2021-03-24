#include "Grayscale.h"

void Grayscale::Image_Grayscale(const char * m_inputFileName, const char * m_outputFileName)
{
  
  constexpr unsigned int Dimension = 2;

  using ComponentType = unsigned char;
  using InputPixelType = itk::RGBPixel< ComponentType >;
  using InputImageType = itk::Image< InputPixelType, Dimension >;

  using ReaderType = itk::ImageFileReader< InputImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName );

  using OutputPixelType = unsigned char;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;

  using FilterType = itk::RGBToLuminanceImageFilter< InputImageType, OutputImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );

  using WriterType = itk::ImageFileWriter< OutputImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName);
  writer->SetInput( filter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
   
    }

}


