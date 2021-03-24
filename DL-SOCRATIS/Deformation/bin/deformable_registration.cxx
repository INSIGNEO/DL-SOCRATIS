
#include "deformable_registration.h"
#include "itkMattesMutualInformationImageToImageMetric.h"





deformable_registration::deformable_registration(std::vector<bool> multistage_mode)
{
trans_reg=multistage_mode[0];
affine_reg=multistage_mode[1];
rigid_reg=multistage_mode[2];
deform_reg=multistage_mode[3];
}


void deformable_registration::resample_reg(const char * m_input, const char * m_out, double space[2], double origin[2], double size[2])
{

  std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  Resample1->resample(m_input,(int)size[0],1);

  using PixelType = unsigned char;

  using ImageType = itk::Image< PixelType,  2 >;
  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer1 = WriterType::New();
  using ImageReaderType = itk::ImageFileReader<ImageType>;

	  ImageReaderType::Pointer  reader1 =  ImageReaderType::New();
  	  reader1->SetFileName(m_input);
  	  reader1->Update();
	  using ResampleFilterType = itk::ResampleImageFilter<ImageType, ImageType>;
	  ResampleFilterType::Pointer resample1 = ResampleFilterType::New();
	  resample1->SetInput(reader1->GetOutput());

	  ImageType::Pointer fixedImage = reader1->GetOutput();
	  PixelType backgroundGrayLevel = 0;
		//space[0]=1.0;
		//space[1]=1.0;
	  resample1->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
          ImageType::SpacingType spacing;

	  spacing[0]=space[0];
	  spacing[1]=space[1];

	  resample1->SetOutputOrigin(origin);
	  resample1->SetOutputSpacing(spacing);
	  resample1->SetOutputDirection(fixedImage->GetDirection());
	  resample1->SetDefaultPixelValue(backgroundGrayLevel);
	  resample1->Update();
  	  writer1->SetFileName(m_out);
	  writer1->SetInput( resample1->GetOutput() );
	  writer1->Update();

}



void deformable_registration::init_reg(const char * m_inputfix, const char * m_inputmove)
{

std::string fix=m_inputfix;
std::string move=m_inputmove;

	if (fix.std::string::compare(move)>0 || fix.std::string::compare(move)<0){
				std::cout<<"translation of two different images... " <<std::endl;
					double center[2];
					center[0]=0;
					center[1]=0;
			 		std::vector<double> center_vector;
					std::shared_ptr<unbiased_ground_trouth > UBGT =std::make_shared<unbiased_ground_trouth >(root);
					center_vector=UBGT->align_images(fix,center,true);
					center[0]=center_vector[0];
					center[1]=center_vector[1];
					center_vector=UBGT->align_images(move,center,false);
		}
	if (fix.std::string::compare(move)==0  ){
				std::cout<<"translation of same image in 256, 256 center " <<std::endl;
					double center[2];
					center[0]=0;
					center[1]=0;
			 		std::vector<double> center_vector;
					std::shared_ptr<unbiased_ground_trouth > UBGT =std::make_shared<unbiased_ground_trouth >(root);
					center_vector=UBGT->align_images(fix,center,false);
					
		}

}
void deformable_registration::multi_stage_reg(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName)
{

if (trans_reg==true){trans_registartion( m_inputfix, m_inputmove, m_inputmove2, m_outputFileName);}
if (affine_reg==true){affine_registartion( m_inputfix, m_inputmove, m_inputmove2, m_outputFileName);}
if (rigid_reg==true){rigid_registartion( m_inputfix, m_inputmove, m_inputmove2, m_outputFileName);}
if (deform_reg==true){deform_registartion( m_inputfix, m_inputmove, m_inputmove2, m_outputFileName);}

}

void deformable_registration::trans_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName)
{
  		std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  		Resample1->resample(m_inputfix,512,1);
  		std::shared_ptr<Resample> Resample12 =std::make_shared<Resample>();
  		Resample12->resample(m_inputmove,512,1);

	  using InputPixelType = unsigned char;
	  using InputImageType = itk::Image<InputPixelType, 2>;
	  using ImageReaderType = itk::ImageFileReader<InputImageType>;
	   ImageReaderType::Pointer  reader1 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader2 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader3 =  ImageReaderType::New();
ImageReaderType::Pointer  Ireader3 =  ImageReaderType::New();
	  constexpr unsigned int Dimension = 2;
	  using PixelType = double;
	  using FixedImageType = itk::Image<PixelType, Dimension>;
	  using MovingImageType = itk::Image<PixelType, Dimension>;

	  // Software Guide : EndCodeSnippet
	    using CastFilterType1 = itk::CastImageFilter<InputImageType, FixedImageType>;
	  CastFilterType1::Pointer caster1 = CastFilterType1::New();
	    using CastFilterType2 = itk::CastImageFilter<InputImageType, MovingImageType>;
	  CastFilterType2::Pointer caster2 = CastFilterType2::New();

 		CastFilterType2::Pointer caster3 = CastFilterType2::New();

	  reader1->SetFileName( m_inputfix);
	  reader2->SetFileName( m_inputmove);

	  reader1->Update();
	  reader2->Update();


	  caster1->SetInput( reader1->GetOutput());
	  caster2->SetInput( reader2->GetOutput());
	  //caster1->Update();
	  //caster2->Update();

	  caster1->Update();
	 caster2->Update();

	  using TransformType = itk::TranslationTransform<double, Dimension>;
	  auto initialTransform = TransformType::New();
/*
	  using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
	  auto optimizer = OptimizerType::New();
	  optimizer->SetLearningRate(0.5);//16
	  optimizer->SetMinimumStepLength(0.00001);
	  optimizer->SetRelaxationFactor(0.5);//0.5
	  optimizer->SetNumberOfIterations(5);
*/


	  using IResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
	  auto Iresampler = IResampleFilterType::New();
	  Iresampler->SetInput(  caster2->GetOutput());
	  auto IfixedImage =   caster1->GetOutput();
	  Iresampler->SetUseReferenceImage(true);
	  Iresampler->SetReferenceImage(IfixedImage);
	  Iresampler->SetDefaultPixelValue(0);

	  using IOutputPixelType = unsigned char;
	  using IOutputImageType = itk::Image<IOutputPixelType, Dimension>;

	  using ICastFilterType = itk::CastImageFilter<FixedImageType, IOutputImageType>;
	  auto Icaster = ICastFilterType::New();
	  auto Icaster3 = CastFilterType1::New();
	  auto Icaster4 = ICastFilterType::New();
  	  Iresampler->Update();
	  Icaster->SetInput(Iresampler->GetOutput());

	  using IWriterType = itk::ImageFileWriter<IOutputImageType>;
	  auto Iwriter = IWriterType::New();
	  Iwriter->SetFileName(m_outputFileName);
	  Iwriter->SetInput(Icaster->GetOutput());
	  Iwriter->Update();




 using MetricType =itk::MattesMutualInformationImageToImageMetricv4<FixedImageType, MovingImageType>;

	//using MetricType = itk::MeanSquaresImageToImageMetricv4<FixedImageType, MovingImageType>;
	  auto metric = MetricType::New();
  metric->SetNumberOfHistogramBins(8);
 using ScalesEstimatorType =itk::RegistrationParameterScalesFromPhysicalShift<MetricType>;
  ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();
  scalesEstimator->SetMetric(metric);
  scalesEstimator->SetTransformForward(true);

  using AOptimizerType = itk::ConjugateGradientLineSearchOptimizerv4Template<double>;
  AOptimizerType::Pointer    optimizer = AOptimizerType::New();

  optimizer->SetScalesEstimator(scalesEstimator);
  optimizer->SetDoEstimateLearningRateOnce(false);
  optimizer->SetDoEstimateLearningRateAtEachIteration(true);//false
  // Software Guide : EndCodeSnippet

  // Set the other parameters of optimizer
  optimizer->SetLowerLimit(0);
  optimizer->SetUpperLimit(0.05);//2
  optimizer->SetEpsilon(0.2);//0.2
  optimizer->SetNumberOfIterations(1000);//200
  optimizer->SetMinimumConvergenceValue(1e-8);//1e-6
  optimizer->SetConvergenceWindowSize(5);//5



	  using RegistrationType = itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType>;
	  auto registration = RegistrationType::New();
	  registration->SetInitialTransform(initialTransform);
	  registration->SetMetric(metric);
	  registration->SetOptimizer(optimizer);
	  registration->SetFixedImage(  caster1->GetOutput());
	  registration->SetMovingImage(  caster2->GetOutput());

	  auto                          movingInitialTransform = TransformType::New();
	  TransformType::ParametersType initialParameters(movingInitialTransform->GetNumberOfParameters());
	  initialParameters[0] = 0;//0
	  initialParameters[1] = 0;//0
	  movingInitialTransform->SetParameters(initialParameters);
	  registration->SetMovingInitialTransform(movingInitialTransform);

	  auto identityTransform = TransformType::New();
	  identityTransform->SetIdentity();
	  registration->SetFixedInitialTransform(identityTransform);

	  constexpr unsigned int numberOfLevels = 1;
	  registration->SetNumberOfLevels(numberOfLevels);

	  RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
	  shrinkFactorsPerLevel.SetSize(1);
	  shrinkFactorsPerLevel[0] = 1;//1
	  registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);

	  RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
	  smoothingSigmasPerLevel.SetSize(1);
	  smoothingSigmasPerLevel[0] = 0;//0
	  registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);

	  try
	  {
	    registration->Update();
	    std::cout << "Optimizer stop condition: " << registration->GetOptimizer()->GetStopConditionDescription()
		      << std::endl;
	  }
	  catch (itk::ExceptionObject & err)
	  {
	    std::cerr << "ExceptionObject caught !" << std::endl;
	    std::cerr << err << std::endl;
	  }

	  auto transform = registration->GetTransform();
	  auto finalParameters = transform->GetParameters();
	  auto translationAlongX = finalParameters[0];
	  auto translationAlongY = finalParameters[1];

	  auto numberOfIterations = optimizer->GetCurrentIteration();

	  auto bestValue = optimizer->GetValue();

	  std::cout << "Result = " << std::endl;
	  std::cout << " Translation X = " << translationAlongX << std::endl;
	  std::cout << " Translation Y = " << translationAlongY << std::endl;
	  std::cout << " Iterations    = " << numberOfIterations << std::endl;
	  std::cout << " Metric value  = " << bestValue << std::endl;



	  auto fixedImage =   caster2->GetOutput();
	  using CompositeTransformType = itk::CompositeTransform<double, Dimension>;
	  auto outputCompositeTransform = CompositeTransformType::New();
	  outputCompositeTransform->AddTransform(movingInitialTransform);
	  outputCompositeTransform->AddTransform(registration->GetModifiableTransform());

	  using ResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
	  auto resampler = ResampleFilterType::New();
	  resampler->SetInput(  caster2->GetOutput());
	  resampler->SetTransform(outputCompositeTransform);

	  resampler->SetUseReferenceImage(true);
	  resampler->SetReferenceImage(fixedImage);
	  resampler->SetDefaultPixelValue(100);

	  using OutputPixelType = unsigned char;
	  using OutputImageType = itk::Image<OutputPixelType, Dimension>;

	  using CastFilterType = itk::CastImageFilter<FixedImageType, OutputImageType>;
	  auto caster = CastFilterType::New();
  resampler->Update();
	  caster->SetInput(resampler->GetOutput());

	  using WriterType = itk::ImageFileWriter<OutputImageType>;
	  auto writer = WriterType::New();
	  writer->SetFileName(m_outputFileName);
	  writer->SetInput(caster->GetOutput());
	  writer->Update();

 if (m_inputmove2!="None"){

 std::cout << "\n The scar registration ... " << std::endl;
	  reader3->SetFileName( m_inputmove2);
	  reader3->Update();
	  caster3->SetInput( reader3->GetOutput());
	  using ResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
	  ResampleFilterType::Pointer resample1 = ResampleFilterType::New();

	  resample1->SetTransform(outputCompositeTransform);
	  resample1->SetInput(caster3->GetOutput());

	  PixelType backgroundGrayLevel = 0;

	  resample1->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	  resample1->SetOutputOrigin(fixedImage->GetOrigin());
	  resample1->SetOutputSpacing(fixedImage->GetSpacing());
	  resample1->SetOutputDirection(fixedImage->GetDirection());
	  resample1->SetDefaultPixelValue(backgroundGrayLevel);



	  WriterType::Pointer     writer1 = WriterType::New();
	  CastFilterType::Pointer caster4 = CastFilterType::New();

	  writer1->SetFileName( m_inputmove2);

	  caster4->SetInput(resample1->GetOutput());
	  writer1->SetInput(caster4->GetOutput());
	  writer1->Update();
	}


}


void deformable_registration::affine_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName)
{
  		std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  		Resample1->resample(m_inputfix,512,1);
  		std::shared_ptr<Resample> Resample12 =std::make_shared<Resample>();
  		Resample12->resample(m_inputmove,512,1);
//sourc3e :  https://github.com/InsightSoftwareConsortium/ITK/blob/master/Examples/RegistrationITKv4/MultiStageImageRegistration1.cxx

	  using InputPixelType = unsigned char;
	  using InputImageType = itk::Image<InputPixelType, 2>;
	  using ImageReaderType = itk::ImageFileReader<InputImageType>;
	   ImageReaderType::Pointer  reader1 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader2 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader3 =  ImageReaderType::New();
	  constexpr unsigned int Dimension = 2;
	  using PixelType = double;
	  using FixedImageType = itk::Image<PixelType, Dimension>;
	  using MovingImageType = itk::Image<PixelType, Dimension>;

	  // Software Guide : EndCodeSnippet
	    using CastFilterType1 = itk::CastImageFilter<InputImageType, FixedImageType>;
	  CastFilterType1::Pointer caster1 = CastFilterType1::New();
	    using CastFilterType2 = itk::CastImageFilter<InputImageType, MovingImageType>;
	  CastFilterType2::Pointer caster2 = CastFilterType2::New();
	  CastFilterType2::Pointer caster3 = CastFilterType2::New();
	  reader1->SetFileName( m_inputfix);
	  reader2->SetFileName( m_inputmove);

	  reader1->Update();
	  reader2->Update();


	  caster1->SetInput( reader1->GetOutput());
	  caster2->SetInput( reader2->GetOutput());
	  caster1->Update();
	  caster2->Update();

  using FixedImageType = itk::Image<PixelType, Dimension>;
  using MovingImageType = itk::Image<PixelType, Dimension>;
  using TTransformType = itk::TranslationTransform<double, Dimension>;
  using TOptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
  using MetricType =itk::MattesMutualInformationImageToImageMetricv4<FixedImageType, MovingImageType>;

	//using MetricType = itk::MeanSquaresImageToImageMetricv4<FixedImageType, MovingImageType>;
	  //auto metric = MetricType::New();

  using TRegistrationType = itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType, TTransformType>;
  TOptimizerType::Pointer    transOptimizer = TOptimizerType::New();
  MetricType::Pointer        transMetric = MetricType::New();
  TRegistrationType::Pointer transRegistration = TRegistrationType::New();

  transRegistration->SetOptimizer(transOptimizer);
  transRegistration->SetMetric(transMetric);
  TTransformType::Pointer movingInitTx = TTransformType::New();
  using ParametersType = TOptimizerType::ParametersType;
  ParametersType initialParameters(movingInitTx->GetNumberOfParameters());
  initialParameters[0] = 0; // Initial offset in mm along X 0
  initialParameters[1] = 0; // Initial offset in mm along Y 0

  movingInitTx->SetParameters(initialParameters);
  transRegistration->SetMovingInitialTransform(movingInitTx);
  using CompositeTransformType = itk::CompositeTransform<double, Dimension>;
  CompositeTransformType::Pointer compositeTransform = CompositeTransformType::New();
  compositeTransform->AddTransform(movingInitTx);



  transRegistration->SetFixedImage(caster1->GetOutput());
  transRegistration->SetMovingImage(caster2->GetOutput());
  transRegistration->SetObjectName("TranslationRegistration");

  constexpr unsigned int numberOfLevels1 = 3;

  TRegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel1;
  shrinkFactorsPerLevel1.SetSize(numberOfLevels1);
  shrinkFactorsPerLevel1[0] = 3; //3
  shrinkFactorsPerLevel1[1] = 2; //3
  shrinkFactorsPerLevel1[2] = 1; //3

  TRegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel1;
  smoothingSigmasPerLevel1.SetSize(numberOfLevels1);

  smoothingSigmasPerLevel1[0] = 1; //2
  smoothingSigmasPerLevel1[1] = 0; //2
  smoothingSigmasPerLevel1[2] = 0; //2

  transRegistration->SetNumberOfLevels(numberOfLevels1);
  transRegistration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel1);
  transRegistration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel1);


  transMetric->SetNumberOfHistogramBins(24);//24
//transMetric->SetMovingImageMask(movingImageReader);
//transMetric->SetFixedImageMask(fixedImageReader);
 // transMetric->SetUseMovingImageGradientFilter(true);
 // transMetric->SetUseFixedImageGradientFilter(true);

  transOptimizer->SetNumberOfIterations(1000);//200
  transOptimizer->SetRelaxationFactor(0.5);//0.5 best

  transOptimizer->SetLearningRate(0.5);//1
  transOptimizer->SetMinimumStepLength(0.00001);//1.5//0.5 best

	//transMetric->Initialize();

  try
  {
    transRegistration->Update();
    std::cout << "Optimizer stop condition: "
              << transRegistration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
  }
  catch (itk::ExceptionObject & err)
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;

  }

  compositeTransform->AddTransform(transRegistration->GetModifiableTransform());

  using ATransformType = itk::AffineTransform<double, Dimension>;

  using AOptimizerType = itk::ConjugateGradientLineSearchOptimizerv4Template<double>;
  using ARegistrationType = itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType, ATransformType>;


  AOptimizerType::Pointer    affineOptimizer = AOptimizerType::New();
  MetricType::Pointer        affineMetric = MetricType::New();
  ARegistrationType::Pointer affineRegistration = ARegistrationType::New();

  affineRegistration->SetOptimizer(affineOptimizer);
  affineRegistration->SetMetric(affineMetric);

  affineRegistration->SetMovingInitialTransform(compositeTransform);

  affineRegistration->SetFixedImage(caster1->GetOutput());
  affineRegistration->SetMovingImage(caster2->GetOutput());
  affineRegistration->SetObjectName("AffineRegistration");

  affineMetric->SetNumberOfHistogramBins(24); //windows number of histogram pixel bins (number of pixels shift it s time smallest 5 especially in binary images close to 5 P.S.in 512x512 best 8 because it is divided perfectly

  	//affineMetric->SetFixedImageMask(fixedImageReader);
	//affineMetric->SetMovingImageMask(movingImageReader);
  //affineMetric->SetUseMovingImageGradientFilter(true);
  //affineMetric->SetUseFixedImageGradientFilter(true);

ATransformType::Pointer affineTx = ATransformType::New();

  using SpacingType = FixedImageType::SpacingType;
  using OriginType = FixedImageType::PointType;
  using RegionType = FixedImageType::RegionType;
  using SizeType = FixedImageType::SizeType;

  FixedImageType::Pointer fixedImage = caster1->GetOutput();

  const SpacingType fixedSpacing = fixedImage->GetSpacing();
  const OriginType  fixedOrigin = fixedImage->GetOrigin();
  const RegionType  fixedRegion = fixedImage->GetLargestPossibleRegion();
  const SizeType    fixedSize = fixedRegion.GetSize();

  ATransformType::InputPointType centerFixed;
  centerFixed[0] = fixedOrigin[0] + fixedSpacing[0] * fixedSize[0] / 2.0;
  centerFixed[1] = fixedOrigin[1] + fixedSpacing[1] * fixedSize[1] / 2.0;

  const unsigned int numberOfFixedParameters = affineTx->GetFixedParameters().Size();
  ATransformType::ParametersType fixedParameters(numberOfFixedParameters);
  for (unsigned int i = 0; i < numberOfFixedParameters; ++i)
  {
    fixedParameters[i] = centerFixed[i];
  }
  affineTx->SetFixedParameters(fixedParameters);
  affineRegistration->SetInitialTransform(affineTx);
  using ScalesEstimatorType =itk::RegistrationParameterScalesFromPhysicalShift<MetricType>;
  ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();
  scalesEstimator->SetMetric(affineMetric);
  scalesEstimator->SetTransformForward(true);

  affineOptimizer->SetScalesEstimator(scalesEstimator);
  affineOptimizer->SetDoEstimateLearningRateOnce(true);
  affineOptimizer->SetDoEstimateLearningRateAtEachIteration(false);//false
  // Software Guide : EndCodeSnippet

  // Set the other parameters of optimizer
  affineOptimizer->SetLowerLimit(0);
  affineOptimizer->SetUpperLimit(2);
  affineOptimizer->SetEpsilon(0.2);
  affineOptimizer->SetNumberOfIterations(1000);//200
  affineOptimizer->SetMinimumConvergenceValue(1e-7);//1e-6
  affineOptimizer->SetConvergenceWindowSize(5);//5

  constexpr unsigned int numberOfLevels2 = 3;

  ARegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel2;
  shrinkFactorsPerLevel2.SetSize(numberOfLevels2);
  shrinkFactorsPerLevel2[0] = 3;
  shrinkFactorsPerLevel2[1] = 2;
  shrinkFactorsPerLevel2[2] = 1;

  ARegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel2;
  smoothingSigmasPerLevel2.SetSize(numberOfLevels2);
  smoothingSigmasPerLevel2[0] = 1;
  smoothingSigmasPerLevel2[1] = 0;
  smoothingSigmasPerLevel2[2] = 0;

  affineRegistration->SetNumberOfLevels(numberOfLevels2);
  affineRegistration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel2);
  affineRegistration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel2);

	//affineMetric->Initialize();
  try
  {
    affineRegistration->Update();
    std::cout << "Optimizer stop condition: "
              << affineRegistration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
  }
  catch (itk::ExceptionObject & err)
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
  }

  compositeTransform->AddTransform(affineRegistration->GetModifiableTransform());
  // Software Guide : EndCodeSnippet

  std::cout << "\nInitial parameters of the registration process: " << std::endl
            << movingInitTx->GetParameters() << std::endl;

  std::cout << "\nTranslation parameters after registration: " << std::endl
            << transOptimizer->GetCurrentPosition() << std::endl
            << " Last LearningRate: " << transOptimizer->GetCurrentStepLength()
            << std::endl;

  std::cout << "\nAffine parameters after registration: " << std::endl
            << affineOptimizer->GetCurrentPosition() << std::endl
            << " Last LearningRate: " << affineOptimizer->GetLearningRate()
            << std::endl;

  using ResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();


  resample->SetTransform(compositeTransform);
  resample->SetInput(caster2->GetOutput());

  PixelType backgroundGrayLevel = 0;

  resample->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
  resample->SetOutputOrigin(fixedImage->GetOrigin());
  resample->SetOutputSpacing(fixedImage->GetSpacing());
  resample->SetOutputDirection(fixedImage->GetDirection());
  resample->SetDefaultPixelValue(backgroundGrayLevel);

  using OutputPixelType = unsigned char;
  using OutputImageType = itk::Image<OutputPixelType, Dimension>;
  using CastFilterType = itk::CastImageFilter<FixedImageType, OutputImageType>;
  using WriterType = itk::ImageFileWriter<OutputImageType>;

  WriterType::Pointer     writer = WriterType::New();
  CastFilterType::Pointer caster = CastFilterType::New();

  writer->SetFileName(m_outputFileName);
	resample->Update();
  caster->SetInput(resample->GetOutput());
	caster->Update();
  writer->SetInput(caster->GetOutput());
  writer->Update();

 if (m_inputmove2!="None"){

 std::cout << "\n The scar registration ... " << std::endl;
	  reader3->SetFileName( m_inputmove2);
	  reader3->Update();
	  caster3->SetInput( reader3->GetOutput());
	  using ResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
	  ResampleFilterType::Pointer resample1 = ResampleFilterType::New();

	  resample1->SetTransform(compositeTransform);
	  resample1->SetInput(caster3->GetOutput());

	  PixelType backgroundGrayLevel = 0;

	  resample1->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	  resample1->SetOutputOrigin(fixedImage->GetOrigin());
	  resample1->SetOutputSpacing(fixedImage->GetSpacing());
	  resample1->SetOutputDirection(fixedImage->GetDirection());
	  resample1->SetDefaultPixelValue(backgroundGrayLevel);



	  WriterType::Pointer     writer1 = WriterType::New();
	  CastFilterType::Pointer caster4 = CastFilterType::New();

	  writer1->SetFileName( m_inputmove2);
	resample1->Update();
	  caster4->SetInput(resample1->GetOutput());
	caster4->Update();
	  writer1->SetInput(caster4->GetOutput());
	  writer1->Update();
	}

}

void deformable_registration::rigid_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName)
{
  		std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  		Resample1->resample(m_inputfix,512,1);
  		std::shared_ptr<Resample> Resample12 =std::make_shared<Resample>();
  		Resample12->resample(m_inputmove,512,1);

//TODO
//sourc3e : https://itk.org/Doxygen/html/SphinxExamples_2src_2Registration_2Common_2WatchRegistration_2Code_8cxx-example.html#_a7

	  using InputPixelType = unsigned char;
	  using InputImageType = itk::Image<InputPixelType, 2>;
	  using ImageReaderType = itk::ImageFileReader<InputImageType>;
	   ImageReaderType::Pointer  reader1 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader2 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader3 =  ImageReaderType::New();
	  constexpr unsigned int Dimension = 2;
	  using PixelType = double;
	  using FixedImageType = itk::Image<PixelType, Dimension>;
	  using MovingImageType = itk::Image<PixelType, Dimension>;

	  // Software Guide : EndCodeSnippet
	    using CastFilterType1 = itk::CastImageFilter<InputImageType, FixedImageType>;
	  CastFilterType1::Pointer caster1 = CastFilterType1::New();
	    using CastFilterType2 = itk::CastImageFilter<InputImageType, MovingImageType>;
	  CastFilterType2::Pointer caster2 = CastFilterType2::New();
	  CastFilterType2::Pointer caster3 = CastFilterType2::New();
	  reader1->SetFileName( m_inputfix);
	  reader2->SetFileName( m_inputmove);

	  reader1->Update();
	  reader2->Update();


	  caster1->SetInput( reader1->GetOutput());
	  caster2->SetInput( reader2->GetOutput());
	  caster1->Update();
	 caster2->Update();
  // Set up registration
  using InternalImageType = itk::Image<double, Dimension>;
  using TransformType = itk::AffineTransform<double, Dimension>;
  using OptimizerType = itk::RegularStepGradientDescentOptimizer;
  using InterpolatorType = itk::LinearInterpolateImageFunction<InternalImageType, double>;
  using MetricType = itk::MattesMutualInformationImageToImageMetric<InternalImageType, InternalImageType>;
  using RegistrationType = itk::ImageRegistrationMethod<InternalImageType, InternalImageType>;
  using InitializerType = itk::CenteredTransformInitializer<TransformType, InputImageType, InputImageType>;
 
  InitializerType::Pointer  initializer = InitializerType::New();
  TransformType::Pointer    transform = TransformType::New();
  OptimizerType::Pointer    optimizer = OptimizerType::New();
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  RegistrationType::Pointer registration = RegistrationType::New();
 
  // Set up the registration framework
  initializer->SetFixedImage(reader1->GetOutput());
  initializer->SetMovingImage(reader2->GetOutput());
  initializer->SetTransform(transform);
 
  transform->SetIdentity();
  //initializer->GeometryOn();
initializer->MomentsOn();
  initializer->InitializeTransform();

  //transform->SetAngle(0.0);
  registration->SetOptimizer(optimizer);
  registration->SetTransform(transform);
  registration->SetInterpolator(interpolator);
 
  MetricType::Pointer metric = MetricType::New();

  registration->SetMetric(metric);
  registration->SetFixedImage(caster1->GetOutput());
  registration->SetMovingImage(caster2->GetOutput());


  // Update to get the size of the region
  //fixedNormalizer->Update();
  InternalImageType::RegionType fixedImageRegion = caster1->GetOutput()->GetBufferedRegion();
  registration->SetFixedImageRegion(fixedImageRegion);
 
  using ParametersType = RegistrationType::ParametersType;
  ParametersType initialParameters(transform->GetNumberOfParameters());
 
  // rotation matrix (identity)
  initialParameters[0] = 1.0; // R(0,0)//1
  initialParameters[1] = 0.0; // R(0,1)
  initialParameters[2] = 0.0; // R(1,0)
  initialParameters[3] = 1.0; // R(1,1)//1
 
  // translation vector
  initialParameters[4] = 0.0;
  initialParameters[5] = 0.0;
 
  registration->SetInitialTransformParameters(initialParameters);
 
  const unsigned int numberOfPixels = fixedImageRegion.GetNumberOfPixels();
  const auto         numberOfSamples = static_cast<unsigned int>(numberOfPixels * 0.05);//0.05
 
  metric->SetNumberOfHistogramBins(260); //24
  metric->SetNumberOfSpatialSamples(numberOfSamples);
 
  optimizer->MinimizeOn();
  optimizer->SetMaximumStepLength(0.05);
  optimizer->SetMinimumStepLength(0.001);
  optimizer->SetNumberOfIterations(2000);
  const unsigned int numberOfParameters = transform->GetNumberOfParameters();
  using OptimizerScalesType = OptimizerType::ScalesType;
  OptimizerScalesType optimizerScales(numberOfParameters);
 double             translationScale = 1/1000.0;//1.0 / 1000.0;
  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;
  optimizerScales[3] = 1.0;
  optimizerScales[4] = translationScale;
  optimizerScales[5] = translationScale;
 
  optimizer->SetScales(optimizerScales);
 
  TransformType::Pointer finalTransform = TransformType::New();
  finalTransform->SetParameters(initialParameters);
  finalTransform->SetFixedParameters(transform->GetFixedParameters());
   FixedImageType::Pointer fixedImage = caster1->GetOutput();
  using ResampleFilterType = itk::ResampleImageFilter<InternalImageType, InternalImageType>;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();
  resample->SetTransform(finalTransform);
  resample->SetInput(caster2->GetOutput());
  resample->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
  resample->SetOutputOrigin(fixedImage->GetOrigin());
  resample->SetOutputSpacing(fixedImage->GetSpacing());
  resample->SetOutputDirection(fixedImage->GetDirection());
  PixelType backgroundGrayLevel = 0;
  resample->SetDefaultPixelValue(backgroundGrayLevel);

    std::cout << "Start rigid registration pipeline of " <<m_inputmove<<", "<<m_inputfix<< std::endl;
  try
  {
    registration->Update();
    std::cout << "Optimizer stop condition: " << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
  }
  catch (itk::ExceptionObject & err)
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
  }
  std::cout << "Final Transform: " << finalTransform << std::endl;
 
  ParametersType finalParameters = registration->GetLastTransformParameters();
  std::cout << "Final Parameters: " << finalParameters << std::endl;
 
  unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  double       bestValue = optimizer->GetValue();

  std::cout << std::endl;
  std::cout << "Result = " << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue << std::endl;
  std::cout << " Numb. Samples = " << numberOfSamples << std::endl;

 using OutputPixelType = unsigned char;
  using OutputImageType = itk::Image<OutputPixelType, Dimension>;
  using CastFilterType = itk::CastImageFilter<FixedImageType, OutputImageType>;
  using WriterType = itk::ImageFileWriter<OutputImageType>;

  WriterType::Pointer     writer = WriterType::New();
  CastFilterType::Pointer caster = CastFilterType::New();

  writer->SetFileName(m_outputFileName);
  resample->Update();
  caster->SetInput(resample->GetOutput());
  writer->SetInput(caster->GetOutput());
  writer->Update();

if (m_inputmove2!="None"){

 std::cout << "\n The scar registration ... " << std::endl;
	  reader3->SetFileName( m_inputmove2);
	  reader3->Update();
	  caster3->SetInput( reader3->GetOutput());
	  using ResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
	  ResampleFilterType::Pointer resample1 = ResampleFilterType::New();

	  resample1->SetTransform(finalTransform);
	  resample1->SetInput(caster3->GetOutput());

	  PixelType backgroundGrayLevel = 0;

	  resample1->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	  resample1->SetOutputOrigin(fixedImage->GetOrigin());
	  resample1->SetOutputSpacing(fixedImage->GetSpacing());
	  resample1->SetOutputDirection(fixedImage->GetDirection());
	  resample1->SetDefaultPixelValue(backgroundGrayLevel);



	  WriterType::Pointer     writer1 = WriterType::New();
	  CastFilterType::Pointer caster4 = CastFilterType::New();

	  writer1->SetFileName( m_inputmove2);

	  caster4->SetInput(resample1->GetOutput());
	  writer1->SetInput(caster4->GetOutput());
	  writer1->Update();
	}



}

void deformable_registration::similarity_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName)
{
 




 		std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  		Resample1->resample(m_inputfix,512,1);
  		std::shared_ptr<Resample> Resample12 =std::make_shared<Resample>();
  		Resample12->resample(m_inputmove,512,1);
//sourc3e :  https://github.com/InsightSoftwareConsortium/ITK/blob/master/Examples/RegistrationITKv4/MultiStageImageRegistration1.cxx

	  using InputPixelType = unsigned char;
	  using InputImageType = itk::Image<InputPixelType, 2>;
	  using ImageReaderType = itk::ImageFileReader<InputImageType>;
	   ImageReaderType::Pointer  reader1 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader2 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader3 =  ImageReaderType::New();
	  constexpr unsigned int Dimension = 2;
	  using PixelType = double;
	  using FixedImageType = itk::Image<PixelType, Dimension>;
	  using MovingImageType = itk::Image<PixelType, Dimension>;

	  // Software Guide : EndCodeSnippet
	    using CastFilterType1 = itk::CastImageFilter<InputImageType, FixedImageType>;
	  CastFilterType1::Pointer caster1 = CastFilterType1::New();
	    using CastFilterType2 = itk::CastImageFilter<InputImageType, MovingImageType>;
	  CastFilterType2::Pointer caster2 = CastFilterType2::New();
	  CastFilterType2::Pointer caster3 = CastFilterType2::New();
	  reader1->SetFileName( m_inputfix);
	  reader2->SetFileName( m_inputmove);

	  reader1->Update();
	  reader2->Update();


	  caster1->SetInput( reader1->GetOutput());
	  caster2->SetInput( reader2->GetOutput());
	  caster1->Update();
	  caster2->Update();

  using FixedImageType = itk::Image<PixelType, Dimension>;
  using MovingImageType = itk::Image<PixelType, Dimension>;
  using TTransformType = itk::TranslationTransform<double, Dimension>;
  using TOptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
  using MetricType =itk::MattesMutualInformationImageToImageMetricv4<FixedImageType, MovingImageType>;

	//using MetricType = itk::MeanSquaresImageToImageMetricv4<FixedImageType, MovingImageType>;
	  //auto metric = MetricType::New();

  using TRegistrationType = itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType, TTransformType>;
  TOptimizerType::Pointer    transOptimizer = TOptimizerType::New();
  MetricType::Pointer        transMetric = MetricType::New();
  TRegistrationType::Pointer transRegistration = TRegistrationType::New();

  transRegistration->SetOptimizer(transOptimizer);
  transRegistration->SetMetric(transMetric);
  TTransformType::Pointer movingInitTx = TTransformType::New();
  using ParametersType = TOptimizerType::ParametersType;
  ParametersType initialParameters(movingInitTx->GetNumberOfParameters());
  initialParameters[0] = 0; // Initial offset in mm along X 0
  initialParameters[1] = 0; // Initial offset in mm along Y 0

  movingInitTx->SetParameters(initialParameters);
  transRegistration->SetMovingInitialTransform(movingInitTx);
  using CompositeTransformType = itk::CompositeTransform<double, Dimension>;
  CompositeTransformType::Pointer compositeTransform = CompositeTransformType::New();
  compositeTransform->AddTransform(movingInitTx);



  transRegistration->SetFixedImage(caster1->GetOutput());
  transRegistration->SetMovingImage(caster2->GetOutput());
  transRegistration->SetObjectName("TranslationRegistration");

  constexpr unsigned int numberOfLevels1 = 3;

  TRegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel1;
  shrinkFactorsPerLevel1.SetSize(numberOfLevels1);
  shrinkFactorsPerLevel1[0] = 3; //3
  shrinkFactorsPerLevel1[1] = 2; //3
  shrinkFactorsPerLevel1[2] = 1; //3

  TRegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel1;
  smoothingSigmasPerLevel1.SetSize(numberOfLevels1);

  smoothingSigmasPerLevel1[0] = 1; //2
  smoothingSigmasPerLevel1[1] = 0; //2
  smoothingSigmasPerLevel1[2] = 0; //2

  transRegistration->SetNumberOfLevels(numberOfLevels1);
  transRegistration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel1);
  transRegistration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel1);


  transMetric->SetNumberOfHistogramBins(8);//24
//transMetric->SetMovingImageMask(movingImageReader);
//transMetric->SetFixedImageMask(fixedImageReader);
//  transMetric->SetUseMovingImageGradientFilter(true);
  //transMetric->SetUseFixedImageGradientFilter(true);

  transOptimizer->SetNumberOfIterations(1000);//200
  transOptimizer->SetRelaxationFactor(0.05);//0.5 best

  transOptimizer->SetLearningRate(0.005);//1
  transOptimizer->SetMinimumStepLength(0.001);//1.5//0.5 best


	//transMetric->Initialize();
  try
  {
    transRegistration->Update();
    std::cout << "Optimizer stop condition: "
              << transRegistration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
  }
  catch (itk::ExceptionObject & err)
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;

  }

  compositeTransform->AddTransform(transRegistration->GetModifiableTransform());

  using STransformType = itk::Similarity2DTransform<double>;

  using SOptimizerType = itk::ConjugateGradientLineSearchOptimizerv4Template<double>;
  using SRegistrationType = itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType, STransformType>;


  SOptimizerType::Pointer    SOptimizer = SOptimizerType::New();
  MetricType::Pointer        SMetric = MetricType::New();
  SRegistrationType::Pointer SRegistration = SRegistrationType::New();

  SRegistration->SetOptimizer(SOptimizer);
  SRegistration->SetMetric(SMetric);

  SRegistration->SetMovingInitialTransform(compositeTransform);

  SRegistration->SetFixedImage(caster1->GetOutput());
  SRegistration->SetMovingImage(caster2->GetOutput());
  SRegistration->SetObjectName("similarity_Registration");

  SMetric->SetNumberOfHistogramBins(8); //windows number of histogram pixel bins (number of pixels shift it s time smallest 5 especially in binary images close to 5 P.S.in 512x512 best 8 because it is divided perfectly

  	//affineMetric->SetFixedImageMask(fixedImageReader);
	//affineMetric->SetMovingImageMask(movingImageReader);
 // SMetric->SetUseMovingImageGradientFilter(true);
  //SMetric->SetUseFixedImageGradientFilter(true);

STransformType::Pointer STx = STransformType::New();

  using SpacingType = FixedImageType::SpacingType;
  using OriginType = FixedImageType::PointType;
  using RegionType = FixedImageType::RegionType;
  using SizeType = FixedImageType::SizeType;

  FixedImageType::Pointer fixedImage = caster1->GetOutput();

  const SpacingType fixedSpacing = fixedImage->GetSpacing();
  const OriginType  fixedOrigin = fixedImage->GetOrigin();
  const RegionType  fixedRegion = fixedImage->GetLargestPossibleRegion();
  const SizeType    fixedSize = fixedRegion.GetSize();

  STransformType::InputPointType centerFixed;
  centerFixed[0] = fixedOrigin[0] + fixedSpacing[0] * fixedSize[0] / 2.0;
  centerFixed[1] = fixedOrigin[1] + fixedSpacing[1] * fixedSize[1] / 2.0;

  const unsigned int numberOfFixedParameters = STx->GetFixedParameters().Size();
  STransformType::ParametersType fixedParameters(numberOfFixedParameters);
  for (unsigned int i = 0; i < numberOfFixedParameters; ++i)
  {
    fixedParameters[i] = centerFixed[i];
  }
  STx->SetFixedParameters(fixedParameters);
  SRegistration->SetInitialTransform(STx);
  using ScalesEstimatorType =itk::RegistrationParameterScalesFromPhysicalShift<MetricType>;
  ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();
  scalesEstimator->SetMetric(SMetric);
  scalesEstimator->SetTransformForward(true);

  SOptimizer->SetScalesEstimator(scalesEstimator);
  SOptimizer->SetDoEstimateLearningRateOnce(true);
  SOptimizer->SetDoEstimateLearningRateAtEachIteration(false);//false
  // Software Guide : EndCodeSnippet

  // Set the other parameters of optimizer
  SOptimizer->SetLowerLimit(0);
  SOptimizer->SetUpperLimit(0.5);
  SOptimizer->SetEpsilon(0.2);
  SOptimizer->SetNumberOfIterations(1000);//200
  SOptimizer->SetMinimumConvergenceValue(1e-6);//1e-6
  SOptimizer->SetConvergenceWindowSize(5);//5

  constexpr unsigned int numberOfLevels2 = 3;

  SRegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel2;
  shrinkFactorsPerLevel2.SetSize(numberOfLevels2);
  shrinkFactorsPerLevel2[0] = 3;
  shrinkFactorsPerLevel2[1] = 2;
  shrinkFactorsPerLevel2[2] = 1;

  SRegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel2;
  smoothingSigmasPerLevel2.SetSize(numberOfLevels2);
  smoothingSigmasPerLevel2[0] = 1;
  smoothingSigmasPerLevel2[1] = 0;
  smoothingSigmasPerLevel2[2] = 0;

  SRegistration->SetNumberOfLevels(numberOfLevels2);
  SRegistration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel2);
  SRegistration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel2);

	//SMetric->Initialize();
  try
  {
    SRegistration->Update();
    std::cout << "Optimizer stop condition: "
              << SRegistration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
  }
  catch (itk::ExceptionObject & err)
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
  }

  compositeTransform->AddTransform(SRegistration->GetModifiableTransform());
  // Software Guide : EndCodeSnippet

  std::cout << "\nInitial parameters of the registration process: " << std::endl
            << movingInitTx->GetParameters() << std::endl;

  std::cout << "\nTranslation parameters after registration: " << std::endl
            << transOptimizer->GetCurrentPosition() << std::endl
            << " Last LearningRate: " << transOptimizer->GetCurrentStepLength()
            << std::endl;

  std::cout << "\nAffine parameters after registration: " << std::endl
            << SOptimizer->GetCurrentPosition() << std::endl
            << " Last LearningRate: " << SOptimizer->GetLearningRate()
            << std::endl;

  using ResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();


  resample->SetTransform(compositeTransform);
  resample->SetInput(caster2->GetOutput());

  PixelType backgroundGrayLevel = 0;

  resample->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
  resample->SetOutputOrigin(fixedImage->GetOrigin());
  resample->SetOutputSpacing(fixedImage->GetSpacing());
  resample->SetOutputDirection(fixedImage->GetDirection());
  resample->SetDefaultPixelValue(backgroundGrayLevel);

  using OutputPixelType = unsigned char;
  using OutputImageType = itk::Image<OutputPixelType, Dimension>;
  using CastFilterType = itk::CastImageFilter<FixedImageType, OutputImageType>;
  using WriterType = itk::ImageFileWriter<OutputImageType>;

  WriterType::Pointer     writer = WriterType::New();
  CastFilterType::Pointer caster = CastFilterType::New();

  writer->SetFileName(m_outputFileName);
	resample->Update();
  caster->SetInput(resample->GetOutput());
	caster->Update();
  writer->SetInput(caster->GetOutput());
  writer->Update();

 if (m_inputmove2!="None"){

 std::cout << "\n The scar registration ... " << std::endl;
	  reader3->SetFileName( m_inputmove2);
	  reader3->Update();
	  caster3->SetInput( reader3->GetOutput());
	  using ResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
	  ResampleFilterType::Pointer resample1 = ResampleFilterType::New();

	  resample1->SetTransform(compositeTransform);
	  resample1->SetInput(caster3->GetOutput());

	  PixelType backgroundGrayLevel = 0;

	  resample1->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	  resample1->SetOutputOrigin(fixedImage->GetOrigin());
	  resample1->SetOutputSpacing(fixedImage->GetSpacing());
	  resample1->SetOutputDirection(fixedImage->GetDirection());
	  resample1->SetDefaultPixelValue(backgroundGrayLevel);



	  WriterType::Pointer     writer1 = WriterType::New();
	  CastFilterType::Pointer caster4 = CastFilterType::New();

	  writer1->SetFileName( m_inputmove2);
	resample1->Update();
	  caster4->SetInput(resample1->GetOutput());
	caster4->Update();
	  writer1->SetInput(caster4->GetOutput());
	  writer1->Update();
	}

}








void deformable_registration::center_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName)
{





 		std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  		Resample1->resample(m_inputfix,512,1);
  		std::shared_ptr<Resample> Resample12 =std::make_shared<Resample>();
  		Resample12->resample(m_inputmove,512,1);

//TODO
//sourc3e : https://itk.org/ItkSoftwareGuide.pdf page 668/997
//https://github.com/InsightSoftwareConsortium/ITK/blob/master/Examples/RegistrationITKv4/ImageRegistration7.cxx

	  using InputPixelType = unsigned char;
	  using InputImageType = itk::Image<InputPixelType, 2>;
	  using ImageReaderType = itk::ImageFileReader<InputImageType>;
	   ImageReaderType::Pointer  reader1 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader2 =  ImageReaderType::New();
	    ImageReaderType::Pointer  reader3 =  ImageReaderType::New();
	  constexpr unsigned int Dimension = 2;
	  using PixelType = double;
	  using FixedImageType = itk::Image<PixelType, Dimension>;
	  using MovingImageType = itk::Image<PixelType, Dimension>;

	  // Software Guide : EndCodeSnippet
	    using CastFilterType1 = itk::CastImageFilter<InputImageType, FixedImageType>;
	  CastFilterType1::Pointer caster1 = CastFilterType1::New();
	    using CastFilterType2 = itk::CastImageFilter<InputImageType, MovingImageType>;
	  CastFilterType2::Pointer caster2 = CastFilterType2::New();
	  CastFilterType2::Pointer caster3 = CastFilterType2::New();
	  reader1->SetFileName( m_inputfix);
	  reader2->SetFileName( m_inputmove);

	  reader1->Update();
	  reader2->Update();


	  caster1->SetInput( reader1->GetOutput());
	  caster2->SetInput( reader2->GetOutput());
	  caster1->Update();
	 caster2->Update();

  // Set up registration
  using InternalImageType = itk::Image<double, Dimension>;
  using TransformType = itk::Euler2DTransform<double>;
  //using OptimizerType = itk::RegularStepGradientDescentOptimizer;
  using InterpolatorType = itk::LinearInterpolateImageFunction<InternalImageType, double>;
  //using MetricType = itk::MattesMutualInformationImageToImageMetric<InternalImageType, InternalImageType>;
  using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
  using MetricType = itk::MeanSquaresImageToImageMetricv4<FixedImageType, MovingImageType>;
  using RegistrationType = itk::ImageRegistrationMethodv4<InternalImageType, InternalImageType>;
  using InitializerType = itk::CenteredTransformInitializer<TransformType, InputImageType, InputImageType>;
 
  InitializerType::Pointer  initializer = InitializerType::New();
  TransformType::Pointer    transform = TransformType::New();
  OptimizerType::Pointer    optimizer = OptimizerType::New();
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  RegistrationType::Pointer registration = RegistrationType::New();
  MetricType::Pointer       metric = MetricType::New();

  // Set up the registration framework
  initializer->SetFixedImage(reader1->GetOutput());
  initializer->SetMovingImage(reader2->GetOutput());
  initializer->SetTransform(transform);
 
  //transform->SetIdentity();
  initializer->GeometryOn();
//initializer->MomentsOn();
  initializer->InitializeTransform();


transform->SetAngle( 0.0 );

  registration->SetOptimizer(optimizer);

  registration->SetMetric(metric);
  registration->SetFixedImage(caster1->GetOutput());
  registration->SetMovingImage(caster2->GetOutput());

 //TransformType::InputPointType centerFixed;
 // centerFixed[0] = 511;
 // centerFixed[1] = 511;

 // const unsigned int numberOfFixedParameters = transform->GetFixedParameters().Size();
  //TransformType::ParametersType fixedParameters(numberOfFixedParameters);
 // for (unsigned int i = 0; i < numberOfFixedParameters; ++i)
  //{
    //fixedParameters[i] = centerFixed[i];
 // }
 // transform->SetFixedParameters(fixedParameters);

  registration->SetInitialTransform(transform);
   registration->InPlaceOn();

 
  
  optimizer->SetLearningRate(0.01);
  optimizer->SetMinimumStepLength(0.00001);
  optimizer->SetNumberOfIterations(2000);

  const unsigned int numberOfParameters = transform->GetNumberOfParameters();
using OptimizerScalesType=OptimizerType::ScalesType;
OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );
	const double translationScale= 1.0 / 1000.0;
	optimizerScales[0]=  1.0;
	optimizerScales[1]=translationScale;
	optimizerScales[2]=translationScale;


  optimizer->SetScales(optimizerScales);
 
 constexpr unsigned int numberOfLevels = 1;

  RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
  shrinkFactorsPerLevel.SetSize(1);
  shrinkFactorsPerLevel[0] = 1;

  RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
  smoothingSigmasPerLevel.SetSize(1);
  smoothingSigmasPerLevel[0] = 0;

  registration->SetNumberOfLevels(numberOfLevels);
  registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
  registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);


  try
  {
    registration->Update();
    std::cout << "Optimizer stop condition: "
              << registration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
  }
  catch (itk::ExceptionObject & err)
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;

  }

  TransformType::ParametersType finalParameters = transform->GetParameters();



  const double finalAngle = finalParameters[0];
  const double finalTranslationX = finalParameters[1];
  const double finalTranslationY = finalParameters[2];

  const double rotationCenterX =
    registration->GetOutput()->Get()->GetFixedParameters()[0];
  const double rotationCenterY =
    registration->GetOutput()->Get()->GetFixedParameters()[1];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();


  // Print out results
  //
  const double finalAngleInDegrees = finalAngle * 180.0 / itk::Math::pi;

  std::cout << std::endl;
  std::cout << "Result = " << std::endl;

  std::cout << " Angle (radians) = " << finalAngle << std::endl;
  std::cout << " Angle (degrees) =  " << finalAngleInDegrees << std::endl;
  std::cout << " Translation X   = " << finalTranslationX << std::endl;
  std::cout << " Translation Y   = " << finalTranslationY << std::endl;
  std::cout << " Fixed Center X  = " << rotationCenterX << std::endl;
  std::cout << " Fixed Center Y  = " << rotationCenterY << std::endl;
  std::cout << " Iterations      = " << numberOfIterations << std::endl;
  std::cout << " Metric value    = " << bestValue << std::endl;

  using ResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
  ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  resampler->SetTransform(transform);
  resampler->SetInput(caster2->GetOutput());

  FixedImageType::Pointer fixedImage = caster1->GetOutput();

  resampler->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
  resampler->SetOutputOrigin(fixedImage->GetOrigin());
  resampler->SetOutputSpacing(fixedImage->GetSpacing());
  resampler->SetOutputDirection(fixedImage->GetDirection());
  resampler->SetDefaultPixelValue(0);

 using OutputPixelType = unsigned char;
  using OutputImageType = itk::Image<OutputPixelType, Dimension>;
  using CastFilterType = itk::CastImageFilter<FixedImageType, OutputImageType>;
  using WriterType = itk::ImageFileWriter<OutputImageType>;

  WriterType::Pointer     writer = WriterType::New();
  CastFilterType::Pointer caster = CastFilterType::New();

  writer->SetFileName(m_outputFileName);
  resampler->Update();
  caster->SetInput(resampler->GetOutput());
  //  caster->Update();
	writer->SetInput(caster->GetOutput());
  writer->Update();

if (m_inputmove2!="None"){

 std::cout << "\n The scar registration ... " << std::endl;
	  reader3->SetFileName( m_inputmove2);
	  reader3->Update();
	  caster3->SetInput( reader3->GetOutput());
	  using ResampleFilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;
	  ResampleFilterType::Pointer resample1 = ResampleFilterType::New();

	  resample1->SetTransform(transform);
	  resample1->SetInput(caster3->GetOutput());

	  PixelType backgroundGrayLevel = 0;

	  resample1->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	  resample1->SetOutputOrigin(fixedImage->GetOrigin());
	  resample1->SetOutputSpacing(fixedImage->GetSpacing());
	  resample1->SetOutputDirection(fixedImage->GetDirection());
	  resample1->SetDefaultPixelValue(backgroundGrayLevel);



	  WriterType::Pointer     writer1 = WriterType::New();
	  CastFilterType::Pointer caster4 = CastFilterType::New();

	  writer1->SetFileName( m_inputmove2);

	  caster4->SetInput(resample1->GetOutput());
	  writer1->SetInput(caster4->GetOutput());
	  writer1->Update();
	}


}

void deformable_registration::deform_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName)
{

//TODO
/*
 if (m_inputmovie2!="None"){
	auto finalParameters = transform->GetParameters();
	  auto translationAlongX = finalParameters[0];
	  auto translationAlongY = finalParameters[1];
	  auto angleimage = finalParameters[2];
	  auto scaleimage = finalParameters[3];
	double center[2];
	center[0]=translationAlongX;
	center[1]=translationAlongY;
	std::shared_ptr< Segmentation_Pipeline > SP=std::make_shared< Segmentation_Pipeline >();
	SP->align_images(m_inputmove2,center);
}
*/

}


void deformable_registration::compute_b_splines(const char * m_inputfix, const char * m_inputmove, const char * m_outputFileName, const char * vector_image ,const char * m_outputFileName2 )
{


  constexpr unsigned int ImageDimension = 2;

  using PixelType = unsigned char;
  using FixedImageType = itk::Image<PixelType, ImageDimension>;
  using MovingImageType = itk::Image<PixelType, ImageDimension>;

  using FixedReaderType = itk::ImageFileReader<FixedImageType>;
  using MovingReaderType = itk::ImageFileReader<MovingImageType>;

  using MovingWriterType = itk::ImageFileWriter<MovingImageType>;
  // Software Guide : EndCodeSnippet

  FixedReaderType::Pointer fixedReader = FixedReaderType::New();
  fixedReader->SetFileName(m_inputfix);
  fixedReader->Update();



  MovingReaderType::Pointer movingReader = MovingReaderType::New();
  MovingWriterType::Pointer movingWriter = MovingWriterType::New();

  movingReader->SetFileName(m_inputmove);
  movingWriter->SetFileName(m_outputFileName);


  FixedImageType::ConstPointer fixedImage = fixedReader->GetOutput();


  using FilterType = itk::ResampleImageFilter<MovingImageType, FixedImageType>;

  FilterType::Pointer resampler = FilterType::New();

  using InterpolatorType = itk::LinearInterpolateImageFunction<MovingImageType, double>;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  resampler->SetInterpolator(interpolator);

  //  Software Guide : BeginCodeSnippet
  FixedImageType::SpacingType   fixedSpacing = fixedImage->GetSpacing();
  FixedImageType::PointType     fixedOrigin = fixedImage->GetOrigin();
  FixedImageType::DirectionType fixedDirection = fixedImage->GetDirection();

  resampler->SetOutputSpacing(fixedSpacing);
  resampler->SetOutputOrigin(fixedOrigin);
  resampler->SetOutputDirection(fixedDirection);


  FixedImageType::RegionType fixedRegion = fixedImage->GetBufferedRegion();
  FixedImageType::SizeType   fixedSize = fixedRegion.GetSize();
  resampler->SetSize(fixedSize);
  resampler->SetOutputStartIndex(fixedRegion.GetIndex());


  resampler->SetInput(movingReader->GetOutput());

  movingWriter->SetInput(resampler->GetOutput());
 
  const unsigned int     SpaceDimension = ImageDimension;
  constexpr unsigned int SplineOrder = 3;
  using CoordinateRepType = double;

  using TransformType =itk::BSplineTransform<CoordinateRepType, SpaceDimension, SplineOrder>;

  TransformType::Pointer bsplineTransform = TransformType::New();
 
  constexpr unsigned int numberOfGridNodes = 7;

  TransformType::PhysicalDimensionsType fixedPhysicalDimensions;
  TransformType::MeshSizeType           meshSize;

  for (unsigned int i = 0; i < SpaceDimension; i++)
  {
    fixedPhysicalDimensions[i] =
      fixedSpacing[i] * static_cast<double>(fixedSize[i] - 1);
  }
  meshSize.Fill(numberOfGridNodes - SplineOrder);

  bsplineTransform->SetTransformDomainOrigin(fixedOrigin);
  bsplineTransform->SetTransformDomainPhysicalDimensions(fixedPhysicalDimensions);
  bsplineTransform->SetTransformDomainMeshSize(meshSize);
  bsplineTransform->SetTransformDomainDirection(fixedDirection);


  using ParametersType = TransformType::ParametersType;
  const unsigned int numberOfParameters = bsplineTransform->GetNumberOfParameters();

  const unsigned int numberOfNodes = numberOfParameters / SpaceDimension;

  ParametersType parameters(numberOfParameters);

  bsplineTransform->SetParameters(parameters);

  resampler->SetTransform(bsplineTransform);

    movingWriter->Update();

  //  Software Guide : EndCodeSnippet

  using VectorType = itk::Vector<float, ImageDimension>;
  using DisplacementFieldType = itk::Image<VectorType, ImageDimension>;

  DisplacementFieldType::Pointer field = DisplacementFieldType::New();
  field->SetRegions(fixedRegion);
  field->SetOrigin(fixedOrigin);
  field->SetSpacing(fixedSpacing);
  field->SetDirection(fixedDirection);
  field->Allocate();

  using FieldIterator = itk::ImageRegionIterator<DisplacementFieldType>;
  FieldIterator fi(field, fixedRegion);

  fi.GoToBegin();

  TransformType::InputPointType    fixedPoint;
  TransformType::OutputPointType   movingPoint;
  DisplacementFieldType::IndexType index;

  VectorType displacement;

  while (!fi.IsAtEnd())
  {
    index = fi.GetIndex();
    field->TransformIndexToPhysicalPoint(index, fixedPoint);
    movingPoint = bsplineTransform->TransformPoint(fixedPoint);
    displacement = movingPoint - fixedPoint;
    fi.Set(displacement);
    ++fi;
  }

// write fields rgb image
using FieldWriterType = itk::ImageFileWriter< DisplacementFieldType >;
FieldWriterType::Pointer fieldWriter = FieldWriterType::New();
fieldWriter->SetFileName( "field.mha" );
fieldWriter->SetInput( field );
fieldWriter->Update();


Image_segment_Thresholding("field.mha" , "field.jpg" , 1, 254, false);
 MovingReaderType::Pointer ImagR =  MovingReaderType::New();
  ImagR->SetFileName("field.jpg" );
ImagR->Update();

if (m_outputFileName2==("None")){
  constexpr unsigned int Dimension2 = 2;
  using RGBPixelType = itk::RGBPixel<unsigned char>;
  using interType = float;
  using interImageType = itk::Image<interType, Dimension2>;
  using RGBImageType = itk::Image<RGBPixelType, Dimension2>;
  RGBImageType::Pointer image = RGBImageType::New();
  using CastFilterType2 = itk::CastImageFilter<  MovingImageType, RGBImageType >;
  using WriterType2 = itk::ImageFileWriter< RGBImageType >;
  WriterType2::Pointer      writer2 =  WriterType2::New();
  CastFilterType2::Pointer  caster2 =  CastFilterType2::New();
  writer2->SetFileName( vector_image);
  caster2->SetInput(ImagR->GetOutput());
  writer2->SetInput( caster2->GetOutput()   );
  writer2->Update();
}

//write transform
  using TransformWriterType = itk::TransformFileWriter;
  TransformWriterType::Pointer transformWriter = TransformWriterType::New();
  transformWriter->AddTransform(bsplineTransform);
  transformWriter->SetFileName("transform_bspline.tfm");
  transformWriter->Update();

//compute strain transform
if (m_outputFileName2==("None")){
	std::shared_ptr<strain_compute> SC =std::make_shared<strain_compute>(root);
	SC->strain_transformation_compute( bsplineTransform,m_outputFileName2 );
}

}

void deformable_registration::compute_demons(const char * m_inputfix, const char * m_inputmove, const char * m_outputFileName, const char * vector_image,const char * m_outputFileName2  )
{

  constexpr unsigned int Dimension = 2;
  using PixelType = unsigned short;
  using FixedImageType = itk::Image< PixelType, Dimension >;
  using MovingImageType = itk::Image< PixelType, Dimension >;
  using FixedImageReaderType = itk::ImageFileReader< FixedImageType  >;
  using MovingImageReaderType = itk::ImageFileReader< MovingImageType >;
  FixedImageReaderType::Pointer fixedImageReader   = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();
  fixedImageReader->SetFileName( m_inputfix );
  movingImageReader->SetFileName( m_inputmove );

  using InternalPixelType = float;
  using InternalImageType = itk::Image< InternalPixelType, Dimension >;
  using FixedImageCasterType = itk::CastImageFilter< FixedImageType,  InternalImageType >;
  using MovingImageCasterType = itk::CastImageFilter< MovingImageType,  InternalImageType >;
  FixedImageCasterType::Pointer fixedImageCaster = FixedImageCasterType::New();
  MovingImageCasterType::Pointer movingImageCaster = MovingImageCasterType::New();
  fixedImageCaster->SetInput( fixedImageReader->GetOutput() );
  movingImageCaster->SetInput( movingImageReader->GetOutput() );

  using MatchingFilterType = itk::HistogramMatchingImageFilter<InternalImageType, InternalImageType >;
  MatchingFilterType::Pointer matcher = MatchingFilterType::New();
  matcher->SetInput( movingImageCaster->GetOutput() );
  matcher->SetReferenceImage( fixedImageCaster->GetOutput() );
  matcher->SetNumberOfHistogramLevels( 1024 );
  matcher->SetNumberOfMatchPoints( 7 );

  using VectorPixelType = itk::Vector< float, Dimension >;
  using DisplacementFieldType = itk::Image<  VectorPixelType, Dimension >;
  using RegistrationFilterType = itk::SymmetricForcesDemonsRegistrationFilter<InternalImageType, InternalImageType,  DisplacementFieldType>;
  RegistrationFilterType::Pointer filter = RegistrationFilterType::New();

  filter->SetFixedImage( fixedImageCaster->GetOutput() );
  filter->SetMovingImage( matcher->GetOutput() );
  filter->SetNumberOfIterations( 1000 );
  filter->SetStandardDeviations( 1.0 );


  using WarperType = itk::WarpImageFilter<  MovingImageType,  MovingImageType,  DisplacementFieldType  >;
  using InterpolatorType = itk::LinearInterpolateImageFunction<  MovingImageType, double  >;
  WarperType::Pointer warper = WarperType::New();
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();
  warper->SetInput( movingImageReader->GetOutput() );
  warper->SetInterpolator( interpolator );
  warper->SetOutputSpacing( fixedImage->GetSpacing() );
  warper->SetOutputOrigin( fixedImage->GetOrigin() );
  warper->SetOutputDirection( fixedImage->GetDirection() );
  warper->SetDisplacementField( filter->GetOutput() );

  using OutputPixelType = unsigned char;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;
  using CastFilterType = itk::CastImageFilter<  InternalImageType, OutputImageType >;
  using WriterType = itk::ImageFileWriter< OutputImageType >;
  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();
  writer->SetFileName( m_outputFileName);
  caster->SetInput(  matcher->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  writer->Update();

// write fields rgb image
using FieldWriterType = itk::ImageFileWriter< DisplacementFieldType >;
FieldWriterType::Pointer fieldWriter = FieldWriterType::New();
fieldWriter->SetFileName( "field.mha" );
fieldWriter->SetInput( filter->GetOutput() );
fieldWriter->Update();


Image_segment_Thresholding("field.mha" , "field.jpg" , 1, 254, false);
  using ReaderTypew = itk::ImageFileReader< OutputImageType >;
  ReaderTypew::Pointer  ImagR = ReaderTypew::New();
  ImagR->SetFileName("field.jpg" );
ImagR->Update();
if (m_outputFileName2==("None")){
 using RGBPixelType = itk::RGBPixel<unsigned char>;
 using RGBImageType = itk::Image<RGBPixelType, Dimension>;
 RGBImageType::Pointer image = RGBImageType::New();
 using CastFilterType2 = itk::CastImageFilter<    OutputImageType , RGBImageType >;
 using WriterType2 = itk::ImageFileWriter< RGBImageType >;
 WriterType2::Pointer      writer2 =  WriterType2::New();
 CastFilterType2::Pointer  caster2 =  CastFilterType2::New();
 writer2->SetFileName( vector_image);
  caster2->SetInput(ImagR->GetOutput() );
  writer2->SetInput( caster2->GetOutput()   );
  writer2->Update();
}

//compute strain transform
if (m_outputFileName2==("None")){
	std::shared_ptr<strain_compute> SC =std::make_shared<strain_compute>(root);

	 DisplacementFieldType::Pointer demons_displacement = DisplacementFieldType::New();
	demons_displacement=filter->GetOutput();

	SC->strain_displacement_compute( demons_displacement ,m_outputFileName2 );

}

}

/*
void deformable_registration::FEM(const char * m_inputfix, const char * m_inputmove, const char * m_outputFileName, const char * vector_image,const char * m_outputFileName2 )
{
constexpr unsigned int ImageDimension = 2;

  using InputPixelType = short;
  using InputImageType = itk::Image<InputPixelType, ImageDimension>;
  using MeshPixelType = double;
  using MeshType = itk::Mesh<MeshPixelType, ImageDimension>;

  using ImageReaderType = itk::ImageFileReader<InputImageType>;

  // Read fixed image
  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName(m_inputfix);

 fixedImageReader->Update();

  // Read moving image
  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName(m_inputmove);

  movingImageReader->Update();

  // Read mask image
  ImageReaderType::Pointer maskImageReader = ImageReaderType::New();
  maskImageReader->SetFileName(m_inputmove);

  maskImageReader->Update();

  // Read mesh

    using FilterType = itk::BinaryMask3DMeshSource< InputImageType, MeshType >;
    FilterType::Pointer filtermesh = FilterType::New();
    filtermesh->SetInput(fixedImageReader->GetOutput());
    filtermesh->SetObjectValue(1);

    using WriterType = itk::MeshFileWriter< MeshType >;
    WriterType::Pointer writermesh = WriterType::New();
    writermesh->SetFileName("mesh.vtk");
    writermesh->SetInput(filtermesh->GetOutput());
 writermesh->Update();
  using MeshReaderType =  itk::MeshFileReader<MeshType>;

  MeshReaderType::Pointer meshReader = MeshReaderType::New();
  meshReader->SetFileName("mesh.vtk");

  meshReader->Update();


  // Create PhysicsBasedNonRigidRegistrationMethod filter
  using DeformationFieldType = itk::Image<itk::Vector<MeshPixelType, ImageDimension>, ImageDimension>;
  using PBNRRFilterType = itk::fem::PhysicsBasedNonRigidRegistrationMethod<InputImageType,InputImageType,InputImageType, MeshType, DeformationFieldType>;

  PBNRRFilterType::Pointer filter = PBNRRFilterType::New();


  InputImageType::Pointer fixedImage = fixedImageReader->GetOutput();
  filter->SetFixedImage(fixedImage);

  InputImageType::Pointer movingImage = movingImageReader->GetOutput();
  filter->SetMovingImage(movingImage);

  InputImageType::Pointer maskImage = maskImageReader->GetOutput();
  filter->SetMaskImage(maskImage);

  MeshType::Pointer mesh = meshReader->GetOutput();
  filter->SetMesh(mesh);

  double selectionFraction = 0.02;
  filter->SetSelectFraction(selectionFraction);

  unsigned int nonConnectivity = 1;
  filter->SetNonConnectivity(nonConnectivity);

  auto blockRadiusValue = static_cast<PBNRRFilterType::ImageSizeType::SizeValueType>(4.0);
  PBNRRFilterType::ImageSizeType blockRadius;
  blockRadius.Fill(blockRadiusValue);
  filter->SetBlockRadius(blockRadius);


  auto searchRadiusValue = static_cast<PBNRRFilterType::ImageSizeType::SizeValueType>(4.0);
  PBNRRFilterType::ImageSizeType searchRadius;
  searchRadius.Fill(searchRadiusValue);
  filter->SetSearchRadius(searchRadius);

  unsigned int approximationSteps = 100;
  filter->SetApproximationSteps(approximationSteps);

  unsigned int outlierRejectionSteps = 10;
  filter->SetOutlierRejectionSteps(outlierRejectionSteps);


 filter->Update();


  // Display the FEM filter to improve code coverage
  const PBNRRFilterType::FEMFilterType * FEMFilter = filter->GetFEMFilter();
  std::cerr << "FEMFilter: " << FEMFilter << std::endl;


  DeformationFieldType::Pointer deformationField = filter->GetOutput();

  // Warp image
  using WarpFilterType = itk::WarpImageFilter<InputImageType, InputImageType, DeformationFieldType>;
  WarpFilterType::Pointer warpFilter = WarpFilterType::New();

  using InterpolatorType = itk::LinearInterpolateImageFunction<InputImageType, double>;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  warpFilter->SetInterpolator(interpolator);

  warpFilter->SetInput(movingImageReader->GetOutput());
  warpFilter->SetOutputSpacing(deformationField->GetSpacing());
  warpFilter->SetOutputOrigin(deformationField->GetOrigin());
  warpFilter->SetDisplacementField(deformationField);

  warpFilter->Update();

  // Write warped image to file
  using WriterType2 = itk::ImageFileWriter<InputImageType>;
  WriterType2::Pointer writer2 = WriterType2::New();
  writer2->SetFileName(m_outputFileName);
  writer2->SetInput(warpFilter->GetOutput());

 writer2->Update();
// link: https://github.com/InsightSoftwareConsortium/ITK/blob/master/Modules/Registration/FEM/test/itkPhysicsBasedNonRigidRegistrationMethodTest.cxx
}

*/

void deformable_registration::Image_segment_Thresholding(const char * m_inputFileName, const char * m_outputFileName, float m_lowerThreshold, float m_upperThreshold, bool inverse)
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
  if (inverse==true){  filter->SetOutsideValue( 255 );}
  filter->ThresholdOutside(  m_lowerThreshold,m_upperThreshold );

  filter->Update();

  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( m_outputFileName );
  writer->Update();
}






