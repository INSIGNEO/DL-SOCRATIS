/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
// Software Guide : BeginLatex
//
// The statistics framework in ITK has been designed for managing multi-variate
// statistics in a natural way. The \subdoxygen{Statistics}{Histogram} class
// reflects this concept clearly since it is a N-variable joint histogram. This
// nature of the Histogram class is exploited in the following example in order
// to build the joint histogram of a color image encoded in RGB values.
//
// Note that the same treatment could be applied further to any vector image
// thanks to the generic programming approach used in the implementation of the
// statistical framework.
//
// The most relevant class in this example is the
// \subdoxygen{Statistics}{ImageToHistogramFilter}. This class will take
// care of adapting the \doxygen{Image} to a list of samples and then to a
// histogram filter. The user is only bound to provide the desired
// resolution on the histogram bins for each one of the image components.
//
// In this example we compute the joint histogram of the three channels of an
// RGB image. Our output histogram will be equivalent to a 3D array of bins.
// This histogram could be used further for feeding a segmentation method based
// on statistical pattern recognition. Such method was actually used during the
// generation of the image in the cover of the Software Guide.
//
// The first step is to include the header files for the histogram filter,
// the RGB pixel type and the Image.
//
// \index{itk::Statistics::ImageToHistogramFilter!header}
// \index{itk::RGBPixel!header}
// \index{itk::RGBPixel!Statistics}
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet

////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#include "Histogram.h"

void Histogram::ImageHistogram(const char * m_inputFileName, const char * bins, const char * m_outputFileName)
{


  constexpr unsigned int Dimension_v = 2;
  using PixelType = unsigned char;
  using ImageType = itk::Image< PixelType, Dimension_v >;
  constexpr unsigned int MeasurementVectorSize = 1; // Grayscale
  const auto binsPerDimension = static_cast< unsigned int >( atoi( bins ) );
  ///
  using ReaderType = itk::ImageFileReader< ImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName );
  ImageType::Pointer image = reader->GetOutput();
  using ImageToHistogramFilterType = itk::Statistics::ImageToHistogramFilter< ImageType >;
  ImageToHistogramFilterType::HistogramType::MeasurementVectorType
    lowerBound(binsPerDimension);
  lowerBound.Fill(0);
  ImageToHistogramFilterType::HistogramType::MeasurementVectorType
    upperBound(binsPerDimension);
  upperBound.Fill(255);
  ImageToHistogramFilterType::HistogramType::SizeType
    size(MeasurementVectorSize);
  size.Fill(binsPerDimension);
  ImageToHistogramFilterType::Pointer imageToHistogramFilter =
    ImageToHistogramFilterType::New();
  imageToHistogramFilter->SetInput( image );
  imageToHistogramFilter->SetHistogramBinMinimum( lowerBound );
  imageToHistogramFilter->SetHistogramBinMaximum( upperBound );
  imageToHistogramFilter->SetHistogramSize( size );
  try
    {
    imageToHistogramFilter->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;

    }

 // The values of the histogram can now be saved into a file by walking through
 // all of the histogram bins and pushing them into a std::ofstream.

  ImageToHistogramFilterType::HistogramType* histogram =
    imageToHistogramFilter->GetOutput();

  std::ofstream histogramFile;
  histogramFile.open( m_outputFileName );
   ImageToHistogramFilterType::HistogramType::ConstIterator itr = histogram->Begin();
   ImageToHistogramFilterType::HistogramType::ConstIterator end = histogram->End();
  using AbsoluteFrequencyType =ImageToHistogramFilterType::HistogramType::AbsoluteFrequencyType;
  
  std::vector<int>::iterator it;
  while( itr != end )
    {
    const AbsoluteFrequencyType frequency = itr.GetFrequency();
    histogramFile.write( (const char *)(&frequency), sizeof(frequency) );
    if (frequency != 0)
      {
      ImageToHistogramFilterType::HistogramType::IndexType index;
      index = histogram->GetIndex(itr.GetInstanceIdentifier());
      std::cout << "Index = " << index << ", Frequency = " << frequency <<", bins= " << binsPerDimension
                << std::endl;
    }
    
  
    
    ++itr;
    }




  histogramFile.close();
  // Set up a 2D scene, add an XY chart to it
  vtkSmartPointer<vtkContextView> view =
    vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(400, 300);
 
  vtkSmartPointer<vtkChartXY> chart =
    vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);
  chart->SetShowLegend(true);
 
  // Create a table with some points in it...
  vtkSmartPointer<vtkTable> table =
    vtkSmartPointer<vtkTable>::New();
 
  vtkSmartPointer<vtkFloatArray> arrX =
    vtkSmartPointer<vtkFloatArray>::New();
  arrX->SetName("X Axis");
  table->AddColumn(arrX);
 
  vtkSmartPointer<vtkFloatArray> arrC =
    vtkSmartPointer<vtkFloatArray>::New();
  arrC->SetName("Frequency");
  table->AddColumn(arrC);
 

 
  // Test charting with a few more points...
 table->SetNumberOfRows(binsPerDimension);
   for( int i = 1; i < binsPerDimension; ++i)
    { 
     table->SetValue(i, 0,i);
     table->SetValue(i,1,histogram->GetFrequency(i));
      }
    ++itr;

  // Add multiple scatter plots, setting the colors etc

  vtkPlot *line = chart->AddPlot(vtkChart::LINE);
  chart->SetShowLegend(true);
#if VTK_MAJOR_VERSION <= 5
  line->SetInput(table, 0, 1);
#else
  line->SetInputData(table, 0, 1);
#endif
  line->SetColor(0, 0, 0, 255);
  line->SetWidth(1.0);


  //Finally render the scene
  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

 
//visualize the histogram file


}

