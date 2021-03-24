/*============================================================================
  Program:    Insight Segmentation & Registration Toolkit
  Language:   C++
  Date:       june 2012
  
  Authors belong to:
  - Ecole Polytechnique Fédérale de Lausanne STI-IEL-LTS5                  http://lts5www.epfl.ch
  - Université de Valenciennes et du Hainaut-Cambrésis - LAMIH, CNRS       http://www.univ-valenciennes.fr/LAMIH/
  Contact: christine.boydev@epfl.ch

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the implied 
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  See the copyright notices for more information.
 ============================================================================*/

#include "itkIntraBinaryShapeBasedInterpolationImageFilter.h"
#include <itkLinearInterpolateImageFunction.h>

namespace itk
{  
/**
   * Constructor
   */
IntraBinaryShapeBasedInterpolationImageFilter::IntraBinaryShapeBasedInterpolationImageFilter()
{
  m_DelineationRatio = 2;  
  m_DelineationZCoordinateArray.clear();
    
  m_IntermediateImage = IntermediateImageType::New();
    
  m_SliceBySliceFilter = SliceBySliceFilterType::New();
  m_DistanceMapImageFilter = DistanceMapImageFilterType::New();
    
  m_ResampleFilter = ResampleFilterType::New();
  m_ResampleFilter->SetDefaultPixelValue( 1 );
  m_Transform = TransformType::New();
  m_Interpolator = LinearInterpolateImageFunction< IntermediateImageType2, double >::New();
}
  
/**
   * Destructor
   */
IntraBinaryShapeBasedInterpolationImageFilter::~IntraBinaryShapeBasedInterpolationImageFilter()
{
}
   
/**
  * Standard PrintSelf method. Print out a description of self. Print
  * the filter parameters.
 */
void IntraBinaryShapeBasedInterpolationImageFilter::PrintSelf( std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent);
  os << indent << "DelineationRatio" << m_DelineationRatio << std::endl;
  os << indent << "Interpolator: " << m_Interpolator.GetPointer() << std::endl;
}
  
void IntraBinaryShapeBasedInterpolationImageFilter::FindZCoordinatesOfDelineatedSlices()
{
  std::cout << "Appel fonction FindZCoordinatesOfDelineatedSlices()" << std::endl;
  InputImageConstPointer inputImage = this->GetInput();
  InputRegionType inputRegion = inputImage->GetLargestPossibleRegion();
    
  ConstSliceItType sliceIt( inputImage, inputRegion );
  sliceIt.SetFirstDirection(0);
  sliceIt.SetSecondDirection(1);
  sliceIt.GoToBegin(); 
    
  InputIndexType inputStart;   
  inputStart = sliceIt.GetIndex();
    
  InputPixelType pixel;
  InputPixelType pixelSum;
  
  // Count the number of slices that have been delineated in the input
  // image and store their z coordinate in a vector using the STL
  // library (std::vector)
  while( !sliceIt.IsAtEnd() )
    {
    pixelSum = 0;
    while( !sliceIt.IsAtEndOfSlice() )
      {
      while( !sliceIt.IsAtEndOfLine() )
        {
        pixel = sliceIt.Get();
        pixelSum += pixel;
        ++sliceIt;
        }
      sliceIt.NextLine();
      }
    if ( pixelSum > 0 ) // it means that the slice is delineated since there is at least one non-zero pixel intensity 
      {
      m_DelineationZCoordinateArray.push_back( sliceIt.GetIndex()[2] );
      }   
    sliceIt.NextSlice();
    }
      
  std::cout << "The first delineated slice is at z : " << m_DelineationZCoordinateArray.front() << std::endl;
  std::cout << "The last delineated slice is at z : " << m_DelineationZCoordinateArray.back() << std::endl;
}
  
void IntraBinaryShapeBasedInterpolationImageFilter::GenerateIntermediateImageInformation()
{    
  std::cout << "Appel fonction GenerateIntermediateImageInformation()" << std::endl;
  InputImageConstPointer inputImage = this->GetInput();
     
  //Define the spacing of the intermediate image 
  const InputSpacingType& inputSpacing = inputImage->GetSpacing();
  IntermediateSpacingType intermediateSpacing;
  intermediateSpacing[0] = inputSpacing[0];
  intermediateSpacing[1] = inputSpacing[1];
  intermediateSpacing[2] = inputSpacing[2];
    
  //Define the starting index of the intermediate regions 
  IntermediateIndexType intermediateStart;
  IntermediateIndexType physicalIntermediateStart;
    
  intermediateStart.Fill(0);
    
  physicalIntermediateStart[0] = 0;
  physicalIntermediateStart[1] = 0;
  physicalIntermediateStart[2] = m_DelineationZCoordinateArray.front();
    
  //Define the size of the intermediate regions 
  InputRegionType inputRegion = inputImage->GetLargestPossibleRegion();
  InputSizeType inputSize = inputRegion.GetSize();
  IntermediateSizeType intermediateSize;
  intermediateSize[0] = inputSize[0];
  intermediateSize[1] = inputSize[1];
  intermediateSize[2] = m_DelineationZCoordinateArray.size();
  
  IntermediateRegionType intermediateRegion( intermediateStart, intermediateSize );
    
  //Define the origin of the intermediate image 
  const InputPointType& inputOrigin = inputImage->GetOrigin();
  IntermediatePointType intermediateOrigin;
    
  for(unsigned int i=0; i< ImageDimension; i++)
    {
    intermediateOrigin[i] = inputOrigin[i] + inputSpacing[i] * physicalIntermediateStart[i];
    }
    
  m_IntermediateImage->SetRegions( intermediateRegion );
  m_IntermediateImage->SetSpacing( intermediateSpacing );
  m_IntermediateImage->SetOrigin( intermediateOrigin );
  m_IntermediateImage->Allocate();
}
  
  
  

  
// Inform pipeline of required output region
void IntraBinaryShapeBasedInterpolationImageFilter::GenerateOutputInformation()
{
  std::cout << "Appel GenerateOutputInformation()" << std::endl;
    
  // call the superclass's implementation of this method
  Superclass::GenerateOutputInformation();
      
  // get pointers to the input and output
  InputImageConstPointer inputImage = this->GetInput();     
  OutputImagePointer outputImage = this->GetOutput();
  if ( !inputImage || !outputImage )
    {
    return;
    }
    
  InputRegionType inputRegion = inputImage->GetLargestPossibleRegion();
  InputSizeType inputSize = inputRegion.GetSize();
  InputIndexType inputStart = inputRegion.GetIndex();
  const InputPointType& inputOrigin = inputImage->GetOrigin();
  const InputSpacingType& inputSpacing = inputImage->GetSpacing();
    
  OutputSizeType outputSize;
  OutputSpacingType outputSpacing;
    
  // Set the size of the output region
  outputSize[0] = inputSize[0];
  outputSize[1] = inputSize[1];
  outputSize[2] = inputSize[2] * m_DelineationRatio; 
    
  //OutputRegionType outputRegion( outputStart, outputSize );
  OutputRegionType outputRegion;
  outputRegion.SetSize( outputSize );
  outputRegion.SetIndex( inputStart );
  outputImage->SetLargestPossibleRegion( outputRegion );
    
  // Set spacing and origin
  outputSpacing[0] = inputSpacing[0];
  outputSpacing[1] = inputSpacing[1];
  outputSpacing[2] = inputSpacing[2] / m_DelineationRatio;
  outputImage->SetSpacing( outputSpacing ); 
  outputImage->SetOrigin( inputOrigin );
}
  
  
  
void IntraBinaryShapeBasedInterpolationImageFilter::GenerateInputRequestedRegion()
{
  std::cout << "Appel fonction GenerateInputRequestedRegion()" << std::endl;
    
  // call the superclass's implementation of this method
  Superclass::GenerateInputRequestedRegion();
    
  InputImagePointer inputImage = const_cast< InputImageType * >( this->GetInput() );
  OutputImagePointer outputImage = this->GetOutput();
  if ( !inputImage || !outputImage )
    {
    return;
    }
  inputImage->SetRequestedRegionToLargestPossibleRegion();
}
  
  
  
  
void IntraBinaryShapeBasedInterpolationImageFilter::GenerateData()
{
  std::cout << "Appel fonction GenerateData()" << std::endl;

  this->FindZCoordinatesOfDelineatedSlices();
  this->GenerateIntermediateImageInformation();
    
  InputImageConstPointer inputImage = this->GetInput(); 
  InputRegionType inputRegion = inputImage->GetLargestPossibleRegion(); 
  const InputPointType& inputOrigin = inputImage->GetOrigin();
  ConstRegionItType inputIt( inputImage, inputRegion );

  IntermediateRegionType intermediateRegion = m_IntermediateImage->GetLargestPossibleRegion();
  RegionItType intermediateIt( m_IntermediateImage, intermediateRegion );
    
  OutputImagePointer outputImage = this->GetOutput();
  OutputRegionType outputRegion = outputImage->GetLargestPossibleRegion();
  OutputSizeType outputSize = outputRegion.GetSize();
  OutputSpacingType outputSpacing = outputImage->GetSpacing();
    
    
  // Copy the slices that are delineated one by one into the m_IntermediateImage
  for( inputIt.GoToBegin(), intermediateIt.GoToBegin(); !inputIt.IsAtEnd(); ++inputIt )
    {
    if( std::find(m_DelineationZCoordinateArray.begin(), m_DelineationZCoordinateArray.end(), inputIt.GetIndex()[2]) != m_DelineationZCoordinateArray.end() )
      {
      intermediateIt.Set( inputIt.Get() );
      ++intermediateIt;
      }
    }
     
  m_SliceBySliceFilter->SetFilter( m_DistanceMapImageFilter );
  m_SliceBySliceFilter->SetInput( m_IntermediateImage );
    
  // Resample the m_IntermediateImage   
  m_ResampleFilter->SetInput( m_SliceBySliceFilter->GetOutput() );
  m_ResampleFilter->SetTransform( m_Transform );
  m_ResampleFilter->SetInterpolator( m_Interpolator );
  m_ResampleFilter->SetSize( outputSize ); 
  m_ResampleFilter->SetOutputSpacing( outputSpacing ); 
  m_ResampleFilter->SetOutputOrigin( inputOrigin ); 
  m_ResampleFilter->Update();    
    
  IntermediateRegionItType itr( m_ResampleFilter->GetOutput(), m_ResampleFilter->GetOutput()->GetLargestPossibleRegion() );

  outputRegion = m_ResampleFilter->GetOutput()->GetLargestPossibleRegion();
  outputImage->SetRegions( outputRegion );
  outputImage->CopyInformation( m_ResampleFilter->GetOutput() );
  outputImage->Allocate();

  OutputRegionItType outItr( outputImage, outputRegion );
    
  // Threshold the output image to get a binary interpolated image
  for( itr.GoToBegin(), outItr.GoToBegin(); !itr.IsAtEnd(); ++itr, ++outItr )
    {
    if( itr.Get() > 0 )
      {
      if( floor(10*itr.Get())/10 > 0)
        {
        outItr.Set(0);
        }
      else
        {
        outItr.Set(1);
        }
      }
    else
      {
      outItr.Set(1);
      }
    }
}
} // end namespace itk
