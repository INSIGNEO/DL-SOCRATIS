//Copyright (c) 2009-2013 ConsortiumOfANTS
//All rights reserved.


#ifndef __read_writedata_h_
#define __read_writedata_h_


#include <iostream>
#include <fstream>
#include <cstdio>
#include "itkVector.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkWarpImageFilter.h"
#include "itkAffineTransform.h"
#include "itkImageRegionIterator.h"
#include "itkResampleImageFilter.h"
#include "itkVariableLengthVector.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkCastImageFilter.h"
#include <sys/stat.h>



template <typename TImageType>
bool ReadImage(itk::SmartPointer<TImageType> & target, const char *file)
{
  enum { ImageDimension = TImageType::ImageDimension };
  if( std::string(file).length() < 3 )
    {
    target = nullptr;
    return false;
    }

  std::string comparetype1 = std::string( "0x" );
  std::string comparetype2 = std::string( file );
  comparetype2 = comparetype2.substr( 0, 2 );
  // Read the image files begin
  if(  comparetype1 == comparetype2  )
    {
    typedef TImageType RImageType;
    void* ptr;
    sscanf(file, "%p", (void **)&ptr);
    typename RImageType::Pointer Rimage = *( static_cast<typename RImageType::Pointer *>( ptr ) );
    /** more needs to be done here to cast the pointer to an image type --- this is a work-around */
    typedef itk::CastImageFilter<RImageType, TImageType> CastFilterType;
    typename CastFilterType::Pointer caster = CastFilterType::New();
    caster->SetInput( Rimage );
    caster->UpdateLargestPossibleRegion();
    target = caster->GetOutput();
    }
  else
    {

    typedef TImageType                      ImageType;
    typedef itk::ImageFileReader<ImageType> FileSourceType;

    typename FileSourceType::Pointer reffilter = FileSourceType::New();
    reffilter->SetFileName( file );
    try
      {
      reffilter->Update();
      }
    catch( itk::ExceptionObject & e )
      {
      std::cerr << "Exception caught during reference file reading " << std::endl;
      std::cerr << e << " file " << file << std::endl;
      target = nullptr;
      std::exception();
      return false;
      }

    // typename ImageType::DirectionType dir;
    // dir.SetIdentity();
    //  reffilter->GetOutput()->SetDirection(dir);

    // std::cout << " setting pointer " << std::endl;
    target = reffilter->GetOutput();
    }
  return true;
}




#endif

