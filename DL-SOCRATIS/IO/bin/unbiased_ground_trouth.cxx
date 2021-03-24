#include "unbiased_ground_trouth.h"

//TODO write a roi and dl model run handler class need it as you use it very often

void unbiased_ground_trouth::Run(std::string manual_labels_path, std::string store_image_path, int size_of_experts, std::string store_label)

{

std::string  path_of_atlas;
path_of_atlas="../Initial_Segmentation/multi_atlas/CIMAS/";


//measurement evaluate
for ( unsigned int ex = 0; ex < size_of_experts; ex++ )
{

std::cout << "The " <<ex+1<<" expert  manual results preprocessing"<<std::endl;

std::shared_ptr< Atlas_segm > atlas1 =std::make_shared< Atlas_segm >(root);

std::string mlp_expert=manual_labels_path+std::to_string(ex)+"/";
std::vector<string> mask =read_images_folder(mlp_expert,0,true);
std::vector<string> folders_name =read_images_folder(mlp_expert,1,true);

	if(ex == 0){
	atlas1->write_json_roi_file(folders_name,"roi.json");
	}

	if (mkdir((store_image_path).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}


	for ( unsigned int k = 0; k < folders_name.size(); k++ )
	{

		if (size_of_experts==1){type_of_input_data="jpg";}
		if (type_of_input_data=="dcm"){
			std::string command = path_of_atlas;
			command +="dcm2niix";
			command += " ";
			command += "-o ";
			command += mask[k];
			command += " ";
			command += "-f ";
			command += "mask_mri";
			command += " ";
			command += "-z y ";
			command += mask[k];		
			int f= system(command.c_str());
			}

		if (mkdir((store_image_path+heart_phase).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}


		if (mkdir((store_image_path+heart_phase+"/mask_manual").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}

	} //end for

	for ( unsigned int k = 0; k < folders_name.size(); k++ )
	{

		if (mkdir((store_image_path+heart_phase+"/mask_manual"+"/"+std::to_string(ex)+"/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
		}
		if (mkdir((store_image_path+heart_phase+"/mask_manual"+"/"+std::to_string(ex)+"/"+folders_name[k]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}

		if (mkdir((store_image_path+heart_phase+"/mask_manual"+"/"+std::to_string(ex)+"/"+folders_name[k]+"/scar/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}

		if (mkdir((store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/myo/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}
	
		if (mkdir((store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/border_zone/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}
	
if (type_of_input_data=="jpg"){
        std::vector<string> mask_img =read_images_folder(mask[k]+"/",0,true);
	int up3=0;
	int up2=0;
	std::string save_number2;
	for ( unsigned int w = 0; w< mask_img.size(); w++ )
	{ up2=w;
			if (up2>9){
				up3=up2-9;
				save_number2="A"+std::to_string(up3);
				
			}
			else
			{
				save_number2=std::to_string(up2);
				
			}
		std::string st_gt_jpg=store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/mask_img_"+save_number2+".jpeg";
		std::stringstream s;
		s << mask_img[w] ;
		std::string spath=s.str();
		boost::filesystem::copy_file(spath, st_gt_jpg,boost::filesystem::copy_option::overwrite_if_exists);

	}

preprocessing(store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/", store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/myo/","jpg");


}



if (type_of_input_data=="dcm"){

	slice2dimages(store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k], mask[k]+"/mask_mri.nii.gz");

	preprocessing(store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/", store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/myo/","myo");

	//preprocessing(store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/", store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/scar/","scar");




	preprocessing(store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/", store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/border_zone/","bz");

	preprocessing(store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/", store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(ex)+"/"+folders_name[k]+"/scar/","scar_bz");}




	}//end for

}//end for

std::string mlp_expert_first=store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(0)+"/";

std:: vector<string> folders_name2 =read_images_folder(mlp_expert_first,1,true);
std::cout << "number of patient: " << folders_name2.size()<<std::endl;

//compute ground truth
//find the min number of the images for the each expert

for ( unsigned int j = 0; j < folders_name2.size(); j++ )
{

int min=10000000;
for ( unsigned int exp = 0; exp < size_of_experts; exp++ )
{
std::string mlp_expert_first=store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(exp)+"/";
std:: vector<string> folders_name2 =read_images_folder(mlp_expert_first,1,true);
std:: vector<string> seg_im =read_images_folder(store_image_path+heart_phase+"/mask_manual"+"/"+ std::to_string(exp)+"/"+folders_name2[j]+"/myo",1,false);
if  (min>seg_im.size()){min=seg_im.size();}

}//end nested for

patient_number.push_back(min);
}//end for





for ( unsigned int h = 0; h < folders_name2.size() ;h++ )
{
	compute(store_image_path+heart_phase+"/mask_manual"+"/" , size_of_experts, folders_name2[h]);
}




//extract roiDL_model detection of gt; 


if (mkdir((store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"myo/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
else{
		cout << "Directory created" << std::endl;
	}

if (mkdir((store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"scar/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
else{
	
	cout << "Directory created" << std::endl;
	}

if (mkdir((store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"border_zone/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
else{
		cout << "Directory created" << std::endl;
	}

std::string mask_compute_myo_new =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"myo/";
std::string mask_compute_scar_new =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"scar/";
std::string mask_compute_scar_bz_new =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"border_zone/";

for ( unsigned int k = 0; k < folders_name2.size() ;k++ ){
	 	//int	images_number=;

		std::cout << "Patient: " <<folders_name2[k]<<" has "<<patient_number[k]<< " MRI slices."<<std::endl;
}
for ( unsigned int k = 0; k < folders_name2.size() ;k++ ){
	std::string mask_compute_myo =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+folders_name2[k]+"/myo/";
	std::string mask_compute_scar =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+folders_name2[k]+"/scar/";


	if (mkdir((store_image_path+"image"+"/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}

	if (mkdir((store_image_path+"image"+"/"+folders_name2[k]+"/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}

	if (mkdir((mask_compute_myo_new+folders_name2[k]+"/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}
	if (mkdir((mask_compute_scar_new+folders_name2[k]+"/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}

	int co=0;
	int up2=0;
	int up=0;


	for ( unsigned int o = 0; o < patient_number[k] ;o++ ){
					std::string save_number;	
					if (up2>9)
					{
						up=up2-9;
						save_number="A"+std::to_string(up);	
					}
					else
					{
						save_number=std::to_string(up2);
						
					}
		if (mkdir((store_image_path+"image/"+folders_name2[k]+"/contour/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
			else{
					cout << "Directory created" << std::endl;
				}
		std::shared_ptr< Segmentation_Pipeline > sgp1 =std::make_shared< Segmentation_Pipeline >(root);
		if (sgp1 ->check_black_image((mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str(),0,0)==true)
		{ 
			std::cout << "Delete black image: "<<"final_myo_"+save_number+".jpeg"<<", "<<(mask_compute_scar+"final_scar_"+save_number+".jpeg").c_str()<<std::endl;
			std::string commandwr2r;
			commandwr2r += "rm -rf ";
			commandwr2r += (mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str();
			int wrf2r= system(commandwr2r.c_str());

			std::string commandwr2r1;
			commandwr2r1 += "rm -rf ";
			commandwr2r1 += (mask_compute_scar+"final_scar_"+save_number+".jpeg").c_str();
			int wrf2r1= system(commandwr2r1.c_str());

			
		
		}

		else{
			boost::filesystem::copy_file( (mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str(), (store_image_path+"image/"+folders_name2[k]+"/myo_"+save_number+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
			boost::filesystem::copy_file( (mask_compute_scar+"final_scar_"+save_number+".jpeg").c_str(), (store_image_path+"image/"+folders_name2[k]+"/contour/scar_"+save_number+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
			

		co++;
		}
up2=up2+1;
	}//end for
patient_number[k]=co;
cout << "new patient "<<folders_name2[k]<<" number: " <<co<<", old: "<< patient_number[k]<< std::endl;
}//end for


std::shared_ptr< Deformation > Def =std::make_shared< Deformation >(root);
Def->Set_patients(folders_name2);
Def->Set_images_per_patient(patient_number);
//Def->Set_store_myo_path(mask_compute_myo_new);
Def->run_roiDL_model("GT_case",  mask_compute_myo_new, mask_compute_scar_new);


std::string mlp_expert_manual=manual_labels_path+"ES/"+"mask_manual/"+std::to_string(0)+"/";
for ( unsigned int k = 0; k < folders_name2.size() ;k++ )
{
	int up=0;
	int up2=0;
	int up3=0;
	int up4=0;
	int up5=0;
       int count_copy_images=0;
	//int	images_number1=;
	for ( unsigned int o = 0; o < patient_number[k] ;o++ )
	{
		std::string save_number;
		if (o>9){
			up=o-9;
			save_number="A"+std::to_string(up);
			
		}
		else
		{
			save_number=std::to_string(o);
		}	

		std::string mask_compute_myo_image =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"myo/"+folders_name2[k]+"/"+"ROItest"+save_number+".jpeg";
		std::string mask_compute_scar_image =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"scar/"+folders_name2[k]+"/"+"myo"+save_number+".jpeg";



		std::shared_ptr< Segmentation_Pipeline > sgp =std::make_shared< Segmentation_Pipeline >(root);

		if (sgp ->check_black_image(mask_compute_myo_image.c_str(),0,0)==true)
		{ 
			std::cout << "Delete black image: "<<mask_compute_myo_image<<", "<<mask_compute_scar_image<<", "<<save_number<<std::endl;
			std::string commandwr2r;
			commandwr2r += "rm -rf ";
			commandwr2r += mask_compute_myo_image;
			int wrf2r= system(commandwr2r.c_str());

			std::string commandwr2r1;
			commandwr2r1 += "rm -rf ";
			commandwr2r1 += mask_compute_scar_image;
			int wrf2r1= system(commandwr2r1.c_str());


			up5++;
		}
		else
		{ 
			std::string save_number2;
			std::cout << "Align masks images: "<<mask_compute_myo_image<<", "<<mask_compute_scar_image<< std::endl;
	
				
			if (up2>9){
				up3=up2-9;
				save_number2="A"+std::to_string(up3);
				
			}
			else
			{
				save_number2=std::to_string(up2);
				
			}

			std::cout << "number images copy: "<<up2<<", "<<save_number2<<", "<<save_number<< std::endl;

			if (save_number2!=save_number)
			{
				std::cout << "Rename mask images "<< std::endl;
				boost::filesystem::copy_file( (mask_compute_myo_image).c_str(), (store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"myo/"+folders_name2[k]+"/"+"ROItest"+save_number2+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);

				boost::filesystem::copy_file( (mask_compute_scar_image).c_str(), (store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"scar/"+folders_name2[k]+"/"+"myo"+save_number2+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
				
				count_copy_images++;

			}


			

			up2=up2+1;
		}


	} //end for


	//delete all images that are more than up2
	std:: vector<string> update_images =read_images_folder(store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"myo/"+folders_name2[k]+"/",0,false);
	std:: vector<string> update_images2 =read_images_folder(store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"scar/"+folders_name2[k]+"/",0,false);



	if (up2<update_images.size())
	{
		for ( unsigned int oi = up2; oi < update_images.size() ;oi++ )
		{


			std::cout << "Delete same images: "<<update_images[oi]<<", "<<update_images2[oi]<<std::endl;
	
			std::string commandwr2r;
			commandwr2r += "rm -rf ";
			commandwr2r += (update_images[oi]);
			int wrf2r= system(commandwr2r.c_str());

			std::string commandwr2r1;
			commandwr2r1 += "rm -rf ";
			commandwr2r1 += (update_images2[oi]);
			int wrf2r1= system(commandwr2r1.c_str());

		
		}// end for


		std:: vector<string> update_imagestest =read_images_folder(store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"scar/"+folders_name2[k]+"/",0,true);




		if (up2!=update_imagestest.size()){std::cout << "ERROR: no equal images update and old after deleted!!!!!!!! "<<std::endl;}
	}
	else if (up2==update_images.size()){std::cout << "NO same images detected "<<std::endl;}
	else{std::cout << "ERROR: the update images are more than initial one overflow issue!!!!!!!! "<<std::endl;}


} //end for





if (store_label=="scar_bz"){


for ( unsigned int k = 0; k < folders_name2.size() ;k++ ){

	std::string mask_compute_scar_bz =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+folders_name2[k]+"/border_zone/";
	int up=0;
	int up2=0;
	
					std::string commandwr2rs1;
			commandwr2rs1 += "rm -rf ";
			commandwr2rs1 += (store_image_path+"image/"+folders_name2[k]+"/contour/*").c_str();
			int wrf2rs1= system(commandwr2rs1.c_str());
	if (mkdir((mask_compute_scar_bz_new+folders_name2[k]+"/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}



	for ( unsigned int o = 0; o < patient_number[k] ;o++ ){
					std::string save_number;	
					if (up2>9)
					{
						up=up2-9;
						save_number="A"+std::to_string(up);	
					}
					else
					{
						save_number=std::to_string(up2);
						
					}


			
			boost::filesystem::copy_file( (mask_compute_scar_bz+"final_scar_bz_"+save_number+".jpeg").c_str(), (store_image_path+"image/"+folders_name2[k]+"/contour/scar_bz_"+save_number+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
	up2=up2+1;
	}//end for


}//end for


std::string commandwr2r1q;
commandwr2r1q += "rm -rf ";
commandwr2r1q += "../data_internal/Data/GT/ROI/test/*";
int wrf2r1w= system(commandwr2r1q.c_str());

Def->run_roiDL_model("GT_case",  mask_compute_myo_new, mask_compute_scar_bz_new);


//align images

//TODO optimize this loop too timeconsuming

std::string mlp_expert_manual=manual_labels_path+"ES/"+"mask_manual/"+std::to_string(0)+"/";
for ( unsigned int k = 0; k < folders_name2.size() ;k++ )
{
	int up=0;
	int up2=0;
	int up3=0;
	int up4=0;
	int up5=0;
       int count_copy_images=0;
	//int	images_number1=;
	for ( unsigned int o = 0; o < patient_number[k] ;o++ )
	{
		std::string save_number;
		if (o>9){
			up=o-9;
			save_number="A"+std::to_string(up);
			
		}
		else
		{
			save_number=std::to_string(o);
		}	

		
		std::string mask_compute_myo_image =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"myo/"+folders_name2[k]+"/"+"ROItest"+save_number+".jpeg";

		std::string mask_compute_scar_bz_image =store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"border_zone/"+folders_name2[k]+"/"+"myo"+save_number+".jpeg";

		std::shared_ptr< Segmentation_Pipeline > sgp =std::make_shared< Segmentation_Pipeline >(root);

		if (sgp ->check_black_image(mask_compute_myo_image.c_str(),0,0)==true)
		{ 
			std::cout << "Delete black image: "<<mask_compute_myo_image<<", "<<mask_compute_scar_bz_image<<", "<<save_number<<std::endl;
			

			std::string commandwr2r1;
			commandwr2r1 += "rm -rf ";
			commandwr2r1 += mask_compute_myo_image;
			int wrf2r1= system(commandwr2r1.c_str());

			std::string commandwr2r1c;
			commandwr2r1c += "rm -rf ";
			commandwr2r1c += mask_compute_scar_bz_image;
			int wrf2r1c= system(commandwr2r1c.c_str());

			up5++;
		}
		else
		{ 
			std::string save_number2;
			std::cout << "Align masks images: "<<mask_compute_myo_image<<", "<<mask_compute_scar_bz_image<< std::endl;
	
				
			if (up2>9){
				up3=up2-9;
				save_number2="A"+std::to_string(up3);
				
			}
			else
			{
				save_number2=std::to_string(up2);
				
			}

			std::cout << "number images copy: "<<up2<<", "<<save_number2<<", "<<save_number<< std::endl;

			if (save_number2!=save_number)
			{
				

				boost::filesystem::copy_file( (mask_compute_myo_image).c_str(), (store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"myo/"+folders_name2[k]+"/"+"ROItest"+save_number2+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
				boost::filesystem::copy_file( (mask_compute_scar_bz_image).c_str(), (store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"border_zone/"+folders_name2[k]+"/"+"myo"+save_number2+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
				count_copy_images++;

			}


			

			up2=up2+1;
		}


	} //end for


	//delete all images that are more than up2

	std:: vector<string> update_images2 =read_images_folder(store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"myo/"+folders_name2[k]+"/",0,false);
	std:: vector<string> update_images3 =read_images_folder(store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"border_zone/"+folders_name2[k]+"/",0,false);


	if (up2<update_images2.size())
	{
		for ( unsigned int oi = up2; oi < update_images2.size() ;oi++ )
		{


			std::cout << "Delete same images: "<<update_images2[oi]<<", "<<update_images3[oi]<<std::endl;
	

			std::string commandwr2r1;
			commandwr2r1 += "rm -rf ";
			commandwr2r1 += (update_images2[oi]);
			int wrf2r1= system(commandwr2r1.c_str());

			std::string commandwr2r1f;
			commandwr2r1f += "rm -rf ";
			commandwr2r1f += (update_images3[oi]);
			int wrf2r15= system(commandwr2r1.c_str());
		}// end for


		std:: vector<string> update_imagestest =read_images_folder(store_image_path+heart_phase+"/mask_manual"+"/" + "ground_truth"+"/"+"myo/"+folders_name2[k]+"/",0,true);




		if (up2!=update_imagestest.size()){std::cout << "ERROR: no equal images update and old after deleted!!!!!!!! "<<std::endl;}
	}

	else if (up2==update_images2.size()){std::cout << "NO same images detected "<<std::endl;}
	else{std::cout << "ERROR: the update images are more than initial one overflow issue!!!!!!!! "<<std::endl;}



} //end for


//delete blcak images

}//end if scar_bz

}

std::vector<double> unbiased_ground_trouth::align_images(std::string image, double center[2], bool compute_center_only){
		std::vector<double> center_vector;

  		std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  		Resample1->resample(image.c_str(),512,1);

		std::shared_ptr< Segmentation_Pipeline > sgp =std::make_shared< Segmentation_Pipeline >(root);

		if (sgp->check_black_image(image.c_str(),0,0)==false){

				if (center[0]==0 && center[1]==0){
					std::cout << "Align based affine registration "<< std::endl;
			 		using PixelType2 = unsigned char;
			  		using FixedImageType2 = itk::Image<PixelType2, 2>;
			 		using PixelType = float;
			  		using FixedImageType = itk::Image<PixelType, 2>;

			  		using FixedImageReaderType = itk::ImageFileReader<FixedImageType2>;
			  		FixedImageReaderType::Pointer  fixedImageReader = FixedImageReaderType::New();
			  		fixedImageReader->SetFileName(image.c_str());
					fixedImageReader->Update();

					using FilterType = itk::CastImageFilter< FixedImageType2, FixedImageType >;
					FilterType::Pointer filter = FilterType::New();
					filter->SetInput( fixedImageReader->GetOutput() );
					filter->Update();


			  		using FixedImageCalculatorType = itk::ImageMomentsCalculator<FixedImageType>;
			  		FixedImageCalculatorType::Pointer fixedCalculator = FixedImageCalculatorType::New();
			  		fixedCalculator->SetImage(filter->GetOutput());
			  		fixedCalculator->Compute();
			 
			 		FixedImageCalculatorType::VectorType fixedCenter =fixedCalculator->GetCenterOfGravity();
					//using MeasurementType2 = double;
					//using MeasurementVectorType2 = itk::Vector< MeasurementType2, 2 >;
					//MeasurementVectorType2 mean_endo_center;
					//mean_endo_center=BS-> Getmean_endo();
					double center2[2];
					center2[0]=0.0;center2[1]=0.0;
					center2[0]=fixedCenter[0];
					center2[1]=fixedCenter[1];
					center_vector.push_back(center2[0]);
					center_vector.push_back(center2[1]);
					if (compute_center_only==false){
						std::shared_ptr< Segmentation_Pipeline > SP=std::make_shared< Segmentation_Pipeline >(root);
						SP->align_images(image.c_str(),center2,0,1);
						std::cout << "Align based affine registration at center: " <<center2[0]<<" , "<<center2[1]<< std::endl;
					}
					std::cout << "The new center is given by : " <<center2[0]<<" , "<<center2[1]<< std::endl;
				}

				else{		

					center_vector.push_back(center[0]);
					center_vector.push_back(center[1]);

					std::cout << "The center is given by user: " <<center[0]<<" , "<<center[1]<< std::endl;
					if (compute_center_only==false){
						std::shared_ptr< Segmentation_Pipeline > SP2=std::make_shared< Segmentation_Pipeline >(root);
						SP2->align_images(image.c_str(),center,0,1);
						//std::cout << "Align based affine registration at given center: " <<center[0]<<" , "<<center[1]<< std::endl;
					}

				}


		}

		else{center_vector.push_back(0);center_vector.push_back(0); std::cout << "Black image not need of align!!!!"<< std::endl;
		}

		return center_vector;


}



void unbiased_ground_trouth::slice2dimages(std::string store_data, std::string imagenii){

cout << "Directory imagenii: " <<imagenii<< std::endl;

std::string save_image=store_data;

	if (mkdir((save_image).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}


std::string command4;
command4 +=path_of_med2jpg;
command4 += "med2image";
command4 += " -i ";
command4 += imagenii;
command4 += " -d ";
command4 += save_image;
command4 += " -o image.jpeg";	
command4 += " -s -1";	
int f= system(command4.c_str());

}


void unbiased_ground_trouth::preprocessing(std::string images_path, std::string store_path , std::string label){

cout << "preprocessing path images: " <<images_path<< std::endl;
std::shared_ptr< Atlas_segm > atlas =std::make_shared< Atlas_segm >(root);
std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();

std::string segm_path= images_path;
std::shared_ptr< Segmentation_Pipeline > sgp =std::make_shared< Segmentation_Pipeline >(root);
//read all the files in myo and roi directory
struct dirent *entry2;
DIR *pDIR1;

std:: vector<string> seg_images =read_images_folder(segm_path,0,false);
std:: vector<string> seg_images_name =read_images_folder(segm_path,1,false);
cout << "preprocessing sizes: " <<seg_images.size()<< std::endl;


for ( unsigned int k = 0; k < seg_images.size(); k++ )
{

cout << "image preprocessing number:  " <<k<< std::endl;
	std::string seg=seg_images[k];
	std::string store_seg=store_path+seg_images_name[k];
	//k-means
	std::shared_ptr<kmeans> kmean =std::make_shared<kmeans>();
	double means_image[4];
	means_image[0]=0;
	means_image[1]=80;
	means_image[2]=150;
	means_image[3]=230;
	int low;
	int up;
	kmean->Image_kmeans(seg.c_str(), (root+"kmeans.jpeg").c_str(), 4,4, means_image);

	using MeasurementVectorType1 = itk::Vector< float, 1 >;
	using SampleType1 = itk::Statistics::ListSample< MeasurementVectorType1 >;
	SampleType1::Pointer sample_PB = SampleType1::New();
	sample_PB=kmean->Getmeans();
	if (label=="jpg"){
		low=4;
		up=sample_PB->GetMeasurementVector(0)[2]+10;
			if (sgp->check_black_image(seg.c_str(),0,0)==false)
			{
				atlas->Image_segment_Thresholding(seg.c_str(), (store_seg).c_str(), low, up, false);
				BI->Image_binary((store_seg).c_str(),(store_seg).c_str(), low-2 ,up);
				for ( unsigned int i = 0 ; i < 10 ; ++i ){
					std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
					SM1->Median_smoothing((store_seg).c_str(),(store_seg).c_str(),1,1);
					}
			}
			else
			{
				boost::filesystem::copy_file( seg.c_str(), (store_seg).c_str(), boost::filesystem::copy_option::overwrite_if_exists);

			}
	}


	if (label=="myo"){
		low=10;
		up=255;
			if (sgp->check_black_image(seg.c_str(),0,0)==false)
			{
				atlas->Image_segment_Thresholding(seg.c_str(), (store_seg).c_str(), low, up, false);
				BI->Image_binary((store_seg).c_str(),(store_seg).c_str(), low-2 ,255);
				for ( unsigned int i = 0 ; i < 10 ; ++i ){
					std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
					SM1->Median_smoothing((store_seg).c_str(),(store_seg).c_str(),1,1);
					}
			}
			else
			{
				boost::filesystem::copy_file( seg.c_str(), (store_seg).c_str(), boost::filesystem::copy_option::overwrite_if_exists);

			}
	}
	if (label=="scar"){
		up=255;
		low=sample_PB->GetMeasurementVector(0)[3];
		//if (sample_PB->GetMeasurementVector(0)[1]==80){low=sample_PB->GetMeasurementVector(0)[3]-10;}//10}
		//if (sample_PB->GetMeasurementVector(0)[1]==80 && sample_PB->GetMeasurementVector(0)[2]==150){low=0;up=1;}//10}
		if (sample_PB->GetMeasurementVector(0)[1]==80 && sample_PB->GetMeasurementVector(0)[2]==150 && sample_PB->GetMeasurementVector(0)[3]==230){low=0;up=1;}//10}


		if (sgp->check_black_image(seg.c_str(),0,0)==false)
		{

			atlas->Image_segment_Thresholding(seg.c_str(), (store_seg).c_str(), low, up, false);
			
			if (low!=0&&up!=1){ BI->Image_binary((store_seg).c_str(),(store_seg).c_str(), low+15 ,up);}

			//for ( unsigned int i = 0 ; i < 10 ; ++i ){
			//	std::shared_ptr<denoising> SM1 =std::make_shared<denoising>();
			//	SM1->Median_smoothing((store_seg).c_str(),(store_seg).c_str(),1,1);
			//	}
		}
		else
		{
			boost::filesystem::copy_file( seg.c_str(), (store_seg).c_str(), boost::filesystem::copy_option::overwrite_if_exists);
			
		}
	}
	bool noscar=false;
	if (label=="bz"){
		low=sample_PB->GetMeasurementVector(0)[2]-10;
		up=sample_PB->GetMeasurementVector(0)[3];
		if (sample_PB->GetMeasurementVector(0)[1]==80){low=sample_PB->GetMeasurementVector(0)[3]-10;up=255; noscar=true;}

		if (sgp->check_black_image(seg.c_str(),0,0)==false)
		{
			atlas->Image_segment_Thresholding(seg.c_str(), (store_seg).c_str(), low, up, false);
			BI->Image_binary((store_seg).c_str(),(store_seg).c_str(), low-2 ,255);
			//for ( unsigned int i = 0 ; i < 10 ; ++i ){
			//	std::shared_ptr<denoising> SM1 =std::make_shared<denoising>();
			//	SM1->Median_smoothing((store_seg).c_str(),(store_seg).c_str(),1,1);
			//	}
		}
		else
		{
			boost::filesystem::copy_file( seg.c_str(), (store_seg).c_str(), boost::filesystem::copy_option::overwrite_if_exists);

		}
	}

	if (label=="scar_bz"){
		low=sample_PB->GetMeasurementVector(0)[3]+10;
		if (low>=250){low=250;}
		up=255;
		if (noscar==true){low=254;up=255;}
		if (sample_PB->GetMeasurementVector(0)[1]==80){low=0;up=1; noscar=true;}

		if (sgp->check_black_image(seg.c_str(),0,0)==false)
		{
			atlas->Image_segment_Thresholding(seg.c_str(), (store_seg).c_str(), low, up, false);
			BI->Image_binary((store_seg).c_str(),(store_seg).c_str(), low-2 ,255);
			//for ( unsigned int i = 0 ; i < 10 ; ++i ){
			//	std::shared_ptr<denoising> SM1 =std::make_shared<denoising>();
			//	SM1->Median_smoothing((store_seg).c_str(),(store_seg).c_str(),1,1);
			//	}
		}
		else
		{
			boost::filesystem::copy_file( seg.c_str(), (store_seg).c_str(), boost::filesystem::copy_option::overwrite_if_exists);

		}
	}



	std::string commandwr12;
	commandwr12 += "rm -rf ";
	commandwr12 += (root+"kmeans.jpeg");
	int wrf12= system(commandwr12.c_str());

}//end for



}

 
void unbiased_ground_trouth::compute(std::string path_labels , int size_expert, std::string patient_names )
{
int value_frondground2=(230/size_expert);
	std::shared_ptr<kmeans> kmean =std::make_shared<kmeans>();
	double means_image[size_expert+1];
	means_image[0]=0;
	for (int i=1;i<size_expert+1;i++){
	means_image[i]=means_image[i-1]+value_frondground2;
	}

	
int pick_position=((size_expert)*0.5)-0.5;

//int pick_position=1;

//test this probably something with and or 
int value_frondground=(240/size_expert);
//240/size_expert;
int  threshold=value_frondground*(size_expert+1)*0.5;
int threshold_scar=value_frondground*(size_expert+1)*0.5;

cout << "The binary  value frondground is: " <<value_frondground<< "grey level intensity"<<std::endl;

cout << "The best voting threshold is: " <<threshold<< "grey level intensity, position of kmean: "<<pick_position<<std::endl;

std::shared_ptr< Atlas_segm > atlas2 =std::make_shared< Atlas_segm >(root);
std::string mask_compute_myo1;
std::string mask_compute_scar1;
std::string mask_compute_border_zone1;

std::string mask_compute_myo2;
std::string mask_compute_scar2;
std::string mask_compute_border_zone2;

std::string mask_compute_myo;
std::string mask_compute_scar;
std::string mask_compute_border_zone;

// final store area of ground truth

mask_compute_myo =path_labels + "ground_truth"+"/"+patient_names+"/myo/";
mask_compute_scar =path_labels + "ground_truth"+"/"+patient_names+"/scar/";
mask_compute_border_zone =path_labels + "ground_truth"+"/"+patient_names+"/border_zone/";


		if (mkdir((path_labels + "ground_truth"+"/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}
		if (mkdir((path_labels + "ground_truth"+"/"+patient_names ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}



		if (mkdir((mask_compute_myo).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}
		if (mkdir((mask_compute_scar ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}

		if (mkdir((mask_compute_border_zone ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}





mask_compute_myo1 =path_labels + std::to_string(0)+"/"+patient_names+"/myo/";

mask_compute_scar1 =path_labels + std::to_string(0)+"/"+patient_names+"/scar/";

mask_compute_border_zone1 =path_labels + std::to_string(0)+"/"+patient_names+"/border_zone/";

if (size_expert==1){mask_compute_scar1 =path_labels + std::to_string(0)+"/"+patient_names+"/myo/";} //for 1 expert and no scar GT the same path}




// take name of images

std:: vector<string> myo =read_images_folder(mask_compute_myo1,1,false);
std:: vector<string> scar =read_images_folder(mask_compute_scar1,1,false);
std:: vector<string> bzone =read_images_folder(mask_compute_border_zone1,1,false);

if (size_expert<2){
int up2=0;
int up=0;
	for ( unsigned int x = 0; x < myo.size(); x++ ){
					std::string save_number;	
					if (up2>9)
					{
						up=up2-9;
						save_number="A"+std::to_string(up);	
					}
					else
					{
						save_number=std::to_string(up2);
						
					}

	boost::filesystem::copy_file( (mask_compute_myo1+myo[x]).c_str(), (mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
	boost::filesystem::copy_file( (mask_compute_scar1+scar[x]).c_str(), (mask_compute_scar+"final_scar_"+save_number+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
up2=up2+1;
		
//boost::filesystem::copy_file( (mask_compute_border_zone1+bzone[x]).c_str(),(mask_compute_border_zone +"final_scar_bz_"+save_number+".jpeg").c_str(),boost::filesystem::copy_option::overwrite_if_exists);
	}
}
else

{

	for ( unsigned int ex = 1; ex < size_expert; ex++ )
	{
	//temporally store of two experts

	mask_compute_myo2 =path_labels + std::to_string(ex)+"/"+patient_names+"/myo/";
	mask_compute_scar2 =path_labels + std::to_string(ex)+"/"+patient_names+"/scar/";
	mask_compute_border_zone2 =path_labels + std::to_string(ex)+"/"+patient_names+"/border_zone/";
	std:: vector<string> myo1 =read_images_folder(mask_compute_myo1,0,false);
	std:: vector<string> scar1 =read_images_folder(mask_compute_scar1,0,false);
	std:: vector<string> bzone1 =read_images_folder(mask_compute_border_zone1,0,false);
	std:: vector<string> myo2 =read_images_folder(mask_compute_myo2,0,false);
	std:: vector<string> scar2 =read_images_folder(mask_compute_scar2,0,false);
	std:: vector<string> bzone2 =read_images_folder(mask_compute_border_zone2,0,false);
	

	// add images
		if (ex-1==0){
			for ( unsigned int x = 0; x < myo1.size(); x++ )
			{
				std::shared_ptr<BinaryImage> B =std::make_shared<BinaryImage>();

				B->Image_binary_intensity_handle(myo1[x].c_str(),myo1[x].c_str(), threshold-2 ,255, value_frondground,0);
				B->Image_binary_intensity_handle(scar1[x].c_str(),scar1[x].c_str(), threshold-2 ,255, value_frondground,0);
				B->Image_binary_intensity_handle(bzone1[x].c_str(),bzone1[x].c_str(), threshold-2 ,255, value_frondground,0);
				
				}//end for
		}//end if
	

		for ( unsigned int x = 0; x < myo2.size(); x++ )
		{

			std::shared_ptr<BinaryImage> B2 =std::make_shared<BinaryImage>();
std::shared_ptr<BinaryImage> B =std::make_shared<BinaryImage>();

				B->Image_binary_intensity_handle(myo2[x].c_str(),myo2[x].c_str(), threshold-2 ,255, value_frondground,0);

			std::shared_ptr<mergeimages> MI2 =std::make_shared<mergeimages>(root);
		 	MI2->Image_add(myo1[x].c_str(), myo2[x].c_str() , (mask_compute_myo+myo[x]).c_str() );	
	
		}

	

		for ( unsigned int s = 0; s < scar2.size(); s++ )
		{
			std::shared_ptr<BinaryImage> B3 =std::make_shared<BinaryImage>();
			B3->Image_binary_intensity_handle(scar2[s].c_str(),scar2[s].c_str(), threshold_scar-2 ,255, value_frondground,0);
			

			std::shared_ptr<mergeimages> MI2s =std::make_shared<mergeimages>(root);
		 	MI2s->Image_add(scar1[s].c_str(), scar2[s].c_str() , (mask_compute_scar+scar[s]).c_str() );	
	
		}

	

		for ( unsigned int b = 0; b < bzone2.size(); b++ )
		{
			std::shared_ptr<BinaryImage> B3 =std::make_shared<BinaryImage>();
			B3->Image_binary_intensity_handle(bzone2[b].c_str(),bzone2[b].c_str(), threshold_scar-2 ,255, value_frondground,0);
			
			
			std::shared_ptr<mergeimages> MI2b =std::make_shared<mergeimages>(root);
		 	MI2b->Image_add(bzone1[b].c_str(), bzone2[b].c_str() , (mask_compute_border_zone+bzone[b]).c_str() );	
	
		}
	

	mask_compute_myo1 =mask_compute_myo;
	mask_compute_scar1 =mask_compute_scar;
	mask_compute_border_zone1 =mask_compute_border_zone;
std::cout << "end for " << "\n";

	}//end for


int up=0;
int up2=0;

std:: vector<string> myoup =read_images_folder(mask_compute_myo,1,false);
for ( unsigned int x = 0; x < myoup.size(); x++ )
{

std::shared_ptr< Segmentation_Pipeline > sgp1 =std::make_shared< Segmentation_Pipeline >(root);
std::shared_ptr< Segmentation_Pipeline > sgp2 =std::make_shared< Segmentation_Pipeline >(root);
std::shared_ptr< Segmentation_Pipeline > sgp3 =std::make_shared< Segmentation_Pipeline >(root);
std::cout << "IMAGES FOR KMEAN THRESH: " << "\n";
std::cout << (mask_compute_scar+scar[x]) << "\n";
std::cout << (mask_compute_myo+myo[x]) << "\n";

					std::string save_number;	
					if (up2>9)
					{
						up=up2-9;
						save_number="A"+std::to_string(up);	
					}
					else
					{
						save_number=std::to_string(up2);
						
					}
	if (sgp1->check_black_image((mask_compute_myo+myo[x]).c_str(),0,0)==false)
	{

		std::shared_ptr<kmeans> kmean =std::make_shared<kmeans>();
		kmean->Image_kmeans((mask_compute_myo+myo[x]).c_str(), (root+"kmeans_myo.jpeg").c_str(), size_expert+1,size_expert+1, means_image);
		using MeasurementVectorType1 = itk::Vector< float, 1 >;
		using SampleType1 = itk::Statistics::ListSample< MeasurementVectorType1 >;
		SampleType1::Pointer sample_PB = SampleType1::New();
		sample_PB=kmean->Getmeans();
		int threshold2=sample_PB->GetMeasurementVector(0)[pick_position]- 10;
		atlas2->Image_segment_Thresholding((root+"kmeans_myo.jpeg").c_str(), (mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str(), threshold2, 255, false);
		std::shared_ptr<BinaryImage> BI2 =std::make_shared<BinaryImage>();
		BI2->Image_binary((mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str(),(mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str(), threshold2-2 ,255);
		for ( unsigned int i = 0 ; i < 1 ; ++i ){//2
								std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
								SM1->Median_smoothing((mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str(),(mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str(),1,1);
								}

	}
	else
	{
		boost::filesystem::copy_file( (mask_compute_myo+myo[x]).c_str(), (mask_compute_myo+"final_myo_"+save_number+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);

	}

	if (sgp2->check_black_image((mask_compute_scar+scar[x]).c_str(),0,0)==false)
	{
		std::shared_ptr<kmeans> kmean2 =std::make_shared<kmeans>();
		kmean2->Image_kmeans((mask_compute_scar+scar[x]).c_str(), (root+"kmeans_scar.jpeg").c_str(), size_expert+1,size_expert+1, means_image);
		using MeasurementVectorType2 = itk::Vector< float, 1 >;
		using SampleType2 = itk::Statistics::ListSample< MeasurementVectorType2 >;
		SampleType2::Pointer sample_PB2 = SampleType2::New();
		sample_PB2=kmean2->Getmeans();
		int threshold3=sample_PB2->GetMeasurementVector(0)[pick_position]-5;//10

		atlas2->Image_segment_Thresholding((root+"kmeans_scar.jpeg").c_str(), (mask_compute_scar+"final_scar_"+save_number+".jpeg").c_str(), threshold3, 255, false);
		std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
		BI->Image_binary((mask_compute_scar+"final_scar_"+save_number+".jpeg").c_str(), (mask_compute_scar+"final_scar_"+save_number+".jpeg").c_str(), threshold3 ,255);
		
								
	}
	else
	{
		boost::filesystem::copy_file( (mask_compute_scar+scar[x]).c_str(), (mask_compute_scar+"final_scar_"+save_number+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);

	}



	std::string commandwr12;
	commandwr12 += "rm -rf ";
	commandwr12 += (root+"kmeans_scar.jpeg");
	int wrf12= system(commandwr12.c_str());

	std::string commandwr123;
	commandwr123 += "rm -rf ";
	commandwr123 += (root+"kmeans_myo.jpeg");
	int wrf123= system(commandwr123.c_str());
	if(size_expert>2) {
		if (sgp3->check_black_image((mask_compute_border_zone+bzone[x]).c_str(),0,0)==false)
		{
			std::shared_ptr<kmeans> kmean2v =std::make_shared<kmeans>();
			kmean2v->Image_kmeans((mask_compute_border_zone+bzone[x]).c_str(), (root+"kmeans_scar_bz.jpeg").c_str(), size_expert+1,size_expert+1, means_image);
			using MeasurementVectorType2 = itk::Vector< float, 1 >;
			using SampleType2 = itk::Statistics::ListSample< MeasurementVectorType2 >;
			SampleType2::Pointer sample_PB2v = SampleType2::New();
			sample_PB2v=kmean2v->Getmeans();
			int threshold3=sample_PB2v->GetMeasurementVector(0)[pick_position]-10;//10

			atlas2->Image_segment_Thresholding((root+"kmeans_scar_bz.jpeg").c_str(), (mask_compute_border_zone+"final_scar_bz_"+save_number+".jpeg").c_str(), threshold3, 255, false);
			std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
			BI->Image_binary((mask_compute_border_zone+"final_scar_bz_"+save_number+".jpeg").c_str(), (mask_compute_border_zone+"final_scar_bz_"+save_number+".jpeg").c_str(), threshold3 ,255);
		
								
		}
		else
		{
		boost::filesystem::copy_file( (mask_compute_border_zone+bzone[x]).c_str(), (mask_compute_border_zone+"final_scar_bz_"+save_number+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);

		}//end if



		std::string commandwr1234;
		commandwr1234 += "rm -rf ";
		commandwr1234 += (root+"kmeans_scar_bz.jpeg");
		int wrf1234= system(commandwr1234.c_str());
	}//end if 



	up2=up2+1;


		
	}//end for

} //end if

}



std:: vector<string> unbiased_ground_trouth::read_images_folder(std::string mlp_expert, int return_case, bool everything_read)
{
vector<string> mask;
struct dirent *entry;
DIR *pDIR;
vector<string> folders_name;

	if( pDIR=opendir(mlp_expert.c_str()) ){
		while(entry = readdir(pDIR)){
			if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
				std::stringstream i;
				std::stringstream name;
				name<<entry->d_name;
				i<<mlp_expert<<entry->d_name;

				std::string test_string=name.str();
				test_string.erase(test_string.begin(),test_string.end()-4);
				//cout << test_string << "\n";
				if (everything_read==false){
					if( test_string == "jpeg" || test_string == ".dcm" || test_string == ".jpg" || test_string == ".png" || test_string == ".vtk"){

						mask.push_back(i.str());
						//cout << i.str() << "\n";
						folders_name.push_back(name.str());
					}
				}//end if
				
				if (everything_read==true){

					mask.push_back(i.str());
					//cout << i.str() << "\n";
					folders_name.push_back(name.str());
				}//end if
			}//end if
			}
		closedir(pDIR);
	}
	std::sort(mask.begin(), mask.end());
	std::sort(folders_name.begin(), folders_name.end());
if (return_case==0){
	return mask;
}
if (return_case==1){
	return folders_name;
}

}



void unbiased_ground_trouth::prepare_run_ROI_DL(std::string store_image_path , std::string mask_compute_myo_new, std::string  mask_compute_roi_new)
{

std::vector<int> patient_number2;
std:: vector<string> folders_patient =read_images_folder((mask_compute_myo_new).c_str(),1,true);
	std::cout << "Prepare the Patinet: for roi dl analysis of: " <<folders_patient.size()<<std::endl;
	
	if (mkdir((mask_compute_roi_new+"ROI/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}
	if (mkdir((mask_compute_myo_new+"mask/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error   " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}
for ( unsigned int k = 0; k < folders_patient.size() ;k++ ){
	std::cout << "Prepare the Patinet: " <<k<<" for roi dl analysis of: " <<folders_patient.size()<<std::endl;

	if (mkdir((mask_compute_roi_new+"ROI/"+folders_patient[k]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}
	if (mkdir((mask_compute_myo_new+"mask/" +folders_patient[k] ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error   " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}
	if (mkdir((store_image_path+"image"+"/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}

	if (mkdir((store_image_path+"image"+"/"+folders_patient[k]+"/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}


	if (mkdir((mask_compute_myo_new+"mask/" +folders_patient[k]+"/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}
	if (mkdir((mask_compute_roi_new+"ROI/"+folders_patient[k]+"/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
			cout << "Directory created" << std::endl;
		}
	int co=0;	
	std:: vector<string> mask_myo =read_images_folder((mask_compute_myo_new+folders_patient[k]+"/" ).c_str(),0,false);		
	std:: vector<string> mask_roi =read_images_folder((mask_compute_roi_new+folders_patient[k]+"/" ).c_str(),0,false);		
	for ( unsigned int o = 0; o < mask_roi.size() ;o++ ){
		if (mkdir((store_image_path+"image/"+folders_patient[k]+"/contour/" ).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
			else{
					cout << "Directory created" << std::endl;
				}
		boost::filesystem::copy_file( (mask_myo[o]).c_str(), (store_image_path+"image/"+folders_patient[k]+"/myo_"+std::to_string(co)+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
		boost::filesystem::copy_file( (mask_roi[o]).c_str(), (store_image_path+"image/"+folders_patient[k]+"/contour/roi_"+std::to_string(co)+".jpeg").c_str(), boost::filesystem::copy_option::overwrite_if_exists);
		co++;


	}//end for

	patient_number2.push_back(mask_roi.size());

}//end for


std::shared_ptr< Deformation > Def =std::make_shared< Deformation >(root);
Def->Set_patients(folders_patient);
Def->Set_images_per_patient(patient_number2);
//Def->Set_store_myo_path(mask_compute_myo_new);
Def->run_roiDL_model( mask_compute_myo_new+"mask/", "S_Scar_case",mask_compute_roi_new+"ROI/");


}





