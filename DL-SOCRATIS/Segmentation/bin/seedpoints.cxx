////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1
//////////////////////////////////////////////////////////////////////////////////
#include "seedpoints.h"
#include "interparameters.h"
#include <bits/stdc++.h> 
#include "Segmentation_Pipeline.h"
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <exception>
#include <fstream>
#include <iostream>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


seedpoints::seedpoints(std::string ROI, std::string inputFileName, std::string inputFileName2, std::string outputFileName ,float TH, THRS TR_threshold, std::string m_root )
{
m_ROI=ROI;
m_inputFileName=inputFileName;
m_inputFileName2=inputFileName2;
m_outputFileName=outputFileName;
THRESHOLD=TH;
TR=TR_threshold;
root=m_root;
	
//std::string initial_image = inputFileName;
}

void seedpoints::Initiallize(){
	
	preprocessing(m_inputFileName.c_str(),m_inputFileName2.c_str());
	compute_boundaries(m_ROI.c_str());
	preprocessing2();

}

void seedpoints::Update(){
	///call functions///
	//preprocessing(m_inputFileName);
	create_vector_of_images((root+"endo_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str());
	compute_distance_of_two_contour();
	find_max_intensity_funct_portion();
	compute_area_seedpoints();
}

void seedpoints::create_vector_of_images(const char * inImage_endo,const char * inImage_epi)
{

	ReaderType::Pointer reader = ReaderType::New();
	using FilterType = itk::CastImageFilter< ImageType, OutputImageType >;
	ImageType::Pointer  inputImage = ImageType::New();
	OutputImageType::Pointer outputImage = OutputImageType::New();
	reader->SetFileName( inImage_epi);
	reader->Update();
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput( reader->GetOutput() );
	filter->Update();
	outputImage = filter->GetOutput();
	/////////////////copy allocate output image///////////////
	inputImage->SetRegions( outputImage->GetRequestedRegion() );
	inputImage->CopyInformation( outputImage );
	inputImage->Allocate();
	sample_epi->SetMeasurementVectorSize( 2 );
	MeasurementVectorType mv;
	///////////////////////define itterators///////////////////
	ConstIteratorType inputIt_epi( inputImage, inputImage->GetRequestedRegion() );
	IteratorType outputIt_epi( outputImage, outputImage->GetRequestedRegion() );
	for ( inputIt_epi.GoToBegin(), outputIt_epi.GoToBegin(); !outputIt_epi.IsAtEnd();++inputIt_epi, ++outputIt_epi)
	{

		if (outputIt_epi.Get()>200){
			 ImageType::IndexType idx = inputIt_epi.GetIndex();
			 mv[0]=idx[0];
			 mv[1]=idx[1];
			 sample_epi->PushBack( mv );
		 }//end if
	} //end for

	ImageType::Pointer  inputImage2 = ImageType::New();
	OutputImageType::Pointer outputImage2 = OutputImageType::New();
	reader->SetFileName( inImage_endo);
	reader->Update();
	FilterType::Pointer filter2 = FilterType::New();
	filter2->SetInput( reader->GetOutput() );
	filter2->Update();
	outputImage2 = filter2->GetOutput();
	/////////////////copy allocate output image///////////////
	inputImage2->SetRegions( outputImage2->GetRequestedRegion() );
	inputImage2->CopyInformation( outputImage2 );
	inputImage2->Allocate();
	sample_endo->SetMeasurementVectorSize( 2 );

	///////////////////////define itterators///////////////////
	ConstIteratorType inputIt_endo( inputImage2, inputImage2->GetRequestedRegion() );
	IteratorType outputIt_endo( outputImage2, outputImage2->GetRequestedRegion() );
	//////////////////store the index of unzero data /////////////////
	for ( inputIt_endo.GoToBegin(), outputIt_endo.GoToBegin(); !inputIt_endo.IsAtEnd();++inputIt_endo, ++outputIt_endo)
	{
		if (outputIt_endo.Get()>200){
			ImageType::IndexType idx = inputIt_endo.GetIndex();
			mv[0]=idx[0];
			mv[1]=idx[1];
			sample_endo->PushBack( mv );
		}//end if
	}//end for

}

void seedpoints::preprocessing(const char * inImage_endo,const char * inImage_epi){
	//////////////////////Estimate initial center of endo///////////
	std::cout << "Preprocessing of computing of seed points..." << inImage_endo<<'\n' ;

	std::shared_ptr<denoising> SM =std::make_shared<denoising>(root);
	std::shared_ptr<Median_Filter> MF =std::make_shared<Median_Filter>();
	std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
	std::shared_ptr<Sobel> SO =std::make_shared<Sobel>();
	std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();


	for ( unsigned int i = 0 ; i < 20 ; ++i ){
		SM->Median_smoothing(inImage_endo,inImage_endo,1,1);
		SM->Median_smoothing(inImage_epi,inImage_epi,1,1);
	}

	SO->Image_Sobel(inImage_epi, (root+"epi_segm.jpg").c_str());
	BI->Image_binary((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(), 40,255);

	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM->Median_smoothing((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(),1,1);
		BI->Image_binary((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(), 40,255);
	}

	SO->Image_Sobel(inImage_endo, (root+"endo_segm.jpg").c_str());
	BI->Image_binary((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(), 40,255);


	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM->Median_smoothing((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(),1,1);
		BI->Image_binary((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(), 40,255);
	}

	MF->Image_Median_Filter((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str());
/*


*/

	////////////////////////set the ROI in center///////////////

	//using MeasurementType2 = double;
	//using MeasurementVectorType2 = itk::Vector< MeasurementType2, 2 >;
	//std::shared_ptr<boundarysegm> BS =std::make_shared<boundarysegm>();
	//BS->determine_mean_shape("epi_segm.jpg", "endo_segm.jpg","none");
	//MeasurementVectorType2 mean_endo_center;
	//mean_endo_center=BS-> Getmean_endo();
	//double center[2];
	//center[0]=mean_endo_center[0];
	//center[1]=mean_endo_center[1];
	//std::shared_ptr<Segmentation_Pipeline> seg_pipe=std::make_shared<Segmentation_Pipeline>();

	//seg_pipe->align_images(  m_ROI, center);
	//seg_pipe->align_images( "endo_segm.jpg", center);
	//seg_pipe->align_images( "epi_segm.jpg", center);


}
void seedpoints::sober_epi_endo(const char * inImage_epiendo, double scale){

	std::shared_ptr<denoising> SM =std::make_shared<denoising>(root);
	std::shared_ptr<Median_Filter> MF =std::make_shared<Median_Filter>();
	std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
	std::shared_ptr<mergeimages> MI2 =std::make_shared<mergeimages>(root);
	std::shared_ptr<mergeimages>MI_add_images =std::make_shared<mergeimages>(root);
	std::shared_ptr<Segmentation_Pipeline > SP=std::make_shared<Segmentation_Pipeline >(root);
	std::shared_ptr<remove_image_object> rih =std::make_shared<remove_image_object>(root);
	std::shared_ptr<Sobel> SO =std::make_shared<Sobel>();
	std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();

	boost::filesystem::copy_file( inImage_epiendo, (root+"comb_myo_b.jpg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
	rih->remove_holles(inImage_epiendo);
	boost::filesystem::copy_file( inImage_epiendo, (root+"inImage_epiendo.jpg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
	for ( unsigned int i = 0 ; i < 10 ; ++i ){
		BI->Image_binary((root+"comb_myo_b.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(), 10,255);
	}
	for ( unsigned int i = 0 ; i < 20 ; ++i ){
		SM->Median_smoothing((root+"comb_myo_b.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(),1,1);
	}
	//check if the epi endo is open region
	MI->Image_merge((root+"inImage_epiendo.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(), (root+"check_black.jpg").c_str()); 
	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM->Median_smoothing((root+"check_black.jpg").c_str(),(root+"check_black.jpg").c_str(),1,1);
	}
	if (SP->check_black_image((root+"check_black.jpg").c_str(),500,240)==true)
	{
		std::cout << "Delete open countor images: "<<std::endl;
			std::string commandwr2r;
			commandwr2r += "rm -rf ";
			commandwr2r += (root+"*.jpg");
			int wrf2r= system(commandwr2r.c_str());
		/*
		//draw cicle in the center of mask and close open epiendo
		double center[2];
		center[0]=0;
		center[1]=0;
		std::vector<double> center_vector;
		std::shared_ptr<unbiased_ground_trouth > UBGT =std::make_shared<unbiased_ground_trouth >(root);
		center_vector=UBGT->align_images((root+"comb_myo_b.jpg").c_str(),center,true);
		center[0]=center_vector[0];
		center[1]=center_vector[1];
		float radius=10*scale;
		float step=5*scale;
		std::string test=(root+"bcomb_myo_b.jpg");
		std::shared_ptr<Segmentation_Pipeline > SP1=std::make_shared<Segmentation_Pipeline >(root);

		while (SP1->check_black_image(test.c_str(),50,100)==false)
		{
			std::shared_ptr<mergeimages> MI21 =std::make_shared<mergeimages>(root);
			MI21->Crop_cicle((root+"comb_myo_b.jpg").c_str(), center[0],center[1],radius,(root+"binary_input_image_test.jpg").c_str());
			radius=radius+step;
			test=(root+"binary_input_image_test.jpg");
		}
		*/

		//MI_add_images->Image_add((root+"inImage_epiendo.jpg").c_str(), (root+"cicle_mask.jpg").c_str(), (root+"inImage_epiendo.jpg").c_str());

	}//end if 
	//rih->remove_holles((root+"inImage_epiendo.jpg").c_str());
	else{
		for ( unsigned int i = 0 ; i < 20 ; ++i ){
			SM->Median_smoothing((root+"inImage_epiendo.jpg").c_str(),(root+"inImage_epiendo.jpg").c_str(),1,1);
			SM->Median_smoothing((root+"comb_myo_b.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(),1,1);
		}

		SO->Image_Sobel((root+"inImage_epiendo.jpg").c_str(), (root+"epi_segm.jpg").c_str());
		BI->Image_binary((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(), 40,255);

		for ( unsigned int i = 0 ; i < 40 ; ++i ){
			SM->Median_smoothing((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(),1,1);
			BI->Image_binary((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(), 40,255);
		}

		SO->Image_Sobel((root+"comb_myo_b.jpg").c_str(), (root+"epiendo_segm.jpg").c_str());
		BI->Image_binary((root+"epiendo_segm.jpg").c_str(),(root+"epiendo_segm.jpg").c_str(), 40,255);

		for ( unsigned int i = 0 ; i < 40 ; ++i ){
			SM->Median_smoothing((root+"epiendo_segm.jpg").c_str(),(root+"epiendo_segm.jpg").c_str(),1,1);
			BI->Image_binary((root+"epiendo_segm.jpg").c_str(),(root+"epiendo_segm.jpg").c_str(), 40,255);
		}


		MI2->Image_merge(((root+"epiendo_segm.jpg").c_str()),(root+"epi_segm.jpg").c_str(), (root+"endo_segm.jpg").c_str());

		for ( unsigned int i = 0 ; i < 2 ; ++i ){
			SM->Median_smoothing((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(),1,1);
		}

		//SO->Image_Sobel((root+"endo_segm.jpg").c_str(), (root+"endo_segm.jpg").c_str());

		//for ( unsigned int i = 0 ; i < 40 ; ++i ){
		//	SM->Median_smoothing((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(),1,1);
		//	BI->Image_binary((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(), 40,255);
		//}

		MF->Image_Median_Filter((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str());
	}//end else
}

void seedpoints::preprocessing2(){

	 std::cout << "Preprocessing to compute  endo and epi second round..." << '\n' ;
	std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
	std::shared_ptr<Median_Filter> MF =std::make_shared<Median_Filter>();
	std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
	std::shared_ptr<Sobel> SO =std::make_shared<Sobel>();
	std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
	std::shared_ptr<BinaryImage> BqI232 =std::make_shared<BinaryImage>();
	std::shared_ptr<remove_image_object> rih =std::make_shared<remove_image_object>(root);
	BqI232->Image_binary((root+"crop_ROI_cut.jpg").c_str(),(root+"binary_endo.jpg").c_str(), 245,255);
	BqI232->Image_binary((root+"crop_ROI2_cut.jpg").c_str(),(root+"binary_epi.jpg").c_str(), 245,255);
	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM1->Median_smoothing((root+"binary_endo.jpg").c_str(),(root+"binary_endo.jpg").c_str(),1,1);
		std::shared_ptr<BinaryImage> Bq =std::make_shared<BinaryImage>();
		Bq->Image_binary((root+"binary_endo.jpg").c_str(),(root+"binary_endo.jpg").c_str(), 245,255);
		SM1->Median_smoothing((root+"binary_epi.jpg").c_str(),(root+"binary_epi.jpg").c_str(),1,1);
		Bq->Image_binary((root+"binary_epi.jpg").c_str(),(root+"binary_epi.jpg").c_str(), 245,255);
	}


	rih->remove_holles((root+"binary_endo.jpg").c_str());
	for ( unsigned int i = 0 ; i < 20 ; ++i ){
		SM1->Median_smoothing((root+"binary_endo.jpg").c_str(),(root+"endo_segm.jpg").c_str(),1,1);
	}
	std::shared_ptr<remove_image_object> rih2 =std::make_shared<remove_image_object>(root);
	rih2->remove_holles((root+"binary_epi.jpg").c_str());
	for ( unsigned int i = 0 ; i < 20 ; ++i ){
		SM1->Median_smoothing((root+"binary_epi.jpg").c_str(),(root+"epi_segm.jpg").c_str(),1,1);
	}


	MI->Image_merge((root+"epi_segm.jpg").c_str(),(root+ "endo_segm.jpg").c_str(), (root+"binary_epi_endo.jpg").c_str());
	SO->Image_Sobel((root+"epi_segm.jpg").c_str(), (root+"epi_segm.jpg").c_str());
	BI->Image_binary((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(), 40,255);

	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM1->Median_smoothing((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(),1,1);
		BI->Image_binary((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(), 40,255);
	}

	SO->Image_Sobel((root+"endo_segm.jpg").c_str(), (root+"endo_segm.jpg").c_str());
	BI->Image_binary((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(), 40,255);

	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM1->Median_smoothing((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(),1,1);
		BI->Image_binary((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(), 40,255);
	}

}

void seedpoints::preprocessing3(const char * inImage_myo){

	 std::cout << "Preprocessing to compute  endo and epi second round..." << '\n' ;
	std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
	std::shared_ptr<Median_Filter> MF =std::make_shared<Median_Filter>();
	std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
	std::shared_ptr<Sobel> SO =std::make_shared<Sobel>();
	std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
	std::shared_ptr<BinaryImage> BqI232 =std::make_shared<BinaryImage>();
	std::shared_ptr<remove_image_object> rih =std::make_shared<remove_image_object>(root);
	BqI232->Image_binary(inImage_myo,(root+"binary_epi.jpg").c_str(), 230,255);

	MI->Image_add((root+"binary_epi.jpg").c_str(), (root+"binary_endo.jpg").c_str(), (root+"binary_epi.jpg").c_str());
	rih->remove_holles((root+"binary_epi.jpg").c_str());

	SO->Image_Sobel((root+"binary_epi.jpg").c_str(), (root+"epi_segm.jpg").c_str());
	BI->Image_binary((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(), 40,255);

	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM1->Median_smoothing((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(),1,1);
		BI->Image_binary((root+"epi_segm.jpg").c_str(),(root+"epi_segm.jpg").c_str(), 40,255);
	}

	SO->Image_Sobel(inImage_myo, (root+"b_epi_endo.jpg").c_str());
	BI->Image_binary((root+"b_epi_endo.jpg").c_str(),(root+"b_epi_endo.jpg").c_str(), 40,255);

	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM1->Median_smoothing((root+"b_epi_endo.jpg").c_str(),(root+"b_epi_endo.jpg").c_str(),1,1);
		BI->Image_binary((root+"b_epi_endo.jpg").c_str(),(root+"b_epi_endo.jpg").c_str(), 40,255);
	}
	MI->Image_merge((root+"b_epi_endo.jpg").c_str(),(root+"epi_segm.jpg").c_str(), (root+"endo_segm.jpg").c_str());

	BI->Image_binary((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(), 40,255);

	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM1->Median_smoothing((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(),1,1);
		BI->Image_binary((root+"endo_segm.jpg").c_str(),(root+"endo_segm.jpg").c_str(), 40,255);
	}

}



void seedpoints::compute_boundaries( const char * initial_image){

	/////////////////level-set call to redefine the conter of epi endo////////////////////
	std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
	std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
	std::shared_ptr<boundarysegm> BS_initial =std::make_shared<boundarysegm>(root);
	std::shared_ptr<boundarysegm> BS =std::make_shared<boundarysegm>(root);
	std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
	std::shared_ptr<mergeimages> MI2 =std::make_shared<mergeimages>(root);
	std::shared_ptr<BinaryImage> BImage =std::make_shared<BinaryImage>();
	std::shared_ptr<BinaryImage> BImage2 =std::make_shared<BinaryImage>();
	std::shared_ptr<remove_image_object> rio =std::make_shared<remove_image_object>(root);
	std::shared_ptr<unbiased_ground_trouth> ugt =std::make_shared<unbiased_ground_trouth>(root);
	using InvertIntensityImageFilterType = itk::InvertIntensityImageFilter <ImageType>;
	 
	std::cout<<"Compute radius and center of cycle masks..."<<std::endl;

	ReaderType::Pointer reader = ReaderType::New();
	  WriterType::Pointer writer = WriterType::New();
	ReaderType::Pointer reader2 = ReaderType::New();
	  WriterType::Pointer writer2 = WriterType::New();
	  InvertIntensityImageFilterType::Pointer invertIntensityFilter2= InvertIntensityImageFilterType::New();

	//create ROI wnside cycle
	BS_initial->determine_mean_shape((root+"binary_input_image_epi.jpg").c_str(), (root+"binary_input_image_endo.jpg").c_str(),"none");
	MeasurementVectorType mean_center;
	mean_center=BS_initial->Getmean();
	int centerx;
	centerx=mean_center[0];
	int centery=mean_center[1];

	MI2->Image_merge( (root+"binary_input_image_epi.jpg").c_str(),(root+"binary_input_image_endo.jpg" ).c_str(),(root+"binary_input_image_epiendo.jpg").c_str());

	double center[2];
	center[0]=0;
	center[1]=0;
	//std::vector<double> center_vector;
	//center_vector=ugt->align_images((root+"binary_input_image_epiendo.jpg"),center,true);
	//center[0]=center_vector[0];
	//center[1]=center_vector[1];
	int radius=0;
	int radius2=0;
	BS->determine_mean_shape((root+"epi_segm.jpg").c_str(), (root+"endo_segm.jpg").c_str(),"none");
	if (sensitivity2_param==true){ BS->SetSensitivity1_param( sensitivity2_param, endo);}
	if (sensitivity3_param==true){ BS->SetSensitivity2_param( sensitivity3_param,  epi);}
	std::shared_ptr<Segmentation_Pipeline > SP1=std::make_shared<Segmentation_Pipeline >(root);
	if((SP1->check_black_image((root+"binary_input_image_endo.jpg" ).c_str(),50,40)==false) && (SP1->check_black_image((root+"binary_input_image_epi.jpg").c_str(),50,40)==false) && (SP1->check_black_image((root+"binary_input_image_epiendo.jpg").c_str(),50,40)==false))
	{
		BS->create_geodestic_seed_points((root+"epi_segm.jpg").c_str(), (root+"endo_segm.jpg").c_str());

		radius=(BS-> Getmax_radius());//+ offset_cycle;
		radius2=(BS-> Getmin_radius()) ;//+ offset_cycle;

		center[0]=centerx;
		center[1]=centery;

		//check the difference between these two threshold is iner value
	}
	else if((SP1->check_black_image((root+"binary_input_image_endo.jpg" ).c_str(),50,40)==false) && (SP1->check_black_image((root+"binary_input_image_epi.jpg").c_str(),50,40)==false) && (SP1->check_black_image((root+"binary_input_image_epiendo.jpg").c_str(),50,40)==true))
	{
		BS->Set_equal_portion_endo_epi(true);
		BS->create_geodestic_seed_points((root+"epi_segm.jpg").c_str(), (root+"endo_segm.jpg").c_str());

		radius=(BS-> Getmax_radius());//+ offset_cycle;
		radius2=(BS-> Getmin_radius()) ;//+ offset_cycle;

		center[0]=centerx;
		center[1]=centery;

		//check the difference between these two threshold is iner value
	}
	else 
	{
		BS->Set_equal_portion_endo_epi(true);
		BS->create_geodestic_seed_points((root+"epi_segm.jpg").c_str(), (root+"endo_segm.jpg").c_str());

		radius=(BS-> Getmax_radius());//+ offset_cycle;
		radius2=(BS-> Getmin_radius()) ;//+ offset_cycle;
		if(SP1->check_black_image((root+"binary_input_image_endo.jpg" ).c_str(),10,40)==true){radius2=0;}
		if(SP1->check_black_image((root+"binary_input_image_epi.jpg" ).c_str(),10,40)==true){radius=0;}
		center[0]=centerx;
		center[1]=centery;

		//check the difference between these two threshold is iner value
	}


//strategy of radious: n(random forest??)

    // check the thikness limit of myocardium
	int test_portion;
	if (radius>radius2){test_portion=(radius-percent_Threshold_initial_thin_of_myo*0.01*radius);}
	if (radius<=radius2){test_portion=(radius2-percent_Threshold_initial_thin_of_myo*0.01*radius2);}
	// check the difference of endo epi
	if (std::abs(radius-radius2)==0 ){
		radius=radius2+test_portion;
		radius2=radius2;
	}
	else if (std::abs(radius-radius2)>test_portion ){
		if (radius>radius2){radius2=radius-test_portion;}
		if (radius<=radius2){radius=radius2; radius2=radius-test_portion;}
	}
	else{std::cout<<"Error: no valid initial segmentation results.."<<std::endl;}

		
	//generally test constrains in max and min radius
	if (std::max(radius,radius2)>max_radious_threshold){
		radius2=std::min(radius,radius2);
		radius=max_radious_threshold;
		test_portion=(radius-percent_Threshold_initial_thin_of_myo*0.01*radius);
		if(radius2>=radius){radius2=radius-test_portion;}
		if (std::abs(radius-radius2)<test_portion ){
			radius2=radius-test_portion;
		}

	}	
	if (std::min(radius,radius2)<min_radious_threshold){
		radius=std::max(radius,radius2);
		radius2=min_radious_threshold;
		test_portion=(radius-percent_Threshold_initial_thin_of_myo*0.01*radius);
		if (std::abs(radius-radius2)>test_portion ){
			radius=radius2+test_portion;
		}		

	}

std::cout<<"Final radius: "<<std::endl;
std::cout<<"endo radius: "<<radius2<<std::endl;
std::cout<<"epi radius: "<<radius<<std::endl;
	MI->inverse=false;

	////////////////////////set the ROI in center///////////////

	std::shared_ptr<Segmentation_Pipeline> seg_pipe=std::make_shared<Segmentation_Pipeline>(root);
	boost::filesystem::copy_file( initial_image, (root+"ROI_align.jpg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);

	seg_pipe->align_images(  (root+"ROI_align.jpg").c_str(), center,0,1); //fixed version
	seg_pipe->align_images( (root+"endo_segm.jpg").c_str(), center,0,1);
	seg_pipe->align_images( (root+"epi_segm.jpg").c_str(), center,0,1);

	std::string initial_image1=(root+"ROI_align.jpg");

	//after aligment the center of mask has to be the center of image so:
	centerx=255;
	centery=255;

	//k-means crop ROI
	borderzone_scars_estimate( initial_image1.c_str(), radius,  radius2, centerx,  centery );

	//smmoth them 
	for ( unsigned int i = 0 ; i < 2 ; ++i ){
		SM1->Median_smoothing((root+"crop_ROI_endo.jpg").c_str(),(root+"crop_ROI_endo.jpg").c_str(),1,1);
		BI->Image_binary((root+"crop_ROI_endo.jpg").c_str(),(root+"crop_ROI_endo.jpg").c_str(), 245,255);
		SM1->Median_smoothing((root+"crop_ROI_epi.jpg").c_str(),(root+"crop_ROI_epi.jpg").c_str(),1,1);
		BI->Image_binary((root+"crop_ROI_epi.jpg").c_str(),(root+"crop_ROI_epi.jpg").c_str(), 245,255);
	}


	BS->level_set((root+"crop_ROI_epi.jpg").c_str(), (root+"crop_ROI_epi.mha").c_str(),(root+"binary_input_image_epi.jpg").c_str(), 100,0.,0.,0,0);
	BImage->Image_binary((root+"crop_ROI_epi.mha").c_str(),(root+"crop_ROI2.jpg").c_str(), 1,255);
	BImage->Image_binary((root+"crop_ROI_endo.jpg").c_str(),(root+"crop_ROI.jpg").c_str(), 1,255);
	//inverse the epi level set to be black
	reader->SetFileName((root+ "crop_ROI2.jpg").c_str());
	reader->Update();
	std::shared_ptr<Change_information> CI =std::make_shared<Change_information>();

	InvertIntensityImageFilterType::Pointer invertIntensityFilter= InvertIntensityImageFilterType::New();
	invertIntensityFilter->SetInput(reader->GetOutput());
	invertIntensityFilter->SetMaximum(255);
	writer2->SetFileName( (root+"crop_ROI2.jpg").c_str() );
	writer2->SetInput(invertIntensityFilter->GetOutput() );
	writer2->Update();
	const char * m_angle="0";
	CI ->Change_information_run((root+"crop_ROI.jpg").c_str(),  (root+"crop_ROI2.jpg").c_str());
	rio->remove_holles((root+"crop_ROI.jpg").c_str());

	std::string epi_endo_b=(root+"binary_epi_endo.jpg");

	MI->Image_add((root+"crop_ROI.jpg").c_str(), (root+"crop_ROI2.jpg").c_str(), (epi_endo_b).c_str());
	BS->level_set((epi_endo_b).c_str(), (root+"binary_epi_endo.mha").c_str(),(root+"crop_ROI.jpg").c_str(), 100,0.,0.,0,0);
	BImage2->Image_binary((root+"binary_epi_endo.mha").c_str(),(root+"binary_epi_endo2.jpg").c_str(), 1,255);

	epi_endo_b=(root+"binary_epi_endo2.jpg");
	//inverse the epi level set to be black
	reader->SetFileName( (root+"binary_epi_endo2.jpg").c_str());
	reader->Update();

	invertIntensityFilter2->SetInput(reader->GetOutput());
	invertIntensityFilter2->SetMaximum(255);

	writer2->SetFileName( epi_endo_b.c_str() );
	writer2->SetInput(invertIntensityFilter2->GetOutput() );
	writer2->Update();

	CI ->Change_information_run((root+"crop_ROI.jpg").c_str(), (epi_endo_b).c_str());
	MI->Crop_cicle((epi_endo_b).c_str(), centerx,centery,radius,(epi_endo_b).c_str());

	for ( unsigned int i = 0 ; i < 40 ; ++i ){
		SM1->Median_smoothing((root+"crop_ROI.jpg").c_str(),(root+"crop_ROI.jpg").c_str(),1,1);
		BI->Image_binary((root+"crop_ROI.jpg").c_str(),(root+"crop_ROI.jpg").c_str(), 245,255);
		SM1->Median_smoothing((epi_endo_b).c_str(),(epi_endo_b).c_str(),1,1);
		BI->Image_binary((epi_endo_b).c_str(),(epi_endo_b).c_str(), 245,255);

	}

////////initiale step:
   	int np=500; //number of pixels
	int noo=4; //number of objects

	std::shared_ptr<remove_image_object> rbio =std::make_shared<remove_image_object>(root);
	rbio->remove_object((root+"crop_ROI.jpg").c_str(), (root+"crop_ROI_cut.jpg").c_str(), 1, "None");
	//rbio->remove_pixel_object( (root+"crop_ROI.jpg").c_str(), np,noo);

	SM1->Median_smoothing((root+"crop_ROI_cut.jpg").c_str(),(root+"crop_ROI_cut.jpg").c_str(),1,1);
	BI->Image_binary((root+"crop_ROI_cut.jpg").c_str(),(root+"crop_ROI_cut.jpg").c_str(), 245,255);
	rio->remove_object((root+"crop_ROI_cut.jpg").c_str(), (root+"crop_ROI_cut.jpg").c_str(),1,"NumberOfPixels");
	//rio->remove_pixel_object( (root+"crop_ROI_cut.jpg").c_str(), np,noo);

	//second
	std::shared_ptr<remove_image_object> rbio2 =std::make_shared<remove_image_object>(root);
	rbio2->remove_object((epi_endo_b).c_str(), (root+"crop_ROI2_cut.jpg").c_str(), 1, "None");
	//boost::filesystem::copy_file((epi_endo_b).c_str(), (root+"crop_ROI2_cut.jpg").c_str(),boost::filesystem::copy_option::overwrite_if_exists);
	//rbio2->remove_pixel_object( (root+"crop_ROI2_cut.jpg").c_str(), np,(noo+1));

	SM1->Median_smoothing((root+"crop_ROI2_cut.jpg").c_str(),(root+"crop_ROI2_cut.jpg").c_str(),1,1);
	BI->Image_binary((root+"crop_ROI2_cut.jpg").c_str(),(root+"crop_ROI2_cut.jpg").c_str(), 245,255);
	rbio->remove_object((root+"crop_ROI2_cut.jpg").c_str(), (root+"crop_ROI2_cut.jpg").c_str(), 1, "None");
	//rbio2->remove_pixel_object( (root+"crop_ROI2_cut.jpg").c_str(), 500,5);

	for ( unsigned int i = 0 ; i < 40 ; ++i ){

		SM1->Median_smoothing((root+"crop_ROI_cut.jpg").c_str(),(root+"crop_ROI_cut.jpg").c_str(),1,1);
		BI->Image_binary((root+"crop_ROI_cut.jpg").c_str(),(root+"crop_ROI_cut.jpg").c_str(), 245,255);


		SM1->Median_smoothing((root+"crop_ROI2_cut.jpg").c_str(),(root+"crop_ROI2_cut.jpg").c_str(),1,1);
		BI->Image_binary((root+"crop_ROI2_cut.jpg").c_str(),(root+"crop_ROI2_cut.jpg").c_str(), 245,255);

	}



	std::shared_ptr<remove_image_object> rio3 =std::make_shared<remove_image_object>(root);
	rio3->remove_object((root+"crop_ROI_cut.jpg").c_str(), (root+"crop_ROI_cut.jpg").c_str(),1,"NumberOfPixels"); //initial 1
	//rio3->remove_pixel_object( (root+"crop_ROI_cut.jpg").c_str(), np,noo);
	SM1->Median_smoothing((root+"crop_ROI_cut.jpg").c_str(),(root+"crop_ROI_cut.jpg").c_str(),1,1);
	BI->Image_binary((root+"crop_ROI_cut.jpg").c_str(),(root+"crop_ROI_cut.jpg").c_str(), 245,255);
	SM1->SetScale_noise(10);
	SM1->SetShot_noise(true);
	SM1->SetnoiseKerner_image((root+"crop_ROI_cut.jpg").c_str());
	BI->Image_binary((root+"crop_ROI_cut.jpg").c_str(),(root+"crop_ROI_cut.jpg").c_str(), 245,255);


	rio3->remove_object((root+"crop_ROI2_cut.jpg").c_str(), (root+"crop_ROI2_cut.jpg").c_str(),1,"NumberOfPixels");//initial 1
	//rio3->remove_pixel_object( (root+"crop_ROI2_cut.jpg").c_str(), (np-100),noo);
	SM1->Median_smoothing((root+"crop_ROI2_cut.jpg").c_str(),(root+"crop_ROI2_cut.jpg").c_str(),1,1);
	BI->Image_binary((root+"crop_ROI2_cut.jpg").c_str(),(root+"crop_ROI2_cut.jpg").c_str(), 245,255);
	SM1->SetScale_noise(10);
	SM1->SetShot_noise(true);
	SM1->SetnoiseKerner_image((root+"crop_ROI2_cut.jpg").c_str());
	BI->Image_binary((root+"crop_ROI2_cut.jpg").c_str(),(root+"crop_ROI2_cut.jpg").c_str(), 245,255);

	for ( unsigned int i = 0 ; i < 52 ; ++i ){

		SM1->Median_smoothing((root+"crop_ROI_cut.jpg").c_str(),(root+"crop_ROI_cut.jpg").c_str(),1,1);
		BI->Image_binary((root+"crop_ROI_cut.jpg").c_str(),(root+"crop_ROI_cut.jpg").c_str(), 200+i,255);

		SM1->Median_smoothing((root+"crop_ROI2_cut.jpg").c_str(),(root+"crop_ROI2_cut.jpg").c_str(),1,1);
		BI->Image_binary((root+"crop_ROI2_cut.jpg").c_str(),(root+"crop_ROI2_cut.jpg").c_str(), 200+i,255);

	}
	std::shared_ptr<remove_image_object> rio4 =std::make_shared<remove_image_object>(root);
	rio4->remove_holles((root+"crop_ROI2_cut.jpg").c_str());
	std::shared_ptr<remove_image_object> rio5 =std::make_shared<remove_image_object>(root);
	rio5->remove_holles((root+"crop_ROI_cut.jpg").c_str());

}

void seedpoints::borderzone_scars_estimate( const char * image, int radius, int radius2, int centerx, int centery )
{
	float kmean_thres=kmeanFWHM_threshold;
	
	if (sensitivity1_param==true){ kmean_thres=kmean_variation_value;}

	std::cout << "  Compute k-means and border zone..." << kmean_thres<<'\n' ;
	int radius_bewond_2=(radius-radius2)+5;
	int radius_upper_midlle=((radius-radius2)*(PORTION_OF_MIDDLE_CICLE_CROP))+radius2;//+2 from portion(1-port)
	std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
	std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
	std::shared_ptr<boundarysegm> BS =std::make_shared<boundarysegm>(root);
	std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
	std::shared_ptr<Thresholding> THRES =std::make_shared<Thresholding>();
	std::shared_ptr<kmeans> kmean_PB =std::make_shared<kmeans>();
	std::shared_ptr<kmeans> kmean =std::make_shared<kmeans>();
	std::shared_ptr<kmeans> kmean_threshold =std::make_shared<kmeans>();

	using InvertIntensityImageFilterType = itk::InvertIntensityImageFilter <ImageType>;
	ReaderType::Pointer reader = ReaderType::New();
	WriterType::Pointer writer = WriterType::New();
	ReaderType::Pointer reader2 = ReaderType::New();
	WriterType::Pointer writer2 = WriterType::New();
	InvertIntensityImageFilterType::Pointer invertIntensityFilter2= InvertIntensityImageFilterType::New();


	double means_PB[7];
	double means[9];
	MI->Crop_cicle(image, centerx,centery,radius_bewond_2,(root+"kmeans_for_PB.jpg").c_str());
	means_PB[0]=0;
	means_PB[1]=100;
	means_PB[2]=170;
	means_PB[3]=190;
	means_PB[4]=205;
	means_PB[5]=225;
	means_PB[6]=240;


	kmean_PB->Image_kmeans((root+"kmeans_for_PB.jpg").c_str(), (root+"kmean_PB_result.jpg").c_str(), 7,7, means_PB);

	using MeasurementVectorType1 = itk::Vector< float, 1 >;
	using SampleType1 = itk::Statistics::ListSample< MeasurementVectorType1 >;
	SampleType1::Pointer sample_PB = SampleType1::New();
	sample_PB=kmean_PB->Getmeans();

	float start_PB=sample_PB->GetMeasurementVector(0)[1];
	float end_PB=sample_PB->GetMeasurementVector(0)[6];

	if (end_PB==240){end_PB=sample_PB->GetMeasurementVector(0)[5];}
	if (end_PB==225){end_PB=sample_PB->GetMeasurementVector(0)[4];}
	if (end_PB==205){end_PB=sample_PB->GetMeasurementVector(0)[3];}
	if (end_PB==190){end_PB=sample_PB->GetMeasurementVector(0)[2];}
	if (end_PB==170){end_PB=sample_PB->GetMeasurementVector(0)[1];}
	if (end_PB==100){end_PB=sample_PB->GetMeasurementVector(0)[0];}

	if (start_PB==100){start_PB=sample_PB->GetMeasurementVector(0)[2];}
	if (start_PB==170){start_PB=sample_PB->GetMeasurementVector(0)[3];}
	if (start_PB==190){start_PB=sample_PB->GetMeasurementVector(0)[4];}
	if (start_PB==205){start_PB=sample_PB->GetMeasurementVector(0)[5];}
	if (start_PB==225){start_PB=sample_PB->GetMeasurementVector(0)[6];}


	int BP_threshold = (end_PB);//+start_PB)*0.5;
//	if(end_PB==start_PB){BP_threshold=(0+end_PB)*0.5;}

	double means_init[12];
	MI->Crop_cicle(image, centerx,centery,radius,(root+"kmeans_for_threshold.jpg").c_str());
	std::shared_ptr<mergeimages> MI_invert =std::make_shared<mergeimages>(root);
	MI_invert->inverse=true;
	MI_invert->Crop_cicle((root+"kmeans_for_threshold.jpg").c_str(), centerx,centery,radius2,(root+"kmeans_for_threshold2.jpg").c_str());
	means_init[0]=0;
	means_init[1]=30;
	means_init[2]=60;
	means_init[3]=90;
	means_init[4]=120;
	means_init[5]=150;
	means_init[6]=170;
	means_init[7]=190;
	means_init[8]=205;
	means_init[9]=225;
	means_init[10]=240;
	means_init[11]=250;

	kmean_threshold->Image_kmeans((root+"kmeans_for_threshold.jpg").c_str(), (root+"kmeans_for_threshold_results.jpg").c_str(), 12,12,means_init);

	const char *kmean_th=(root+"init_kmean.jpg").c_str();

	SampleType1::Pointer sample_means = SampleType1::New();
	sample_means=kmean_threshold->Getmeans();
	SampleType1::Pointer sample = SampleType1::New();


	
	int offset=7;

	means[0]=0;
	means[1]=120;
	means[2]=150;
	means[3]=170;
	means[4]=190;
	means[5]=205;
	means[6]=225;
	means[7]=240;
	means[8]=250;
	
	float start=sample_means->GetMeasurementVector(0)[1];
	float end=sample_means->GetMeasurementVector(0)[11];

	if (end==250){end=sample_means->GetMeasurementVector(0)[10];}
	if (end==240){end=sample_means->GetMeasurementVector(0)[9];}
	if (end==225){end=sample_means->GetMeasurementVector(0)[8];}
	if (end==205){end=sample_means->GetMeasurementVector(0)[7];}
	if (end==190){end=sample_means->GetMeasurementVector(0)[6];}
	if (end==170){end=sample_means->GetMeasurementVector(0)[5];}
	if (end==150){end=sample_means->GetMeasurementVector(0)[4];}
	if (end==120){end=sample_means->GetMeasurementVector(0)[3];}
	if (end==90){end=sample_means->GetMeasurementVector(0)[2];}
	if (end==60){end=sample_means->GetMeasurementVector(0)[1];}

	if (start==30){start=sample_means->GetMeasurementVector(0)[2];}
	if (start==60){start=sample_means->GetMeasurementVector(0)[3];}
	if (start==90){start=sample_means->GetMeasurementVector(0)[4];}
	if (start==120){start=sample_means->GetMeasurementVector(0)[5];}
	if (start==150){start=sample_means->GetMeasurementVector(0)[6];}
	if (start==170){start=sample_means->GetMeasurementVector(0)[7];}
	if (start==190){start=sample_means->GetMeasurementVector(0)[8];}
	if (start==205){start=sample_means->GetMeasurementVector(0)[9];}
	if (start==225){start=sample_means->GetMeasurementVector(0)[10];}
	if (start==240){start=sample_means->GetMeasurementVector(0)[11];}

	std::cout << " Estimation: PB threshold "<< BP_threshold<<'\n';

	if (BP_threshold< end)
	{
		int scar_thresh=((end-start)*kmean_thres) +start;
 		if(end==start){scar_thresh=(0+end)*kmean_thres;}
		std::cout << " Initial estimation: Scars threshold "<< scar_thresh<<'\n' ;
		THRES->Image_based_Thresholding((root+"kmeans_for_threshold_results.jpg").c_str(), (root+"initial_scars.jpg").c_str(), scar_thresh, 255);


		kmean->Image_kmeans((root+"initial_scars.jpg").c_str(), (root+"scars_result.jpg").c_str(), 9,9, means);
		sample=kmean->Getmeans();
		float start2=sample->GetMeasurementVector(0)[1];
		float end2=sample->GetMeasurementVector(0)[8];

		if (end2==250){end2=sample->GetMeasurementVector(0)[7];}
		if (end2==240){end2=sample->GetMeasurementVector(0)[6];}
		if (end2==225){end2=sample->GetMeasurementVector(0)[5];}
		if (end2==205){end2=sample->GetMeasurementVector(0)[4];}
		if (end2==190){end2=sample->GetMeasurementVector(0)[3];}
		if (end2==170){end2=sample->GetMeasurementVector(0)[2];}
		if (end2==150){end2=sample->GetMeasurementVector(0)[1];}

		if (start2==120){start2=sample->GetMeasurementVector(0)[2];}
		if (start2==150){start2=sample->GetMeasurementVector(0)[3];}
		if (start2==170){start2=sample->GetMeasurementVector(0)[4];}
		if (start2==190){start2=sample->GetMeasurementVector(0)[5];}
		if (start2==205){start2=sample->GetMeasurementVector(0)[6];}
		if (start2==225){start2=sample->GetMeasurementVector(0)[7];}
		if (start2==240){start2=sample->GetMeasurementVector(0)[8];}

		int scar_thresh2=((end2 -start2)*kmean_thres) + start2;
		if(end2==start2){scar_thresh2=end2;}

		THRES->Image_based_Thresholding((root+"initial_scars.jpg").c_str(), (root+"scars_result2.jpg").c_str(), scar_thresh2, 255);
		std::cout << " Second estimation: Scars threshold "<< (end2 -start2)*kmean_thres + start2<<'\n' ;
	
		THRES->Image_based_Thresholding((root+"initial_scars.jpg").c_str(), (root+"bloodpool.jpg").c_str(), 10, scar_thresh2);
		THRES->Image_based_Thresholding((root+"kmeans_for_threshold_results.jpg").c_str(), (root+"healthymyo.jpg").c_str(), 5, (scar_thresh-(scar_thresh*0.1)));

		BI->Image_binary((root+"scars_result2.jpg").c_str(),(root+"binary_initial_scars.jpg").c_str(), 30,255);
		BI->Image_binary((root+"bloodpool.jpg").c_str(),(root+"bloodpool_binary.jpg").c_str(), 10,255);
		BI->Image_binary((root+"healthymyo.jpg").c_str(),(root+"healthymyo_binary.jpg").c_str(), 10,255);
		MI->Crop_cicle((root+"bloodpool_binary.jpg").c_str(), centerx,centery,radius_upper_midlle,(root+"bloodpool_binary.jpg").c_str());
		MI_invert->Crop_cicle((root+"binary_initial_scars.jpg").c_str(), centerx,centery,radius2,(root+"binary_initial_scars.jpg").c_str());
		for ( unsigned int i = 0 ; i < 10 ; ++i ){
			BI->Image_binary((root+"binary_initial_scars.jpg").c_str(),(root+"binary_initial_scars.jpg").c_str(), 10,255);
			SM1->Median_smoothing((root+"binary_initial_scars.jpg").c_str(),(root+"binary_initial_scars.jpg").c_str(),1,1);
			BI->Image_binary((root+"bloodpool_binary.jpg").c_str(),(root+"bloodpool_binary.jpg").c_str(), 10,255);
			SM1->Median_smoothing((root+"bloodpool_binary.jpg").c_str(),(root+"bloodpool_binary.jpg").c_str(),1,1);
			BI->Image_binary((root+"healthymyo_binary.jpg").c_str(),(root+"healthymyo_binary.jpg").c_str(), 10,255);
			SM1->Median_smoothing((root+"healthymyo_binary.jpg").c_str(),(root+"healthymyo_binary.jpg").c_str(),1,1);
		}
			scar_on=true;
			 scar_threshold_k_means=scar_thresh2;
	}
	if (BP_threshold>= end)
	{
		//int PB_thresh=(end);
 		int PB_thresh=(end-start)*0.45 +start;
 		if(end==start){PB_thresh=(0+end)*0.45;}

		std::cout << " No Scars detected: Blood pool level "<<PB_thresh<<'\n' ;
		THRES->Image_based_Thresholding((root+"kmeans_for_threshold_results.jpg").c_str(), (root+"bloodpool.jpg").c_str(), PB_thresh, 255);
		THRES->Image_based_Thresholding((root+"kmeans_for_threshold_results.jpg").c_str(), (root+"healthymyo.jpg").c_str(), 10, (PB_thresh-(PB_thresh*0.1)));
		MI_invert->Crop_cicle((root+"bloodpool.jpg").c_str(), centerx,centery,radius2,(root+"bloodpool.jpg").c_str());
		BI->Image_binary((root+"bloodpool.jpg").c_str(),(root+"bloodpool_binary.jpg").c_str(), 10,255);
		MI->Crop_cicle((root+"bloodpool_binary.jpg").c_str(), centerx,centery,radius_upper_midlle,(root+"bloodpool_binary.jpg").c_str());
		BI->Image_binary((root+"healthymyo.jpg").c_str(),(root+"healthymyo_binary.jpg").c_str(), 10,255);
		for ( unsigned int i = 0 ; i < 10 ; ++i ){
			BI->Image_binary((root+"bloodpool_binary.jpg").c_str(),(root+"bloodpool_binary.jpg").c_str(), 10,255);
			SM1->Median_smoothing((root+"bloodpool_binary.jpg").c_str(),(root+"bloodpool_binary.jpg").c_str(),1,1);
			BI->Image_binary((root+"healthymyo_binary.jpg").c_str(),(root+"healthymyo_binary.jpg").c_str(), 10,255);
			SM1->Median_smoothing((root+"healthymyo_binary.jpg").c_str(),(root+"healthymyo_binary.jpg").c_str(),1,1);
		}
			scar_on=false;
			 scar_threshold_k_means=end;
	}

	//levelset

	BS->level_set(image, (root+"endo_segm.mha").c_str(),(root+"epi_segm.jpg").c_str(), 10, 0.0, 0.0, 0.0, 0.0);
	BI->Image_binary((root+"endo_segm.mha").c_str(),(root+"level_segm_endo.jpg").c_str(), 1,255);
	reader->SetFileName( (root+"level_segm_endo.jpg").c_str());
	reader->Update();
	//check center value
	ImageType::IndexType pixelIndex;
	pixelIndex[0] = centerx;      // x position of the pixel
	pixelIndex[1] = centery;      // y position of the pixel
	ImageType::Pointer m_image = ImageType::New(); 
	m_image=reader->GetOutput();
	ImageType::PixelType pixelValue = m_image->GetPixel( pixelIndex );

	if (pixelValue<=10){
		  invertIntensityFilter2->SetInput(reader->GetOutput());
		  invertIntensityFilter2->SetMaximum(255);
		  writer2->SetFileName((root+"level_segm_endo.jpg").c_str() );
		  writer2->SetInput(invertIntensityFilter2->GetOutput() );
		  writer2->Update();
		  writer2->SetFileName((root+"level_segm_epi.jpg").c_str() );
		  writer2->SetInput(m_image);
		  writer2->Update();
	}

	if (pixelValue>=200){
		  invertIntensityFilter2->SetInput(reader->GetOutput());
		  invertIntensityFilter2->SetMaximum(255);
		  writer2->SetFileName((root+"level_segm_epi.jpg").c_str() );
		  writer2->SetInput(invertIntensityFilter2->GetOutput() );
		  writer2->Update();
	}


	//Crop in the two cycles
	MI->Crop_cicle((root+"level_segm_epi.jpg").c_str(), centerx,centery,radius,(root+"crop_ROI_epi.jpg").c_str());
	MI->Crop_cicle((root+"level_segm_endo.jpg").c_str(), centerx,centery,radius2,(root+"crop_ROI_endo.jpg").c_str());
std::shared_ptr<ConnectCombonentFilter> CCF =std::make_shared<ConnectCombonentFilter>();
std::shared_ptr<BinaryImage> BI2 =std::make_shared<BinaryImage>();


	if (scar_on==true)
	{
		MI->Crop_cicle((root+"binary_initial_scars.jpg").c_str(), centerx,centery,radius_upper_midlle,(root+"binary_initial_scars.jpg").c_str());//crop with between radius and radius2 cycle for the BP or scars of RV

		std::shared_ptr<mergeimages> MI_add_images =std::make_shared<mergeimages>(root);
CCF->Image_CCF((root+"crop_ROI_epi.jpg").c_str(),(root+"crop_ROI_epi_init.jpg").c_str(),(root+"healthymyo_binary.jpg").c_str());

BI2->Image_binary((root+"crop_ROI_epi_init.jpg").c_str(),(root+"crop_ROI_epi.jpg").c_str(), 10,255);
		MI_add_images->Image_add((root+"crop_ROI_epi.jpg").c_str(), (root+"binary_initial_scars.jpg").c_str(), (root+"crop_ROI_epi.jpg").c_str());
		MI_add_images->Image_merge( (root+"crop_ROI_endo.jpg").c_str(),(root+"binary_initial_scars.jpg").c_str() ,(root+"crop_ROI_endo.jpg").c_str());
		MI_add_images->Image_add( (root+"crop_ROI_endo.jpg").c_str(),(root+"bloodpool_binary.jpg").c_str() ,(root+"crop_ROI_endo.jpg").c_str());
		MI_add_images->Image_merge((root+"crop_ROI_epi.jpg").c_str(), (root+"bloodpool_binary.jpg").c_str(), (root+"crop_ROI_epi.jpg").c_str());
	}

	if (scar_on==false)
	{	std::shared_ptr<mergeimages> MI_add_images2 =std::make_shared<mergeimages>(root);
CCF->Image_CCF((root+"crop_ROI_epi.jpg").c_str(),(root+"crop_ROI_epi_init.jpg").c_str(),(root+"healthymyo_binary.jpg").c_str());
		MI_add_images2->Image_add( (root+"crop_ROI_endo.jpg").c_str(),(root+"bloodpool_binary.jpg").c_str() ,(root+"crop_ROI_endo.jpg").c_str());

BI2->Image_binary((root+"crop_ROI_epi_init.jpg").c_str(),(root+"crop_ROI_epi.jpg").c_str(), 10,255);
		MI_add_images2->Image_merge((root+"crop_ROI_epi.jpg").c_str(), (root+"bloodpool_binary.jpg").c_str(), (root+"crop_ROI_epi.jpg").c_str());
	}

	if (end<=THRESHOLD_Intensity_rebuild_loop){rebuild_loop=true;}
	std::shared_ptr<mergeimages> MI_papilaris =std::make_shared<mergeimages>(root);
	MI_papilaris->Crop_cicle((root+"crop_ROI_epi_init.jpg").c_str(), centerx,centery,radius2,(root+"papilaris.jpg").c_str());
	for ( unsigned int i = 0 ; i < 40 ; ++i ){
			BI->Image_binary((root+"papilaris.jpg").c_str(),(root+"papilaris.jpg").c_str(), 10,255);
			SM1->Median_smoothing((root+"papilaris.jpg").c_str(),(root+"papilaris.jpg").c_str(),1,1);
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


void seedpoints::compute_distance_of_two_contour(){

	std::cout << "  Compute distances of two contours..." << '\n' ;

	int point;
	int activeDimension=0;
	int activeDimension2=1;
	SampleType::Pointer sample_endo_initial = SampleType::New();
	sample_endo_initial->SetMeasurementVectorSize( 2 );
	SampleType::Pointer sample_endo_initial2 = SampleType::New();
	sample_endo_initial2->SetMeasurementVectorSize( 2 );
	SampleType::Pointer sample_epi_initial = SampleType::New();
	sample_epi_initial->SetMeasurementVectorSize( 2 );
	samplemaximum->SetMeasurementVectorSize( 2 );
	sample_offset->SetMeasurementVectorSize( 2 );
	//initialize vectors and samples
	MeasurementVectorType queryPointendo;
	MeasurementVectorType queryPointepi;
	MeasurementVectorType queryPointendohalf;
	MeasurementVectorType queryPointepihalf;
	MeasurementVectorType queryPointepi_next;
	MeasurementVectorType distance;
	MeasurementVectorType distance_test;
	SubsampleType::Pointer sample_endo_initialx = SubsampleType::New();
	SubsampleType::Pointer sample_endo_initialy = SubsampleType::New();

	// heapSort area

	sample_endo_initialx->SetSample(sample_endo);
	sample_endo_initialx->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_endo_initialx,activeDimension, 0, sample_endo_initialx->Size() );
	sample_endo_initialy->SetSample(sample_endo);
	sample_endo_initialy->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_endo_initialy,activeDimension2, 0, sample_endo_initialy->Size() );

	SubsampleType::Pointer sample_epi_initial2 = SubsampleType::New();
	sample_epi_initial2->SetSample(sample_epi);
	sample_epi_initial2->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_epi_initial2,activeDimension, 0, sample_epi_initial2->Size() );


	//create the half of epi and endo saubsamples and sorted in y dimension
	SampleType::Pointer s_ep_hu = SampleType::New();
	SampleType::Pointer s_en_hu = SampleType::New();
	SampleType::Pointer s_ep_hd = SampleType::New();
	SampleType::Pointer s_en_hd = SampleType::New();

	SubsampleType::Pointer sample_epi_half_upper = SubsampleType::New();
	SubsampleType::Pointer sample_endo_half_upper = SubsampleType::New();
	SubsampleType::Pointer sample_epi_half_down = SubsampleType::New();
	SubsampleType::Pointer sample_endo_half_down = SubsampleType::New();;
	int range=std::max(sample_endo->Size(),sample_epi->Size());
	for (unsigned int i=0;i<range;++i){
		if(i<(int)sample_endo->Size()*0.5){
			queryPointendo=sample_endo_initialx->GetMeasurementVector(i);
			s_en_hd->PushBack(queryPointendo);
		}
		if((i>=(int)sample_endo->Size()*0.5) && (i<sample_endo->Size())){
			queryPointendohalf=sample_endo_initialx->GetMeasurementVector(i);
			s_en_hu->PushBack(queryPointendohalf);
		}

		if(i<(int)sample_epi->Size()*0.5){
			queryPointepi=sample_epi_initial2->GetMeasurementVector(i);
			s_ep_hd->PushBack(queryPointepi);
		}

		if((i>=(int)sample_epi->Size()*0.5) && (i<sample_epi->Size())){
			queryPointepihalf=sample_epi_initial2->GetMeasurementVector(i);
			s_ep_hu->PushBack(queryPointepihalf);
		}

	}//end for

	sample_epi_half_upper->SetSample(s_ep_hu);
	sample_epi_half_upper->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_epi_half_upper,activeDimension2, 0, sample_epi_half_upper->Size() );

	sample_endo_half_upper->SetSample(s_en_hu);
	sample_endo_half_upper->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_endo_half_upper,activeDimension2, 0, sample_endo_half_upper->Size() );

	sample_epi_half_down->SetSample(s_ep_hd);
	sample_epi_half_down->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_epi_half_down,activeDimension2, 0, sample_epi_half_down->Size() );

	sample_endo_half_down->SetSample(s_en_hd);
	sample_endo_half_down->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_endo_half_down,activeDimension2, 0, sample_endo_half_down->Size() );

	//define maximum y and x of endo
	int minimumx_endo=sample_endo_initialx->GetMeasurementVector(0)[0];
	int maximumx_endo=sample_endo_initialx->GetMeasurementVector(sample_endo_initialx->Size()-1)[0];
	int minimumxy_endo=sample_endo_initialx->GetMeasurementVector(0)[1];
	int maximumxy_endo=sample_endo_initialx->GetMeasurementVector(sample_endo_initialx->Size()-1)[1];

	int minimumy_endo=sample_endo_initialy->GetMeasurementVector(0)[1];
	int maximumy_endo=sample_endo_initialy->GetMeasurementVector(sample_endo_initialy->Size()-1)[1];
	int minimumyx_endo=sample_endo_initialy->GetMeasurementVector(0)[0];
	int maximumyx_endo=sample_endo_initialy->GetMeasurementVector(sample_endo_initialy->Size()-1)[0];

	///check 1,2
	int run_end= std::min(sample_endo_half_upper->Size(),sample_epi_half_upper->Size());
	int run_max= std::max(sample_endo_half_upper->Size(),sample_epi_half_upper->Size());
	int step= run_max-run_end;
	int o=0;
	double gap=step*0.25;
	int step_size=1;
	double total_gap=run_end*0.25;
	double gaprate=gap/total_gap;
	double reduce_total=1-gaprate;
	int round=0;
	for (unsigned int i=0;i<run_end;++i){
		if (sample_endo_half_upper->Size()>sample_epi_half_upper->Size()){
			queryPointendo[0]=sample_endo_half_upper->GetMeasurementVector(o)[0];
			queryPointendo[1]=sample_endo_half_upper->GetMeasurementVector(o)[1];
			queryPointepi[1]=sample_epi_half_upper->GetMeasurementVector(i)[1];
			queryPointepi[0]=sample_epi_half_upper->GetMeasurementVector(i)[0];
		}
		if (sample_endo_half_upper->Size()<sample_epi_half_upper->Size()){
			queryPointendo[0]=sample_endo_half_upper->GetMeasurementVector(i)[0];
			queryPointendo[1]=sample_endo_half_upper->GetMeasurementVector(i)[1];
			queryPointepi[1]=sample_epi_half_upper->GetMeasurementVector(o)[1];
			queryPointepi[0]=sample_epi_half_upper->GetMeasurementVector(o)[0];
		}


		if (sample_endo_half_upper->Size() == sample_epi_half_upper->Size()){
			queryPointendo[0]=sample_endo_half_upper->GetMeasurementVector(i)[0];
			queryPointendo[1]=sample_endo_half_upper->GetMeasurementVector(i)[1];
			queryPointepi[1]=sample_epi_half_upper->GetMeasurementVector(i)[1];
			queryPointepi[0]=sample_epi_half_upper->GetMeasurementVector(i)[0];
		}
		int start=round+(total_gap)*reduce_total*0.5;
		int middle=start+(total_gap)*gaprate;
		int finish=middle+(total_gap)*reduce_total*0.5;
		if (i<start){step_size=1;};
		if (i>=start && i<middle){step_size=2;};
		if (i>=middle && i<finish+2){step_size=1;};
		o=o+step_size;
		if(i==finish+2){round=round+total_gap;}

		distance=distanceMetric->Evaluate( queryPointendo, queryPointepi );
		sample_endo_initial->PushBack(queryPointendo);
		samplemaximum->PushBack(queryPointepi);
		sample_min_epi_end->PushBack(distance);


	} //end for


	sample_endo_initial2=sample_endo_initial;
	///check 3,4
	run_end= std::min(sample_endo_half_upper->Size(),sample_epi_half_down->Size());
	run_max= std::max(sample_endo_half_upper->Size(),sample_epi_half_down->Size());
	step= run_max-run_end;
	o=0;
	gap=step*0.25;
	step_size=1;
	total_gap=run_end*0.25;
	gaprate=gap/total_gap;
	reduce_total=1-gaprate;
	round=0;
	for (unsigned int i=0;i<run_end;++i){
		if (sample_endo_half_down->Size()>sample_epi_half_down->Size()){
			queryPointendo[0]=sample_endo_half_down->GetMeasurementVector(o)[0];
			queryPointendo[1]=sample_endo_half_down->GetMeasurementVector(o)[1];
			queryPointepi[1]=sample_epi_half_down->GetMeasurementVector(i)[1];
			queryPointepi[0]=sample_epi_half_down->GetMeasurementVector(i)[0];
		}
		if (sample_endo_half_down->Size()<sample_epi_half_down->Size()){
			queryPointendo[0]=sample_endo_half_down->GetMeasurementVector(i)[0];
			queryPointendo[1]=sample_endo_half_down->GetMeasurementVector(i)[1];
			queryPointepi[1]=sample_epi_half_down->GetMeasurementVector(o)[1];
			queryPointepi[0]=sample_epi_half_down->GetMeasurementVector(o)[0];
		}

		if (sample_endo_half_down->Size() == sample_epi_half_down->Size()){
			queryPointendo[0]=sample_endo_half_down->GetMeasurementVector(i)[0];
			queryPointendo[1]=sample_endo_half_down->GetMeasurementVector(i)[1];
			queryPointepi[1]=sample_epi_half_down->GetMeasurementVector(i)[1];
			queryPointepi[0]=sample_epi_half_down->GetMeasurementVector(i)[0];
		}

		int start=round+(total_gap)*reduce_total*0.5;
		int middle=start+(total_gap)*gaprate;
		int finish=middle+(total_gap)*reduce_total*0.5;
		if (i<start){step_size=1;};
		if (i>=start && i<middle){step_size=2;};
		if (i>=middle && i<finish+2){step_size=1;};
		o=o+step_size;
		if(i==finish+2){round=round+total_gap;}


		distance=distanceMetric->Evaluate( queryPointendo, queryPointepi );
		sample_endo_initial2->PushBack(queryPointendo);
		samplemaximum->PushBack(queryPointepi);
		sample_min_epi_end->PushBack(distance);


	} //end for


	sample_endo=sample_endo_initial2;
	std::cout << "///////////////////////////////////////////////" << '\n' ;
	std::cout << "Final sample endo size : " << sample_endo->Size() << '\n' ;
	std::cout << "Final sample epi size : " << samplemaximum->Size() << '\n' ; 
	std::cout << "Final distance sample size : " << sample_min_epi_end->Size() << '\n' ; 
	std::cout << "///////////////////////////////////////////////" << '\n' ;
}







void seedpoints::find_max_intensity_funct_portion(){

	typedef itk::Vector< float, 2 > MeasurementVectorType;
	MeasurementVectorType mvtest;
	ReaderType::Pointer reader = ReaderType::New();
	using FilterType = itk::CastImageFilter< ImageType, OutputImageType >;
	ImageType::Pointer  inputImage = ImageType::New();
	OutputImageType::Pointer outputImage = OutputImageType::New();

	reader->SetFileName( m_inputFileName2.c_str());
	reader->Update();
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput( reader->GetOutput() );
	filter->Update();
	outputImage = filter->GetOutput();
	/////////////////copy allocate output image///////////////
	inputImage->SetRegions( outputImage->GetRequestedRegion() );
	inputImage->CopyInformation( outputImage );
	inputImage->Allocate();
	ConstIteratorType inputIt( inputImage, inputImage->GetRequestedRegion()); 
	IteratorType outputIt( outputImage, outputImage->GetRequestedRegion() );

	for ( outputIt.GoToBegin(), inputIt.GoToBegin(); !outputIt.IsAtEnd();++inputIt, ++outputIt)
	{

		if (outputIt.Get()>=TR[0]){
			ImageType::IndexType idx = inputIt.GetIndex();
			mvtest[0]=idx[0];
			mvtest[1]=idx[1];
			sample_high_intens->PushBack( mvtest );
		}//end if

	}//end for

}


void seedpoints::compute_area_seedpoints(){

	std::cout << " Compute seed points..." << '\n' ;
	//TODO more effiecient way takes too long!!!!!
	SampleType::Pointer sampleseed = SampleType::New();
	MeasurementVectorType queryPointendo;
	MeasurementVectorType queryPointepi;
	MeasurementVectorType queryPointu;
	MeasurementVectorType distance1;
	MeasurementVectorType distance2;

	unsigned int numbersample = sample_high_intens->Size();
	if (THRESHOLD==1){sampleseed=sample_high_intens;}
	if (THRESHOLD!=1){
	//compute seed points
	for (unsigned int i=0;i<sample_endo->Size();++i){
		queryPointendo[0]=sample_endo->GetMeasurementVector(i)[0];
		queryPointendo[1]=sample_endo->GetMeasurementVector(i)[1];
		queryPointepi[0]=samplemaximum->GetMeasurementVector(i)[0]; //samplemaximum
		queryPointepi[1]=samplemaximum->GetMeasurementVector(i)[1]; //samplemaximum
		distance2=sample_min_epi_end->GetMeasurementVector(i); 

		for (unsigned int o=0;o<numbersample;++o){
		queryPointu[0]=sample_high_intens->GetMeasurementVector(o)[0];
		queryPointu[1]=sample_high_intens->GetMeasurementVector(o)[1];
		   
			   if((queryPointendo[0]<=queryPointu[0])&&(queryPointu[0]<=queryPointepi[0])&&(queryPointendo[1]<=queryPointu[1])&&(queryPointu[1]<=queryPointepi[1])){
			     	distance1=distanceMetric->Evaluate( queryPointu, queryPointendo );
			     	if(std::abs(distance1[0]/distance2[0]) <=THRESHOLD){sampleseed->PushBack(queryPointu);}

			     }// edn if
			  if((queryPointendo[0]>=queryPointu[0])&&(queryPointu[0]>=queryPointepi[0])&&(queryPointendo[1]>=queryPointu[1])&&(queryPointu[1]>=queryPointepi[1])){
			    	distance1=distanceMetric->Evaluate( queryPointu, queryPointendo );
			    	if(std::abs(distance1[0]/distance2[0]) <=THRESHOLD){sampleseed->PushBack(queryPointu);}
		
			    }//end if
			 if((queryPointendo[0]<=queryPointu[0])&&(queryPointu[0]<=queryPointepi[0])&&(queryPointendo[1]>=queryPointu[1])&&(queryPointu[1]>=queryPointepi[1])){
			  	distance1=distanceMetric->Evaluate( queryPointu, queryPointendo );
			  	if(std::abs(distance1[0]/distance2[0]) <=THRESHOLD){sampleseed->PushBack(queryPointu);}
		
			   }//end if
			 if((queryPointendo[0]>=queryPointu[0])&&(queryPointu[0]>=queryPointepi[0])&&(queryPointendo[1]<=queryPointu[1])&&(queryPointu[1]<=queryPointepi[1]))
			  {
				distance1=distanceMetric->Evaluate( queryPointu, queryPointendo );
			   	if(std::abs(distance1[0]/distance2[0]) <=THRESHOLD){sampleseed->PushBack(queryPointu);}

			  }//end if
		 }//end for
	}//end for
	}//end if
	write_seed_image(sampleseed);

}


void seedpoints::write_seed_image(SampleType::Pointer sampleseed){

	//Write the seed point image
	MeasurementVectorType test;
	ReaderType::Pointer reader = ReaderType::New();
	using FilterType = itk::CastImageFilter< ImageType, OutputImageType >;
	ImageType::Pointer  inputImage = ImageType::New();
	OutputImageType::Pointer outputImage = OutputImageType::New();
	reader->SetFileName( m_inputFileName.c_str());
	reader->Update();
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput( reader->GetOutput() );
	filter->Update();
	outputImage = filter->GetOutput();
	/////////////////copy allocate output image///////////////
	inputImage->SetRegions( outputImage->GetRequestedRegion() );
	inputImage->CopyInformation( outputImage );
	inputImage->Allocate();

	ConstIteratorType imageit( inputImage, inputImage->GetRequestedRegion()); 
	IteratorType imagefit( outputImage, outputImage->GetRequestedRegion()); 

	for ( imageit.GoToBegin(), imagefit.GoToBegin(); !imageit.IsAtEnd(); ++imageit, ++imagefit)
	{
		int terminate=0;
		ImageType::IndexType idx2 = imageit.GetIndex();
		test[0]=idx2[0];
		test[1]=idx2[1];
		imagefit.Set(0);

		for ( unsigned int count = 0 ; count < sampleseed->Size() ; ++count )
		{
			if (test==sampleseed->GetMeasurementVector(count)){
				imagefit.Set(255 );
			}//end if
		}//end for

	}//end for

	//from float to char
	using FilterType_c2f = itk::CastImageFilter<OutputImageType,ImageType>;
	FilterType_c2f::Pointer filter_c2f = FilterType_c2f::New();
	filter_c2f->SetInput(   outputImage );
	filter_c2f->Update();
	//Write new image
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( m_outputFileName.c_str() );
	writer->SetInput( filter_c2f->GetOutput() );
	writer->Update();

}





