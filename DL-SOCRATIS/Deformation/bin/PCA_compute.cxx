
#include "PCA_compute.h"


void PCA_compute::compute_3DPCA(std::string inputpatientroot, std::vector<string> patient_folders){

folders_name=patient_folders;

patient_store_path=inputpatientroot;

// normalize the number of the strain images in max register capacity
//set reference the first patient

std::shared_ptr<unbiased_ground_trouth > UBGT0 =std::make_shared<unbiased_ground_trouth >(root);
vector<string> path_patient_imagesref1= UBGT0->read_images_folder((inputpatientroot+patient_folders[0]+"/strain/"), 0, true);

create_max_length_image_number(path_patient_imagesref1,max_location );

std::shared_ptr<unbiased_ground_trouth > UBGT01 =std::make_shared<unbiased_ground_trouth >(root);
vector<string> path_patient_imagesref= UBGT01->read_images_folder((inputpatientroot+patient_folders[max_location]+"/strain/"), 0, true);

path_create(patient_store_path + "/registration/");
vector<string> registration_data_folders;
for ( unsigned int o2 = 0; o2 < max; o2++){
		path_create(patient_store_path + "/registration/"+std::to_string(o2)+"/");

		registration_data_folders.push_back(patient_store_path + "/registration/"+std::to_string(o2)+"/");

		std::string output_main= patient_store_path + "/registration/"+std::to_string(o2)+"/registration_" + std::to_string(max_location) + "_field.jpg";

		boost::filesystem::copy_file((path_patient_imagesref[o2]).c_str(), output_main.c_str(),boost::filesystem::copy_option::overwrite_if_exists);

	} //end for


for ( unsigned int k = 0; k < patient_folders.size(); k++)
{
	if (k!=max_location){
		std::shared_ptr<unbiased_ground_trouth > UBGT =std::make_shared<unbiased_ground_trouth >(root);
		vector<string> path_patient_images= UBGT->read_images_folder((inputpatientroot+patient_folders[k]+"/strain/"), 0, true);
		create_max_length_image_number(path_patient_images,k );
		std::shared_ptr<unbiased_ground_trouth > UBGT1 =std::make_shared<unbiased_ground_trouth >(root);
		vector<string> path_patient_images_full= UBGT1->read_images_folder((inputpatientroot+patient_folders[k]+"/strain/"), 0, true);



		//registrate images

		for ( unsigned int o1 = 0; o1 < max; o1++){

			std::string output_main= patient_store_path + "/registration/"+std::to_string(o1)+"/registration_" + std::to_string(k) + "_field.jpg";
			std::shared_ptr<Segmentation_Pipeline > seg =std::make_shared<Segmentation_Pipeline >(root);

			if (seg->check_black_image((path_patient_images_full[o1]).c_str(),3000,20)==false){
				std::shared_ptr<deformable_registration > deformable_regist =std::make_shared<deformable_registration >(root);

				deformable_regist->compute_demons((path_patient_imagesref[o1]).c_str(), (path_patient_images_full[o1]).c_str(), output_main.c_str(), "None" , "None"  );
			}//end if
		} //end for
	}//end if
}//end for

//PCA compute
for ( unsigned int k2 = 0; k2 < max; k2++)
{
compute_PCA((registration_data_folders[k2]).c_str(), patient_folders.size(), patient_folders.size());
}

}

void PCA_compute::create_max_length_image_number( std::vector<string> images, int patient_number ){

//define steps 
int specific_case_images=images.size();
int step=max/specific_case_images;
int gap;
if (step<1)
{
 	cerr << "Error : max number of image passed. Overflow of register... " << strerror(errno) << endl; 
}
else{
	gap=max-step*specific_case_images  ;    
}

//create black images
std::shared_ptr<deformable_registration> thresh =std::make_shared<deformable_registration>(root);
int half_gap=gap*0.5;
int residual=gap-half_gap-half_gap;
int real_images_count=0;
bool check_gap=true;
int count_step=step;
for ( unsigned int o = 0; o < max; o++)
{


	if(o<=half_gap){
		thresh->Image_segment_Thresholding((images[0]).c_str(), (patient_store_path+folders_name[patient_number]+"strain_position_"+std::to_string(o)+".jpg").c_str(), 1, 2, false);
	}
	if((o>half_gap)&&(o<=(max-half_gap-1))){

		if (count_step>=step){check_gap==true;}
		if (count_step<step){check_gap==false;}

		if (check_gap==true){
			boost::filesystem::copy_file((images[real_images_count]).c_str(), (patient_store_path+folders_name[patient_number]+"strain_position_"+std::to_string(o)+".jpg").c_str(),boost::filesystem::copy_option::overwrite_if_exists);
			std::string commandwr2r;
			commandwr2r += "rm -rf ";
			commandwr2r += images[real_images_count];
			int wrf2r= system(commandwr2r.c_str());
			real_images_count++;
			count_step=0;
		}

		if (check_gap==false){
			thresh->Image_segment_Thresholding((images[0]).c_str(), (patient_store_path+folders_name[patient_number]+"strain_position_"+std::to_string(o)+".jpg").c_str(), 1, 2, false);
			count_step++;
			}

	if((o>half_gap)&&(o>(max-half_gap-1))){
		thresh->Image_segment_Thresholding((images[0]).c_str(), (patient_store_path+folders_name[patient_number]+"strain_position_"+std::to_string(o)+".jpg").c_str(), 1, 2, false);
		}

	}//end if
}//end for
}

void PCA_compute::compute_PCA(const char *inputfolder, int number_train, int modes_train){

  const   unsigned int        Dimension = 2;
  typedef float    my_PixelType;
  typedef itk::Image< my_PixelType, Dimension >    ImageType2;

  typedef  itk::ImageFileReader< ImageType2 > ReaderType;
  typedef  itk::ImageFileWriter<  ImageType  > WriterType;
  typedef itk::MultiplyImageFilter < ImageType2 , ImageType2 ,ImageType2 > ScaleType;

  unsigned int nb_train= number_train;

  using FilterType = itk::CastImageFilter< ImageType, ImageType2  >;
  FilterType::Pointer filter2 = FilterType::New();

  using FilterType2 = itk::CastImageFilter< ImageType2, ImageType  >;
  FilterType2::Pointer filter3 = FilterType2::New();
  std::vector<ImageType2::Pointer> trainingImages( nb_train );


  itk::NumericSeriesFileNames::Pointer fileNamesCreator =itk::NumericSeriesFileNames::New();

  fileNamesCreator->SetStartIndex( 0 );
  fileNamesCreator->SetEndIndex( nb_train - 1 );
  fileNamesCreator->SetSeriesFormat( inputfolder);
  const std::vector<std::string> & shapeModeFileNames = fileNamesCreator->GetFileNames();



  for ( unsigned int k = 0; k < nb_train; k++ )
    {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( shapeModeFileNames[k].c_str() );
    reader->Update();
    trainingImages[k] = reader->GetOutput();

    }
  typedef itk::ImagePCAShapeModelEstimator<ImageType2,   ImageType2 >  my_Estimatortype;
  my_Estimatortype::Pointer filter = my_Estimatortype::New();
  filter->SetNumberOfTrainingImages(nb_train);
  filter->SetNumberOfPrincipalComponentsRequired(modes_train);
  for ( unsigned int k = 0; k < nb_train; k++ )
  {
	filter->SetInput(k, trainingImages[k] );
   }


  unsigned int nb_modes=modes_train;

  itk::NumericSeriesFileNames::Pointer fileNamesOutCreator = itk::NumericSeriesFileNames::New();

  fileNamesOutCreator->SetStartIndex( 0 );
  fileNamesOutCreator->SetEndIndex( nb_modes-1  );
//////////////////////////////
    // Creating a directory 
    if (mkdir("PCA", 0777) == -1) 
        cerr << "Error :  " << strerror(errno) << endl; 
  
    else
        cout << "Directory created" << std::endl;
/////////////////////////////////////////
  fileNamesOutCreator->SetSeriesFormat( "PCA/PCA_combonent_%d.jpg" );
  const std::vector<std::string> & outFileNames = fileNamesOutCreator->GetFileNames();

  ScaleType::Pointer scaler = ScaleType::New();

  filter->Update();
  my_Estimatortype::VectorOfDoubleType v=filter->GetEigenValues();
  double sv_mean=sqrt(v[0]);
//   std::cout << "v: " << v[2] << std::endl;
//std::cout << "v: " << v[1] << std::endl;
//std::cout << "v: " << v[0] << std::endl;
//v[modes_train-1]=v[modes_train-1]*10000000000000;
  for ( unsigned int k = 0; k < nb_modes; k++ )
    {
    double sv=sqrt(std::abs(v[k]));
    double sv_n=(sv/sv_mean)*255;
    //double sv_n=sv;
    std::cout << "writing: " << outFileNames[k] << std::endl;

    std::cout << "svd[" << k << "]: " << sv << " norm: " << sv_n << std::endl;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outFileNames[k].c_str() );
    scaler->SetInput(filter->GetOutput(k));
    scaler->SetConstant(sv_n);
     filter3->SetInput( scaler->GetOutput() );
    writer->SetInput( filter3->GetOutput() );
    writer->Update();
    }


}
void PCA_compute::path_create(std::string store_data_path){

		if (mkdir(store_data_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
}
