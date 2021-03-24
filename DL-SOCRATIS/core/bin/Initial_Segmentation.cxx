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


#include "Initial_Segmentation.h"





void Initial_Segmentation::Run(std::string python_path,std::string conf_path, int num_classes, std::string m_heart_phase){




//measurement evaluate
vector<string> input;
vector<string> conf;
vector<string> h_phase;
//read all the files in myo and roi directory
bool both_phases=false;
struct dirent *entry;
DIR *pDIR;
std::string    myo_path;
int loop_phase=1;
heart_phase=m_heart_phase;

if ( m_heart_phase=="BT"){h_phase.push_back("ED"); h_phase.push_back("ES");loop_phase=2; both_phases=true;}



for ( unsigned int x = 0; x < loop_phase; x++ )
{
	if (both_phases==true){heart_phase=h_phase[x];}

	if( pDIR=opendir(python_path.c_str()) ){
		while(entry = readdir(pDIR)){
			if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, "Conf*") != 0){
				std::stringstream i;
				std::stringstream n;
				i<<python_path<<entry->d_name;
				n<<entry->d_name;
					//cout << n.str() << "\n";
				if((n.str()==(heart_phase+"_endo.py")) || (n.str()==(heart_phase+"_epi.py")))
					{  
				     	input.push_back(i.str());
					cout << i.str() << "\n";
					}
				}
			}
			closedir(pDIR);
		}

	struct dirent *entry2;
	DIR *pDIR2;

	if( pDIR2=opendir(conf_path.c_str()) ){
		while(entry2 = readdir(pDIR2)){
			if( strcmp(entry2->d_name, ".") != 0 && strcmp(entry2->d_name, "..") != 0 ){
				std::stringstream i2;
				std::stringstream n2;
				i2<<conf_path<<entry2->d_name;
				n2<<entry2->d_name;
				//cout << n2.str() << "\n";
				if(n2.str()==(heart_phase+".config"))
					{  
				     	conf.push_back(i2.str());
				cout << i2.str() << "\n";
					}
				}
			}
		closedir(pDIR2);
		}



	for ( unsigned int k = 0; k < num_classes; k++ )
	{
	//std::string commandwr2r;
	//commandwr2r += "rm -rf ";
	//commandwr2r += "../data_internal/Data/MAIN/test/*.jpeg";
	//int wrf2r= system(commandwr2r.c_str());

		std::cout << "Run the deep learning model for the " << k <<" class."<<std::endl;
		//https://docs.python.org/3/c-api/arg.html
		//FILE* fp;
		//FILE* confp;
	 	//std::ostringstream conf_pathv;
		//PyRun_SimpleString("print ('Start the data creation...of :') ");
		//Py_SetProgramName(conf_pathv.str().c_str());
		//Py_Initialize();
		//PyRun_SimpleString("import sys");
		//std::ostringstream python_path;
		//python_path<<input[k];
		//std::cout <<python_path.str()<<std::endl;
		//PyRun_SimpleString(python_path.str().c_str());
		//PySys_SetArgv(0,conf_path.c_str());
		//fp = _Py_fopen(python_path.str().c_str(), "r");
		//confp = _Py_fopen(conf_path.c_str(), "r");
		//PyRun_SimpleFileEx(fp, python_path.str().c_str(),1);
		//Py_Finalize();
		std::cout << "input file: " << input[k].c_str() <<std::endl;
		std::cout << "config file: " << conf[0].c_str() <<std::endl;
		std::string command = "python3 ";
		command += input[k];
		command += " ";
		command += conf[0];
		int f= system(command.c_str());
		Copy(store_data_path, store_myo_path, store_roi_path, k+1);

	}//end for



	int n=0;
	int inverse_count=0;//patients.size()-1;

	if (num_classes==2)
	{

	std::cout << "Post-processing of DL images number of classes: 2" << std::endl;

		std::string myo_image_dl;
		std::string epi_image_dl;
		std::string endo_imag;
		std::string endo_image_dl;
		std::string first_mask;
		std::string second_mask;
		std::string extent=".jpeg"; //or jpg
		for ( unsigned int k = 0; k < patients.size(); k++ )
			{

				myo_path=store_myo_path;
				myo_path=(myo_path+patients[inverse_count])+'/';
				if (mkdir(myo_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}
	 			int up=1;

				for ( unsigned int o = n; o < n+nb_images[inverse_count]; o++ ){

					std::string save_number;
					int check_number=(o-n);
					if (check_number>9){
						save_number="A"+std::to_string(up);
						up++;
					}
					else
					{
						save_number=std::to_string(check_number);
					}


					myo_image_dl= myo_path+"myo"+save_number+extent;
					epi_image_dl= myo_path+"epi"+save_number+extent;
					endo_image_dl= myo_path+"endo"+save_number+extent;
					first_mask=store_data_path+heart_phase+'/'+std::to_string(1)+"/image_"+std::to_string(1)+"_class_"+ std::to_string(o)+".jpeg";
					std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
					std::shared_ptr< Atlas_segm > atlas =std::make_shared< Atlas_segm >(root);
					std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
					std::shared_ptr< Segmentation_Pipeline > sgp =std::make_shared< Segmentation_Pipeline >(root);
					BinaryShape::Pointer binaryshape = BinaryShape::New();
					BinaryShape::Pointer binaryshape2 = BinaryShape::New();
					int low=122;
					int up=255;
					//filtering of labels

				//{
						if (sgp->check_black_image(first_mask.c_str(),0,0)==false)
							{
								atlas->Image_segment_Thresholding(first_mask.c_str(), first_mask.c_str(), low, up, false);
									std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
								std::shared_ptr<remove_image_object> riob =std::make_shared<remove_image_object>(root);
								riob->remove_holles(first_mask.c_str());
								//riob->remove_object(first_mask.c_str(), endo_image_dl.c_str(),3,"NumberOfPixels");
								riob->remove_object(first_mask.c_str(),  first_mask.c_str() , 2, "None");
								std::shared_ptr<BinaryImage> BI2c =std::make_shared<BinaryImage>();
								BI2c->Image_binary(first_mask.c_str(),first_mask.c_str(), low-2 ,255);//255

								for ( unsigned int i = 0 ; i < 1 ; ++i ){
									std::shared_ptr<BinaryImage> BI2z =std::make_shared<BinaryImage>();
									BI2z->Image_binary(first_mask.c_str(),first_mask.c_str(), low-2 ,255);//255
									std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
									SM1->Median_smoothing(first_mask.c_str(),first_mask.c_str(),1,1);
									}

								
								

								if (x==1){

								
									std::shared_ptr<mergeimages> MI_crop_images =std::make_shared<mergeimages>(root);
									MI_crop_images->Image_overlap( first_mask.c_str(), (store_data_path+"ED"+'/'+std::to_string(1)+"/image_"+std::to_string(1)+"_class_"+ std::to_string(o)+".jpeg").c_str() , endo_image_dl.c_str());

								}
								else{
									boost::filesystem::copy_file( first_mask.c_str(), endo_image_dl.c_str(), boost::filesystem::copy_option::overwrite_if_exists);
								}


								std::shared_ptr<remove_image_object> riob2 =std::make_shared<remove_image_object>(root);
								//riob2->remove_holles(endo_image_dl.c_str());

							
							}
							else
							{
								boost::filesystem::copy_file( first_mask.c_str(),  endo_image_dl.c_str(), boost::filesystem::copy_option::overwrite_if_exists);

							}

					second_mask=store_data_path+heart_phase+'/'+std::to_string(2)+"/image_"+std::to_string(2)+"_class_"+ std::to_string(o)+".jpeg";
					
						if (sgp->check_black_image(second_mask.c_str(),0,0)==false)
							{
								atlas->Image_segment_Thresholding(second_mask.c_str(), second_mask.c_str(), low, up, false);
								std::shared_ptr<remove_image_object> rio =std::make_shared<remove_image_object>(root);
								rio->remove_holles(second_mask.c_str());
								//rio->remove_object(second_mask.c_str(),myo_image_dl.c_str(),1,"NumberOfPixels");

								std::shared_ptr<remove_image_object> rbio =std::make_shared<remove_image_object>(root);
								rbio->remove_object(second_mask.c_str(), second_mask.c_str() , 2, "None");
								std::shared_ptr<BinaryImage> BI2 =std::make_shared<BinaryImage>();
								BI2->Image_binary( second_mask.c_str(), second_mask.c_str(), low-2 ,255);//255
								for ( unsigned int i = 0 ; i < 1 ; ++i ){
									std::shared_ptr<BinaryImage> BI2s =std::make_shared<BinaryImage>();
									BI2s->Image_binary( second_mask.c_str(), second_mask.c_str(), low-2 ,255);//255
									std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
									SM1->Median_smoothing( second_mask.c_str(), second_mask.c_str(),1,1);
	
									}

								

								if (x==1){//1

									
									std::shared_ptr<mergeimages> MI_crop_images =std::make_shared<mergeimages>(root);
									MI_crop_images->Image_overlap( second_mask.c_str(), (store_data_path+"ED"+'/'+std::to_string(2)+"/image_"+std::to_string(2)+"_class_"+ std::to_string(o)+".jpeg").c_str(), epi_image_dl.c_str());

								}
								else{
								boost::filesystem::copy_file(second_mask.c_str(), epi_image_dl.c_str(), boost::filesystem::copy_option::overwrite_if_exists);
								
								}

								//rio->remove_holles(myo_image_dl.c_str());
							
							}
							else
							{
								boost::filesystem::copy_file( second_mask.c_str(), epi_image_dl.c_str(), boost::filesystem::copy_option::overwrite_if_exists);

							}

						//boost::filesystem::copy_file( first_mask.c_str(), endo_image_dl.c_str(), boost::filesystem::copy_option::overwrite_if_exists);
						//boost::filesystem::copy_file( second_mask.c_str(), myo_image_dl.c_str(), boost::filesystem::copy_option::overwrite_if_exists);

						std::shared_ptr<mergeimages> MI_crop_images =std::make_shared<mergeimages>(root);
						MI->Image_merge(second_mask.c_str(),first_mask.c_str(),myo_image_dl.c_str());
					}//edn for
				n+=nb_images[inverse_count];
				inverse_count ++;//=inverse_count-1;
			}//end for
	}//end if

	if (num_classes==1)
	{

		std::cout << "Post-processing of DL images number of classes: 1" << std::endl;
		std::string myo_image_dl;
		std::string endo_image_dl;
		std::string first_mask;
		std::string second_mask;


		for ( unsigned int k = 0; k < patients.size(); k++ )
			{

				myo_path=store_myo_path;
				myo_path=(myo_path+patients[inverse_count])+'/';
				if (mkdir(myo_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					cerr << "Error :  " << strerror(errno) << endl; 
				}
				else{
					cout << "Directory created" << std::endl;
				}
	 			int up=1;
				for ( unsigned int o = n; o < n+nb_images[inverse_count]; o++ )

				{
					std::string save_number;
					int check_number=(o-n);
					if (check_number>9){
						save_number="A"+std::to_string(up);
						up++;
					}
					else
					{
						save_number=std::to_string(check_number);
					}
			
					//std::string myo_path_image= myo_path+"myo"+save_number+".jpeg";
					//myo_image_dl=store_data_path+heart_phase+'/'+std::to_string(0)+"/image_"+std::to_string(0)+"_class_"+ std::to_string(o)+".jpeg";
					//boost::filesystem::copy_file(myo_image_dl, myo_path_image, boost::filesystem::copy_option::overwrite_if_exists);


					myo_image_dl= myo_path+"epi"+save_number+".jpeg";
					endo_image_dl= myo_path+"endo"+save_number+".jpeg";
					first_mask=store_data_path+heart_phase+'/'+std::to_string(1)+"/image_"+std::to_string(1)+"_class_"+ std::to_string(o)+".jpeg";
					std::shared_ptr<mergeimages> MI =std::make_shared<mergeimages>(root);
					std::shared_ptr< Atlas_segm > atlas =std::make_shared< Atlas_segm >(root);
					std::shared_ptr<BinaryImage> BI =std::make_shared<BinaryImage>();
					std::shared_ptr< Segmentation_Pipeline > sgp =std::make_shared< Segmentation_Pipeline >(root);
					BinaryShape::Pointer binaryshape = BinaryShape::New();
					BinaryShape::Pointer binaryshape2 = BinaryShape::New();
					int low=102;
					int up=255;
					//filtering of labels


					if (sgp->check_black_image(first_mask.c_str(),0,0)==false)
						{
					
							for ( unsigned int i = 0 ; i < 40 ; ++i ){
								atlas->Image_segment_Thresholding(first_mask.c_str(), first_mask.c_str(), low, up, false);
								BI->Image_binary(first_mask.c_str(),first_mask.c_str(), low-2 ,255);
								std::shared_ptr<denoising> SM1 =std::make_shared<denoising>(root);
								SM1->Median_smoothing(first_mask.c_str(),first_mask.c_str(),1,1);
								}
							std::shared_ptr<remove_image_object> riob =std::make_shared<remove_image_object>(root);
							riob->remove_holles(first_mask.c_str());
							//riob->remove_object(first_mask.c_str(), endo_image_dl.c_str(),3,"NumberOfPixels");
							std::shared_ptr<remove_image_object> rbio =std::make_shared<remove_image_object>(root);
							rbio->remove_object(first_mask.c_str(), first_mask.c_str() , 1, "None");
							//check if need merge of ES ED
							if (x==1){

								std::shared_ptr<mergeimages> MI_crop_images =std::make_shared<mergeimages>(root);
								MI_crop_images->Image_overlap( first_mask.c_str(), (store_data_path+"ED"+'/'+std::to_string(1)+"/image_"+std::to_string(1)+"_class_"+ std::to_string(o)+".jpeg").c_str() , endo_image_dl.c_str());
							}
							else{
								boost::filesystem::copy_file( first_mask.c_str(), endo_image_dl.c_str(), boost::filesystem::copy_option::overwrite_if_exists);
							}

							std::shared_ptr<remove_image_object> riob2 =std::make_shared<remove_image_object>(root);
							//riob2->remove_holles(endo_image_dl.c_str());

						}
					else
						{
							boost::filesystem::copy_file( first_mask.c_str(),  endo_image_dl.c_str() , boost::filesystem::copy_option::overwrite_if_exists);

						}

					if (mkdir((store_data_path+heart_phase+'/'+std::to_string(2)).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
						cerr << "Error :  " << strerror(errno) << endl; 
					}
					else{
						cout << "Directory created" << std::endl;
					}
					second_mask=store_data_path+heart_phase+'/'+std::to_string(2)+"/image_"+std::to_string(2)+"_class_"+ std::to_string(o)+".jpeg";
					boost::filesystem::copy_file( endo_image_dl.c_str(), second_mask.c_str(), boost::filesystem::copy_option::overwrite_if_exists);

					if (x==1){

						std::shared_ptr<mergeimages> MI_crop_images =std::make_shared<mergeimages>(root);
						MI_crop_images->Image_overlap( second_mask.c_str(), (store_data_path+"ED"+'/'+std::to_string(2)+"/image_"+std::to_string(2)+"_class_"+ std::to_string(o)+".jpeg").c_str() , myo_image_dl.c_str());

					}
					else{
						boost::filesystem::copy_file( second_mask.c_str(), myo_image_dl.c_str(), boost::filesystem::copy_option::overwrite_if_exists);

					}
					std::shared_ptr<remove_image_object> riobw =std::make_shared<remove_image_object>(root);
					//riobw->remove_holles(myo_image_dl.c_str());

					//scale filter 0.5
					
			}//end for
			n+=nb_images[inverse_count];
			inverse_count ++;//=inverse_count-1;

		}//end for
	}//end if

}//end for


std::string commandwr2r;
commandwr2r += "rm -rf ";
commandwr2r += "../../data_internal/Data/MAIN/test/*.jpeg";
int wrf2r= system(commandwr2r.c_str());
}


void Initial_Segmentation::Copy(std::string store_data_path, std::string store_myo_path, std::string store_roi_path, int classes)
{
cout << "Copy data..." << std::endl;
//classes==0 is the atlas case where there is no need of corp of images
//classes>=1 is the DL version where in the worst case you will have one class.
std::string store_path_temporaly;
std::string store_myo_path_image_background;
std::string store_myo_path_image_forground;
std::string store_roi_path_image;
std::string store_path_temporaly2=store_data_path;//+std::to_string(classes)+'/';
std::string store_path_temporaly1=store_data_path+heart_phase+'/';
store_path_temporaly=store_path_temporaly1+std::to_string(classes)+'/';
std::string roi_contour;
std::string class_image_dl;
std::string myo_path;
std::string store_myo_path_mask;
std::string case_dl;
std::string ext=".jpeg";//or jpeg

	if (mkdir(store_roi_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		cout << "Directory created" << std::endl;
	}
	if (store_myo_path!=" ")
	{
		if (mkdir(store_myo_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
	}

if (classes!=0 && classes!=-1){
		cout << "Directory created" <<store_path_temporaly<< std::endl;
	if (mkdir(store_path_temporaly1.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		cout << "Directory created" << std::endl;
	}
	case_dl="main";

		cout << "Directory created" <<store_path_temporaly<< std::endl;
	if (mkdir(store_path_temporaly.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		cout << "Directory created" << std::endl;
	}


}


std::string roi_path;
int half_number_of_images=0;
int n=0;
int inverse_count=patients.size()-1;
std::cout<<patients.size()<<endl;

for ( unsigned int k = 0; k < patients.size(); k++ ){half_number_of_images += nb_images[k];}

for ( unsigned int k = 0; k < patients.size(); k++ )
	{   
		if (classes==0)
		{
			case_dl="ROI";
			if (store_myo_path!=" ")
			{
				myo_path=(store_myo_path+patients[inverse_count])+'/';
					//cout << "Directory created " <<store_myo_path<< std::endl;
					//cout << "Directory created " <<patients[inverse_count]<< std::endl;
					//cout << "Directory created " <<myo_path<< std::endl;
					if (mkdir(myo_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
						cerr << "Error :  " << strerror(errno) << endl; 
						}
					else{
						cout << "Directory created" << std::endl;
						}

			}

		};
		std::cout<<"Patient number: "<<k+1<<"/"<<patients.size()<<endl;
		std::cout<<"Patient name: "<<patients[inverse_count]<<endl;
		std::cout<<"Patient number of MRI slices: "<<nb_images[inverse_count]<<endl;
		roi_path=(store_roi_path+patients[inverse_count])+'/';

		if (mkdir(roi_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		if (classes==-1)
		{
			if (mkdir((roi_path+"contour").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}


		}
		//std::cout<<"Copy images to folder " <<store_data_path<<std::endl;
		int up=1;
		for ( unsigned int o = n; o < n+nb_images[inverse_count]; o++ )
		{	

			int image_number=(n+nb_images[inverse_count]-o-1)+n;
			int image_number2=o;
			std::string save_number;
			int check_number=(o-n); //before only o-n add 1 to start the count from 1

			if (check_number>9){
				save_number="A"+std::to_string(up);
				up++;
			}

			else

			{
				save_number=std::to_string(check_number);
			}	

			if (classes==0)			
			{	
				store_myo_path_mask=store_data_path+case_dl+"masks"+std::to_string((image_number))+ext; //image_number before
				std::string myo_path_image= myo_path+"myo"+save_number+".jpeg";
				boost::filesystem::copy_file(store_myo_path_mask, myo_path_image, boost::filesystem::copy_option::overwrite_if_exists);
			}

			if (classes>0)
			{
				class_image_dl=store_path_temporaly+"image_"+std::to_string(classes)+"_class_"+ std::to_string((image_number2))+".jpeg";
				store_myo_path_image_background=store_path_temporaly2+case_dl+"mask_"+std::to_string(o)+ext;
				store_myo_path_image_forground=store_path_temporaly2+case_dl+"mask_"+std::to_string(half_number_of_images+o)+ext;
				boost::filesystem::copy_file( store_myo_path_image_forground,class_image_dl,boost::filesystem::copy_option::overwrite_if_exists);
			}


			if (classes<=1 && classes>=0)
			{
				store_roi_path_image= store_data_path+case_dl+"test"+std::to_string(image_number2)+ext;
				roi_path=(store_roi_path+patients[inverse_count])+'/';
				roi_path=roi_path+case_dl+"test"+save_number+".jpeg";
				boost::filesystem::copy_file(store_roi_path_image, roi_path,boost::filesystem::copy_option::overwrite_if_exists);	
			}

			if (classes==-1)
			{
				store_roi_path_image= store_data_path+"/AtoB_"+patients[inverse_count]+"_"+std::to_string(check_number+1)+ext;
				roi_path=(store_roi_path+'/'+patients[inverse_count])+'/';
				roi_path=roi_path+case_dl+"test"+save_number+".jpeg";
				roi_contour=(store_roi_path+patients[inverse_count])+'/'+"contour/mask"+save_number+".jpeg";
				boost::filesystem::copy_file(store_roi_path_image, roi_path,boost::filesystem::copy_option::overwrite_if_exists);	
				boost::filesystem::copy_file(store_roi_path_image, roi_contour,boost::filesystem::copy_option::overwrite_if_exists);
			}
		}
		n = n+nb_images[inverse_count];
		inverse_count=inverse_count-1;
	}
		if (classes==-1)
		{
			std::string commandwr2r;
			commandwr2r += "rm -rf ";
			commandwr2r += store_data_path;
			commandwr2r +="*";
			commandwr2r +=ext;
			int wrf2r= system(commandwr2r.c_str());	
		}

}



