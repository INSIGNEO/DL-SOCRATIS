////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1
//Issues: modification of TH of max/min rate
//////////////////////////////////////////////////////////////////////////////////
#include "rebuild_myo_mask.h"

////////////////////////////////////////////////////////////////////////////////////////////


void rebuild_myo_mask::Run(const char * inputFileName)
{
int image_size=512;

cut_symetric_images( inputFileName,(root+"upperhalf_symetric_myo.jpg").c_str(),(root+"lowerhalf_symetric_myo.jpg").c_str(),(root+"middleupperhalf_symetric_myo.jpg").c_str(),(root+"middlelowerhalf_symetric_myo.jpg").c_str());
int vol_upper=compute_volume((root+"upperhalf_symetric_myo.jpg").c_str());
int vol_lower=compute_volume((root+"lowerhalf_symetric_myo.jpg").c_str());
int vol_midl_lower=compute_volume((root+"middlelowerhalf_symetric_myo.jpg").c_str());
int vol_midl_upper=compute_volume((root+"middleupperhalf_symetric_myo.jpg").c_str());

int case_symetre;
if (vol_upper>vol_lower && vol_midl_upper>vol_midl_lower){case_symetre=1;}
if (vol_upper<vol_lower && vol_midl_upper>vol_midl_lower){case_symetre=2;}
if (vol_upper>=vol_lower && vol_midl_upper<=vol_midl_lower){case_symetre=3;}
if (vol_upper<=vol_lower && vol_midl_upper<=vol_midl_lower){case_symetre=4;}

//endo epi distances compute

cut_symetric_images( (root+"epi_segm.jpg").c_str(),(root+"epi_1_4.jpg").c_str(),(root+"epi_2_4.jpg").c_str(),(root+"epi_3_4.jpg").c_str(),(root+"epi_4_4.jpg").c_str());
cut_symetric_images( (root+"endo_segm.jpg").c_str(),(root+"endo_1_4.jpg").c_str(),(root+"endo_2_4.jpg").c_str(),(root+"endo_3_4.jpg").c_str(),(root+"endo_4_4.jpg").c_str());
int dist_1_2;
int dist_2_2;
int radius_epi_1_4;
int radius_epi_2_4;
int radius_epi_3_4;
int radius_epi_4_4;

std::shared_ptr< Segmentation_Pipeline > sgp =std::make_shared< Segmentation_Pipeline >(root);


if (case_symetre==1){

	if( (sgp->check_black_image((root+"epi_3_4.jpg").c_str(),5,20)==false) && (sgp->check_black_image((root+"endo_3_4.jpg").c_str(),5,20)==false)){
		dist_1_2=compute_median_thickness((root+"epi_3_4.jpg").c_str(), (root+"endo_3_4.jpg").c_str()); 
	}
	else{dist_1_2=0;}

	if ((sgp->check_black_image((root+"epi_1_4.jpg").c_str(),5,20)==false) && (sgp->check_black_image((root+"endo_1_4.jpg").c_str(),5,20)==false)) {
		dist_2_2=compute_median_thickness((root+"epi_1_4.jpg").c_str(), (root+"endo_1_4.jpg").c_str());
	}
	else{dist_2_2=0;}
}

if (case_symetre==2){

	if( (sgp->check_black_image((root+"epi_2_4.jpg").c_str(),50,20)==false) && (sgp->check_black_image((root+"endo_2_4.jpg").c_str(),5,20)==false)){
		dist_1_2=compute_median_thickness((root+"epi_2_4.jpg").c_str(), (root+"endo_2_4.jpg").c_str()); 
	}
	else{dist_1_2=0;}

	if ((sgp->check_black_image((root+"epi_3_4.jpg").c_str(),50,20)==false)&& (sgp->check_black_image((root+"endo_3_4.jpg").c_str(),5,20)==false)) {
		dist_2_2=compute_median_thickness((root+"epi_3_4.jpg").c_str(), (root+"endo_3_4.jpg").c_str());
	}
	else{dist_2_2=0;}
}

if (case_symetre==3){

	if( (sgp->check_black_image((root+"epi_1_4.jpg").c_str(),5,20)==false) && (sgp->check_black_image((root+"endo_1_4.jpg").c_str(),5,20)==false)){
		dist_1_2=compute_median_thickness((root+"epi_1_4.jpg").c_str(), (root+"endo_1_4.jpg").c_str()); 
	}
	else{dist_1_2=0;}

	if ((sgp->check_black_image((root+"epi_4_4.jpg").c_str(),5,20)==false) && (sgp->check_black_image((root+"endo_4_4.jpg").c_str(),5,20)==false)) {
		dist_2_2=compute_median_thickness((root+"epi_4_4.jpg").c_str(), (root+"endo_4_4.jpg").c_str());
	}
	else{dist_2_2=0;}
}

if (case_symetre==4){

	if( (sgp->check_black_image((root+"epi_2_4.jpg").c_str(),5,20)==false) && (sgp->check_black_image((root+"endo_2_4.jpg").c_str(),5,20)==false)){
		dist_1_2=compute_median_thickness((root+"epi_2_4.jpg").c_str(), (root+"endo_2_4.jpg").c_str()); 
	}
	else{dist_1_2=0;}

	if ((sgp->check_black_image((root+"epi_4_4.jpg").c_str(),5,20)==false)&& (sgp->check_black_image((root+"endo_4_4.jpg").c_str(),5,20)==false)) {
		dist_2_2=compute_median_thickness((root+"epi_4_4.jpg").c_str(), (root+"endo_4_4.jpg").c_str());
	}
	else{dist_2_2=0;}
}

//compute median radius of epi contour
std::shared_ptr<seedpoints> SP_distance_comp =std::make_shared<seedpoints>(root);
SampleType::Pointer contour1 = SampleType::New();
SampleType::Pointer contour2 = SampleType::New();
SampleType::Pointer contour3 = SampleType::New();
SampleType::Pointer contour4 = SampleType::New();

MeasurementVectorType center1;
center1[0]=image_size/2-1;
center1[1]=image_size/2-1;

if( (sgp ->check_black_image((root+"epi_1_4.jpg").c_str(),5,20)==false) && (sgp ->check_black_image((root+"endo_1_4.jpg").c_str(),5,20)==false)){
		SP_distance_comp->create_vector_of_images((root+"endo_1_4.jpg").c_str(),(root+"epi_1_4.jpg").c_str());
		contour1= SP_distance_comp->GetSample_epi();
}
	else{contour1->PushBack(center1);}





MeasurementVectorType center2;
center2[1]=0;
center2[0]=image_size/2-1;
std::shared_ptr<seedpoints> SP_distance_comp1 =std::make_shared<seedpoints>(root);
if( (sgp ->check_black_image((root+"epi_2_4.jpg").c_str(),5,20)==false) && (sgp ->check_black_image((root+"endo_2_4.jpg").c_str(),5,20)==false)){
		SP_distance_comp1->create_vector_of_images((root+"endo_2_4.jpg").c_str(),(root+"epi_2_4.jpg").c_str());
		contour2= SP_distance_comp->GetSample_epi();
}
	else{contour2->PushBack(center2);}




MeasurementVectorType center3;
center3[0]=0;
center3[1]=image_size/2-1;
std::shared_ptr<seedpoints> SP_distance_comp2 =std::make_shared<seedpoints>(root);
if( (sgp ->check_black_image((root+"epi_3_4.jpg").c_str(),5,20)==false) && (sgp ->check_black_image((root+"endo_3_4.jpg").c_str(),5,20)==false)){
		SP_distance_comp2->create_vector_of_images((root+"endo_3_4.jpg").c_str(),(root+"epi_3_4.jpg").c_str());
		contour3= SP_distance_comp2->GetSample_epi();
}
	else{contour3->PushBack(center3);}





MeasurementVectorType center4;
center4[1]=0;
center4[0]=0;
std::shared_ptr<seedpoints> SP_distance_comp3 =std::make_shared<seedpoints>(root);
if( (sgp ->check_black_image((root+"epi_4_4.jpg").c_str(),5,20)==false) && (sgp ->check_black_image((root+"endo_4_4.jpg").c_str(),5,20)==false)){
		SP_distance_comp3->create_vector_of_images((root+"endo_4_4.jpg").c_str(),(root+"epi_4_4.jpg").c_str());
		contour4= SP_distance_comp3->GetSample_epi();
}
	else{contour4->PushBack(center4);}




//int radius_epi_1_4=compute_median_distance_radius( contour1, center1);
//int radius_epi_2_4=compute_median_distance_radius( contour2, center2);
//int radius_epi_3_4=compute_median_distance_radius( contour3, center3);
//int radius_epi_4_4=compute_median_distance_radius( contour4, center4);
if (case_symetre==1){
radius_epi_1_4=compute_median_distance_radius( contour1, center1);
radius_epi_2_4=radius_epi_1_4;
radius_epi_3_4=compute_median_distance_radius( contour3, center3);
radius_epi_4_4=radius_epi_3_4;
}

if (case_symetre==2){
radius_epi_2_4=compute_median_distance_radius( contour2, center2);
radius_epi_1_4=radius_epi_2_4;
radius_epi_3_4=compute_median_distance_radius( contour3, center3);
radius_epi_4_4=radius_epi_3_4;
}

if (case_symetre==3){
radius_epi_1_4=compute_median_distance_radius( contour1, center1);
radius_epi_2_4=radius_epi_1_4;
radius_epi_4_4=compute_median_distance_radius( contour4, center4);
radius_epi_3_4=radius_epi_4_4;
}

if (case_symetre==4){
radius_epi_2_4=compute_median_distance_radius( contour2, center2);
radius_epi_1_4=radius_epi_2_4;
radius_epi_4_4=compute_median_distance_radius( contour4, center4);
radius_epi_3_4=radius_epi_4_4;
}

  std::cout << "radius_epi_1_4: " << radius_epi_1_4 << std::endl;
  std::cout << "radius_epi_2_4: " << radius_epi_2_4 << std::endl;
  std::cout << "radius_epi_3_4: " << radius_epi_3_4 << std::endl;
  std::cout << "radius_epi_4_4: " << radius_epi_4_4 << std::endl;

int cent[2];

cent[0]=image_size/2-1;
cent[1]=image_size/2-1;

draw_median_cycle(dist_1_2, radius_epi_1_4, (root+"mask_1_4.jpg").c_str(),cent );

int cent1[2];
cent1[1]=0;
cent1[0]=image_size/2-1;

draw_median_cycle(dist_1_2,radius_epi_2_4, (root+"mask_2_4.jpg").c_str(), cent1);

int cent2[2];
cent2[1]=image_size/2-1;
cent2[0]=0;

draw_median_cycle(dist_2_2, radius_epi_3_4, (root+"mask_3_4.jpg").c_str(),cent2 );

int cent3[2];
cent3[0]=0;
cent3[1]=0;

draw_median_cycle(dist_2_2, radius_epi_4_4, (root+"mask_4_4.jpg").c_str(),cent3);

//add the new masks

std::shared_ptr<mergeimages> MI_add_images2 =std::make_shared<mergeimages>(root);
MI_add_images2->Image_add( (root+"mask_1_4.jpg").c_str(),(root+"upperhalf_symetric_myo.jpg").c_str() ,(root+"final_myo_1_4.jpg").c_str());
MI_add_images2->Image_add( (root+"mask_2_4.jpg").c_str(),(root+"lowerhalf_symetric_myo.jpg").c_str() ,(root+"final_myo_2_4.jpg").c_str());
MI_add_images2->Image_add( (root+"mask_3_4.jpg").c_str(),(root+"middleupperhalf_symetric_myo.jpg").c_str() ,(root+"final_myo_3_4.jpg").c_str());
MI_add_images2->Image_add( (root+"mask_4_4.jpg").c_str(),(root+"middlelowerhalf_symetric_myo.jpg").c_str() ,(root+"final_myo_4_4.jpg").c_str());

add_images((root+"final_myo_1_4.jpg").c_str(),(root+"final_myo_2_4.jpg").c_str(),(root+"final_myo_3_4.jpg").c_str(),(root+"final_myo_4_4.jpg").c_str(),(root+"final_myo_mask.jpg").c_str());

std::shared_ptr<denoising> SM =std::make_shared<denoising>(root);
std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();


for ( unsigned int i = 0 ; i < 20 ; ++i ){
	BI->Image_binary((root+"final_myo_mask.jpg").c_str(),(root+"final_myo_mask.jpg").c_str(), 40,255);
	SM->Median_smoothing((root+"final_myo_mask.jpg").c_str(),(root+"final_myo_mask.jpg").c_str(),1,1);
}
// create the symetric mask
//create_new_symmetric_myo(case_symetre);

}

////////////////////////////////////////////////////////////////////////////////////////////


void rebuild_myo_mask::cut_symetric_images(const char * inputFileName, const char * outputFileName1, const char * outputFileName2, const char * outputFileName3, const char * outputFileName4){
int image_size=512;

using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 2 >;

using ReaderType = itk::ImageFileReader< ImageType >;
using WriterType = itk::ImageFileWriter< ImageType >;


  ReaderType::Pointer featureReader = ReaderType::New();
  featureReader->SetFileName( inputFileName );
  featureReader->Update();

  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  ImageType::SizeType size;
  unsigned int NumRows = image_size/2;
  unsigned int NumCols = image_size/2;
  size[0] = NumRows;
  size[1] = NumCols;

  ImageType::RegionType desiredRegion(start, size);
  std::cout << "desiredRegion: " << desiredRegion << std::endl;
  
  using FilterType = itk::ExtractImageFilter< ImageType, ImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->SetExtractionRegion(desiredRegion);
  filter->SetInput(featureReader->GetOutput());
  filter->SetDirectionCollapseToIdentity(); // This is required.
  filter->Update();
 
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName(outputFileName1 );
  writer->Update();



  ReaderType::Pointer featureReader2 = ReaderType::New();
  featureReader2->SetFileName( inputFileName );
  featureReader2->Update();

  ImageType::IndexType start2;
  start2[1] = image_size/2;
  start2[0] = 0;
  ImageType::RegionType desiredRegion2(start2, size);
  std::cout << "desiredRegion: " << desiredRegion2 << std::endl;
  
  FilterType::Pointer filter2 = FilterType::New();
  filter2->SetExtractionRegion(desiredRegion2);
  filter2->SetInput(featureReader2->GetOutput());
  filter2->SetDirectionCollapseToIdentity(); // This is required.
  filter2->Update();
 
  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetInput( filter2->GetOutput() );
  writer2->SetFileName(outputFileName2 );
  writer2->Update();


 

  ImageType::IndexType start3;
  start3[1] = 0;
  start3[0] = image_size/2;
  ImageType::RegionType desiredRegion3(start3, size);
  std::cout << "desiredRegion: " << desiredRegion3 << std::endl;
  
  FilterType::Pointer filter3 = FilterType::New();
  filter3->SetExtractionRegion(desiredRegion3);
  filter3->SetInput(featureReader->GetOutput());
  filter3->SetDirectionCollapseToIdentity(); // This is required.
  filter3->Update();
 
  WriterType::Pointer writer3 = WriterType::New();
  writer3->SetInput( filter3->GetOutput() );
  writer3->SetFileName(outputFileName3 );
  writer3->Update();



  ImageType::IndexType start4;
  start4[0] = image_size/2;
  start4[1] = image_size/2;

  ImageType::RegionType desiredRegion4(start4, size);
  std::cout << "desiredRegion: " << desiredRegion4 << std::endl;
  
  FilterType::Pointer filter4 = FilterType::New();
  filter4->SetExtractionRegion(desiredRegion4);
  filter4->SetInput(featureReader2->GetOutput());
  filter4->SetDirectionCollapseToIdentity(); // This is required.
  filter4->Update();
 
  WriterType::Pointer writer4 = WriterType::New();
  writer4->SetInput( filter4->GetOutput() );
  writer4->SetFileName(outputFileName4 );
  writer4->Update();

}

////////////////////////////////////////////////////////////////////////////////////////////


int rebuild_myo_mask::compute_volume(const char * image){

  std::cout << "compute_volume... " << std::endl;
int vol_result=0;

using PixelType = unsigned char;
constexpr unsigned int Dimension = 2;
using ImageType = itk::Image< PixelType,  Dimension >;
using WriterType = itk::ImageFileWriter< ImageType >;
using ReaderType = itk::ImageFileReader< ImageType >;
using OutputImageType = itk::Image< float, 2 >;
typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< OutputImageType > IteratorType;

typedef itk::Vector< float, 2 > MeasurementVectorType;
MeasurementVectorType mvtest;
ReaderType::Pointer reader = ReaderType::New();
using FilterType = itk::CastImageFilter< ImageType, OutputImageType >;
ImageType::Pointer  inputImage = ImageType::New();
OutputImageType::Pointer outputImage = OutputImageType::New();

reader->SetFileName( image);
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

	if (outputIt.Get()>0.0){
		vol_result++;
	}//end if

}//end for

return vol_result;
}

int rebuild_myo_mask::compute_median_distance_radius(SampleType::Pointer contour, MeasurementVectorType center){


  std::cout << "compute_median_distance_radius from  " <<center[0]<<","<<center[1]<<" center."<< std::endl;

using SubsampleType = itk::Statistics::Subsample<SampleType>;
SampleType::Pointer distance_center = SampleType::New();

int total_size=contour->Size();
MeasurementVectorType distance;
MeasurementVectorType distance_contour;
std::cout << "number of points   " <<total_size<< std::endl;
int median_distance_radius=0;

if (total_size!=0){
	for(unsigned int r = 0; r < total_size; r++)
	{
		distance_contour[0]=contour->GetMeasurementVector(r)[0];
		distance_contour[1]=contour->GetMeasurementVector(r)[1];
	//std::cout << "distance_epi   " <<distance_contour[0]<<","<<distance_contour[1]<<" center."<< std::endl;

		distance=distanceMetric->Evaluate( distance_contour, center );
	//std::cout << "final distance   " <<distance[0]<<","<<distance[1]<<" center."<< std::endl;

		distance_center->PushBack(distance);
	}

	using SubsampleType = itk::Statistics::Subsample<SampleType>;


	SubsampleType::Pointer sample_dist_sort = SubsampleType::New();
	sample_dist_sort->SetSample(distance_center);
	sample_dist_sort->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_dist_sort,0, 0, sample_dist_sort->Size() );
	int median=total_size*0.5;
	if ((total_size & 1) != 0){ median++;}
	int max=total_size-1;
	int position=median;
	if(max_boundary_case==true){position=max;}
	median_distance_radius=sample_dist_sort->GetMeasurementVector(position)[0];
	int offset_filter=10;
	while (median_distance_radius>=255){
	std::cout << "filter of radius distance from:   " <<median_distance_radius<< std::endl; 
	median_distance_radius=sample_dist_sort->GetMeasurementVector(position)[offset_filter];
	offset_filter=offset_filter+1;
	std::cout << "to:   " <<median_distance_radius<< std::endl;}
	}
return median_distance_radius;

}

double rebuild_myo_mask::compute_median(SampleType::Pointer contour){


	int total_size=contour->Size();
	double median_distance2=0;

	using SubsampleType = itk::Statistics::Subsample<SampleType>;
	SubsampleType::Pointer sample_dist_sort2 = SubsampleType::New();
	sample_dist_sort2->SetSample(contour);
	sample_dist_sort2->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_dist_sort2,0, 0, contour->Size() );
	int median=total_size*0.5-1;
	if ((total_size & 1) != 0){ median++;}
	int position=median;
	std::cout << "median index:   " <<position<< std::endl;
	median_distance2=sample_dist_sort2->GetMeasurementVector(position)[0]*100;
	std::cout << "median value:   " <<median_distance2<< std::endl;
	return median_distance2;
}




////////////////////////////////////////////////////////////////////////////////////////////


int rebuild_myo_mask::compute_median_thickness(const char * epicont, const char * endocont){


std::cout << "  Compute median thickness of two contours..." << '\n' ;
using MeasurementVectorType = itk::Vector< float, 2 >;
using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;
using SubsampleType = itk::Statistics::Subsample<SampleType>;

std::shared_ptr<seedpoints> SP_distance_comp =std::make_shared<seedpoints>(root);
SP_distance_comp->create_vector_of_images(endocont,epicont);
SampleType::Pointer distance_copy = SampleType::New();
SP_distance_comp->compute_distance_of_two_contour();
distance_copy= SP_distance_comp->Getdistance_epi_endo();

int median_distance=0;
int total_size=distance_copy->Size();
if (total_size==0){
	std::cout << "  median distance: " << '\n' ;
	std::cout <<0<< '\n' ;
	median_distance=0;
}
if (total_size!=0){
	SubsampleType::Pointer sample_dist = SubsampleType::New();
	sample_dist->SetSample(distance_copy);
	sample_dist->InitializeWithAllInstances();
	itk::Statistics::Algorithm::HeapSort< SubsampleType >( sample_dist,0, 0, sample_dist->Size() );
	int median=total_size*0.5+1;
	std::cout << "  median distance: " << '\n' ;
	std::cout <<sample_dist->GetMeasurementVector(median)[0]<< '\n' ;
	median_distance=sample_dist->GetMeasurementVector(median)[0];
}
return median_distance;

}


////////////////////////////////////////////////////////////////////////////////////////////


void rebuild_myo_mask::draw_median_cycle(int median_dist, int radius_epi, const char * outputFileName,int cycle[2] ){
std::cout << "  draw_median_cycle... " << '\n' ;
using ImageType = itk::Image< unsigned char, 2 >;
//check based the max thickness yu alaoud
if (median_dist>100){median_dist=100;}
int radius_small=radius_epi-median_dist;
if (radius_epi<=median_dist){ radius_small=radius_epi; radius_epi=radius_epi+median_dist;}


  ImageType::Pointer image = ImageType::New();

  ImageType::IndexType start;
  start[0] =   0;  // first index on X
  start[1] =   0;  // first index on Y
  //start[2] =   0;  // first index on Z

  ImageType::SizeType  size;
  size[0]  = 256;  // size along X
  size[1]  = 256;  // size along Y
  //size[2]  = 200;  // size along Z

  ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  image->SetRegions( region );
  image->Allocate();
  for( int r = 0; r < 256; r++){
  for(int  c = 0; c < 256; c++)
    {
      ImageType::IndexType pixelIndex;
      pixelIndex[0] = r;
      pixelIndex[1] = c; 

      image->SetPixel(pixelIndex, 0);

	}

	}



unsigned int c=0;
 int start_r;
 int end_r;
end_r=std::abs(cycle[0]+radius_epi);
start_r=(cycle[0]-radius_epi);
if (cycle[0]==255){end_r=255; start_r=std::abs(cycle[0]-radius_epi);}
if (cycle[0]==0){end_r=cycle[0]+radius_epi; start_r=0;}

for(unsigned int r = start_r; r < end_r; r++)
{

	//constrains y
	int end=0;
	int start=0;
	double ro=std::pow(radius_epi,2);
	double r2=std::pow(radius_small,2);
	int update_x=std::abs(cycle[0]-r);
	double xnew=std::pow(update_x,2);
	if (cycle[1]==255){
		end= (int) cycle[1];
		start=(int) cycle[1]- std::sqrt(std::abs(ro-xnew));
	}
	if (cycle[1]==0){
		end= (int) cycle[1] + std::sqrt(std::abs(ro-xnew));
		start=0;
	}


	  for( c = start; c < end; c++)
	    {

	      ImageType::IndexType pixelIndex;
	      pixelIndex[0] = r;
	      pixelIndex[1] = c; 

	      image->SetPixel(pixelIndex, 254);



	      }

}

unsigned int c2=0;
 int start_r2;
 int end_r2;
if (cycle[0]==255){end_r2=255; start_r2=std::abs(cycle[0]-radius_small);}
if (cycle[0]==0){end_r2=cycle[0]+radius_small; start_r2=0;}

for(unsigned int r2 = start_r2; r2 < end_r2; r2++)
{

	//constrains y
	int end2=0;
	int start2=0;
	double ro2=std::pow(radius_small,2);
	int update_x2=std::abs(cycle[0]-r2);
	double xnew2=std::pow(update_x2,2);
	if (cycle[1]==255){
		end2= (int) cycle[1];
		start2=(int) cycle[1]- std::sqrt(std::abs(ro2-xnew2));
	}
	if (cycle[1]==0){
		end2= (int) cycle[1] + std::sqrt(std::abs(ro2-xnew2));
		start2=0;
	}


	  for( c2 = start2; c2 < end2; c2++)
	    {

	      ImageType::IndexType pixelIndex2;
	      pixelIndex2[0] = r2;
	      pixelIndex2[1] = c2; 

	      image->SetPixel(pixelIndex2, 0);



	      }

}
/*
unsigned int c2=0;
 int start_r2;
 int end_r2;
end_r2=cycle[0]+radius_small;
start_r2=cycle[0]-radius_small;
if (end_r2>255){end_r2=255;}
if (start_r2<(-255)){start_r2=0;}
if (start_r2<0){start_r2=std::abs(start_r2);}

for(unsigned int r2 = start_r2; r2 < end_r2; r2++)
{

	//constrains y
	int end2=0;
	int start2=0;
	double ro2=std::pow(radius_small,2);
	int update_x2=std::abs(r2-cycle[0]);
	double xnew2=std::pow(update_x2,2);
	end2= (int) std::sqrt(std::abs(ro2-xnew2))+cycle[1];
	start2=(int) std::sqrt(std::abs(ro2-xnew2))-cycle[1];
	//std::cout << start<<std::endl;
	if (end2>255){end2=255;}
	if (start2<(-255)){start2=0;}
	if (start2<0){start2=std::abs(start2);}

	  for( c2 = start2; c2 < end2; c2++)
	    {

	      ImageType::IndexType pixelIndex2;
	      pixelIndex2[0] = r2;
	      pixelIndex2[1] = c2; 

	      image->SetPixel(pixelIndex2, 0);



	      }

}
*/



using WriterType = itk::ImageFileWriter< ImageType >;
WriterType::Pointer writer0 = WriterType::New();
writer0->SetFileName( outputFileName);
writer0->SetInput( image );
writer0->Update();

}

////////////////////////////////////////////////////////////////////////////////////////////

void rebuild_myo_mask::create_new_symmetric_myo(int case_sym){

const char * input;
const char * input2;
if (case_sym==1){input=(root+"upperhalf_symetric_myo.jpg").c_str(); input2=(root+"middleupperhalf_symetric_myo.jpg").c_str();}
if (case_sym==2){input=(root+"lowerhalf_symetric_myo.jpg").c_str();input2=(root+"middleupperhalf_symetric_myo.jpg").c_str();}
if (case_sym==3){input=(root+"upperhalf_symetric_myo.jpg").c_str(); input2=(root+"middlelowerhalf_symetric_myo.jpg").c_str();}
if (case_sym==4){input=(root+"lowerhalf_symetric_myo.jpg").c_str(); input2=(root+"middlelowerhalf_symetric_myo.jpg").c_str();}

  using PixelType = unsigned char;
  using ImageType = itk::Image< PixelType,  2 >;
  using ReaderType = itk::ImageFileReader< ImageType >;
  using WriterType = itk::ImageFileWriter< ImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  reader->SetFileName( input2 );
  using FilterType = itk::FlipImageFilter< ImageType >;
  FilterType::Pointer filter = FilterType::New();
  using FlipAxesArrayType = FilterType::FlipAxesArrayType;
  FlipAxesArrayType flipArray;
  flipArray[0] = false;
  flipArray[1] = true;
  filter->SetFlipAxes( flipArray );

  filter->SetInput( reader->GetOutput() );
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName((root+"mirror_symetric_myo2.jpg").c_str() );
  writer->Update();


  ReaderType::Pointer reader2 = ReaderType::New();
  WriterType::Pointer writer2 = WriterType::New();
  reader2->SetFileName( input );
  using FilterType = itk::FlipImageFilter< ImageType >;
  FilterType::Pointer filter2 = FilterType::New();
  using FlipAxesArrayType = FilterType::FlipAxesArrayType;
  FlipAxesArrayType flipArray2;
  flipArray2[0] = false;
  flipArray2[1] = true;
  filter2->SetFlipAxes( flipArray2 );

  filter2->SetInput( reader2->GetOutput() );
  writer2->SetInput( filter2->GetOutput() );
  writer2->SetFileName((root+"mirror_symetric_myo.jpg").c_str() );
  writer2->Update();


if (case_sym==1){add_images(input, (root+"mirror_symetric_myo.jpg").c_str(), input2,  (root+"mirror_symetric_myo2.jpg").c_str(),(root+"symetric_myo.jpg").c_str());}
if (case_sym==2){add_images( (root+"mirror_symetric_myo.jpg").c_str(), input, input2, (root+"mirror_symetric_myo2.jpg").c_str(),(root+"symetric_myo.jpg").c_str());}
if (case_sym==3){add_images(input, (root+"mirror_symetric_myo.jpg").c_str(), (root+"mirror_symetric_myo2.jpg").c_str(), input2,(root+"symetric_myo.jpg").c_str());}
if (case_sym==4){add_images( (root+"mirror_symetric_myo.jpg").c_str(),input, (root+"mirror_symetric_myo2.jpg").c_str(),input2,(root+"symetric_myo.jpg").c_str());}

}

////////////////////////////////////////////////////////////////////////////////////////////

void rebuild_myo_mask::add_images(const char * inputFileName1, const char * inputFileName2, const char * inputFileName3, const char * inputFileName4,const char * outputimage){

using ImageType = itk::Image< unsigned char, 2 >;

  ImageType::Pointer image = ImageType::New();

  ImageType::IndexType start;
  start[0] =   0;  // first index on X
  start[1] =   0;  // first index on Y
  //start[2] =   0;  // first index on Z

  ImageType::SizeType  size;
  size[0]  = 512;  // size along X
  size[1]  = 512;  // size along Y
  //size[2]  = 200;  // size along Z

  ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  image->SetRegions( region );
  image->Allocate();
  for( int r = 0; r < 512; r++){
  for(int  c = 0; c < 512; c++)
    {
      ImageType::IndexType pixelIndex;
      pixelIndex[0] = r;
      pixelIndex[1] = c; 

      image->SetPixel(pixelIndex, 0);

}

}
  using WriterType = itk::ImageFileWriter< ImageType >;
  WriterType::Pointer writer0 = WriterType::New();
  writer0->SetFileName( outputimage );
  writer0->SetInput( image );
  writer0->Update();
  int startX = 0;
  int startY = 0;


  ImageType::IndexType index;
  index[0] = startX;
  index[1] = startY;

  using ReaderType = itk::ImageFileReader< ImageType >;
  ReaderType::Pointer sourceReader = ReaderType::New();
  sourceReader->SetFileName( inputFileName1 );
  sourceReader->Update();

  using FilterType = itk::PasteImageFilter< ImageType, ImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->SetSourceImage( sourceReader->GetOutput() );
  filter->SetSourceRegion( sourceReader->GetOutput()->GetLargestPossibleRegion() );
  filter->SetDestinationImage( image );
  filter->SetDestinationIndex( index );

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputimage);
  writer->SetInput( filter->GetOutput() );
  writer->Update();

  int startX2 = 0;
  int startY2 = 256;

  ImageType::IndexType index2;
  index2[0] = startX2;
  index2[1] = startY2;


  ReaderType::Pointer sourceReader2 = ReaderType::New();
  sourceReader2->SetFileName( inputFileName2 );
  sourceReader2->Update();

  ReaderType::Pointer destinationReader2 = ReaderType::New();
  destinationReader2->SetFileName( outputimage );

  FilterType::Pointer filter2 = FilterType::New();
  filter2->SetSourceImage( sourceReader2->GetOutput() );
  filter2->SetSourceRegion( sourceReader2->GetOutput()->GetLargestPossibleRegion() );
  filter2->SetDestinationImage( destinationReader2->GetOutput() );
  filter2->SetDestinationIndex( index2);

  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetFileName( outputimage );
  writer2->SetInput( filter2->GetOutput() );
  writer2->Update();

  int startX3 = 256;
  int startY3 = 0;

  ImageType::IndexType index3;
  index3[0] = startX3;
  index3[1] = startY3;


  ReaderType::Pointer sourceReader3 = ReaderType::New();
  sourceReader3->SetFileName( inputFileName3 );
  sourceReader3->Update();

  ReaderType::Pointer destinationReader3 = ReaderType::New();
  destinationReader3->SetFileName( outputimage );

  FilterType::Pointer filter3 = FilterType::New();
  filter3->SetSourceImage( sourceReader3->GetOutput() );
  filter3->SetSourceRegion( sourceReader3->GetOutput()->GetLargestPossibleRegion() );
  filter3->SetDestinationImage( destinationReader3->GetOutput() );
  filter3->SetDestinationIndex( index3);

  WriterType::Pointer writer3 = WriterType::New();
  writer3->SetFileName( outputimage );
  writer3->SetInput( filter3->GetOutput() );
  writer3->Update();

  int startX4 = 256;
  int startY4 = 256;

  ImageType::IndexType index4;
  index4[0] = startX4;
  index4[1] = startY4;


  ReaderType::Pointer sourceReader4= ReaderType::New();
  sourceReader4->SetFileName( inputFileName4 );
  sourceReader4->Update();

  ReaderType::Pointer destinationReader4 = ReaderType::New();
  destinationReader4->SetFileName( outputimage );

  FilterType::Pointer filter4 = FilterType::New();
  filter4->SetSourceImage( sourceReader4->GetOutput() );
  filter4->SetSourceRegion( sourceReader4->GetOutput()->GetLargestPossibleRegion() );
  filter4->SetDestinationImage( destinationReader4->GetOutput() );
  filter4->SetDestinationIndex( index4);

  WriterType::Pointer writer4 = WriterType::New();
  writer4->SetFileName( outputimage );
  writer4->SetInput( filter4->GetOutput() );
  writer4->Update();
}




