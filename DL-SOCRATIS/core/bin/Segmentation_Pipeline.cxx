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


#include "Segmentation_Pipeline.h"
#include <dirent.h>
#include <interparameters.h> 




void Segmentation_Pipeline::Run(const char * m_myoFolder, const char * m_ROIFolder, string m_SCARFolder,int m_nb_train)
{

std::string roifolder=m_ROIFolder;
std::string myofolder=m_myoFolder;
vector<string> myo;
vector<string> roi;
//read all the files in myo and roi directory
struct dirent *entry;
DIR *pDIR;



if (segmentation_part=="scar"){

	std::string myo_path;
	std::string roi_path;
	std::string scar_path;
	std::shared_ptr< unbiased_ground_trouth >UGT2=std::make_shared< unbiased_ground_trouth >(root);
	std:: vector<string> patients =UGT2->read_images_folder(m_ROIFolder,1,true);
	std::string scar_path_patient;
	std::vector<int> nb_images_num;
	if (Get_need_dl_case()==true)
	{
		
		for ( unsigned int k = 0; k < m_nb_train; k++ )
		{
			std::cout << " Start the DL ROI process of images of PATIENT: "<< k+1 <<std::endl;
		  	std::string myo_path1=m_myoFolder;
			std::string roi_path1=m_ROIFolder;
			std::string scar_path1=m_SCARFolder;
			
			myo_path=(myo_path1+patients[k])+'/';
			roi_path=(roi_path1+patients[k])+'/';
			scar_path_patient=scar_path1+patients[k]+"/";
			// Creating  directory 

			if (mkdir(scar_path_patient.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			dcm_converter(myo_path, roi_path);
			std::shared_ptr< unbiased_ground_trouth >UGT22=std::make_shared< unbiased_ground_trouth >(root);
			std:: vector<string> images =UGT22->read_images_folder(roi_path,1,false);
			nb_images_num.push_back(images.size());

		}// end for


		// roi dl pass 

		UGT2->prepare_run_ROI_DL(m_SCARFolder , (myofolder), (roifolder));



	}//end if

		//std::shared_ptr<denoising > SM_norm =std::make_shared<denoising>(root);
		//	SM_norm->Normalize_all_images(m_ROIFolder);
		//	int mean_total=SM_norm->Get_mean_total_images();
		//	int SD_total=SM_norm->Get_SD_total_images();

		m_nb_train=0;
		for ( unsigned int k = 0; k < m_nb_train; k++ )
		{
			std::cout << " PATIENT: "<< k <<" / "<<m_nb_train<<std::endl;


		  	std::string myo_path2=m_myoFolder;
			std::string roi_path2=m_ROIFolder;
			std::string scar_path2=m_SCARFolder;
			
			std::string myo_path_final=(myo_path2+"mask/"+patients[k])+'/';
			std::string roi_path_final=(roi_path2+"ROI/"+patients[k])+'/';
			std::string scar_path_patient_final=scar_path2+patients[k]+"/";
			// Creating  directory 

			if (mkdir(scar_path_patient_final.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			std::string store_roi= roi_path_final;
			std::shared_ptr< unbiased_ground_trouth >UGT4=std::make_shared< unbiased_ground_trouth >(root);
			std:: vector<string> store_roi_images =UGT4->read_images_folder(store_roi,0,false);
			int mean_group=0;
			int SD_group=0;
			bool norm_patient=false;
			if (norm_patient==true){
				std::cout << "normalization of : "<< k <<" / "<<m_nb_train<<" patient"<<std::endl;
				std::shared_ptr<denoising > SM_norm =std::make_shared<denoising>(root);
				SM_norm->Normalize_group_images(store_roi_images);
				mean_group=SM_norm->Get_mean_group_images();
				SD_group=SM_norm->Get_SD_group_images();
			}

			match_myo_ROI_mask(myo_path_final, roi_path_final,mean_group,SD_group);
			std::shared_ptr< unbiased_ground_trouth >UGT2=std::make_shared< unbiased_ground_trouth >(root);
			std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
			std::string store_masks_images= roi_path_final+"/"+"store_mask_myo/";

			std::string store_myo= myo_path_final;
			std:: vector<string> mask_images =UGT2->read_images_folder(store_masks_images,0,false);
			std::shared_ptr< unbiased_ground_trouth >UGT3=std::make_shared< unbiased_ground_trouth >(root);
			std:: vector<string> myo_images =UGT3->read_images_folder(store_myo,0,false);

		
				std::cout<<" roi images : "<<store_roi_images.size() <<std::endl;
				std::cout<<" myo images : "<<myo_images.size()<<std::endl;
				std::cout<<" mask images : "<<mask_images.size() <<std::endl;


			for ( unsigned int k = 0; k < mask_images.size(); k++ )
			{

				

				if (mkdir((scar_path_patient_final+std::to_string(k)+"/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}
				
					for ( unsigned int i = 0 ; i < 60 ; ++i ){
						SM1->Median_smoothing((mask_images[k]).c_str(),(mask_images[k]).c_str(),1,1);
					}
					std::string ROI=store_roi_images[k];
					image_detect=(mask_images[k]);
					binaryepi_endo_mask=myo_images[k];
					 binary_myo_mask=binaryepi_endo_mask;
					inputFileName_ROI=ROI.c_str();
					threshold_define();
					//extract epi endo
					std::shared_ptr< seedpoints >seed=std::make_shared< seedpoints >(root);
					double scale = 1;
					seed->sober_epi_endo(binaryepi_endo_mask.c_str(), scale);
					if (boost::filesystem::exists((root+"epi_segm.jpg").c_str())==true)
					{
						segmentation();
						copy_to_directory((scar_path_patient_final+std::to_string(k)+"/").c_str(),"all");
					}
					delete_images_localy(root);
			}//end for
		//	delete_images_localy(myo_path_final);
		//	delete_images_localy(roi_path_final);
			delete_images_localy(roi_path2+"/*/roi");
			delete_images_localy(roi_path2+"/*/stor");
			delete_images_localy(roi_path2+"/*/myo");
			//delete_images_localy(scar_path2+"image/");
		}//end for 
}

if ( segmentation_part=="scar_bz"){

	std::string myo_path;
	std::string roi_path;
	std::string scar_path;
	std::shared_ptr< unbiased_ground_trouth >UGT2=std::make_shared< unbiased_ground_trouth >(root);
	std:: vector<string> patients =UGT2->read_images_folder(m_ROIFolder,1,true);
	std::string scar_path_patient;
	std::vector<int> nb_images_num;
	if (Get_need_dl_case()==true)
	{
		
		for ( unsigned int k = 0; k < m_nb_train; k++ )
		{
			std::cout << " Start the DL ROI process of images of PATIENT: "<< k+1 <<std::endl;
		  	std::string myo_path1=m_myoFolder;
			std::string roi_path1=m_ROIFolder;
			std::string scar_path1=m_SCARFolder;
			
			myo_path=(myo_path1+patients[k])+'/';
			roi_path=(roi_path1+patients[k])+'/';
			scar_path_patient=scar_path1+patients[k]+"/";
			// Creating  directory 

			if (mkdir(scar_path_patient.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}

			//dcm_converter(myo_path, roi_path);

			std::shared_ptr< unbiased_ground_trouth >UGT22=std::make_shared< unbiased_ground_trouth >(root);
			std:: vector<string> images =UGT22->read_images_folder(roi_path,1,false);
			std:: vector<string> myo =UGT22->read_images_folder(myo_path,0,false);
			//std::cout << images << std::endl;
//			int size_expert=3;
//			int value_frondground=(240/size_expert);
//			int  threshold=value_frondground*(size_expert+1)*0.5;
//			int threshold_scar=value_frondground*(size_expert+1)*0.5;
//			for ( unsigned int x = 0; x < images.size(); x++ )
//			{
//				std::shared_ptr<BinaryImage> B =std::make_shared<BinaryImage>();
//
//				B->Image_binary_intensity_handle(myo[x].c_str(),myo[x].c_str(), threshold-5 ,255, 250,0);
//			}
			nb_images_num.push_back(images.size());

		}// end for


		// roi dl pass 
std::cout <<myofolder<< std::endl;
std::cout << roifolder<< std::endl;

		UGT2->prepare_run_ROI_DL(m_SCARFolder , (myofolder), (roifolder));



	}//end if
		for ( unsigned int k = 0; k < m_nb_train; k++ )
		{
			std::cout << " PATIENT: "<< k <<" / "<<m_nb_train<<std::endl;


		  	std::string myo_path2=m_myoFolder;
			std::string roi_path2=m_ROIFolder;
			std::string scar_path2=m_SCARFolder;
			
			std::string myo_path_final=(myo_path2+"/mask/"+patients[k])+'/';
			std::string roi_path_final=(roi_path2+"/ROI/"+patients[k])+'/';
			std::string scar_path_patient_final=scar_path2+patients[k]+"/";
			// Creating  directory 

			if (mkdir(scar_path_patient_final.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			std::string store_roi= roi_path_final;
			std::shared_ptr< unbiased_ground_trouth >UGT4=std::make_shared< unbiased_ground_trouth >(root);
			std:: vector<string> store_roi_images =UGT4->read_images_folder(store_roi,0,false);
			int mean_group=0;
			int SD_group=0;
			bool norm_patient=false;

			if (norm_patient==true){
				std::cout << "normalization of : "<< k <<" / "<<m_nb_train<<" patient"<<std::endl;
				std::shared_ptr<denoising > SM_norm =std::make_shared<denoising>(root);
				SM_norm->Normalize_group_images(store_roi_images);
				mean_group=SM_norm->Get_mean_group_images();
				SD_group=SM_norm->Get_SD_group_images();
			}

			match_myo_ROI_mask(myo_path_final, roi_path_final,mean_group,SD_group);
			std::shared_ptr< unbiased_ground_trouth >UGT2=std::make_shared< unbiased_ground_trouth >(root);
			std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
			std::string store_masks_images= roi_path_final+"/"+"store_mask_myo/";

			std::string store_myo= myo_path_final;
			std:: vector<string> mask_images =UGT2->read_images_folder(store_masks_images,0,false);
			std::shared_ptr< unbiased_ground_trouth >UGT3=std::make_shared< unbiased_ground_trouth >(root);
			std:: vector<string> myo_images =UGT3->read_images_folder(store_myo,0,false);

		
				std::cout<<" roi images : "<<store_roi_images.size() <<std::endl;
				std::cout<<" myo images : "<<myo_path_final<<std::endl;
				std::cout<<" mask images : "<<mask_images.size() <<std::endl;


			for ( unsigned int k = 0; k < mask_images.size(); k++ )
			{

				

				if (mkdir((scar_path_patient_final+std::to_string(k)+"/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}
				
					for ( unsigned int i = 0 ; i < 60 ; ++i ){
						SM1->Median_smoothing((mask_images[k]).c_str(),(mask_images[k]).c_str(),1,1);
					}
					std::string ROI=store_roi_images[k];
					image_detect=(mask_images[k]);
					binaryepi_endo_mask=myo_images[k];
					binary_myo_mask=binaryepi_endo_mask;
					inputFileName_ROI=ROI.c_str();
					threshold_define();
					//extract epi endo
					std::shared_ptr< seedpoints >seed=std::make_shared< seedpoints >(root);
					double scale = 1;
					seed->sober_epi_endo(binaryepi_endo_mask.c_str(), scale);
					if (boost::filesystem::exists((root+"epi_segm.jpg").c_str())==true)
					{
						segmentation();
						copy_to_directory((scar_path_patient_final+std::to_string(k)+"/").c_str(),"all");
					}
					delete_images_localy(root);
			}//end for
			//delete_images_localy(myo_path_final);
			//delete_images_localy(roi_path_final);
			delete_images_localy(roi_path2+"/*/roi");
			delete_images_localy(roi_path2+"/*/stor");
			delete_images_localy(roi_path2+"/*/myo");
			//delete_images_localy(scar_path2+"image/");
		}//end for 
}

	//myo segmentation pipelines only


if (segmentation_part=="myo")
{
			if( pDIR=opendir(m_myoFolder) ){
				while(entry = readdir(pDIR)){
					if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
					std::stringstream m;
					m<<m_myoFolder<<entry->d_name;
					myo.push_back(m.str());
					cout << m.str() << "\n";
					}
					}
				closedir(pDIR);
			}

			if( pDIR=opendir(m_ROIFolder) ){
				while(entry = readdir(pDIR)){
					if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
					std::stringstream r;
					r<<m_ROIFolder<<entry->d_name;
					roi.push_back(r.str());
					cout << r.str() << "\n";
					}
					}
				closedir(pDIR);
			}
			std::sort(myo.begin(),myo.end());
			std::sort(roi.begin(),roi.end());
			std::cout<<" The myo segmentation of the "<<m_nb_train<< " files start now ..."<<std::endl;
			  for ( unsigned int k = 0; k < m_nb_train; k++ )
			    {
				int epi_count=k+m_nb_train;
				itk::NumericSeriesFileNames::Pointer fileNamesCreator =itk::NumericSeriesFileNames::New();
				fileNamesCreator->SetStartIndex( 0 );
				fileNamesCreator->SetEndIndex( m_nb_train - 1 );
				fileNamesCreator->SetSeriesFormat( myo[k]);
				const std::vector<std::string> & shapeModeFileNamesmyo = fileNamesCreator->GetFileNames();

				itk::NumericSeriesFileNames::Pointer fileNamesCreator3 =itk::NumericSeriesFileNames::New();
				fileNamesCreator3->SetStartIndex( 0 );
				fileNamesCreator3->SetEndIndex( m_nb_train - 1 );
				fileNamesCreator3->SetSeriesFormat( myo[epi_count]);
				const std::vector<std::string> & shapeModeFileNamesepi = fileNamesCreator3->GetFileNames();

				itk::NumericSeriesFileNames::Pointer fileNamesCreator2 =itk::NumericSeriesFileNames::New();
				fileNamesCreator2->SetStartIndex( 0 );
				fileNamesCreator2->SetEndIndex( m_nb_train - 1 );
				fileNamesCreator2->SetSeriesFormat( roi[k]);
				const std::vector<std::string> & shapeModeFileNamesROI = fileNamesCreator2->GetFileNames();

				std::vector<ImageType::Pointer> myo_training( m_nb_train );
				std::vector<ImageType::Pointer> ROI_training( m_nb_train );

				ReaderType::Pointer reader = ReaderType::New();
				reader->SetFileName( shapeModeFileNamesmyo[k].c_str() );
				reader->Update();
				myo_training[k] = reader->GetOutput();

				ReaderType::Pointer reader2 = ReaderType::New();
				reader2->SetFileName( shapeModeFileNamesROI[k].c_str() );
				reader2->Update();
				ROI_training[k] = reader2->GetOutput();

			    	inputFileName_ROI=shapeModeFileNamesROI[k].c_str(); 
				inputFileName=shapeModeFileNamesepi[k].c_str();
				inputFileName2=shapeModeFileNamesmyo[k].c_str();
				std::cout<<"/////////////////////////////////////////////////////////////////////"<<std::endl;
				std::cout<<" The segmentation of the "<<k+1<<"/"<<m_nb_train<< " files: "<<std::endl;
				std::cout<<" endo file : "<<shapeModeFileNamesmyo[k].c_str() <<std::endl;
				std::cout<<" epi file : "<<shapeModeFileNamesepi[k].c_str() <<std::endl;
				std::cout<<" roi file : "<<shapeModeFileNamesROI[k].c_str() <<std::endl;
				std::cout<<"/////////////////////////////////////////////////////////////////////"<<std::endl;
				string SCARFolder_final=m_SCARFolder+std::to_string(k)+"/";
				//std::cout<<" scar path : "<<SCARFolder_final.c_str() <<std::endl;
				if (mkdir(SCARFolder_final.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" <<SCARFolder_final.c_str()<< std::endl;
				}

				cout << "Normalize image of interest ROI" << std::endl;
				std::shared_ptr<denoising > SM_norm =std::make_shared<denoising>(root);
				SM_norm->Normalize_image(inputFileName_ROI, (root+"normalize_ROI.jpg").c_str(),100,10 );
				//inputFileName_ROI="normalize_ROI.jpg";

				preprocessing();
				if (check_black_image(inputFileName2,500,40)==false && check_black_image(inputFileName,500,40)==false)
				{
						inputFileName=inputFileName_ROI;
						reduce_noise();
						inputFileName=shapeModeFileNamesepi[k].c_str();
						if(SM_norm->Getmean()<=10){
							inputFileName_ROI=(root+"blurring5.jpg").c_str(); 
						}
						if(SM_norm->Getmean()>10){
							inputFileName_ROI=inputFileName_ROI; // set the original
						}

						restimate_myo();
						reduce_noise();
						copy_to_directory(SCARFolder_final.c_str(),"2");
						delete_images_localy(root);

				}
			}// end for

	}




//both segmentation pipelines scar myo
	if(segmentation_part=="both")

	{

		if( pDIR=opendir(m_myoFolder) ){
			while(entry = readdir(pDIR)){
				if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
				std::stringstream m;
				m<<m_myoFolder<<entry->d_name;
				myo.push_back(m.str());
				cout << m.str() << "\n";
				}
				}
			closedir(pDIR);
		}//end if

		if( pDIR=opendir(m_ROIFolder) ){
			while(entry = readdir(pDIR)){
				if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
				std::stringstream r;
				r<<m_ROIFolder<<entry->d_name;
				roi.push_back(r.str());
				cout << r.str() << "\n";
				}
				}
			closedir(pDIR);
		}//end if

		std::sort(myo.begin(),myo.end());
		std::sort(roi.begin(),roi.end());
		std::cout<<" The both myo-scar segmentation of the "<<m_nb_train<< " files start now ..."<<std::endl;
			int mean_group=100;
			int SD_group=10;
			bool norm_patient=false;
			if (norm_patient==true){
				//std::cout << "normalization of : "<< k <<" / "<<m_nb_train<<" patient"<<std::endl;
				std::shared_ptr<denoising > SM_norm =std::make_shared<denoising>(root);
				SM_norm->Normalize_group_images(roi);
				mean_group=SM_norm->Get_mean_group_images();
				SD_group=SM_norm->Get_SD_group_images();
			}


		for ( unsigned int k = 0; k < m_nb_train; k++ )
		{
			int epi_count=k+m_nb_train;
			itk::NumericSeriesFileNames::Pointer fileNamesCreator =itk::NumericSeriesFileNames::New();
			fileNamesCreator->SetStartIndex( 0 );
			fileNamesCreator->SetEndIndex( m_nb_train - 1 );
			fileNamesCreator->SetSeriesFormat( myo[k]);
			const std::vector<std::string> & shapeModeFileNamesmyo = fileNamesCreator->GetFileNames();

			itk::NumericSeriesFileNames::Pointer fileNamesCreator3 =itk::NumericSeriesFileNames::New();
			fileNamesCreator3->SetStartIndex( 0 );
			fileNamesCreator3->SetEndIndex( m_nb_train - 1 );
			fileNamesCreator3->SetSeriesFormat( myo[epi_count]);
			const std::vector<std::string> & shapeModeFileNamesepi = fileNamesCreator3->GetFileNames();

			itk::NumericSeriesFileNames::Pointer fileNamesCreator2 =itk::NumericSeriesFileNames::New();
			fileNamesCreator2->SetStartIndex( 0 );
			fileNamesCreator2->SetEndIndex( m_nb_train - 1 );
			fileNamesCreator2->SetSeriesFormat( roi[k]);
			const std::vector<std::string> & shapeModeFileNamesROI = fileNamesCreator2->GetFileNames();

			std::vector<ImageType::Pointer> myo_training( m_nb_train );
			std::vector<ImageType::Pointer> ROI_training( m_nb_train );

			ReaderType::Pointer reader = ReaderType::New();
			reader->SetFileName( shapeModeFileNamesmyo[k].c_str() );
			reader->Update();
			myo_training[k] = reader->GetOutput();

			ReaderType::Pointer reader2 = ReaderType::New();
			reader2->SetFileName( shapeModeFileNamesROI[k].c_str() );
			reader2->Update();
			ROI_training[k] = reader2->GetOutput();

		    	inputFileName_ROI=shapeModeFileNamesROI[k].c_str(); 
			inputFileName=shapeModeFileNamesepi[k].c_str();
			inputFileName2=shapeModeFileNamesmyo[k].c_str();
			std::cout<<"/////////////////////////////////////////////////////////////////////"<<std::endl;
			std::cout<<" The segmentation of the "<<k+1<<"/"<<m_nb_train<< " files: "<<std::endl;
			std::cout<<" endo file : "<<shapeModeFileNamesmyo[k].c_str() <<std::endl;
			std::cout<<" epi file : "<<shapeModeFileNamesepi[k].c_str() <<std::endl;
			std::cout<<" roi file : "<<shapeModeFileNamesROI[k].c_str() <<std::endl;
			std::cout<<"/////////////////////////////////////////////////////////////////////"<<std::endl;
			string SCARFolder_final=m_SCARFolder+std::to_string(k)+"/";
			//std::cout<<" scar path : "<<SCARFolder_final.c_str() <<std::endl;
			if (mkdir(SCARFolder_final.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" <<SCARFolder_final.c_str()<< std::endl;
			}

			cout << "Normalize image of interest ROI" << std::endl;
	
			std::shared_ptr<denoising > SM_norm =std::make_shared<denoising>(root);
			SM_norm->Normalize_image(inputFileName_ROI, inputFileName_ROI,mean_group,SD_group);
			
//inputFileName_ROI="normalize_ROI.jpg";
			preprocessing();

			if (check_black_image(inputFileName2,500,40)==false && check_black_image(inputFileName,500,40)==false)
			{
				inputFileName=inputFileName_ROI;
				reduce_noise();
				inputFileName=shapeModeFileNamesepi[k].c_str();
				if(SM_norm->Getmean()<=10){
					inputFileName_ROI=(root+"blurring5.jpg").c_str(); 
				}
				if(SM_norm->Getmean()>10){
					inputFileName_ROI=inputFileName_ROI; // set the original
				}
				restimate_myo();
				reduce_noise();
				if (threshold_of_scar_intensity!=0){
					threshold_define();
					segmentation();
					copy_to_directory(SCARFolder_final.c_str(),"all");
				}
				else{
				copy_to_directory(SCARFolder_final.c_str(),"2");
				}

				delete_images_localy(root);

			}//end if

		}// end for
	
	}

}



void Segmentation_Pipeline::preprocessing()
{


//TODO to take both png and jpg images
std::cout<<" ................................"<<std::endl;
std::cout<<" PREPROCESSING..."<<std::endl;
//first reading
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );
  reader->Update();

  ImageType::Pointer image = reader->GetOutput();

  ImageType::RegionType region = image->GetLargestPossibleRegion();

  ImageType::SizeType size = region.GetSize();
std::cout<<"InputImage epi size : "<<size<<std::endl;



  std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
if (higherorequal(size)==false){
  Resample1->resample(inputFileName,512,1);
}

//second reading
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( inputFileName2 );
  reader2->Update();

  ImageType::Pointer image2 = reader2->GetOutput();

  ImageType::RegionType region2 = image2->GetLargestPossibleRegion();

  ImageType::SizeType size2 = region2.GetSize();
std::cout<<"InputImage endo size : "<<size2<<std::endl;



if (higherorequal(size2)==false){
  Resample1->resample(inputFileName2,512,1);
}

//third reading 
ReaderType::Pointer reader3 = ReaderType::New();
reader3->SetFileName( inputFileName_ROI );
reader3->Update();

ImageType::Pointer image3 = reader3->GetOutput();

ImageType::RegionType region3 = image3->GetLargestPossibleRegion();

 size = region3.GetSize();

if (higherorequal(size)==false){
  Resample1->resample(inputFileName_ROI,512,1);
}




std::cout<<"Binary threshold of the initials image.."<<std::endl;
	std::shared_ptr<BinaryImage> BqI1 =std::make_shared<BinaryImage>();
	std::shared_ptr<kmeans> kmean =std::make_shared<kmeans>();
	double means[2];
	means[0]=0;
	means[1]=50;

	kmean->Image_kmeans( inputFileName, (root+"kmeans_epi_image.jpg").c_str(), 2,2, means);
	using MeasurementVectorType1 = itk::Vector< float, 1 >;
	using SampleType1 = itk::Statistics::ListSample< MeasurementVectorType1 >;
	SampleType1::Pointer sample = SampleType1::New();
	SampleType1::Pointer sample_endo = SampleType1::New();
	sample=kmean->Getmeans();
	int offset=10;

	BqI1->Image_binary((root+"kmeans_epi_image.jpg").c_str(),(root+"binary_input_image_epi.jpg").c_str(), sample->GetMeasurementVector(0)[0]+offset ,255);
	std::shared_ptr<BinaryImage> BqI2 =std::make_shared<BinaryImage>();
	std::shared_ptr<kmeans> kmean_endo =std::make_shared<kmeans>();
	kmean_endo->Image_kmeans( inputFileName2, (root+"kmeans_endo_image.jpg").c_str(), 2,2, means);
	sample_endo=kmean_endo->Getmeans();
	BqI2->Image_binary((root+"kmeans_endo_image.jpg").c_str(),(root+"binary_input_image_endo.jpg").c_str(), sample_endo->GetMeasurementVector(0)[0]+offset,255);	
	for ( unsigned int i = 0 ; i < 10 ; ++i ){
		std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
		SM1->Median_smoothing((root+"binary_input_image_epi.jpg").c_str(),(root+"binary_input_image_epi.jpg").c_str(),1,1);
		std::shared_ptr<denoising> SM2 =std::make_shared<denoising>(root);
		SM2->Median_smoothing((root+"binary_input_image_endo.jpg").c_str(),(root+"binary_input_image_endo.jpg").c_str(),1,1);
	
		std::shared_ptr<BinaryImage> Bq =std::make_shared<BinaryImage>();
		Bq->Image_binary((root+"binary_input_image_epi.jpg").c_str(),(root+"binary_input_image_epi.jpg").c_str(), PRE_PROCESSING_BINARY_THRESHOLD,255);
		Bq->Image_binary((root+"binary_input_image_endo.jpg").c_str(),(root+"binary_input_image_endo.jpg").c_str(), PRE_PROCESSING_BINARY_THRESHOLD,255);



	}

}





void Segmentation_Pipeline::restimate_myo()
{


BinaryShape::Pointer binaryshape = BinaryShape::New();
ReaderType::Pointer reader = ReaderType::New();
WriterType::Pointer writer = WriterType::New();
float TH= PORTION_OF_SCARS_SEGMENT_SEEDPOINTS;
std::cout<<" ................................"<<std::endl;
std::cout<<" RESTIMATE MYO..."<<std::endl;
if (myo_new==true){

	std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
	std::shared_ptr<seedpoints> SP_start =std::make_shared<seedpoints>(inputFileName_ROI, (root+"binary_input_image_endo.jpg"),(root+"binary_input_image_epi.jpg"), (root+"seedpoints.jpg"), TH,TR,root);
	SP_start->border_zone_ellimination=border_zone_value;

	SP_start->SetSensitivity1_param( sensitivity1_param, kmean);
	SP_start->SetSensitivity2_param( sensitivity2_param, endo);
	SP_start->SetSensitivity3_param( sensitivity3_param, epi);

	SP_start->Initiallize();
	std::shared_ptr<BinaryImage> BqI232 =std::make_shared<BinaryImage>();
	BqI232->Image_binary((root+"binary_epi_endo.jpg").c_str(),(root+"binary_input_image.jpg").c_str(), 245,255);	
	std::shared_ptr<mergeimages> MI_add_image =std::make_shared<mergeimages>(root);

	for ( unsigned int i = 0 ; i < 40 ; ++i ){
			SM1->Median_smoothing((root+"binary_input_image.jpg").c_str(),(root+"binary_input_image.jpg").c_str(),1,1);
			std::shared_ptr<BinaryImage> Bq =std::make_shared<BinaryImage>();
			Bq->Image_binary((root+"binary_input_image.jpg").c_str(),(root+"binary_input_image.jpg").c_str(), 245,255);

	}

	std::shared_ptr<rebuild_myo_mask> RMM =std::make_shared<rebuild_myo_mask>(root);
	RMM->Run((root+"binary_input_image.jpg").c_str());
	// Combine watersheed and p(x) images
	std::shared_ptr<ConnectCombonentFilter> CCF =std::make_shared<ConnectCombonentFilter>();
	std::shared_ptr<BinaryImage> BI2 =std::make_shared<BinaryImage>();
	std::shared_ptr<mergeimages> MI_crop_images =std::make_shared<mergeimages>(root);
	MI_crop_images->Image_merge( (root+"crop_ROI_epi.jpg").c_str(),(root+"binary_endo.jpg").c_str() ,(root+"crop_ROI_epi.jpg").c_str());
	//CCF->Image_CCF("crop_ROI_epi.jpg","comb_myo2.jpg","final_myo_mask.jpg");
	CCF->Image_CCF((root+"healthymyo_binary.jpg").c_str(),(root+"comb_myo2.jpg").c_str(),(root+"final_myo_mask.jpg").c_str());
	BI2->Image_binary((root+"comb_myo2.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(), 1,255);//1
	for ( unsigned int i = 0 ; i < 40 ; ++i ){
			SM1->Median_smoothing((root+"comb_myo_b.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(),1,1);
			std::shared_ptr<BinaryImage> Bq =std::make_shared<BinaryImage>();
			BI2->Image_binary((root+"comb_myo_b.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(), 5,255);//5

	}
	bool scar_on=SP_start->Get_scar_exist();
	if (scar_on==true){MI_crop_images->Image_add( (root+"comb_myo_b.jpg").c_str(),(root+"binary_initial_scars.jpg").c_str() ,(root+"comb_myo_b.jpg").c_str());}
	//CCF->Image_CCF("crop_ROI_epi.jpg","comb_myo.jpg","comb_myo_b.jpg");
	BI2->Image_binary((root+"comb_myo_b.jpg").c_str(),(root+"comb_myo.jpg").c_str(), 240,255);
	int times_run=0;
	
	//crop the paapilaries if excist
	
	if (loop_rebuild!=0 || (SP_start->Get_times_loop())==true){
		if ((SP_start->Get_times_loop())==true){times_run=loop_rebuild+5;}
		for (int passing_times=0; passing_times<times_run+1; ++passing_times){

		std::shared_ptr<rebuild_myo_mask> RMM =std::make_shared<rebuild_myo_mask>(root);
		RMM->Run((root+"comb_myo_b.jpg").c_str());
		// Combine watersheed and p(x) images
		std::shared_ptr<ConnectCombonentFilter> CCF =std::make_shared<ConnectCombonentFilter>();
		std::shared_ptr<BinaryImage> BI2 =std::make_shared<BinaryImage>();
		std::shared_ptr<mergeimages> MI_crop_images2 =std::make_shared<mergeimages>(root);
		MI_crop_images2->Image_merge( (root+"crop_ROI_epi.jpg").c_str(),(root+"binary_endo.jpg").c_str() ,(root+"crop_ROI_epi.jpg").c_str());
		//CCF->Image_CCF("final_myo_mask.jpg","comb_myo.jpg","crop_ROI_epi.jpg");
		CCF->Image_CCF((root+"healthymyo_binary.jpg").c_str(),(root+"comb_myo2.jpg").c_str(),(root+"final_myo_mask.jpg").c_str());
		BI2->Image_binary((root+"comb_myo2.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(), 1,255);//1
		for ( unsigned int i = 0 ; i < 40 ; ++i ){
				SM1->Median_smoothing((root+"comb_myo_b.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(),1,1);
				std::shared_ptr<BinaryImage> Bq =std::make_shared<BinaryImage>();
				BI2->Image_binary((root+"comb_myo_b.jpg").c_str(),(root+"comb_myo_b.jpg").c_str(), 5,255);//5

		}
		if (scar_on==true){MI_crop_images2->Image_add( (root+"comb_myo_b.jpg").c_str(),(root+"binary_initial_scars.jpg").c_str() ,(root+"comb_myo_b.jpg").c_str());}
		//CCF->Image_CCF("crop_ROI_epi.jpg","comb_myo.jpg","comb_myo_b.jpg");
		BI2->Image_binary((root+"comb_myo_b.jpg").c_str(),(root+"comb_myo.jpg").c_str(), 245,255);
		}

	}

	
		for ( unsigned int i = 0 ; i < 20 ; ++i ){
			SM1->Median_smoothing((root+"comb_myo.jpg").c_str(),(root+"comb_myo.jpg").c_str(),1,1);
			BI2->Image_binary((root+"comb_myo.jpg").c_str(),(root+"comb_myo.jpg").c_str(), 245,255); //10

		}
		MI_add_image->Image_add((root+"papilaris.jpg").c_str(), (root+"comb_myo.jpg").c_str(), (root+"comb_myo_papilaries.jpg").c_str());

	for ( unsigned int i = 0 ; i < 10 ; ++i ){
		SM1->Median_smoothing((root+"comb_myo_papilaries.jpg").c_str(),(root+"comb_myo_papilaries.jpg").c_str(),1,1);
		BI2->Image_binary((root+"comb_myo_papilaries.jpg").c_str(),(root+"comb_myo_papilaries.jpg").c_str(), 245,255); //40
	}
		SP_start->preprocessing3((root+"comb_myo_papilaries.jpg").c_str());

		//std::shared_ptr<remove_image_object> rbio =std::make_shared<remove_image_object>(root);
		//rbio->remove_object((root+"comb_myo_papilaries.jpg").c_str() , (root+"comb_myo_papilaries.jpg").c_str() , 1, "None");

		//std::shared_ptr<remove_image_object> rbio2 =std::make_shared<remove_image_object>(root);
		//rbio2->remove_object((root+"comb_myo_b.jpg").c_str() , (root+"b.jpg").c_str() , 1, "None");

		std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
		MI->Crop_binary((root+"ROI_align.jpg").c_str(),(root+"comb_myo_papilaries.jpg").c_str(),(root+"myo.jpg").c_str());



		for ( unsigned int i = 0 ; i < 50 ; ++i ){
			SM1->Median_smoothing((root+"myo.jpg").c_str(),(root+"myo.jpg").c_str(),1,1);
			//std::shared_ptr<remove_image_object> rbio =std::make_shared<remove_image_object>();
			//SP_start->remove_object("myo.jpg", "myo.jpg",10000000,"NumberOfPixels");

		}

	scar_rebuild_phase=SP_start->GetScar_kmean();

}

binary_myo_mask=(root+"comb_myo_papilaries.jpg");
inputFileName=(root+"myo.jpg").c_str();
binaryepi_endo_mask=(root+"binary_input_image.jpg");

////TEST RG histogramm////
//std::shared_ptr<RGhistogram_test > RGtest =std::make_shared<RGhistogram_test>(inputFileName,bins);

}


void Segmentation_Pipeline::reduce_noise()
{

std::cout<<" ................................"<<std::endl;
std::cout<<" DENOISE NEW MYO..."<<std::endl;

//TODO issue with wiener ti
double SNR_new;
double SNR_new1;
double SNR_new2;
double SNR_new3;
double SNR_new4;
double SNR_new5;

std::shared_ptr<denoising > SM_image =std::make_shared<denoising>(root);
std::shared_ptr<denoising > SM1 =std::make_shared<denoising>(root);
SM_image->Curv_anisotropicDiffusionfilter(inputFileName,(root+"smoothing.jpg").c_str(),time, conductance_parameter);
double SNR=SM_image->Compute_PSNR((root+"smoothing.jpg").c_str(),inputFileName);
SNR_new=SNR;




//Blurring
////////////////////////////////////////////////////////////////////////
std::shared_ptr<denoising > SM_image1 =std::make_shared<denoising>(root);
SM_image1->Initialize();
std::cout<<" ................................"<<std::endl;
SM_image1->Landweber_deconvolution(inputFileName, (root+"blurring3.jpg").c_str(), "10" );
SNR_new3=SM_image1->Compute_PSNR((root+"blurring3.jpg").c_str(),inputFileName);

//SM_image1->~denoising();
///////////////////////////////////////////////////////////////////////////////////

////////////////////////// Poison NOISE ////////////////////////////////
//std::shared_ptr<denoising > SM_image2 =std::make_shared<denoising>();
//SM_image2->SetShot_noise(true);
//SM_image2->SetScale_noise(0.5);
//SM_image2->Initialize();
//SM_image->SetnoiseKerner_image("smoothing.jpg");
//SM_image2->Richardson_Lucy_deconvolution("smoothing.jpg", "blurring4.jpg",  "50" );
//SNR_new4=SM_image2->Compute_PSNR("blurring4.jpg","smoothing.jpg");
//SM_image2->~denoising();
///////////////////////////////////////////////////////////////////////////////////

/////////////////////////Gaussian NOISE//////////////////////////////////
//bool value=true;
//SM_image->SetGauss_noise(value);
//SM_image->SetMean_noise(100);
//SM_image->SetSTD_noise(10);
//SM_image->SetSaltPeper_noise(value);
//SM_image->SetSpeckle_noise(true);inputFileName2
//SM_image->Initialize();
//SM_image->SetnoiseKerner_image("smoothing.jpg");
//SM_image->Wiener_deconvolution("smoothing.jpg", "blurring1.jpg", 10 );
//SNR_new1=SM_image->Compute_PSNR("blurring1.jpg","smoothing.jpg");
//SM_image->Tikhonov_deconvolution("smoothing.jpg", "blurring2.jpg",  0.01 );
//SNR_new2=SM_image->Compute_PSNR("blurring2.jpg","smoothing.jpg");
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//SNR_new4=SM_image->Compute_PSNR("blurring4.jpg","smoothing.jpg");
SM_image->Binomial_deconvolution(inputFileName, (root+"blurring5.jpg").c_str(),  "10" );
SNR_new5=SM_image->Compute_PSNR((root+"blurring5.jpg").c_str(),inputFileName);

////////////////////////////////////////////////////////////////////////

std::cout<<" /////////////////////////////////////////////////////////////////////////////////////////////"<<std::endl;
std::cout<<"                                       PSNR RESULTS                          "<<std::endl;
std::cout<<"                                                                "<<std::endl;
std::cout<<" TYPEFILTER                            ORIGINAL PSNR                FILTER PSNR     "<<std::endl;

std::cout<<" Curv anisotropic Diffusion:           "<<SNR<<"		              "<< SNR_new                                   << std::endl;

//std::cout<<" Wiener_deconvolution:                 "<<SNR<<"		              "<< SNR_new1                                   <<   std::endl;

//std::cout<<" Tikhonov_deconvolution:               "<<SNR<<"		              "<< SNR_new2                                    <<  std::endl;

std::cout<<" Landweber_deconvolution:              "<<SNR<<"		              "<< SNR_new3                                    << std::endl;

//std::cout<<" Richardson_Lucy_deconvolution:        "<<SNR<<"		              "<< SNR_new4                                    << std::endl;

std::cout<<" Binomial_deconvolution:               "<<SNR<<"		              "<< SNR_new5                                     << std::endl;


std::cout<<" //////////////////////////////////////////////////////////////////////////////////////////////"<<std::endl;
std::string image_detect_draft=(root+"smoothing.jpg");


std::vector<double> SNR_values = {SNR, SNR_new3, SNR_new5}; 
auto minmax = std::minmax_element(SNR_values.begin(), SNR_values.end());
std::vector<double> SNR_value=SNR_values;
SNR_value.erase(std::remove(SNR_value.begin(), SNR_value.end(), *minmax.first), SNR_value.end());
SNR_value.erase(std::remove(SNR_value.begin(), SNR_value.end(), *minmax.second), SNR_value.end());


if (*minmax.second<=45 && *minmax.second >=25) 
{	if (*minmax.second==SNR){
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image((root+"smoothing.jpg").c_str());
		image_detect_draft=(root+"smoothing.jpg");
	}
	if (*minmax.second==SNR_new3){
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image((root+"blurring3.jpg").c_str());
		image_detect_draft=(root+"blurring3.jpg");
	}
	if (*minmax.second==SNR_new5){
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image((root+"blurring5.jpg").c_str());
		image_detect_draft=(root+"blurring5.jpg");
	}



}
else if (*minmax.first<=45 && *minmax.first >=25) 
{	if (*minmax.first==SNR){
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image((root+"smoothing.jpg").c_str());
		image_detect_draft=(root+"smoothing.jpg");
			}
	if (*minmax.first==SNR_new3){
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image((root+"blurring3.jpg").c_str());
		image_detect_draft=(root+"blurring3.jpg");
	}
	if (*minmax.first==SNR_new5){
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image((root+"blurring5.jpg").c_str());
		image_detect_draft=(root+"blurring5.jpg");
	}


}



else if (SNR_value[0]<=45 && SNR_value[0] >=25) 
{	if (SNR_value[0]==SNR){
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image((root+"smoothing.jpg").c_str());
		image_detect_draft=(root+"smoothing.jpg");
	}
	if (SNR_value[0]==SNR_new3){
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image((root+"blurring3.jpg").c_str());
		image_detect_draft=(root+"blurring3.jpg").c_str();
	}
	if (SNR_value[0]==SNR_new5){
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image((root+"blurring5.jpg").c_str());
		image_detect_draft=(root+"blurring5.jpg");
		
	}


}

else  
{	
		SM1->SetScale_noise(1);
		SM1->SetShot_noise(true);
		SM1->SetnoiseKerner_image(inputFileName);
		image_detect_draft=inputFileName;

}


/*
//IV->vision("blurring1.jpg", "Initial denoising Wiener_deconvolution");
//IV->vision("blurring2.jpg", "Initial denoising Tikhonov_deconvolution");
IV->vision("blurring3.jpg", "Initial denoising Landweber_deconvolution");
//IV->vision("blurring4.jpg", "Initial Richardson_Lucy_deconvolution");
IV->vision("blurring5.jpg", "Binomial_deconvolution");

//IH1->ImageHistogram("blurring1.jpg",bins ,"b");
//IH1->ImageHistogram("blurring2.jpg",bins ,"b");
IH1->ImageHistogram("blurring3.jpg",bins ,"b");
//IH1->ImageHistogram("blurring4.jpg",bins ,"b");
IH1->ImageHistogram("blurring5.jpg",bins ,"b");

*/


image_detect=image_detect_draft;

}






void Segmentation_Pipeline::threshold_define()

{
std::cout<<" ................................"<<std::endl;
std::cout<<" DETECT SCAR THRESHOLD..."<<std::endl;
 int activeDimension = 0;
SampleType::Pointer sample_TH = SampleType::New();
int number_of_models=0;

try
    {
std::shared_ptr<HistogramGaussianRician > HGR =std::make_shared<HistogramGaussianRician>(image_detect.c_str(),bins);
    HGR->detect_max=false;
    HGR->RRGG=true;
    HGR->Update();
meanTH mv;

mv=HGR->GetmeanR();
sample_TH->PushBack(mv);
mv=HGR->GetmeanR2();
sample_TH->PushBack(mv);
mv=HGR->GetmeanG();
sample_TH->PushBack(mv);
mv=HGR->GetmeanG2();
sample_TH->PushBack(mv);
++number_of_models;

    }
  catch( itk::ExceptionObject & excp ){    std::cerr << excp << std::endl;}

try{
std::shared_ptr<HistogramGaussianRician > HGR1 =std::make_shared<HistogramGaussianRician>(image_detect.c_str(),bins);

     HGR1->detect_max=true;
    HGR1->RRGG=true;
    HGR1->Update();
meanTH mv1;

mv1=HGR1->GetmeanR();
sample_TH->PushBack(mv1);
mv1=HGR1->GetmeanR2();
sample_TH->PushBack(mv1);
mv1=HGR1->GetmeanG();
sample_TH->PushBack(mv1);
mv1=HGR1->GetmeanG2();
sample_TH->PushBack(mv1);
++number_of_models;


    }
  catch( itk::ExceptionObject & excp1 ){    std::cerr << excp1 << std::endl;}
     try{
std::shared_ptr<HistogramGaussianRician > HGR2 =std::make_shared<HistogramGaussianRician>(image_detect.c_str(),bins);
     HGR2->detect_max=true;
    HGR2->RRG=true;
    HGR2->Update();
meanTH mv2;

mv2=HGR2->GetmeanR();
sample_TH->PushBack(mv2);
mv2=HGR2->GetmeanR2();
sample_TH->PushBack(mv2);
mv2=HGR2->GetmeanG();
sample_TH->PushBack(mv2);

++number_of_models;


    }
  catch( itk::ExceptionObject & excp2 ){    std::cerr << excp2 << std::endl;}
    try{
std::shared_ptr<HistogramGaussianRician > HGR3 =std::make_shared<HistogramGaussianRician>(image_detect.c_str(),bins);
     HGR3->detect_max=false;
    HGR3->RRG=true;
    HGR3->Update();
meanTH mv3;

mv3=HGR3->GetmeanR();
sample_TH->PushBack(mv3);
mv3=HGR3->GetmeanR2();
sample_TH->PushBack(mv3);
mv3=HGR3->GetmeanG();
sample_TH->PushBack(mv3);
++number_of_models;


    }
  catch( itk::ExceptionObject & excp3 ){    std::cerr << excp3 << std::endl;}
    try{
std::shared_ptr<HistogramGaussianRician > HGR4 =std::make_shared<HistogramGaussianRician>(image_detect.c_str(),bins);
     HGR4->detect_max=true;
    HGR4->RGG=true;
    HGR4->Update();

meanTH mv4;

mv4=HGR4->GetmeanR();
sample_TH->PushBack(mv4);
mv4=HGR4->GetmeanG2();
sample_TH->PushBack(mv4);
mv4=HGR4->GetmeanG();
sample_TH->PushBack(mv4);
++number_of_models;

    }
  catch( itk::ExceptionObject & excp4 ){    std::cerr << excp4 << std::endl;}
    try{
std::shared_ptr<HistogramGaussianRician > HGR5 =std::make_shared<HistogramGaussianRician>(image_detect.c_str(),bins);
     HGR5->detect_max=false;
    HGR5->RGG=true;
    HGR5->Update();
meanTH mv5;
mv5=HGR5->GetmeanR();
sample_TH->PushBack(mv5);
mv5=HGR5->GetmeanG2();
sample_TH->PushBack(mv5);
mv5=HGR5->GetmeanG();
sample_TH->PushBack(mv5);
++number_of_models;

    }
  catch( itk::ExceptionObject & excp5 ){    std::cerr << excp5 << std::endl;}
    try{
std::shared_ptr<HistogramGaussianRician > HGR6 =std::make_shared<HistogramGaussianRician>(image_detect.c_str(),bins);
     HGR6->detect_max=true;
    HGR6->RG=true;
    HGR6->Update();
meanTH mv6;
mv6=HGR6->GetmeanR();
sample_TH->PushBack(mv6);
mv6=HGR6->GetmeanG();
sample_TH->PushBack(mv6);
++number_of_models;


    }
  catch( itk::ExceptionObject & excp6 ){    std::cerr << excp6 << std::endl;}


 try   {
std::shared_ptr<HistogramGaussianRician > HGR7 =std::make_shared<HistogramGaussianRician>(image_detect.c_str(),bins);
   
     HGR7->detect_max=false;
    HGR7->RG=true;
    HGR7->Update();

meanTH mv7;
mv7=0;
mv7=HGR7->GetmeanR();
sample_TH->PushBack(mv7);
mv7=HGR7->GetmeanG();
sample_TH->PushBack(mv7);
++number_of_models;


    }
  catch( itk::ExceptionObject & excp7 ){    std::cerr << excp7 << std::endl;
}

//}}}}}}}}

if (sample_TH->Size() !=0){
	SubsampleType::Pointer subsample_TH = SubsampleType::New();
	subsample_TH->SetSample(sample_TH);
	subsample_TH->InitializeWithAllInstances();



	itk::Statistics::Algorithm::HeapSort < SubsampleType >( subsample_TH, activeDimension, 0, subsample_TH->Size() );

	TR=subsample_TH->GetMeasurementVector(0);
	TG=subsample_TH->GetMeasurementVector(subsample_TH->Size()-number_of_models);//min gaussian
	//TG=subsample_TH->GetMeasurementVector(subsample_TH->Size()-1);//max gaussian case for FWHM

	std::cout<<" Final Risian Threshold "<< TR <<std::endl;
	std::cout<<"Final Gaussian Threshold "<< TG <<std::endl;

	if(TG[0]<threshold_of_scar_intensity){TR=254; TG=255;}; //no scar
}
if (sample_TH->Size() ==0){TR=254; TG=255;};

}



void Segmentation_Pipeline::segmentation()
{

float rgmm_thres=FWHM_threshold_low;
float rgmm_kmean_thres=combine_FWHM_threshold_low;
if (sensitivity4_param==true){ rgmm_thres=rgmm;}
if (sensitivity5_param==true){ rgmm_kmean_thres=rgmm_kmean;}

//std::shared_ptr<interparameters> inter_param =std::make_shared<interparameters>();

std::cout<<" ................................"<<std::endl;
std::cout<<" SEGMENT SCARS..."<<std::endl;
std::shared_ptr<denoising> SMcopy =std::make_shared<denoising>(root);
SMcopy->Median_smoothing(image_detect.c_str(),(root+"myo_mask.jpg").c_str(),1,1);
if (check_black_image(image_detect.c_str(),10,40)==true){std::cout<<" No SCAR tissue..."<<std::endl;}

else{
	int RGMM_fwhm=(TG[0]-TR[0])*rgmm_thres + TR[0];
	//TG[0]=RGMM_fwhm;
	// create linear partial volume model
	std::shared_ptr<estportion> LPVM =std::make_shared<estportion>();
	LPVM->estportion_compute( (root+"myo_mask.jpg").c_str(),(root+"portion_image.jpg").c_str(),TG,TR);
	float TH= PORTION_OF_SCARS_SEGMENT_SEEDPOINTS;
	///Compute the seedpoints
	// take the FWHM of kmean and mixture model 
	//int RGMM_fwhm=(TG[0]-TR[0])*rgmm_thres + TR[0];
	int threshold_portion;
	if (segmentation_part=="scar" || segmentation_part=="scar_bz"){threshold_portion=RGMM_fwhm;}
	else { threshold_portion=((std::abs(scar_rebuild_phase-RGMM_fwhm))*rgmm_kmean_thres) +std::min(scar_rebuild_phase,RGMM_fwhm);}

	std::cout<<" RGG model scar estimation: "<<RGMM_fwhm<<std::endl;
	std::cout<<" K-means myo rebuild phase scar esstimation: "<<scar_rebuild_phase<<std::endl;
	std::cout<<" Bzone threshold: "<<threshold_portion<<std::endl;
	std::cout<<" core scar threshold : "<<(threshold_portion+255)/0.5<<std::endl;

	float fwhm_ratio=0.75;
	int base=TR[0];
	int core_zone=((255-base)*fwhm_ratio)+base+5; //again all as this was / instend of *
	int B_zone=threshold_portion;

	std::shared_ptr<seedpoints> SP =std::make_shared<seedpoints>(inputFileName_ROI, binaryepi_endo_mask.c_str(),(root+"myo_mask.jpg").c_str(), (root+"seedpoints.jpg"), TH,threshold_portion,root);

	//std::shared_ptr<seedpoints> SP =std::make_shared<seedpoints>(inputFileName_ROI, "binary_input_image.jpg","portion_image.jpg", "seedpoints.jpg", TH,(TG[0]+TR[0])*0.5); //FWHM case better with max intensity
	SP->Update();

	//Watershed
	std::shared_ptr<Watershed> SEG =std::make_shared<Watershed>();
	SEG->SeedWatershedFilterType((root+"myo_mask.jpg").c_str(),(root+"seedpoints.jpg").c_str(),(root+"scars_with_seeds.jpg").c_str());
	std::shared_ptr<BinaryImage> BI2 =std::make_shared<BinaryImage>();
std::shared_ptr<BinaryImage> BI2d =std::make_shared<BinaryImage>();
	// Combine watersheed and p(x) images
	std::shared_ptr<ConnectCombonentFilter> CCF =std::make_shared<ConnectCombonentFilter>();
	std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);


	if (segmentation_part=="scar_bz"){BI2->Image_binary((root+"portion_image.jpg").c_str(),(root+"binary_portion_image.jpg").c_str(), core_zone,255);
	BI2d->Image_binary((root+"portion_image.jpg").c_str(),(root+"binary_bz.jpg").c_str(), 10,core_zone-10);}
	else{BI2->Image_binary((root+"portion_image.jpg").c_str(),(root+"binary_portion_image.jpg").c_str(), 10,255); 
        BI2d->Image_binary((root+"portion_image.jpg").c_str(),(root+"binary_bz.jpg").c_str(), 0,20);}

	BI2->Image_binary((root+"scars_with_seeds.jpg").c_str(),(root+"scars_with_seeds.jpg").c_str(), 20,254);//40
	CCF->Image_CCF((root+"scars_with_seeds.jpg").c_str(),(root+"combine_image.jpg").c_str(),(root+"binary_portion_image.jpg").c_str());
	//CCF->Image_CCF((root+"scars_with_seeds.jpg").c_str(),(root+"combine_image.jpg").c_str(),(root+"portion_image.jpg").c_str());

	// Binary the scars
	BI2->Image_binary((root+"combine_image.jpg").c_str(),(root+"binary_combine_image.jpg").c_str(), 10,255);
	std::shared_ptr<denoising> SM =std::make_shared<denoising>(root);
	for ( unsigned int i = 0 ; i < 2 ; ++i ){//10
	SM->Median_smoothing((root+"binary_combine_image.jpg").c_str(),(root+"binary_combine_image.jpg").c_str(),1,1);
	std::shared_ptr<BinaryImage> BI23 =std::make_shared<BinaryImage>();
	BI23->Image_binary((root+"binary_combine_image.jpg").c_str(),(root+"binary_combine_image.jpg").c_str(), 10,255);
	}
	//align labels of segmentation

	using MeasurementType2 = double;
	using MeasurementVectorType2 = itk::Vector< MeasurementType2, 2 >;
	std::shared_ptr<boundarysegm> BS =std::make_shared<boundarysegm>(root);
	BS->determine_mean_shape((root+"epi_segm.jpg").c_str(), (root+"endo_segm.jpg").c_str(),"none");
	MeasurementVectorType2 mean_center;
	mean_center=BS-> Getmean();
	double center[2];
	center[0]=mean_center[0];
	center[1]=mean_center[1];
	align_images( (root+"comb_myo_b.jpg").c_str(),center,0,1);
	align_images( binary_myo_mask.c_str(),center,0,1);
	align_images( (root+"binary_combine_image.jpg").c_str(),center,0,1);
	align_images( (root+"binary_bz.jpg").c_str(),center,0,1);
	align_images( (root+"epi_segm.jpg").c_str(),center,0,1);
	align_images( (root+"endo_segm.jpg").c_str(),center,0,1);
}

// set the origin of image in mean of endo..


}


void Segmentation_Pipeline::align_images(const char * m_predictFileName, double mean_endo_center[2], double angle, double scale)
{
if (mean_endo_center[0]==0) {mean_endo_center[0]=0;}
if (mean_endo_center[1]==0) {mean_endo_center[1]=0;}
if (angle==0) {angle=0;}
if (scale==0){ scale=1;}

using PixelType = unsigned char;
constexpr unsigned int Dimension = 2;
using ImageType = itk::Image< PixelType,  Dimension >;
using WriterType = itk::ImageFileWriter< ImageType >;
using FilterType = itk::ChangeInformationImageFilter< ImageType >;
using ReaderType = itk::ImageFileReader< ImageType >;

ReaderType::Pointer reader = ReaderType::New();
WriterType::Pointer writer = WriterType::New();
FilterType::Pointer filter = FilterType::New();

reader->SetFileName( m_predictFileName);
reader->Update();

ImageType::ConstPointer inputImage = reader->GetOutput();
ImageType::PointType     origin    = inputImage->GetOrigin();
ImageType::SpacingType   spacing   = inputImage->GetSpacing();
ImageType::DirectionType direction = inputImage->GetDirection();
ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

ImageType::PointType::VectorType translation;
translation[0] = mean_endo_center[0];
translation[1] = mean_endo_center[1];


vector<int> center_new;

//center_new.push_back(255-translation[0]);
//center_new.push_back(255-translation[1]);
center_new.push_back(origin[0]+translation[0]);
center_new.push_back(origin[1]+translation[1]);

//ImageType::PointType   originew;
// originew[0]=(int)translation[0];
// originew[1]=(int)translation[1];

std::cout<<" align image  " <<std::endl;
std::cout<<"translation in X " <<translation[0]<<std::endl;
std::cout<<"translation in Y " <<translation[1]<<std::endl;
//std::cout<<"old origin " <<origin[0]<<", "<<origin[1]<<std::endl;
//std::cout<<"new origin " <<originew[0]<<", "<<originew[1]<<std::endl;
//std::cout<<"origin in X " <<origin[0]<<std::endl;
//std::cout<<"origin in Y " <<origin[1]<<std::endl;


//filter->SetOutputOrigin( origin );
//filter->ChangeOriginOn();
//filter->SetOutputSpacing( spacing );
//filter->ChangeSpacingOn();
//writer->SetFileName( m_predictFileName );
//filter->SetInput(reader->GetOutput() );
//writer->SetInput( filter->GetOutput() );
//writer->Update();

std::shared_ptr<Change_information> CI =std::make_shared<Change_information>();
CI ->Change_information_2D_similarity_transform(m_predictFileName, center_new, scale, angle );



}




void Segmentation_Pipeline::copy_to_directory( const char * SCARFolder, std::string case_copy)
{
std::cout<<"Copy images to folder " <<SCARFolder<<std::endl;
if (case_copy=="all"){
	std::string scars;
		std::string command2 = "cp -r";
		command2 +=" ";
		command2 +=(root+"*.jpg ");
		command2 += SCARFolder;
		int f= system(command2.c_str());
	}

if (case_copy=="2"){
	//std::string scars;
		std::string command21 = "cp -r";
		command21 +=" ";
		command21 +=(root+"comb_myo_b.jpg ");
		command21 += SCARFolder;
		int f1= system(command21.c_str());

	//std::string scars;
		//std::string command22 = "cp ";
		//command22 +=
		//command22 +="comb_myo_papilaries.jpg ";
		//command22 += SCARFolder;
		//int f2= system(command22.c_str());

	}


}




void Segmentation_Pipeline::delete_images_localy(std::string path)
{

std::cout<<"Delete images locally... "<<std::endl;

	std::string command2 = "rm -rf ";
	command2 +=
	command2 +=(path+"*.jpg"); //befor *.jpeg
	int f= system(command2.c_str());

std::cout<<" END..."<<std::endl;
std::cout<<" ................................"<<std::endl;
}


void Segmentation_Pipeline::dcm_converter(std::string mask,std::string ROIName){

	std::string  path_of_atlas;
	path_of_atlas="../Initial_Segmentation/multi_atlas/CIMAS/";
	if (mask_input=="dcm")
	{
			std::string command = path_of_atlas;
			command +="dcm2niix";
			command += " ";
			command += "-o ";
			command += mask;
			command += " ";
			command += "-f ";
			command += "mask_mri";
			command += " ";
			command += "-z y ";
			command += mask;		
			int f= system(command.c_str());
	}
	std::string command1 = path_of_atlas;
			command1 +="dcm2niix";
			command1 += " ";
			command1 += "-o ";
			command1 += ROIName;
			command1 += " ";
			command1 += "-f ";
			command1 += "roi_mri";
			command1 += " ";
			command1 += "-z y ";
			command1 += ROIName;		
			int f1= system(command1.c_str());

	std::string store_masks=mask+"store_myo/";
	std::string store_roi=ROIName+"store_roi/";
	std::string store_myo_masks= ROIName+"store_mask_myo/";
	if (mask_input=="dcm"){
		if (mkdir(store_masks.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
					cout << "Directory created" << std::endl;
		}
	}
	if (mkdir(store_roi.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
				cout << "Directory created" << std::endl;
	}
	if (mkdir(store_myo_masks.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
				cout << "Directory created" << std::endl;
	}
	std::string path_store_mask=mask;
	std::shared_ptr< unbiased_ground_trouth >UGT=std::make_shared< unbiased_ground_trouth >(root);
	if (mask_input=="dcm"){UGT->slice2dimages(store_masks, mask+"mask_mri.nii.gz"); path_store_mask=store_masks;}
	UGT->slice2dimages(store_roi, ROIName+"roi_mri.nii.gz");


}


void Segmentation_Pipeline:: match_myo_ROI_mask(std::string mask,std::string ROIName, int mean_group,int SD_group){
	std::string store_masks=mask;//+"store_myo/";
	std::string store_roi=ROIName;//+"store_roi/";
	std::string store_myo_masks= ROIName+"store_mask_myo/";
	std::string path_store_mask=mask;
	std::shared_ptr< unbiased_ground_trouth >UGT=std::make_shared< unbiased_ground_trouth >(root);

	if (mask_input=="dcm"){
		//if (mkdir(store_masks.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		//			cerr << "Error :  " << strerror(errno) << endl; 
		//}
		//else{
					cout << "Directory created" << std::endl;
		//}
 	path_store_mask=store_masks;
	}

	if (mkdir(store_myo_masks.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
				cout << "Directory created" << std::endl;
	}
/*
	if (mkdir(store_myo_masks.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
				cout << "Directory created" << std::endl;
	}
*/
	std::shared_ptr< unbiased_ground_trouth >UGT1=std::make_shared< unbiased_ground_trouth >(root);
	std:: vector<string> mask_images =UGT1->read_images_folder(path_store_mask,0,false);
	std:: vector<string> roi_images =UGT1->read_images_folder(store_roi,0,false);

std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
for ( unsigned int k = 0; k < roi_images.size(); k++ ){

	std::shared_ptr<Resample> Resample0 =std::make_shared<Resample>();
	Resample0->resample((roi_images[k]).c_str(),512,1);
	std::shared_ptr<Resample> Resample2 =std::make_shared<Resample>();
	Resample2->resample((mask_images[k]).c_str(),512,1);
	std::shared_ptr<BinaryImage> Bq_start =std::make_shared<BinaryImage>();
	//Bq_start->Image_binary((mask_images[k]).c_str(),(mask_images[k]).c_str(), 50,255);//5 (when case of one patient uncomment this)


	std::shared_ptr<denoising > SM_norm1 =std::make_shared<denoising>(root);
	if (SD_group!=0 && mean_group!=0){SM_norm1->Normalize_image(roi_images[k].c_str(), roi_images[k].c_str(),mean_group,SD_group);}

		for ( unsigned int i = 0 ; i < 50 ; ++i ){
				std::shared_ptr<denoising> SM =std::make_shared<denoising>(root);
				SM->Median_smoothing((mask_images[k]).c_str(),(mask_images[k]).c_str(),1,1);
				std::shared_ptr<BinaryImage> Bq =std::make_shared<BinaryImage>();
				Bq->Image_binary((mask_images[k]).c_str(),(mask_images[k]).c_str(), 245,255);//5

		}
	inputFileName=(roi_images[k]).c_str();

	reduce_noise();

	MI->Crop_binary((image_detect).c_str(),(mask_images[k]).c_str(),(store_myo_masks+"myo"+std::to_string(k)+".jpg").c_str());
}

}



bool Segmentation_Pipeline::check_black_image(const char * image, int element_number, int threshold_value_intens)
{

bool result=true;

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
int thres_black=element_number;
int thres_noise=0;
for ( outputIt.GoToBegin(), inputIt.GoToBegin(); !outputIt.IsAtEnd();++inputIt, ++outputIt)
{

	if (outputIt.Get()>threshold_value_intens)
	{
		thres_noise++;
		if(thres_noise>thres_black){result=false;std::cout<<" NO black image..."<<std::endl; outputIt.GoToEnd();inputIt.GoToEnd();--inputIt; --outputIt;}
	}//end if

}//end for

return result;
}
