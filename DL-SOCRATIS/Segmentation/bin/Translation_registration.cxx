////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1
//acknolagkement: https://itk.org/Doxygen/html/Examples_2RegistrationITKv3_2ImageRegistration12_8cxx-example.html#_a14
//////////////////////////////////////////////////////////////////////////////////
#include "Translation_registration.h"


class CommandIterationUpdate : public itk::Command
{
public:
  using Self = CommandIterationUpdate;
  using Superclass = itk::Command;
  using Pointer = itk::SmartPointer<Self>;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  using OptimizerType = itk::RegularStepGradientDescentOptimizer;
  using OptimizerPointer = const OptimizerType *;
  void Execute(itk::Object *caller, const itk::EventObject & event) override
    {
    Execute( (const itk::Object *)caller, event);
    }
  void Execute(const itk::Object * object, const itk::EventObject & event) override
    {
    OptimizerPointer optimizer = static_cast< OptimizerPointer >( object );
    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "   ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};


void Translation_registration::Translation_registration_Filter(const char * m_fixedinputFileName,const char * m_movinginputFileName, const char * m_maskinputFileName, const char * m_outputFileName){

  constexpr unsigned int Dimension = 2;
  using PixelType = float;
  using FixedImageType = itk::Image< PixelType, Dimension >;
  using MovingImageType = itk::Image< PixelType, Dimension >;
  using TransformType = itk::CenteredRigid2DTransform< double >;
  using OptimizerType = itk::RegularStepGradientDescentOptimizer;
  using MetricType = itk::MeanSquaresImageToImageMetric<FixedImageType,MovingImageType >;
  using InterpolatorType = itk:: LinearInterpolateImageFunction<MovingImageType,double>;
  using RegistrationType = itk::ImageRegistrationMethod<FixedImageType,MovingImageType >;
  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);
  registration->SetInterpolator(interpolator);
  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );
  using FixedImageReaderType = itk::ImageFileReader< FixedImageType  >;
  using MovingImageReaderType = itk::ImageFileReader< MovingImageType >;
  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();
  fixedImageReader->SetFileName(  m_fixedinputFileName );
  movingImageReader->SetFileName(  m_movinginputFileName );
  registration->SetFixedImage(fixedImageReader->GetOutput());
  registration->SetMovingImage(movingImageReader->GetOutput());
  fixedImageReader->Update();
  registration->SetFixedImageRegion(
  fixedImageReader->GetOutput()->GetBufferedRegion() );
  using TransformInitializerType = itk::CenteredTransformInitializer<TransformType,FixedImageType,MovingImageType >;
  TransformInitializerType::Pointer initializer = TransformInitializerType::New();
  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedImageReader->GetOutput() );
  initializer->SetMovingImage( movingImageReader->GetOutput() );
  initializer->MomentsOn();
  initializer->InitializeTransform();
  transform->SetAngle( 0.0 );
  registration->SetInitialTransformParameters( transform->GetParameters() );
  using OptimizerScalesType = OptimizerType::ScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );
  const double translationScale = 1.0 / 1000.0;
  optimizerScales[0] = 1.0;
  optimizerScales[1] = translationScale;
  optimizerScales[2] = translationScale;
  optimizerScales[3] = translationScale;
  optimizerScales[4] = translationScale;
  optimizer->SetScales( optimizerScales );
  optimizer->SetMaximumStepLength( 0.1    );
  optimizer->SetMinimumStepLength( 0.001 );
  optimizer->SetNumberOfIterations( 200 );
  // Create the Command observer and register it with the optimizer.
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  //  Here we instantiate the type of the \doxygen{ImageMaskSpatialObject}
  //  using the same dimension of the images to be registered.
  //

  using MaskType = itk::ImageMaskSpatialObject< Dimension >;

  //  Then we use the type for creating the spatial object mask that will
  //  restrict the registration to a reduced region of the image.
  //

  MaskType::Pointer  spatialObjectMask = MaskType::New();
  // Software Guide : EndCodeSnippet
  //  Software Guide : BeginLatex
  //
  //  The mask in this case is read from a binary file using the
  //  \code{ImageFileReader} instantiated for an \code{unsigned char} pixel
  //  type.
  //
  //  Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  using ImageMaskType = itk::Image< unsigned char, Dimension >;
  using MaskReaderType = itk::ImageFileReader< ImageMaskType >;
  MaskReaderType::Pointer  maskReader = MaskReaderType::New();
  maskReader->SetFileName( m_maskinputFileName );

  //
  //  As usual, the reader is triggered by invoking its \code{Update()} method.
  //  Since this may eventually throw an exception, the call must be placed in
  //  a \code{try/catch} block. Note that a full fledged application will place
  //  this \code{try/catch} block at a much higher level, probably under the
  //  control of the GUI.
  //

  try
    {
    maskReader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    }

  spatialObjectMask->SetImage( maskReader->GetOutput() );

  metric->SetFixedImageMask( spatialObjectMask );
  try
    {
    registration->Update();
    std::cout << "Optimizer stop condition = "
              << registration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    }
  OptimizerType::ParametersType finalParameters =
                    registration->GetLastTransformParameters();
  const double finalAngle           = finalParameters[0];
  const double finalRotationCenterX = finalParameters[1];
  const double finalRotationCenterY = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  const double bestValue = optimizer->GetValue();
  // Print out results
  const double finalAngleInDegrees = finalAngle * 45.0 / std::atan(1.0);
  std::cout << "Result = " << std::endl;
  std::cout << " Angle (radians) " << finalAngle  << std::endl;
  std::cout << " Angle (degrees) " << finalAngleInDegrees  << std::endl;
  std::cout << " Center X      = " << finalRotationCenterX  << std::endl;
  std::cout << " Center Y      = " << finalRotationCenterY  << std::endl;
  std::cout << " Translation X = " << finalTranslationX  << std::endl;
  std::cout << " Translation Y = " << finalTranslationY  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;
  transform->SetParameters( finalParameters );
  TransformType::MatrixType matrix = transform->GetMatrix();
  TransformType::OffsetType offset = transform->GetOffset();
  std::cout << "Matrix = " << std::endl << matrix << std::endl;
  std::cout << "Offset = " << std::endl << offset << std::endl;

  //  Now we resample the moving image using the transform resulting from the
  //  registration process.


  using ResampleFilterType = itk::ResampleImageFilter< MovingImageType, FixedImageType >;
  TransformType::Pointer finalTransform = TransformType::New();
  finalTransform->SetParameters( finalParameters );
  finalTransform->SetFixedParameters( transform->GetFixedParameters() );
  ResampleFilterType::Pointer resample = ResampleFilterType::New();
  resample->SetTransform( finalTransform );
  resample->SetInput( movingImageReader->GetOutput() );
  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );
  resample->SetDefaultPixelValue( 100 );
  using OutputPixelType = unsigned char;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;
  using CastFilterType = itk::CastImageFilter< FixedImageType, OutputImageType >;
  using WriterType = itk::ImageFileWriter< OutputImageType >;
  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();
  writer->SetFileName( m_outputFileName );
  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  writer->Update();
 
//if need differences before and after registration

 //using DifferenceFilterType = itk::SquaredDifferenceImageFilter< FixedImageType, FixedImageType, OutputImageType >;
  //DifferenceFilterType::Pointer difference = DifferenceFilterType::New();
  //WriterType::Pointer writer2 = WriterType::New();
  //writer2->SetInput( difference->GetOutput() );
  // Compute the difference image between the
  // fixed and resampled moving image.
  //if( argc >= 6 )
  //  {
  //  difference->SetInput1( fixedImageReader->GetOutput() );
  //  difference->SetInput2( resample->GetOutput() );
  //  writer2->SetFileName( argv[5] );
  //  writer2->Update();
  //  }
  // Compute the difference image between the
  // fixed and moving image before registration.
  //if( argc >= 7 )
  //  {
  //  writer2->SetFileName( argv[6] );
  //  difference->SetInput1( fixedImageReader->GetOutput() );
  //  difference->SetInput2( movingImageReader->GetOutput() );
  //  writer2->Update();
  //  }

}
 



