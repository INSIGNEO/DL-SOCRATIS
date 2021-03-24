
#include "strain_compute.h"





void strain_compute::from_jpg_to_vtk(std::string jpg_file, std::string vtk_file)
{
 vtkSmartPointer<vtkJPEGReader> jpegReader =vtkSmartPointer<vtkJPEGReader>::New();
  jpegReader->SetFileName ( jpg_file.c_str() );

  vtkSmartPointer<vtkXMLImageDataWriter> writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
	writer->SetInputConnection(jpegReader->GetOutputPort());
	writer->SetFileName(vtk_file.c_str());
	writer->Write();

}


void strain_compute::from_vtk_to_jpg(std::string vtk2_file , std::string jpg2_file)
{


  vtkSmartPointer<vtkPolyDataReader> readerpoly = vtkSmartPointer<vtkPolyDataReader>::New();
  readerpoly->SetFileName(vtk2_file.c_str());
  readerpoly->Update();

 vtkSmartPointer<vtkJPEGWriter> jpegWriter =vtkSmartPointer<vtkJPEGWriter>::New();
	jpegWriter->SetInputConnection(readerpoly->GetOutputPort());
	jpegWriter->SetFileName(jpg2_file.c_str());
	jpegWriter->Write();

}

void strain_compute::strain_displacement_compute(InputImageType_strain::Pointer displacementImage,const char * m_outputFileName )
{
typedef float PixelType_strain;
	  using OutputPixelType = unsigned char;
	  using OutputImageType = itk::Image<itk::SymmetricSecondRankTensor< PixelType_strain, 2>,2 >;
	  //using RGBPixelType = itk::RGBPixel<unsigned char>;

	 // using RGBImageType = itk::Image<RGBPixelType, 2>;

	typedef itk::StrainImageFilter< InputImageType_strain, PixelType_strain, PixelType_strain > FilterType;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput( displacementImage );
	filter->SetStrainForm( FilterType::GREENLAGRANGIAN );
	filter->Update();

	typedef FilterType::OutputImageType TensorImageType;
	TensorImageType::Pointer strainTensorImage=TensorImageType::New();
	strainTensorImage = filter->GetOutput();


  using WriterType2 = itk::ImageFileWriter<TensorImageType >;
 WriterType2::Pointer      writer_tensor =  WriterType2::New();
  writer_tensor->SetFileName( "strain_displace.vtk" );
  writer_tensor->SetInput( strainTensorImage   );
  writer_tensor->Update();



from_vtk_to_jpg("strain_displace.vtk" , m_outputFileName );


}



void strain_compute::strain_transformation_compute(TransformType::Pointer inputTransform,const char * m_outputFileName )
{
typedef float PixelType_strain;
	  using OutputPixelType = unsigned char;
	  using OutputImageType = itk::Image<itk::SymmetricSecondRankTensor< PixelType_strain, 2>,2 >;

	typedef itk::TransformToStrainFilter< TransformType, PixelType_strain, PixelType_strain > TransformToStrainFilterType;
	TransformToStrainFilterType::Pointer filter = TransformToStrainFilterType::New();
	filter->SetStrainForm( TransformToStrainFilterType::GREENLAGRANGIAN );
	// Create output image information.
	typedef TransformToStrainFilterType::SizeType SizeType;
	SizeType size;
	size.Fill( 20 );
	filter->SetSize( size );
	typedef TransformToStrainFilterType::SpacingType SpacingType;
	SpacingType spacing;
	spacing.Fill( 0.7 );
	//filter->SetOutputSpacing( spacing );
	typedef TransformToStrainFilterType::PointType OriginType;
	OriginType origin;
	origin.Fill( -10.0 );
	//filter->SetOutputOrigin( origin );
	filter->SetTransform( inputTransform );
	filter->Update();
	typedef TransformToStrainFilterType::OutputImageType TensorImageType;
	TensorImageType::Pointer strainTensorImage=TensorImageType::New();
	strainTensorImage = filter->GetOutput();


  using WriterType2 = itk::ImageFileWriter<TensorImageType >;
 WriterType2::Pointer      writer_tensor =  WriterType2::New();
  writer_tensor->SetFileName( "strain_transform.vtk" );
  writer_tensor->SetInput( strainTensorImage   );
  writer_tensor->Update();
from_vtk_to_jpg("strain_transform.vtk" , m_outputFileName );

}


void strain_compute::strain_length_run(std::string Deform_path, std::string store_path  )
{


struct dirent *entry;
DIR *pDIR;

if( pDIR=opendir(Deform_path.c_str()) ){
	while(entry = readdir(pDIR)){
		if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
		std::stringstream name;
		name<<entry->d_name;
		folders_name.push_back(name.str());
		}
		}
	closedir(pDIR);
}
std::sort(folders_name.begin(), folders_name.end());

std::string strain_roi_store;
//vector<string> strain_patient_store;

//fill in the number of images slices per patient
for ( unsigned int kp = 0; kp < folders_name.size(); kp++ )
{
	vector<string> inp;
	struct dirent *entry3p;
	DIR *pDIR2p;
	//strain_patient_store.push_back(store_path+folders_name[kp]);
	strain_roi_store=store_path+folders_name[kp];


		if (mkdir((strain_roi_store).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}

	std::string number_path=Deform_path+folders_name[kp]+"/displacement/";

	if( pDIR2p=opendir((number_path).c_str()) ){
		while(entry3p = readdir(pDIR2p)){
			if( strcmp(entry3p->d_name, ".") != 0 && strcmp(entry3p->d_name, "..") != 0 && strcmp(entry3p->d_name, "contour") != 0 ) {
			std::stringstream i3p;
			i3p<<(number_path)<<entry3p->d_name;
		        inp.push_back(i3p.str());
			//cout << i3p.str() << "\n";
			}
			}
		closedir(pDIR2p);
	}

	patient_number.push_back(inp.size());
	std::sort(inp.begin(), inp.end());
	std::string T= "1" ; //ED to ES
	std::string dt= "1"; //ED to ES
	std::cout << " Run the isotropic line strain compute for the " << kp <<" patient."<<std::endl;
	strain_length_contour_compute(inp, number_path,   T,  dt, strain_roi_store);


}//end for

//create repository store


std::string commandwr2r;
commandwr2r += "rm -rf ";
commandwr2r += "../data_internal/Input/image_ES_dl/image/*/ROI";
int wrf2r= system(commandwr2r.c_str());

}


void strain_compute::strain_length_contour_compute(vector<string>  inp, std::string number_path,  std::string T, std::string dt,std::string strain_roi_store)
{//TODO modification for more slices

 
	for ( unsigned int o = 0; o < inp.size(); o++ )
	{


		determ_ED_ES(inp[o],number_path,o);
	 	//from_jpg_to_vtk(outES, number_path+"/deform_slice_"+std::to_string(o)+"_1.vtk"  );
	 	//from_jpg_to_vtk(outED, number_path+"/deform_slice_"+std::to_string(o)+"_0.vtk"  );

		std::cout << " Compute isotropic line strain for the " << o <<" slice."<<std::endl;
		std::string command2 = "python3 ";
		command2 += path_of_strain;
		command2 +="strain.py";
		command2 += " ";
		command2 += number_path+"/deform_slice_"+std::to_string(o); //vtk contours
		command2 += " ";
		command2 += T;
		command2 += " ";
		command2 += dt;
		command2 += " ";
		command2 += strain_roi_store+"strain_slice_"+std::to_string(o);
		command2 += " ";
		
		//int f= system(command2.c_str());

		//from_vtk_to_jpg(strain_roi_store+"strain_slice_"+std::to_string(o)+"_1.vtk", strain_roi_store+"strain_slice_"+std::to_string(o)+".jpg" );
		//from_vtk_to_jpg(strain_roi_store+"strain_slice_"+std::to_string(o)+"_0.vtk", strain_roi_store+"strain_slice_"+std::to_string(o)+".jpg" );
	}

}

void strain_compute::determ_ED_ES(std::string inp, std::string patient_store_path, unsigned int o)
{

	std::shared_ptr<Segmentation_Pipeline> seg =std::make_shared<Segmentation_Pipeline>(root);
bool black=seg->check_black_image((inp).c_str(),5000,30);
if (black==false){

	//k-means smoothing and detection 
		std::shared_ptr<kmeans> kmean_def =std::make_shared<kmeans>();
		double mean_def[4];
		mean_def[0]=0;
		mean_def[1]=30;
		mean_def[2]=100;
		mean_def[3]=180;


		kmean_def->Image_kmeans((inp).c_str(), (patient_store_path+"kmeans"+std::to_string(o)+".jpg").c_str(), 4,4, mean_def);
		using MeasurementVectorType1 = itk::Vector< float, 1 >;
		using SampleType1 = itk::Statistics::ListSample< MeasurementVectorType1 >;
		SampleType1::Pointer sample_PB = SampleType1::New();
		sample_PB=kmean_def->Getmeans();

		double mean=sample_PB->GetMeasurementVector(0)[1];
		double mean_interval[2];
		mean_interval[0]=sample_PB->GetMeasurementVector(0)[0];
		mean_interval[1]=sample_PB->GetMeasurementVector(0)[2]-5;
		std::shared_ptr<deformable_registration> thresh =std::make_shared<deformable_registration>(root);
		thresh->Image_segment_Thresholding((patient_store_path+"kmeans"+std::to_string(o)+".jpg").c_str(), (patient_store_path+"threshold_displacement"+std::to_string(o)+".jpg").c_str(), mean_interval[0], mean_interval[1], false);




	//region-growing
	std::shared_ptr<Region_grown> RG =std::make_shared<Region_grown>();

	using PixelType = unsigned char;
	using ImageType = itk::Image< PixelType, 2 >;
	typedef  itk::ImageFileReader<  ImageType  > ReaderType;
	ReaderType::Pointer reader2 = ReaderType::New();
	reader2->SetFileName( (inp).c_str() );
	reader2->Update();
	ImageType::Pointer image2 = reader2->GetOutput();
	ImageType::RegionType region2 = image2->GetLargestPossibleRegion();
	ImageType::SizeType size2 = region2.GetSize();

	// determine the seed points
	int radius_bewond_2=10;
	int size=0;
	using MeasurementVectorType = itk::Vector< float, 2 >;
	using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;
	SampleType::Pointer seedpoints = SampleType::New();

	bool exit=false;

	while (size<=0){

		std::string command;
		command += "rm -rf ";
		command += (patient_store_path+"seed_points_determ"+std::to_string(o)+".jpg");
		int wrf2= system(command.c_str());

		std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
		 MI->Crop_cicle((patient_store_path+"kmeans"+std::to_string(o)+".jpg").c_str(), 255,255,radius_bewond_2,(patient_store_path+"seed_points_determ"+std::to_string(o)+".jpg").c_str());
		seedpoints=RG->determine_gridmask((patient_store_path+"seed_points_determ"+std::to_string(o)+".jpg").c_str(),(patient_store_path+"gridmask"+std::to_string(o)+".jpg").c_str(), mean_interval);
		size=seedpoints->Size();

	radius_bewond_2=radius_bewond_2+2;
	if (radius_bewond_2>=240){size=1;std::cout << "there is no displacement between ES and ED  " <<std::endl; exit=true;}
	}
	std::cout << " list for seedpoint size " << size<<std::endl;

	if (exit==false){
		int case_heap=0;
		MeasurementVectorType seed_get=RG->determine_nearest_point(seedpoints,case_heap);
		int case_heap2=1;
		MeasurementVectorType seed_get2=RG->determine_nearest_point(seedpoints,case_heap2);

		int seed[2];
		int seed2[2];

		seed[0]=seed_get[0];
		seed[1]=seed_get[1];
		seed2[0]=seed_get2[0];
		seed2[1]=seed_get2[1];

/*
		float low=sample_PB->GetMeasurementVector(0)[0];
		float high=sample_PB->GetMeasurementVector(0)[2]-10;
		float multiplier=2;
		float multiplier2=2;

				//std::cout << " seed one " << seed[0]<<","<<seed[1] <<" position"<<std::endl;
				//std::cout << " seed two " << seed2[0]<<","<<seed2[1] <<" position"<<std::endl;
		int radius=20;
		int radius2=20;
		RG->Confident_Connected((patient_store_path+"kmeans"+std::to_string(o)+".jpg").c_str(), (patient_store_path+"Confid_connect_"+std::to_string(o)+".jpg").c_str(), seed, multiplier, radius);
		RG->Vector_Confident_Connected((patient_store_path+"kmeans"+std::to_string(o)+".jpg").c_str(), (patient_store_path+"VConfid_connect_"+std::to_string(o)+".jpg").c_str(), low,  high, seed2,  multiplier2,  radius2);

		//level set
		//std::shared_ptr<BinaryImage> BI2 =std::make_shared<BinaryImage>();
		//std::shared_ptr<boundarysegm> BS =std::make_shared<boundarysegm>();
		//std::shared_ptr<Sobel> SO_cycle =std::make_shared<Sobel>();
		//SO_cycle->Image_Sobel("cicle_mask.jpg", (patient_store_path+"cycle_mask"+std::to_string(o)+".jpg").c_str());
		//BS->level_set((patient_store_path+"kmeans"+std::to_string(o)+".jpg").c_str(), "diff.mha",(patient_store_path+"cycle_mask"+std::to_string(o)+".jpg").c_str(), 10, 0.0, 0.0, 0.0, 0.0);
		//BI2->Image_binary("diff.mha",(patient_store_path+"levelset"+std::to_string(o)+".jpg").c_str(), 1,255);


		using PixelType = unsigned char;
		using ImageType = itk::Image< PixelType, 2 >;
		using OutputImageType = itk::Image< PixelType, 2 >;
		typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;
		typedef  itk::ImageFileReader<  OutputImageType  > ReaderType;
		using InvertIntensityImageFilterType = itk::InvertIntensityImageFilter <ImageType>;
		WriterType::Pointer writer2 = WriterType::New();
		ReaderType::Pointer Reader = ReaderType::New();

		Reader->SetFileName((patient_store_path+"Confid_connect_"+std::to_string(o)+".jpg").c_str() );
		Reader->Update();

		InvertIntensityImageFilterType::Pointer invertIntensityFilter2= InvertIntensityImageFilterType::New();



		//check center value
		ImageType::IndexType pixelIndex;
		pixelIndex[0] = 0;      // x position of the pixel
		pixelIndex[1] = 511;      // y position of the pixel
		ImageType::Pointer m_image = ImageType::New(); 
		m_image=Reader->GetOutput();
		ImageType::PixelType pixelValue = m_image->GetPixel( pixelIndex );

		ImageType::IndexType pixelIndex2;
		pixelIndex2[0] = 511;      // x position of the pixel
		pixelIndex2[1] = 511;      // y position of the pixel
		ImageType::PixelType pixelValue2 = m_image->GetPixel( pixelIndex2 );

		ImageType::IndexType pixelIndex3;
		pixelIndex3[0] = 0;      // x position of the pixel
		pixelIndex3[1] = 0;      // y position of the pixel
		ImageType::PixelType pixelValue3 = m_image->GetPixel( pixelIndex3 );

		if (pixelValue<=10 || pixelValue2<=10 || pixelValue3<=10){
			writer2->SetFileName((patient_store_path+"inv_Confid_connect_"+std::to_string(o)+".jpg").c_str() );
			writer2->SetInput(Reader->GetOutput() );
			writer2->Update();
		}

		if (pixelValue>=200 || pixelValue2>=200 || pixelValue3>=200){
			invertIntensityFilter2->SetInput(Reader->GetOutput());
			invertIntensityFilter2->SetMaximum(255);
			writer2->SetFileName((patient_store_path+"inv_Confid_connect_"+std::to_string(o)+".jpg").c_str() );
			writer2->SetInput(invertIntensityFilter2->GetOutput() );
			writer2->Update();
		}
		std::shared_ptr<mergeimages> MI2 =std::make_shared<mergeimages>();
	 MI2->Image_add((patient_store_path+"inv_Confid_connect_"+std::to_string(o)+".jpg").c_str(), (patient_store_path+"VConfid_connect_"+std::to_string(o)+".jpg").c_str() ,(patient_store_path+"displacement_final"+std::to_string(o)+".jpg").c_str() );
*/



		// contour extraction
			std::shared_ptr<Sobel> SO =std::make_shared<Sobel>();
			std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
			std::shared_ptr<denoising> SM =std::make_shared<denoising>(root);
			std::shared_ptr<mergeimages> MI_add_images =std::make_shared<mergeimages>(root);


			BI->Image_binary((patient_store_path+"threshold_displacement"+std::to_string(o)+".jpg").c_str(),(patient_store_path+"binary_displacement"+std::to_string(o)+".jpg").c_str(), 40,255);


			std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
			 MI->Crop_cicle((patient_store_path+"binary_displacement"+std::to_string(o)+".jpg").c_str(), seed[1],seed2[0],125,(patient_store_path+"binary_displacement_LV"+std::to_string(o)+".jpg").c_str());



		//level set
		std::shared_ptr<BinaryImage> BI2 =std::make_shared<BinaryImage>();
		std::shared_ptr<boundarysegm> BS =std::make_shared<boundarysegm>(root);
		std::shared_ptr<Sobel> SO_cycle =std::make_shared<Sobel>();
		SO_cycle->Image_Sobel("cicle_mask.jpg", (patient_store_path+"cycle_mask"+std::to_string(o)+".jpg").c_str());
		BS->level_set((patient_store_path+"normal_ES_" +std::to_string( o) + ".jpg").c_str(), "diff.mha",(patient_store_path+"cycle_mask"+std::to_string(o)+".jpg").c_str(), 10, 0.0, 0.0, 0.0, 0.0);
		BI2->Image_binary("diff.mha",(patient_store_path+"levelset"+std::to_string(o)+".jpg").c_str(), 1,255);
			 MI->Crop_cicle((patient_store_path+"levelset"+std::to_string(o)+".jpg").c_str(), seed[1],seed2[0],125,(patient_store_path+"binary_ED_LV"+std::to_string(o)+".jpg").c_str());

MI_add_images->Image_add((patient_store_path+"binary_ED_LV"+std::to_string(o)+".jpg").c_str(), (patient_store_path+"binary_displacement_LV"+std::to_string(o)+".jpg").c_str() ,(patient_store_path+"binary_displacement_LV_ED"+std::to_string(o)+".jpg").c_str() );


//countor ES

			SO->Image_Sobel((patient_store_path+"binary_displacement_LV_ED"+std::to_string(o)+".jpg").c_str(), (patient_store_path+"countor_displacement_ES"+std::to_string(o)+".jpg").c_str());
			for ( unsigned int i = 0 ; i < 40 ; ++i ){
				SM->Median_smoothing((patient_store_path+"countor_displacement_ES"+std::to_string(o)+".jpg").c_str(), (patient_store_path+"countor_displacement_ES"+std::to_string(o)+".jpg").c_str(),1,1);
				BI->Image_binary((patient_store_path+"countor_displacement_ES"+std::to_string(o)+".jpg").c_str(),(patient_store_path+"countor_displacement_ES"+std::to_string(o)+".jpg").c_str(), 40,255);

			using PixelType = unsigned char;
			using ImageType = itk::Image< PixelType, 2 >;
			typedef  itk::ImageFileWriter<  ImageType  > WriterType;
			typedef  itk::ImageFileReader<  ImageType  > ReaderTye;
			using BinaryShape = itk::BinaryShapeOpeningImageFilter<ImageType>;
			BinaryShape::Pointer binaryshape = BinaryShape::New();
			ReaderType::Pointer reader2 = ReaderType::New();
			  WriterType::Pointer writer2 = WriterType::New();
			reader2->SetFileName((patient_store_path+"countor_displacement_ES"+std::to_string(o)+".jpg").c_str() );
			reader2->Update();
			binaryshape->SetInput( reader2->GetOutput() );
			binaryshape->SetForegroundValue( 255 );
			binaryshape->SetBackgroundValue( 0 );
			binaryshape->SetLambda( 1 );
			//binaryshape->SetReverseOrdering( true );
			binaryshape->SetFullyConnected( false );
			binaryshape->SetAttribute( "NumberOfPixels" );

			writer2->SetInput( binaryshape->GetOutput() );
			writer2->SetFileName((patient_store_path+"countor_displacement_ES"+std::to_string(o)+".jpg").c_str() );
			writer2->Update();


			}


//countor ED

			SO->Image_Sobel((patient_store_path+"binary_ED_LV"+std::to_string(o)+".jpg").c_str(), (patient_store_path+"countor_displacement_ED"+std::to_string(o)+".jpg").c_str());
			for ( unsigned int i = 0 ; i < 40 ; ++i ){
				SM->Median_smoothing((patient_store_path+"countor_displacement_ED"+std::to_string(o)+".jpg").c_str(), (patient_store_path+"countor_displacement_ED"+std::to_string(o)+".jpg").c_str(),1,1);
				BI->Image_binary((patient_store_path+"countor_displacement_ED"+std::to_string(o)+".jpg").c_str(),(patient_store_path+"countor_displacement_ED"+std::to_string(o)+".jpg").c_str(), 40,255);

			using PixelType = unsigned char;
			using ImageType = itk::Image< PixelType, 2 >;
			typedef  itk::ImageFileWriter<  ImageType  > WriterType;
			typedef  itk::ImageFileReader<  ImageType  > ReaderTye;
			using BinaryShape = itk::BinaryShapeOpeningImageFilter<ImageType>;
			BinaryShape::Pointer binaryshape = BinaryShape::New();
			ReaderType::Pointer reader2 = ReaderType::New();
			  WriterType::Pointer writer2 = WriterType::New();
			reader2->SetFileName((patient_store_path+"countor_displacement_ED"+std::to_string(o)+".jpg").c_str() );
			reader2->Update();
			binaryshape->SetInput( reader2->GetOutput() );
			binaryshape->SetForegroundValue( 255 );
			binaryshape->SetBackgroundValue( 0 );
			binaryshape->SetLambda( 1 );
			//binaryshape->SetReverseOrdering( true );
			binaryshape->SetFullyConnected( false );
			binaryshape->SetAttribute( "NumberOfPixels" );

			writer2->SetInput( binaryshape->GetOutput() );
			writer2->SetFileName((patient_store_path+"countor_displacement_ED"+std::to_string(o)+".jpg").c_str() );
			writer2->Update();


			}



			outES=patient_store_path+"countor_displacement_ES"+std::to_string(o)+".jpg";
			outED=patient_store_path+"countor_displacement_ED"+std::to_string(o)+".jpg";
	}
}
else {std::cout << "Empty images ES and ED...  " <<std::endl;}

}


