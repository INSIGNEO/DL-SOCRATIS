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

#include "unbiased_ground_trouth.h"
#include "Segmentation_Pipeline.h"
#include "Initial_Segmentation.h"
#include "Evaluation.h"
#include "Atlas_segm.h"
#include "Interpolate_3D.h"
#include "configuration.h"
#include <memory>
#include <string>
#include <iostream>
#include <stdlib.h>    
#include <algorithm>
#include "itkListSample.h"
#include "itkStatisticsAlgorithm.h"
#include "itkVector.h"
#include <ctime>
#include "Deformation.h"
#include "strain_compute.h"
#include "Sensitivity.h"
#include "Translation_space.h"

//public
using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 1 >;
using ReaderType = itk::ImageFileReader< ImageType >;
using MeasurementType = double;
using MeasurementVectorType = itk::Vector< MeasurementType, 1 >;
using SampleType = itk::Statistics::ListSample<MeasurementVectorType>;
using SubsampleType = itk::Statistics::Subsample<SampleType>;




//main function exe parser
int main( int argc, char* argv[] )
{
fprintf(stdout,"%s Version %d.%d\n", argv[0],TestPipeline_VERSION_MAJOR, TestPipeline_VERSION_MINOR);
fprintf(stdout,"Usage: %s number\n",argv[0]);
std::string root="./";

std::clock_t start;
double duration;
start = std::clock();


const char * confg_file;

////////////////////////////////////////////////////////////////////////
if (argc ==1){
	confg_file=("../config.cfg");
	cout << "file load : " <<"../config.cfg" << endl;
}
if (argc >1){
        confg_file=argv[1];
        cout << "file load : " <<confg_file<< endl;
}
////////////////////////////////////////////////////////////////////////////////////////

//configuration pass values
std::shared_ptr<configuration > CONFG =std::make_shared<configuration>();
CONFG->read(confg_file);


//translation space

if( CONFG->Get_translation_space()==true){
	
		std::shared_ptr<configuration > CONFG2 =std::make_shared<configuration>();
		CONFG2-> Set_segm(true);
		CONFG2-> read(confg_file);
		vector<int> nb_images= CONFG2->Get_patients_images_number();
		vector<string>  patients=CONFG2->Get_patients_paths();
		std::string heart_phase= CONFG2->Get_heart_phase();


	// translation
	std::string m_python_path=CONFG2->Get_trans_paths();
	std::string store_data_path=CONFG2->Get_store_paths();
	std::string m_init_image_space=CONFG2->Get_init_space();
	std::string m_final_image_space=CONFG2->Get_final_space();

	std::shared_ptr<Translation_space > trans_space =std::make_shared<Translation_space >(root);
	trans_space->Set_patients(patients);
	trans_space->Set_images_per_patient(nb_images);
	trans_space->Set_heart_phase(heart_phase);
	trans_space->Run_trans(m_python_path, m_init_image_space, m_final_image_space,store_data_path);


}


//initial segmentation

if( CONFG->Get_init_segm()==true){
	if( CONFG->Get_dl_segm()==true){
		std::shared_ptr<configuration > CONFG2 =std::make_shared<configuration>();
		CONFG2-> Set_segm(true);
		CONFG2-> read(confg_file);

		int num_patient= CONFG2->Get_number_of_patients();
		vector<string> init_segmentation_paths= CONFG2->Get_init_segmentation_paths();
		std::string    store_data_path=init_segmentation_paths[0];
		std::string  python_path=init_segmentation_paths[2];
		std::string  conf_path=init_segmentation_paths[1];
		int num_classes=CONFG2->Get_num_classes();
		vector<string> store_segmentation_paths= CONFG2->Get_segmentation_paths();
		std::string    store_myo_path=store_segmentation_paths[0];
		std::string    store_roi_path=store_segmentation_paths[1];

		std::string command2 = "rm -rf ";
		command2 +="";
		command2 +=(store_myo_path+"*"); 
		int f= system(command2.c_str());

		std::string command21 = "rm -rf ";
		command21 +="";
		command21 +=(store_roi_path+"*"); 
		int f1= system(command21.c_str());

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
		std::string heart_phase= CONFG2->Get_heart_phase();
		init_segmentation->Set_patients(patients);
		init_segmentation->Set_images_per_patient(nb_images);	
		init_segmentation->Store(store_data_path, store_myo_path, store_roi_path);
		init_segmentation->Run(python_path,conf_path,num_classes,heart_phase);
	}//end if

	if( CONFG->Get_atlas_segm()==true){
		std::shared_ptr<configuration > CONFG2 =std::make_shared<configuration>();
		CONFG2-> Set_segm(true);
		CONFG2-> read(confg_file);

		vector<string> atlas_paths= CONFG2->Get_atlas_paths();
		std::string   data_path=atlas_paths[0];
		std::string  landmarks_path=atlas_paths[1];

		vector<string> store_segmentation_paths= CONFG2->Get_segmentation_paths();
		std::string    store_myo_path=store_segmentation_paths[0];
		std::string    store_roi_path=store_segmentation_paths[1];
		std::string command2 = "rm -rf ";
		command2 +=" ";
		command2 +=(store_myo_path+"*"); 
		int f= system(command2.c_str());

		std::string command21 = "rm -rf ";
		command21 +=" ";
		command21 +=(store_roi_path+"*"); 
		int f1= system(command21.c_str());

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
		std::shared_ptr<Atlas_segm > init_atlas =std::make_shared<Atlas_segm >(root);
		std::string heart_phase= CONFG2->Get_heart_phase();
		std::string  type_of_input_data=CONFG2->Get_type_of_input_data();
		init_atlas->Set_heart_phase(heart_phase);
		init_atlas->Set_type_of_input_data(type_of_input_data);	
		init_atlas->Store(store_myo_path, store_roi_path);
		init_atlas->Run(data_path, landmarks_path);
	}//end if


}//end if



if( CONFG->Get_segm()==true){
	vector<int> nb_images= CONFG->Get_patients_images_number();
	int num_patient= CONFG->Get_number_of_patients();
	vector<string> segmentation_paths= CONFG->Get_segmentation_paths();
	vector<string>  patients=CONFG->Get_patients_paths();
	std::string  myo_path=segmentation_paths[0];
	std::string  roi_path=segmentation_paths[1];
	std::string  scar_path=segmentation_paths[2];
	std::string  segm_case=segmentation_paths[3];
	std::string scar_path_patient;
	bool bz=CONFG->Get_segmentation_border_zone_bool();
	int tosi=CONFG->Get_TH_Scar_intens();
	if (mkdir(scar_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		cout << "Directory created" << std::endl;
	}
	// segmentation
	if (segm_case=="scar" || segm_case=="scar_bz" ){

		std::string command2 = "rm -rf ";
		command2 +="";
		command2 +=(myo_path+"mask*"); 
		int f= system(command2.c_str());


		std::string command21 = "rm -rf ";
		command21 +="";
		command21 +=(roi_path+"ROI* "); 
		command21 +=(roi_path+"*/stor* "); 
		command21 +=(roi_path+"*/roi* "); 
		int f1= system(command21.c_str());



		std::shared_ptr<Segmentation_Pipeline > segmentation =std::make_shared<Segmentation_Pipeline>(root);
		segmentation->Set_segmentation_case( segm_case);
		segmentation->SetBorderZone( bz);
		segmentation->SetThreshold_of_scar_intens( tosi);
		segmentation->Run(segmentation_paths[0].c_str(),segmentation_paths[1].c_str(),segmentation_paths[2], num_patient);
	}
	else
	{
		for ( unsigned int k = 0; k < num_patient; k++ )
		{
		int start=0;
		if (k>=start){
			std::cout << " PATIENT: "<< k <<std::endl;
		  	myo_path=segmentation_paths[0];
			roi_path=segmentation_paths[1];
			scar_path=segmentation_paths[2];
			myo_path=(myo_path+patients[k])+'/';
			roi_path=(roi_path+patients[k])+'/';
			scar_path_patient=scar_path+patients[k]+"/";
			// Creating  directory 

			if (mkdir(scar_path_patient.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			std::cout << myo_path << std::endl;
			std::cout << roi_path << std::endl;
			std::cout << scar_path_patient  << std::endl;
			int nb_images_patient=nb_images[k];
			std::shared_ptr<Segmentation_Pipeline > segmentation_scar =std::make_shared<Segmentation_Pipeline>(root);
			segmentation_scar->Set_segmentation_case( segm_case);
			segmentation_scar->SetBorderZone( bz);
			segmentation_scar->SetThreshold_of_scar_intens( tosi);
			segmentation_scar->Run(myo_path.c_str(),roi_path.c_str(),scar_path_patient,nb_images_patient);

			}//end continue if
		}//end for
	}//end else
}//end if

// 3D slice manual scars


if( CONFG->Get_inter()==true){
	if( CONFG->Get_slice()==true){
		vector<int> inb_images= CONFG->Get_inter_patients_images_number();
		int inum_patient= CONFG->Get_number_of_patients();
		vector<string> interpolation_paths= CONFG->Get_interpolate_paths();
		vector<string>  ipatients=CONFG->Get_inter_patients_paths();
		std::string    threeD_path=interpolation_paths[0];
		std::string    store_labels_path=interpolation_paths[1];
		if (mkdir(store_labels_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		for ( unsigned int k = 0; k < inum_patient; k++ )
		{
			std::cout << " PATIENT: "<< k <<std::endl;

			threeD_path=interpolation_paths[0];
			store_labels_path=interpolation_paths[1];


			threeD_path=(threeD_path+ipatients[k])+'/';
			store_labels_path=(store_labels_path+ipatients[k])+'/';
		
			// Creating  directory 

			if (mkdir(store_labels_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			std::cout << threeD_path << std::endl;
			std::cout << store_labels_path << std::endl;
			int inb_images_patient = inb_images[k];

			std::shared_ptr<Interpolate_3D > slice_3D =std::make_shared<Interpolate_3D>();
			slice_3D->threeD_to_2D_slicer(threeD_path.c_str(), store_labels_path.c_str(), inb_images_patient);	

		}
	}

}


//evaluate results && ground truth

if( CONFG->Get_eval()==true){

	std::shared_ptr<configuration > CONFG2 =std::make_shared<configuration>();
	CONFG2-> Set_eval(true);
	CONFG2-> Set_segm(true);
	CONFG2-> Set_init_segm(true);
	CONFG2-> read(confg_file);


	std::string heart_phase= CONFG2->Get_heart_phase(); //take from config
	if (heart_phase=="BT"){heart_phase="ED";}
	vector<string> evaluation_paths= CONFG2->Get_evaluation_paths();
	int number_experts=CONFG2->Get_evaluation_number_experts();
	std::string    manual_path=evaluation_paths[0];
	std::string    predict_labels_path=evaluation_paths[1];

	std::string manual_path_myo=manual_path +heart_phase+"/mask_manual/"+ "ground_truth"+"/";
	std::string predictpath=predict_labels_path+heart_phase+"/";

	std::shared_ptr<unbiased_ground_trouth > UBGT1 =std::make_shared<unbiased_ground_trouth >(root);
	std::size_t found=manual_path.find("intra");
	std::string gtruthcase="inter";
	if (found!=std::string::npos){gtruthcase="intra";}

		if (mkdir((evaluation_paths[0]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}

		if (mkdir((manual_path).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		if (mkdir((evaluation_paths[1]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
	vector<string> segmentation_paths= CONFG2->Get_segmentation_paths();
	std::string  segm_case=segmentation_paths[3];
	if (segm_case=="both"){segm_case="all";}

	if( CONFG2->Get_copy()==true){
		//be sure that the folders are deleted
		std::shared_ptr<Sensitivity > sens1 =std::make_shared<Sensitivity>( );
		sens1->delete_folder_localy(manual_path +heart_phase);
		sens1->delete_folder_localy(manual_path +"image");
		sens1->delete_folder_localy(segmentation_paths[2] +"image");
		sens1->delete_folder_localy(predictpath );

		std::shared_ptr<unbiased_ground_trouth >UNGT =std::make_shared<unbiased_ground_trouth>(root);
		UNGT->Set_heart_phase(heart_phase);
		UNGT->Run(manual_path, manual_path, number_experts,segm_case);

		std::string path_of_code_results=segmentation_paths[2]; //take from config

		std::shared_ptr<Evaluation > EV =std::make_shared<Evaluation>(root);
		EV->copy_pedrict_data( predictpath,  path_of_code_results,  segm_case);
	}


//evaluate results
	std::shared_ptr<Evaluation > EV1 =std::make_shared<Evaluation>(root);
	vector<string> patient_folders= UBGT1->read_images_folder((predictpath), 1, true);
	//align match two images lists

	EV1->preprocessing_align_match_images(predictpath, manual_path_myo ,  patient_folders, segm_case, gtruthcase );
	
	

	std::string manual_path_scar=manual_path +heart_phase+"/mask_manual/"+ "ground_truth"+"/";

	
	std::shared_ptr<Evaluation > EV2 =std::make_shared<Evaluation>(root);
	if (segm_case=="all" || segm_case=="scar"){
EV1->compute_dice_sensitivity_specificity_metric(predictpath, manual_path_myo ,  patient_folders, "myo" );
	
		EV2->compute_dice_sensitivity_specificity_metric(predictpath, manual_path_scar ,  patient_folders, "scar" );
	}
	if ( segm_case=="scar_bz"){
		EV1->compute_dice_sensitivity_specificity_metric(predictpath, manual_path_myo ,  patient_folders, "myo" );
		EV2->compute_dice_sensitivity_specificity_metric(predictpath, manual_path_scar ,  patient_folders, "scar" );
	
		EV2->compute_dice_sensitivity_specificity_metric(predictpath, manual_path_scar ,  patient_folders, "border_zone" );
	}


}

// 3D interpolation


if( CONFG->Get_inter()==true){
        if(CONFG->Get_eval()==false || CONFG->Get_copy()==false){std::shared_ptr<Evaluation > EVcopy =std::make_shared<Evaluation>(root);EVcopy->copy(confg_file);			cout << "Copy predict data..." << std::endl;}
	if( CONFG->Get_interpolate()==true){
		std::shared_ptr<configuration > CONFG2 =std::make_shared<configuration>();
		CONFG2-> Set_eval(true);
		CONFG2-> read(confg_file);
		vector<int> inb_images= CONFG2->Get_inter_patients_images_number();
		int inum_patient= CONFG2->Get_number_of_patients();
		vector<string> interpolation_paths= CONFG2->Get_interpolate_paths();
		vector<string>  ipatients=CONFG2->Get_inter_patients_paths();
		vector<bool>  ivision=CONFG2->Get_3D_vision();
		std::string    store3D_path=interpolation_paths[2];
		std::string    predict_2D_path=CONFG2->Get_evaluation_paths()[1];
		int gap_mri=CONFG2->Get_gap_mri();
		double a=CONFG2->Get_alpha();
		if (mkdir(store3D_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		for ( unsigned int k = 0; k < inum_patient; k++ )
		{
			std::cout << " PATIENTs: "<< k <<std::endl;

			store3D_path=interpolation_paths[2];
			predict_2D_path=CONFG2->Get_evaluation_paths()[1];
			store3D_path=(store3D_path+ipatients[k])+'/';
			predict_2D_path=(predict_2D_path+ipatients[k])+'/';		
			// Creating  directory 

			if (mkdir(store3D_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(predict_2D_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			std::cout << store3D_path<< std::endl;
			std::cout << predict_2D_path<< std::endl;
			int inb_images_patient = inb_images[k];

			std::string predict_2D_path_scar=predict_2D_path+"Scars/";
			std::string predict_2D_path_scarbz=predict_2D_path+"BZScars/";
			std::string predict_2D_path_myo=predict_2D_path+"Myo/";
			std::string predict_2D_path_endo=predict_2D_path+"Endo/";
			std::string predict_2D_path_epi=predict_2D_path+"Epi/";
			std::string store3D_path_scar=store3D_path+"Scars/";

			std::string store3D_path_scarbz=store3D_path+"BZScars/";
			std::string store3D_path_myo=store3D_path+"Myo/";
			std::string store3D_path_endo=store3D_path+"Endo/";
			std::string store3D_path_epi=store3D_path+"Epi/";
//create folders
			if (mkdir(predict_2D_path_scar.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(predict_2D_path_scarbz.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(predict_2D_path_myo.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(predict_2D_path_endo.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(predict_2D_path_epi.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(store3D_path_scar.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(store3D_path_scarbz.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(store3D_path_epi.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(store3D_path_endo.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir(store3D_path_myo.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}


			std::shared_ptr<Interpolate_3D > Inter_3D =std::make_shared<Interpolate_3D>();
                        Inter_3D->Set_alpha(a);
			Inter_3D->SetGap(gap_mri);
			Inter_3D->twoD_to_3D_interpolater(predict_2D_path_myo.c_str(),store3D_path_myo.c_str(),  inb_images_patient, "3dMyo", ivision[2]);
			Inter_3D->twoD_to_3D_interpolater(predict_2D_path_scarbz.c_str(),store3D_path_scarbz.c_str(),  inb_images_patient, "3dScarsbz",ivision[0]);
			Inter_3D->twoD_to_3D_interpolater(predict_2D_path_scar.c_str(),store3D_path_scar.c_str(),  inb_images_patient, "3dScars",ivision[0]);
			Inter_3D->twoD_to_3D_interpolater(predict_2D_path_endo.c_str(),store3D_path_endo.c_str(),  inb_images_patient, "3dEndo", ivision[1]);
			Inter_3D->twoD_to_3D_interpolater(predict_2D_path_epi.c_str(),store3D_path_epi.c_str(),  inb_images_patient, "3dEpi", ivision[2]);

			std::string s1("3dScars.vtu"); 
			string store_scar=store3D_path_scar+s1;
			std::string s2("3dMyo.vtu"); 
			string store_myo=store3D_path_endo+s2;
			Inter_3D->create_LV(store_myo,store_scar,store3D_path_scar.c_str());
	

		}
	}

}


if( CONFG->Get_deform_reg()==true){

		std::shared_ptr<configuration > CONFG2 =std::make_shared<configuration>();
		if( CONFG->Get_deform_dl_seg()==true){CONFG2->Set_init_segm_dl(true); CONFG2-> Set_segm(true);}

		CONFG2-> read(confg_file);
		vector<string> deform_images= CONFG2-> Get_deform_images();
		std::string    inputfix =deform_images[0];
		std::string  inputmove =deform_images[1];
		std::string store_def_path = CONFG2->Get_store_deform_path();
		cout << store_def_path << std::endl;

		if (mkdir(store_def_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}

		std::shared_ptr<Deformation > deformation_compute =std::make_shared<Deformation >(root);
		if( CONFG->Get_deform_dl_seg()==true){deformation_compute->Setconfig(confg_file);}

		deformation_compute->Run(inputfix, inputmove, store_def_path,CONFG->Get_deform_dl_seg());






		std::shared_ptr<strain_compute > strain_comp =std::make_shared<strain_compute >(root);

		//strain_comp->Run(store_def_path,store_def_path+"/strain_ED_ES/");

	}//end if	

//sensitivity

if( CONFG->Get_sens()==true){


	vector<string> sens_string= CONFG->Get_sensitivity_strings();
	vector<float> low_boundary= CONFG->Get_sensitivity_low();
	vector<float> high_boundary= CONFG->Get_sensitivity_high();
	vector<float> SD= CONFG->Get_sensitivity_SD();
	vector<bool> parameters= CONFG->Get_sensitivity_parameters();
	std::string path=sens_string[1];
	std::string mode=sens_string[0];
	std::shared_ptr<Sensitivity > sens =std::make_shared<Sensitivity>(parameters, mode );
	sens->Set_lower_border(low_boundary);
	sens->Set_higher_border(high_boundary);
	sens->Set_Initial_SD(SD);
	sens->RUN( confg_file,path);

}//end if



duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

std::cout<<"Time to finish the segmentation: "<< duration <<'\n';
}
