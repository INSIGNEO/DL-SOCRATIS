#include "remove_image_object.h"


void remove_image_object::Image_removeholes(const char * m_inputFileName, const char * m_outputFileName )
{

  constexpr unsigned int Dimension = 2;
  using PixelType = unsigned char;
  using ImageType = itk::Image< PixelType, Dimension >;
  using ReaderType = itk::ImageFileReader< ImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_inputFileName );
  using FilterType = itk::BinaryFillholeImageFilter< ImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetForegroundValue( itk::NumericTraits< PixelType >::max() );
  //filter->SetFullyConnected(true);
  //itk::SimpleFilterWatcher watcher(filter, "filter");
  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_outputFileName );
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

void remove_image_object::remove_holles( const char * init_image){
	typedef itk::Image< unsigned char, 2 > IType;
	typedef itk::ImageFileReader< IType > ReaderType;
	typedef itk::ImageFileReader< IType > ReaderType;
	const char * inputFileName = init_image;
	bool check_point=false;
	std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
	std::shared_ptr<denoising> SM =std::make_shared<denoising>(root);
	int Itterator=1;
	while (check_point==false){
		Image_removeholes(inputFileName, inputFileName);
		//check if white
		BI->Image_binary(inputFileName,inputFileName, 10,255);
		std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
		if (Itterator==10){check_point=true;}
		++Itterator;

	}
	ReaderType::Pointer reader_final = ReaderType::New();
	reader_final->SetFileName( inputFileName  );
	reader_final->Update();
	typedef itk::ImageFileWriter< IType > WriterType;
	WriterType::Pointer writer_final = WriterType::New();
	writer_final->SetFileName(inputFileName );
	writer_final->SetInput( reader_final->GetOutput());
	writer_final->Update();
	for ( unsigned int i = 0 ; i < 20 ; ++i ){
		SM->Median_smoothing(inputFileName,inputFileName,1,1);
	}
}


void remove_image_object::remove_object( const char * i_image, const char * out_image, int number, const char * type){

	typedef itk::Image< unsigned char, 2 > IType;
	typedef itk::ImageFileReader< IType > ReaderType;
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName( i_image );
	reader->Update();

	using OutputImageType = itk::Image< unsigned short, 2 >;

  	using ConnectedComponentImageFilterType = itk::ConnectedComponentImageFilter <IType, OutputImageType >;
 	ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New ();
  	connected->SetInput(reader->GetOutput());
  	connected->Update();
 	std::cout << "Number of objects: " << connected->GetObjectCount() << std::endl;
 	using LabelShapeKeepNObjectsImageFilterType = itk::LabelShapeKeepNObjectsImageFilter< OutputImageType >;
	LabelShapeKeepNObjectsImageFilterType::Pointer opening = LabelShapeKeepNObjectsImageFilterType::New();
	opening->SetInput( connected->GetOutput() );
	opening->SetBackgroundValue( 0 );
	opening->SetNumberOfObjects( number );

	if (type=="perimeter"){opening->SetAttribute( LabelShapeKeepNObjectsImageFilterType::LabelObjectType::PERIMETER );}
	if (type=="NumberOfPixels"){opening->SetAttribute(LabelShapeKeepNObjectsImageFilterType::LabelObjectType::NUMBER_OF_PIXELS );}
	if (type=="roundness"){opening->SetAttribute(LabelShapeKeepNObjectsImageFilterType::LabelObjectType::ROUNDNESS);}
	if (type=="feret_diameter"){opening->SetAttribute(LabelShapeKeepNObjectsImageFilterType::LabelObjectType::FERET_DIAMETER );}
	if (type=="None"){std::cout << "  Default background attribute function smallest pixel intensity" << '\n' ;}//do nothing default values}

        using RescaleFilterType = itk::RescaleIntensityImageFilter< OutputImageType, IType >;
        RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
        rescaleFilter->SetOutputMinimum(0);
        rescaleFilter->SetOutputMaximum(itk::NumericTraits<unsigned char>::max());
        rescaleFilter->SetInput(opening->GetOutput());

	typedef itk::ImageFileWriter< IType > WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput( rescaleFilter->GetOutput() );
	writer->SetFileName( out_image );
	writer->Update();
}

void remove_image_object::remove_binary_object( const char * i_image, const char * out_image, int lambda, const char * type,  std::string remove_type)
{

	typedef itk::Image< unsigned char, 2 > IType;
	typedef itk::Image< float, 2 > FType;
	typedef itk::ImageFileReader< IType > ReaderType;
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName( i_image );
	reader->Update();

        //using RescaleFilterType = itk::RescaleIntensityImageFilter< IType, IType >;
        //RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
        //rescaleFilter->SetOutputMinimum(0);
        //rescaleFilter->SetOutputMaximum(255);
        //rescaleFilter->SetInput(reader->GetOutput());

	using CastFilterType2 = itk::CastImageFilter<IType, FType>;
	CastFilterType2::Pointer caster2 = CastFilterType2::New();
	caster2->SetInput( reader->GetOutput());
	caster2->Update();
 
  	using MinimumMaximumImageCalculatorType = itk::MinimumMaximumImageCalculator<FType>;
  	MinimumMaximumImageCalculatorType::Pointer minimumMaximumImageCalculatorFilter = MinimumMaximumImageCalculatorType::New();
  	minimumMaximumImageCalculatorFilter->SetImage(caster2->GetOutput());
  	minimumMaximumImageCalculatorFilter->Compute();

	using BinaryShapeOpeningImageFilter =  itk::BinaryShapeOpeningImageFilter< IType >;
	BinaryShapeOpeningImageFilter::Pointer opening = BinaryShapeOpeningImageFilter::New();
  	opening->SetInput(reader->GetOutput());
	opening->SetLambda(lambda);
	opening->SetForegroundValue(minimumMaximumImageCalculatorFilter->GetMaximum());
	opening->SetBackgroundValue(minimumMaximumImageCalculatorFilter->GetMinimum());
	opening->SetFullyConnected(true);//count the pixel if and only if are fully connected

	if (remove_type=="bigger"){opening->ReverseOrderingOn(); std::cout << "  Bigger label pixels remove" << '\n' ;}
	if (remove_type=="smaller"){std::cout << "  Smallest pixel remove" << '\n' ;}//do nothing default values}
	if (remove_type=="None"){std::cout << "  Default smallest pixel remove" << '\n' ;}//do nothing default values}
	opening->Update();

	if (type=="perimeter"){opening->SetAttribute( BinaryShapeOpeningImageFilter::LabelObjectType::PERIMETER );}
	if (type=="NumberOfPixels"){opening->SetAttribute(BinaryShapeOpeningImageFilter::LabelObjectType::NUMBER_OF_PIXELS );}
	if (type=="roundness"){opening->SetAttribute(BinaryShapeOpeningImageFilter::LabelObjectType::ROUNDNESS);}
	if (type=="feret_diameter"){opening->SetAttribute(BinaryShapeOpeningImageFilter::LabelObjectType::FERET_DIAMETER );}
	if (type=="None"){std::cout << "  Default background attribute function smallest pixel intensity" << '\n' ;}//do nothing default values}


	typedef itk::ImageFileWriter< IType> WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput( opening->GetOutput() );
	writer->SetFileName( out_image );
	writer->Update();
}

void remove_image_object::remove_pixel_object( const char * i_image, int number_pixel, int nbr_object){

		//leave only the nbr big objects
		remove_object(i_image , i_image ,  nbr_object+1, "None");
		
		
		
		//remove small objects "noise"

		remove_binary_object(i_image , i_image,   number_pixel, "None","None");

		
		// smooth
		smooth_images( i_image,  1, 200 );


		//leave only the nbr big objects
		remove_object(i_image , i_image ,  nbr_object-1, "None");

		// smooth
		smooth_images( i_image,  1, 1 );

		// smooth
		smooth_images( i_image,  20, 220 );
		

}

void remove_image_object::smooth_images( std::string predictim,  int loop, int thres )
{
					for ( unsigned int i = 0 ; i < loop ; ++i ){
						std::shared_ptr<BinaryImage> BI1 =std::make_shared<BinaryImage>();
						std::shared_ptr<denoising> SM =std::make_shared<denoising>(root);
						BI1->Image_binary((predictim).c_str(),(predictim).c_str(), thres,255);
						SM->Median_smoothing((predictim).c_str(),(predictim).c_str(),1,1);
						
					}

					
						
}
