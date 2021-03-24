/*===========================================================================
  Program:    Insight Segmentation & Registration Toolkit
  Language:   C++
  Date:       june 2012

  Authors belong to:
  - Ecole Polytechnique Fédérale de Lausanne STI-IEL-LTS5                  http://lts5www.epfl.ch
  - Université de Valenciennes et du Hainaut-Cambrésis - LAMIH, CNRS       http://www.univ-valenciennes.fr/LAMIH/
  Contact: christine.boydev@epfl.ch
  Please email with any bugs or suggestions.

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the copyright notices for more information.
=============================================================================*/

#ifndef __itkIntraBinaryShapeBasedInterpolationImageFilter_h
#define __itkIntraBinaryShapeBasedInterpolationImageFilter_h

#include <itkImageToImageFilter.h>
#include <itkImage.h>

#include <itkImageSliceConstIteratorWithIndex.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

#include <itkSliceBySliceImageFilter.h>
#include <itkSignedDanielssonDistanceMapImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkInterpolateImageFunction.h>


namespace itk
{
/** \class IntraBinaryShapeBasedInterpolationImageFilter
  * \brief Interpolates  a 3D segmented binary image wherein all slices are segmented.
  * This filter takes as input a 3D binary image wherein the object of
  * interest is represented in full, that is, on all contiguous slices
  * that comprise it. Such filter yields an upsampled 3D binary image
  * with thinner delineated contours.
  * Note that the DelineationRatio defined as the ratio of the
  * delineated slices in the output image (i.e. after interpolation)
  * to the number of the delineated slices in the input image
  * (i.e. before interpolation) needs to be set up.
  * Reference paper : S.P. Raya and J.K. Udupa. Shape-based
  * interpolation of multidimensional objects. 1990
  */

class ITK_EXPORT IntraBinaryShapeBasedInterpolationImageFilter :
    public itk::ImageToImageFilter < itk::Image< unsigned char, 3 >, itk::Image< unsigned char, 3 > >
{
public:
  typedef unsigned char InputPixelType;
  typedef double        IntermediatePixelType;

  typedef itk::Image< InputPixelType, 3 >        InputImageType;
  typedef itk::Image< InputPixelType, 3 >        IntermediateImageType;
  typedef itk::Image< IntermediatePixelType, 3 > IntermediateImageType2;
  typedef itk::Image< InputPixelType, 2 >        InternalInImageType;
  typedef itk::Image< IntermediatePixelType, 2 > InternalOutImageType;
  typedef itk::Image< InputPixelType, 3 >        OutputImageType;

  /** Standard class typedefs used for object creation with the object
   * factory.
   */
  typedef IntraBinaryShapeBasedInterpolationImageFilter             Self;
  typedef itk::ImageToImageFilter<InputImageType, OutputImageType>  Superclass;
  typedef itk::SmartPointer<Self>                                   Pointer;
  typedef itk::SmartPointer<const Self>                             ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( IntraBinaryShapeBasedInterpolationImageFilter, ImageToImageFilter );

  /** Typedefs to access images. */
  typedef itk::ImageSliceConstIteratorWithIndex< InputImageType >   ConstSliceItType;
  typedef itk::ImageRegionConstIterator< InputImageType >           ConstRegionItType;
  typedef itk::ImageRegionIterator< InputImageType >                RegionItType;
  typedef itk::ImageRegionIterator< IntermediateImageType2 >        IntermediateRegionItType;
  typedef itk::ImageRegionIterator< OutputImageType >               OutputRegionItType;

  typedef InputImageType::Pointer                          InputImagePointer;
  typedef InputImageType::ConstPointer                     InputImageConstPointer;
  typedef InputImageType::RegionType                       InputRegionType;
  typedef InputImageType::SizeType                         InputSizeType;
  typedef InputImageType::SpacingType                      InputSpacingType;
  typedef InputImageType::IndexType                        InputIndexType;
  typedef InputImageType::PointType                        InputPointType;

  typedef IntermediateImageType::Pointer                   IntermediateImagePointer;
  typedef IntermediateImageType::RegionType                IntermediateRegionType;
  typedef IntermediateImageType::SizeType                  IntermediateSizeType;
  typedef IntermediateImageType::SpacingType               IntermediateSpacingType;
  typedef IntermediateImageType::IndexType                 IntermediateIndexType;
  typedef IntermediateImageType::PointType                 IntermediatePointType;

  typedef OutputImageType::Pointer                         OutputImagePointer;
  typedef OutputImageType::RegionType                      OutputRegionType;
  typedef OutputImageType::SizeType                        OutputSizeType;
  typedef OutputImageType::SpacingType                     OutputSpacingType;

  /** Set/Get the delineation ratio defined as the ratio of the
    * delineated slices in the output image (i.e. after interpolation) to
    * the number of the delineated slices in the input image (i.e. before
    * interpolation)*/
  itkSetMacro( DelineationRatio, unsigned int );
  itkGetConstReferenceMacro( DelineationRatio, unsigned int );

  itkStaticConstMacro( ImageDimension, unsigned int, 3 );

  typedef itk::InterpolateImageFunction< IntermediateImageType2, double>     InterpolatorType;

  /** Set the interpolator function.  The default is
    * itk::LinearInterpolateImageFunction. Some
    * other options are itk::NearestNeighborInterpolateImageFunction
    * and itk::BSplineInterpolateImageFunction
    * (which provides a higher order of interpolation).  */
  itkSetObjectMacro( Interpolator, InterpolatorType );
  /** Get a pointer to the interpolator function. */
  itkGetConstObjectMacro( Interpolator, InterpolatorType );

protected:
  /** Default constructor and destructor. */
  IntraBinaryShapeBasedInterpolationImageFilter();
  ~IntraBinaryShapeBasedInterpolationImageFilter();

  // Override since the output image has a different size as that of the input
  void GenerateOutputInformation();

  // Override since the filter requests a larger requested region on the input
  void GenerateInputRequestedRegion();

  void GenerateData();

  // Find all slices that are delineated (i.e. wherein there exist
  // non-zero pixel intensities).
  // They must be joint/contiguous in the input image.
  // The distance map calculation will be performed on these slices
  // only to gain time of calculation.
  void FindZCoordinatesOfDelineatedSlices();

  // Generate the information (origin, spacing, size,
  // LagestPossibleRegion,...) of the image made of the delineated
  // slices only
  void GenerateIntermediateImageInformation();

  /** Display. */
  void PrintSelf(std::ostream& os, Indent indent) const;

  typedef itk::SliceBySliceImageFilter< InputImageType, IntermediateImageType2 >                      SliceBySliceFilterType;
  typedef itk::SignedDanielssonDistanceMapImageFilter< InternalInImageType, InternalOutImageType >    DistanceMapImageFilterType;
  typedef itk::ResampleImageFilter< IntermediateImageType2, IntermediateImageType2 >                  ResampleFilterType;
  typedef itk::IdentityTransform< double, 3 >                                                         TransformType;

  SliceBySliceFilterType::Pointer                                                            m_SliceBySliceFilter;
  DistanceMapImageFilterType::Pointer                                                        m_DistanceMapImageFilter;
  ResampleFilterType::Pointer                                                                m_ResampleFilter;
  TransformType::Pointer                                                                     m_Transform;

private:
  //The copy constructor and assignment operator should be declared
  // private and not implemented, to prevent instantiating the ﬁlter
  // without the factory methods
  IntraBinaryShapeBasedInterpolationImageFilter( const Self& );  // intentionally not implemented
  void operator=( const Self& );                                 // intentionally not implemented
  unsigned int                            m_DelineationRatio;    // defined as the ratio of the delineated slices in the output
  // image (i.e. after interpolation) to the number of the
  // delineated slices in the input image (i.e. before interpolation)
  std::vector< int >                      m_DelineationZCoordinateArray;
  IntermediateImagePointer                m_IntermediateImage;
  InterpolatorType::Pointer      m_Interpolator;
};
} // end namespace itk
#endif  // #define __itkIntraBinaryShapeBasedInterpolationImageFilter_h
