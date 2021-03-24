#include "image_vision.h"

 
void image_vision::vision(const char * m_inputImageFileName, const char * text)
{
  typedef itk::Image<unsigned char, 2>  ImageType;
  ImageType::Pointer image;
  typedef itk::ImageFileReader<ImageType> ReaderType;
 
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputImageFileName);


  image = reader->GetOutput();

  
 
  QuickView viewer;
  viewer.AddImage(image.GetPointer(), false, text );
 // viewer.AddImage(rescaleFilter->GetOutput());
  viewer.Visualize();
 

}


//void image_vision::scater_plot(const char * m_inputImageFileName)
//{



//}
