////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#include "kmeans.h"
 
void kmeans::Image_kmeans(const char * m_inputImageFileName, const char * m_outputImageFileName, const unsigned int m_useNonContiguousLabels, const unsigned int m_numberOfInitialClasses, double m_argoffset[])
{

  //sample usage
  //./kMeansClustering input.jpg output.jpg 1 3 0 100 200

 unsigned int count;
  std::vector<double> userMeans;
  for( unsigned int k = 0; k < m_numberOfInitialClasses; k++ )
    {
     
    const double userProvidedInitialMean = m_argoffset[k];
    userMeans.push_back(userProvidedInitialMean);
    }
 
  // Define the pixel type and dimension of the image that we intend to
  // classify. 
 
  typedef signed short       PixelType;
  const unsigned int          Dimension = 2;
 
  typedef itk::Image<PixelType, Dimension > ImageType;
 
  // create a reader  
  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputImageFileName );
 
  // Instantiate the ScalarImageKmeansImageFilter  
  typedef itk::ScalarImageKmeansImageFilter< ImageType > KMeansFilterType;
 
  KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();
 
  kmeansFilter->SetInput( reader->GetOutput() );
 
  // Make the output image intellegable by expanding the range of output image values, if desired
 
  kmeansFilter->SetUseNonContiguousLabels( m_useNonContiguousLabels );
 
  // initialize using the user input means
 
    for( unsigned k = 0; k < m_numberOfInitialClasses; k++ )
    {
    kmeansFilter->AddClassWithInitialMean( userMeans[k] );
    }
 
  // Create and setup a writer
 
  typedef KMeansFilterType::OutputImageType  OutputImageType;
 
  typedef itk::ImageFileWriter< OutputImageType > WriterType;
 
  WriterType::Pointer writer = WriterType::New();
 
  writer->SetInput( kmeansFilter->GetOutput() );
 
  writer->SetFileName( m_outputImageFileName );
 
  // execut the pipeline
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encountered while writing ";
    std::cerr << " image file : " << m_outputImageFileName << std::endl;
    std::cerr << excp << std::endl;
  
    }
 
  // inspect the means
  KMeansFilterType::ParametersType estimatedMeans = kmeansFilter->GetFinalMeans();
 
  const unsigned int numberOfClasses = estimatedMeans.Size();
 
  for ( unsigned int i = 0 ; i < numberOfClasses ; ++i )
    {
    std::cout << "cluster[" << i << "] ";
    std::cout << "    estimated mean : " << estimatedMeans[i] << std::endl;
 Means->PushBack(estimatedMeans[i]);
    }
 

 
}
