/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////


#include "Deformation.h"



void Deformation::Run(std::string ES_path, std::string ED_path, std::string store_path, bool dl  )
{

	if (mkdir((store_myo_path).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}


std::string ES_path_roi;
std::string ED_path_roi;


struct dirent *entry11;
DIR *pDIR11;

if( pDIR11=opendir(ED_path.c_str()) ){
	while(entry11 = readdir(pDIR11)){
		if( strcmp(entry11->d_name, ".") != 0 && strcmp(entry11->d_name, "..") != 0 ){
		std::stringstream name;
		name<<entry11->d_name;
		folders_name.push_back(name.str());
		}
		}
	closedir(pDIR11);
}
std::sort(folders_name.begin(), folders_name.end());

std::shared_ptr<Atlas_segm > init_atlas =std::make_shared<Atlas_segm >(root);
init_atlas->write_json_roi_file(folders_name,"roidl.json");
//fill in the number of images slices per patient
for ( unsigned int kp = 0; kp < folders_name.size(); kp++ )
{
	vector<string> inp;
	struct dirent *entry3p;
	DIR *pDIR2p;
		ES_path_roi=ES_path+folders_name[kp]+"/ROI";
		ED_path_roi=ED_path+folders_name[kp]+"/ROI";

		if (mkdir((ES_path_roi).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}

		if (mkdir((ED_path_roi).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}

		if (mkdir((store_myo_path+"/"+folders_name[kp]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
			}
		else{
			cout << "Directory created" << std::endl;
			}
	std::string number_path=ED_path+"/"+folders_name[kp];
	if( pDIR2p=opendir((number_path+"/").c_str()) ){
		while(entry3p = readdir(pDIR2p)){
			if( strcmp(entry3p->d_name, ".") != 0 && strcmp(entry3p->d_name, "..") != 0 && strcmp(entry3p->d_name, "contour") != 0 && strcmp(entry3p->d_name, "ROI") != 0){
			std::stringstream i3p;
			i3p<<(number_path+"/")<<entry3p->d_name;
		        inp.push_back(i3p.str());
			//cout << i3.str() << "\n";
			}
			}
		closedir(pDIR2p);
	}

	patient_number.push_back(inp.size());

}

// run ROI

/*

if (dl==true){

	run_unet_DL_model("ED");

	run_unet_DL_model("ES");
}
else{run_roiDL_model(ES_path, ED_path, "None");}

*/


postprocessing_ROI("ED",dl );
postprocessing_ROI("ES",dl );

//create repository store

vector<string> output;
for ( unsigned int k = 0; k < folders_name.size(); k++ )
{
	std::string store_path_patient=store_path+folders_name[k];
			if (mkdir(store_path_patient.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
						cerr << "Error :  " << strerror(errno) << endl; 
					}
					else{
						cout << "Directory created" << std::endl;
					}
		        output.push_back(store_path_patient);
}
std::sort(output.begin(), output.end());

//run analysis
for ( unsigned int kp = 0; kp < folders_name.size(); kp++ )
{

	//ES_path_roi=ES_path+folders_name[kp]+"/ROI/";
	//ED_path_roi=ED_path+folders_name[kp]+"/ROI/";
	ES_path_roi=sstore_path+"ES"+folders_name[kp];
	ED_path_roi=sstore_path+"ED"+folders_name[kp];

	vector<string> input2;
	struct dirent *entry2;
	DIR *pDIR1;

	if( pDIR1=opendir(ES_path_roi.c_str()) ){
			while(entry2 = readdir(pDIR1)){
				if( strcmp(entry2->d_name, ".") != 0 && strcmp(entry2->d_name, "..") != 0 ){
				std::stringstream io;
				io<<ES_path_roi<<entry2->d_name;
				input2.push_back(io.str());
				cout << io.str() << "\n";
				}
				}
			closedir(pDIR1);
	}
	std::sort(input2.begin(), input2.end());

	vector<string> input;
	struct dirent *entry;
	DIR *pDIR;

	if( pDIR=opendir(ED_path_roi.c_str()) ){
			while(entry = readdir(pDIR)){
				if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
				std::stringstream io;
				io<<ED_path_roi<<entry->d_name;
				input.push_back(io.str());
				cout << io.str() << "\n";
				}
				}
			closedir(pDIR);
	}

	std::sort(input.begin(), input.end());

		cout << "Patient_number: " <<kp<<"/"<<folders_name.size()<< "\n";
		cout << "Patient_name: " <<folders_name[kp]<< "\n";
		//cout << "Patient_name: " <<input_patient.size()<< "\n";
		//cout << "Patient_name: " <<input_patient2.size()<< "\n";
		//cout << "Patient_name: " <<output.size()<< "\n";
		each_patient_compute(input, input2,  output[kp]);




}



// workflow for PCA compute


// registration of images (strategy of how to register the images with respect of number...
// add them in PCA of image 
//create the 2D shape models of strain
//store with some way
std::shared_ptr<PCA_compute > PCA =std::make_shared<PCA_compute>(root);
int max_images_number=(int) *std::max_element(patient_number.begin(),patient_number.end());
auto largest=std::max_element(patient_number.begin(),patient_number.end());
int max_images_location= std::distance(patient_number.begin(), largest);
cout << "Larger Patient MRI slices number: " <<max_images_number<< "\n";
cout << "The patient number: " <<max_images_location<< "\n";
PCA->SetMax_images_number(max_images_number+2);
PCA->SetMax_images_location(max_images_location);
PCA->compute_3DPCA(store_path, folders_name);



//do the same for scars

//write code to compair the two PCAs


}

//std::string commandwr2r;
//commandwr2r += "rm -rf ";
//commandwr2r += "../data_internal/Input/image_ES_dl/image/*/ROI";
//int wrf2r= system(commandwr2r.c_str());

//std::string commandwr2re;
//commandwr2re += "rm -rf ";
//commandwr2re += "../data_internal/Input/image_ED_dl/image/*/ROI";
//int wrf2re= system(commandwr2re.c_str());


void Deformation::each_patient_compute(std::vector<string> input12, std::vector<string> input22, std::string output)
{ 

if (mkdir((output + "/deformation").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
	cerr << "Error :  " << strerror(errno) << endl; 
}
else{
	cout << "Directory created" << std::endl;
}

if (mkdir((output + "/registration").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
	cerr << "Error :  " << strerror(errno) << endl; 
}
else{
	cout << "Directory created" << std::endl;
}

if (mkdir((output + "/strain").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
	cerr << "Error :  " << strerror(errno) << endl; 
}
else{
	cout << "Directory created" << std::endl;
}

if (mkdir((output + "/strain2").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
	cerr << "Error :  " << strerror(errno) << endl; 
}
else{
	cout << "Directory created" << std::endl;
}

for ( unsigned int k = 0; k < input12.size(); k++ )
{
std::string vector_image= output + "/deformation/displacement_" + std::to_string(k) + ".jpg";
std::string vector_image_binary= output + "/strain/strain_daimons_" +std::to_string( k) + ".jpg";
std::string output_main= output + "/registration/registration_" + std::to_string(k) + "_field.jpg";

std::string vector_image2= output + "/deformation/displacement2_" + std::to_string(k) + ".jpg";
std::string vector_image_binary2= output + "/strain2/strain_b_spline_" +std::to_string( k) + ".jpg";
std::string output_main2= output + "/registration/registration2_" + std::to_string(k) + ".jpg";

cout << "Patient_slice: " <<k<<"/"<<input22.size()<< "\n";
//cout << "Patient_image_ED: " <<input12[k]<< "\n";
//cout << "Patient_image_ES: " <<input22[k]<< "\n";

//based in u-net crop the area



std::shared_ptr<deformable_registration > deformable_regist =std::make_shared<deformable_registration >(root);

deformable_regist->compute_demons((input12[k]+"/comb_myo_b.jpg").c_str(), (input22[k]+"/comb_myo_b.jpg").c_str(), output_main.c_str(), vector_image.c_str(), vector_image_binary.c_str() );

clean_data();
deformable_regist->compute_b_splines((input12[k]+"/comb_myo_b.jpg").c_str(), (input22[k]+"/comb_myo_b.jpg").c_str(), output_main2.c_str(), vector_image2.c_str(), vector_image_binary2.c_str()  );
clean_data();


}



}



void Deformation::run_roiDL_model(std::string store_ES_path, std::string store_ED_path, std::string roi_path)
{
std::string roi_image_path2="../../Initial_Segmentation/atlas/";
	if (root=="./"){roi_image_path2="../Initial_Segmentation/atlas/";}
if( roi_path!="None"){ def_roi_run=roi_image_path2; store_myo_path=roi_path;}



if(store_ES_path!="None" && store_ED_path!="None" && (store_ES_path!="GT_case" && store_ED_path!="S_Scar_case")){
	std::string command = "python3 ";
	command += def_roi_run+"roi.py";
	command += " ";
	command += def_roi_run+"roi_ES.config";
	int f= system(command.c_str());

	std::string store_path_roi=store_roi_image_path+"ROI/test/";
		if (mkdir( store_path_roi.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}


	std::shared_ptr< Initial_Segmentation > init_segmentation =std::make_shared< Initial_Segmentation >(root);
	vector<int> nb_images= patient_number;
	vector<string>  patients;
	for (unsigned int l = 0; l < folders_name.size(); l++ ){
		patients.push_back(folders_name[l]+"/ROI");
	}
	init_segmentation->Set_patients(patients);
	init_segmentation->Set_images_per_patient(nb_images);
	init_segmentation->Copy( store_path_roi, store_myo_path, store_ES_path, 0);

	std::string commandwr21;
	commandwr21 += "rm -rf ";
	commandwr21 +=  store_path_roi +"/*";
	int wrf21= system(commandwr21.c_str());


	std::string command1 = "python3 ";
	command1 += def_roi_run+"roi.py";
	command1 += " ";
	command1 += def_roi_run+"roi_ED.config";
	int f1= system(command1.c_str());

		if (mkdir( store_path_roi.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}


	//vector<int> nb_images= patient_number;
	//vector<string>  patients=folders_name;
	//init_segmentation->Set_patients(patients);
	//init_segmentation->Set_images_per_patient(nb_images);
	init_segmentation->Copy( store_path_roi, store_myo_path, store_ED_path, 0);

	//std::string commandwr121;
	//commandwr121 += "rm -rf ";
	//commandwr121 +=  store_path_roi +"/*";
	//int wrf121= system(commandwr121.c_str());

	//std::string commandwr121m;
	//commandwr121m += "rm -rf ";
	//commandwr121m +=  store_myo_path ;
	//int wrf121m= system(commandwr121m.c_str());
}


if(store_ES_path=="None" || store_ED_path=="None"){

	std::string store_path;
	std::string roi_file;
	if(store_ES_path=="None" ){store_path=store_ED_path; roi_file="roi_ED.config";}
	if(store_ED_path=="None" ){store_path=store_ES_path; roi_file="roi_ES.config";}

	std::string command = "python3 ";
	command += def_roi_run+"roi.py";
	command += " ";
	command += def_roi_run+roi_file;
	int f= system(command.c_str());

	std::string store_path_roi=store_roi_image_path+"ROI/test/";
		if (mkdir( store_path_roi.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}


	std::shared_ptr< Initial_Segmentation > init_segmentation =std::make_shared< Initial_Segmentation >(root);
	vector<int> nb_images= patient_number;
	vector<string>  patients;
	for (unsigned int l = 0; l < folders_name.size(); l++ ){
		patients.push_back(folders_name[l]+"/ROI");
	}
	init_segmentation->Set_patients(patients);
	init_segmentation->Set_images_per_patient(nb_images);
	init_segmentation->Copy( store_path_roi, store_myo_path, store_path, 0);

	//std::string commandwr21;
	//commandwr21 += "rm -rf ";
	//commandwr21 +=  store_path_roi +"/*";
	//int wrf21= system(commandwr21.c_str());
}

if(store_ES_path=="GT_case" || store_ED_path=="S_Scar_case"){

	std::string store_path;
	std::string kind;
	std::string roi_file;
	if(store_ES_path=="GT_case" ){store_path=store_ED_path; roi_file="GT_roi.config"; kind="GT";}
	if(store_ED_path=="S_Scar_case" ){store_path=store_ES_path; roi_file="S_Scar_roi.config"; kind="SS";}

	std::cout << "GT_case run file at path: " << def_roi_run+roi_file<<std::endl;

	std::string command = "python3 ";
	command += def_roi_run+"roi.py";
	command += " ";
	command += def_roi_run+roi_file;
	int f= system(command.c_str());

	std::string store_path_roi="../../data_internal/Data/"+kind+"/ROI/test/";

if (root=="./"){

	store_path_roi="../data_internal/Data/"+kind+"/ROI/test/";
		if (mkdir(("../data_internal/Data/"+kind+"/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}

		if (mkdir( ("../data_internal/Data/"+kind+"/ROI/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}

		if (mkdir( store_path_roi.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}
}
else{

		if (mkdir(("../../data_internal/Data/"+kind).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}

		if (mkdir( ("../../data_internal/Data"+kind+"/ROI/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}

		if (mkdir( store_path_roi.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}

}
	std::shared_ptr< Initial_Segmentation > init_segmentation =std::make_shared< Initial_Segmentation >(root);
	vector<int> nb_images= patient_number;
	vector<string>  patients;
	for (unsigned int l = 0; l < folders_name.size(); l++ ){
		patients.push_back(folders_name[l]);
	}
	init_segmentation->Set_patients(patients);
	init_segmentation->Set_images_per_patient(nb_images);
	init_segmentation->Copy( store_path_roi, store_myo_path, store_path, 0);

	//std::string commandwr21;
	//commandwr21 += "rm -rf ";
	//commandwr21 +=  store_path_roi +"/*";
	//int wrf21= system(commandwr21.c_str());
}


}


void Deformation::run_unet_DL_model(std::string cardiac_phase)
{
std::shared_ptr<configuration > CONFG =std::make_shared<configuration>();
CONFG-> read(confg_file);
std::shared_ptr<configuration > CONFG2 =std::make_shared<configuration>();
		if( CONFG->Get_deform_dl_seg()==true){CONFG2->Set_init_segm_dl(true);CONFG2-> Set_segm(true);}
		CONFG2-> read(confg_file);
		int num_patient= CONFG2->Get_number_of_patients();
		vector<string> init_segmentation_paths= CONFG2->Get_init_segmentation_paths();
		std::string    store_data_path=init_segmentation_paths[0];//+cardiac_phase+"/";
		std::string  python_path=init_segmentation_paths[2];
		std::string  conf_path=init_segmentation_paths[1];
		int num_classes=CONFG2->Get_num_classes();
		vector<string> store_segmentation_paths= CONFG2->Get_segmentation_paths();
		std::string    store_myo_path=store_segmentation_paths[0]+cardiac_phase+"/";
		std::string    store_roi_path=store_segmentation_paths[1]+cardiac_phase+"/";


		if (mkdir(store_data_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}

		if (mkdir(store_myo_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}

		if (mkdir(store_roi_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		// segmentation

		std::shared_ptr<Initial_Segmentation > init_segmentation =std::make_shared<Initial_Segmentation >(root);
		vector<int> nb_images= CONFG2->Get_patients_images_number();
		vector<string>  patients=CONFG2->Get_patients_paths();
		init_segmentation->Set_patients(patients);
		init_segmentation->Set_images_per_patient(nb_images);	
		init_segmentation->Store(store_data_path, store_myo_path, store_roi_path);
		init_segmentation->Run(python_path,conf_path,num_classes,cardiac_phase);


}





void Deformation::postprocessing_ROI( std::string cardiac_phase, bool dl )
{

// create cycle and pass from re-estimate of atlas extract final ES

//std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>();

//MI->Image_merge(m_inputfix.c_str(),m_inputmove.c_str(),m_outputFileName.c_str());


if( dl==true){

	std::shared_ptr<configuration > CONFG2 =std::make_shared<configuration>();
		CONFG2->Set_init_segm_dl(true);CONFG2-> Set_segm(true);
		CONFG2-> read(confg_file);
	vector<int> nb_images= patient_number;
	int num_patient= folders_name.size();
	vector<string> segmentation_paths= CONFG2->Get_segmentation_paths();
	vector<string>  patients=folders_name;//CONFG2->Get_patients_paths();
	std::string    myo_path=segmentation_paths[0];
	std::string    roi_path=segmentation_paths[1];
	sstore_path=segmentation_paths[2];
	std::string sstore_path_patient;
	bool bz=CONFG2->Get_segmentation_border_zone_bool();
	int tosi=0;//CONFG2->Get_TH_Scar_intens();
	if (mkdir(sstore_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		cout << "Directory created" << std::endl;
	}
	// segmentation
	if (mkdir((sstore_path+cardiac_phase+"/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		cout << "Directory created" << std::endl;
	}

	for ( unsigned int k = 0; k < num_patient; k++ )
	{
		std::cout << " PATIENT: "<< k <<std::endl;
	  	myo_path=segmentation_paths[0];
		roi_path=segmentation_paths[1];
		sstore_path=segmentation_paths[2];
		myo_path=(myo_path+cardiac_phase+"/"+patients[k])+'/';
		roi_path=(roi_path+cardiac_phase+"/"+patients[k])+'/';
		sstore_path_patient=sstore_path+cardiac_phase+"/"+patients[k]+"/";




		// Creating  directory 

		if (mkdir(sstore_path_patient.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		std::cout << myo_path << std::endl;
		std::cout << roi_path << std::endl;
		std::cout << sstore_path_patient  << std::endl;
		int nb_images_patient=nb_images[k];
		std::shared_ptr<Segmentation_Pipeline > segmentation =std::make_shared<Segmentation_Pipeline>(root);
		segmentation->SetBorderZone( bz);
		segmentation->SetThreshold_of_scar_intens( tosi);
		segmentation->Run(myo_path.c_str(),roi_path.c_str(),sstore_path_patient,nb_images_patient);
	
	}

}









}
