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


#include "Evaluation.h"
#include <string>

int Evaluation::evaluate_images_dice_metric( const char * predictFileName, const char *manualFileName, std::string storeFolderName, int slices_numb, double rotation_angle, double scale, std::vector<int> center_array )
{
//measurement evaluate
vector<string> manual;
vector<string> predict;
//read all the files in myo and roi directory
struct dirent *entry;
DIR *pDIR;


if( pDIR=opendir(manualFileName) ){
	while(entry = readdir(pDIR)){
		if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
		std::stringstream m;
		m<<manualFileName<<entry->d_name;
                manual.push_back(m.str());
		cout << m.str() << "\n";
		}
		}
	closedir(pDIR);
}

if( pDIR=opendir(predictFileName) ){
	while(entry = readdir(pDIR)){
		if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
		std::stringstream r;
		r<<predictFileName<<entry->d_name;
                predict.push_back(r.str());
		cout << r.str() << "\n";
		}
		}
	closedir(pDIR);
}

std::sort(manual.begin(),manual.end());
std::sort(predict.begin(),predict.end());
std::cout<<" The evaluation of the "<<slices_numb<< " files start now ..."<<std::endl;
double sum_dice=0;
double sum_total=0;
double sum_jaccard=0;
  for ( unsigned int k = 0; k < slices_numb; k++ )
    {

	itk::NumericSeriesFileNames::Pointer fileNamesCreator =itk::NumericSeriesFileNames::New();
	fileNamesCreator->SetStartIndex( 0 );
	fileNamesCreator->SetEndIndex( slices_numb - 1 );
	fileNamesCreator->SetSeriesFormat( manual[k]);
	const std::vector<std::string> & shapeModeFileNamesm = fileNamesCreator->GetFileNames();
	itk::NumericSeriesFileNames::Pointer fileNamesCreator2 =itk::NumericSeriesFileNames::New();
	fileNamesCreator2->SetStartIndex( 0 );
	fileNamesCreator2->SetEndIndex( slices_numb - 1 );
	fileNamesCreator2->SetSeriesFormat( predict[k]);
	const std::vector<std::string> & shapeModeFileNamesp = fileNamesCreator2->GetFileNames();

	std::vector<ImageType::Pointer> m_training( slices_numb );
	std::vector<ImageType::Pointer> p_training( slices_numb );

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName( shapeModeFileNamesm[k].c_str() );
	reader->Update();
	m_training[k] = reader->GetOutput();

	ReaderType::Pointer reader2 = ReaderType::New();
	reader2->SetFileName( shapeModeFileNamesp[k].c_str() );
	reader2->Update();
	p_training[k] = reader2->GetOutput();

    	predictFile=shapeModeFileNamesp[k].c_str(); 
	manualFile=shapeModeFileNamesm[k].c_str();
	string storeFolderName_final=storeFolderName+std::to_string(k)+"/";
	// create directore
	
	if (mkdir(storeFolderName_final.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
        	cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		cout << "Directory created" <<storeFolderName_final.c_str()<< std::endl;
	}

	std::shared_ptr<evaluate > eval =std::make_shared<evaluate>();
	eval->m_angle=rotation_angle;
	eval->scale=scale;
	eval->SetCenter(center_array);
	eval->evaluate_results(predictFile,manualFile,storeFolderName_final);

        sum_dice += eval->GetDice();
	sum_total +=eval->GetTotal();
	sum_jaccard +=eval->GetJaccard();
	//store the scar data as it was
	//eval->m_angle=-rotation_angle;
	//eval->scale=(double)1.00/scale;
	//eval->SetCenter(center_array);
	//eval->evaluate_results(predictFile,manualFile,storeFolderName_final);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

total_dice =sum_dice/slices_numb;
total_total=sum_total/slices_numb;
total_jaccard=sum_jaccard/slices_numb;



}
void Evaluation::copy(const char *confg_file){

std::shared_ptr<configuration > CONFG =std::make_shared<configuration>();
CONFG-> Set_segm(true);
CONFG-> Set_eval(true);
CONFG-> read(confg_file);
vector<int> nb_images= CONFG->Get_patients_images_number();
int num_patient= CONFG->Get_number_of_patients();
vector<string> segmentation_paths= CONFG->Get_segmentation_paths();
vector<string>  patients=CONFG->Get_patients_paths();
std::string  scar_path=segmentation_paths[2];
std::string scar_path_patient;
vector<string> evaluation_paths= CONFG->Get_evaluation_paths();
vector<string>  epatients=CONFG->Get_eval_patients_paths();
std::string    predict_path=evaluation_paths[1];
if (mkdir(predict_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
	cerr << "Error :  " << strerror(errno) << endl; 
}
else{
	cout << "Directory created" << std::endl;
}

std::string predict;
std::string scars;
std::string predictbz;
std::string scarsbz;
std::string enpredict;
std::string endo;
std::string epredict;
std::string epi;
std::string myo;
std::string predictm;
for ( unsigned int k = 0; k < num_patient; k++ )
	{

		predict_path=evaluation_paths[1];
		predict_path=(predict_path+epatients[k])+'/';
		scar_path_patient=scar_path+epatients[k]+"/";
		std::cout << " PATIENT: "<< k <<std::endl;
		std::cout<<"Copy predict scar,epi,endo images to folder :" <<predict_path<<std::endl;
		std::cout<<"From folder :" <<scar_path_patient<<std::endl;
		// Creating  directory 

		if (mkdir(predict_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}

		if (mkdir((predict_path+"Scars/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		if (mkdir((predict_path+"Endo/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		if (mkdir((predict_path+"Epi/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		if (mkdir((predict_path+"Myo/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		if (mkdir((predict_path+"BZScars/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}
		else{
			cout << "Directory created" << std::endl;
		}
		int nb_images_patient=nb_images[k];
		for ( unsigned int kk = 0; kk < nb_images_patient; kk++ ){
			std::stringstream p;

			p << predict_path <<"Scars/" <<std::to_string(kk)<<".jpg" ;
			predict=p.str();
			std::stringstream s;
			s << scar_path_patient <<std::to_string(kk)<< "/binary_combine_image.jpg" ;
			scars=s.str();
			boost::filesystem::copy_file(scars, predict,boost::filesystem::copy_option::overwrite_if_exists);

			std::stringstream enp;
			enp << predict_path <<"Endo/" <<std::to_string(kk)<<".jpg" ;
			enpredict=enp.str();
			std::stringstream ens;
			ens << scar_path_patient <<std::to_string(kk)<< "/endo_segm.jpg" ;
			endo=ens.str();
			boost::filesystem::copy_file(endo, enpredict,boost::filesystem::copy_option::overwrite_if_exists);

			std::stringstream ep;
			ep << predict_path <<"Epi/" <<std::to_string(kk)<<".jpg" ;
			epredict=ep.str();
			std::stringstream es;
			es << scar_path_patient <<std::to_string(kk)<< "/epi_segm.jpg" ;
			epi=es.str();
			boost::filesystem::copy_file(epi, epredict,boost::filesystem::copy_option::overwrite_if_exists);
				
			std::stringstream pm;
			pm << predict_path <<"Myo/" <<std::to_string(kk)<<".jpg" ;
			predictm=pm.str();
			std::stringstream sm;
			sm << scar_path_patient <<std::to_string(kk)<< "/comb_myo_b.jpg" ;
			myo=sm.str();
			boost::filesystem::copy_file(myo, predictm,boost::filesystem::copy_option::overwrite_if_exists);

			std::stringstream pbz;
			pbz << predict_path <<"BZScars/" <<std::to_string(kk)<<".jpg" ;
			predictbz=pbz.str();
			std::stringstream sbz;
			sbz << scar_path_patient <<std::to_string(kk)<< "/binary_bz.jpg" ;
			scarsbz=sbz.str();
			boost::filesystem::copy_file(scarsbz, predictbz,boost::filesystem::copy_option::overwrite_if_exists);
			
			}


}
}

void Evaluation::copy_pedrict_data( std::string predictpath_store, std::string path_of_code_results,  std::string image_file )
{

	std::shared_ptr<unbiased_ground_trouth > UBGT0 =std::make_shared<unbiased_ground_trouth >(root);
	vector<string> patient_folders= UBGT0->read_images_folder((path_of_code_results), 1, true);

for ( unsigned int k = 0; k < patient_folders.size(); k++){

			if (mkdir((predictpath_store).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}

			if (mkdir((predictpath_store+patient_folders[k]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}

			if (mkdir((predictpath_store+patient_folders[k]+"/scar").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}

			if (mkdir((predictpath_store+patient_folders[k]+"/border_zone").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}
			if (mkdir((predictpath_store+patient_folders[k]+"/myo").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				cerr << "Error :  " << strerror(errno) << endl; 
			}
			else{
				cout << "Directory created" << std::endl;
			}



//copy images if case of myo scar bz
//scar name file :
// myo name file (no pupilaries)

	vector<string> patient_images= UBGT0->read_images_folder((path_of_code_results+patient_folders[k]+"/"), 1, true);

	int up=0;
	int up2=0;

	for ( unsigned int o = 0; o< patient_images.size(); o++){

		if( image_file=="myo"){

				if( fexists( path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/comb_myo_b.jpg" )==true)
				{
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

					boost::filesystem::copy_file( (path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/comb_myo_b.jpg"), (predictpath_store+patient_folders[k]+"/"+image_file+"/myo_"+ save_number+".jpg"), boost::filesystem::copy_option::overwrite_if_exists);
					up2=up2+1;
					

				}
				else{std::cout << "Directory file is emty no image in this folder" << std::endl;}				
			}


		if( (image_file=="all"))
			{
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
				//myo
				if( ((fexists( path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/binary_combine_image.jpg" )==true) && ( fexists( path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/comb_myo_b.jpg")==true ) ))
				{
					boost::filesystem::copy_file( (path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/binary_combine_image.jpg"), (predictpath_store+patient_folders[k]+"/scar"+"/scar_"+ save_number+".jpg"), boost::filesystem::copy_option::overwrite_if_exists);
					boost::filesystem::copy_file( (path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/comb_myo_b.jpg"), (predictpath_store+patient_folders[k]+"/myo"+"/myo_"+ save_number+".jpg"), boost::filesystem::copy_option::overwrite_if_exists);
					up2=up2+1;

				}
				else{std::cout << "Directory file of myo and scar is empty no image in this folder" << std::endl;}
			}
			

		if( (image_file=="scar_bz"))
			{
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
				//myo
				if( ((fexists( path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/binary_combine_image.jpg" )==true) && ( fexists( path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/comb_myo_b.jpg")==true ) ))
				{
					boost::filesystem::copy_file( (path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/binary_combine_image.jpg"), (predictpath_store+patient_folders[k]+"/scar"+"/scar_"+ save_number+".jpg"), boost::filesystem::copy_option::overwrite_if_exists);
					boost::filesystem::copy_file( (path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/comb_myo_b.jpg"), (predictpath_store+patient_folders[k]+"/myo"+"/myo_"+ save_number+".jpg"), boost::filesystem::copy_option::overwrite_if_exists);
					boost::filesystem::copy_file( (path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/binary_bz.jpg"), (predictpath_store+patient_folders[k]+"/border_zone"+"/scarbz_"+ save_number+".jpg"), boost::filesystem::copy_option::overwrite_if_exists);
					up2=up2+1;

				}
				else{std::cout << "Directory file of myo and scar is empty no image in this folder" << std::endl;}
			}



if( (image_file=="scar"))
			{
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
				//myo
				if( ((fexists( path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/binary_combine_image.jpg" )==true) && ( fexists( path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/comb_myo_b.jpg")==true ) ))
				{
					boost::filesystem::copy_file( (path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/binary_combine_image.jpg"), (predictpath_store+patient_folders[k]+"/scar"+"/scar_"+ save_number+".jpg"), boost::filesystem::copy_option::overwrite_if_exists);
					boost::filesystem::copy_file( (path_of_code_results+patient_folders[k]+"/"+std::to_string(o)+"/comb_myo_b.jpg"), (predictpath_store+patient_folders[k]+"/myo"+"/myo_"+ save_number+".jpg"), boost::filesystem::copy_option::overwrite_if_exists);
					up2=up2+1;


				}
				else{std::cout << "Directory file of myo and scar is empty no image in this folder" << std::endl;}


			}//edn if

	}//end for


}//end for

}

void Evaluation::compute_dice_sensitivity_specificity_metric( std::string predictpath, std::string manualpath,  std::vector<string> patient_folders, std::string label )
{
std::cout << "Start the metrics computation." << std::endl;

SampleType::Pointer median = SampleType::New();
MeasurementVectorType mmedian;

float dice_metric_sum=0;
float specificity_metric_sum=0;
float sensitivity_metric_sum=0;
float ACD_metric_sum=0;
float Hausd_metric_sum=0;
std::vector<float> mean_patient_dice;
std::vector<float> mean_patient_sen;
std::vector<float> mean_patient_spec;
std::vector<float> mean_patient_ACD;
std::vector<float> mean_patient_hausd;
float std_sum[5];
std_sum[0]=0;
std_sum[1]=0;
std_sum[2]=0;
std_sum[3]=0;
std_sum[4]=0;
float std[5];

for ( unsigned int k = 0; k < patient_folders.size(); k++)
{
	float metric_sum[5];
	metric_sum[0]=0;
	metric_sum[1]=0;
	metric_sum[2]=0;
	metric_sum[3]=0;
	metric_sum[4]=0;
	//std::shared_ptr<unbiased_ground_trouth > UBGT0 =std::make_shared<unbiased_ground_trouth >();
	//vector<string> path_predict_masks= UBGT0->read_images_folder((predictpath+patient_folders[k]+"/"+label), 0, true);
	//vector<string> path_manual_ground_masks= UBGT0->read_images_folder((manualpath+patient_folders[k]+"/"+label), 0, true);
	std::string path_predict_masks= (predictpath+patient_folders[k]+"/"+label+"/");
	std::string path_manual_ground_masks= (manualpath+label+"/"+patient_folders[k]+"/");

	if (root=="./"){evaluation_file="../IO/";}

	std::string command1 = "python3 ";
	command1 += evaluation_file+"evaluate_code.py";
	command1 += " ";
	command1 += path_predict_masks;
	command1 += " ";
	command1 +=  path_manual_ground_masks;
	int f1= system(command1.c_str());


	  string line;
	  std::ifstream myfile ((path_predict_masks+"/evaluation.txt").c_str());
std::cout << "File evaluation path:"<<path_predict_masks+"  of : "<<std::to_string(k+1)<<"/"<<std::to_string(patient_folders.size())<< '\n';
	   int i=0;
	  if (myfile.is_open())
	  {
	    while ( std::getline (myfile,line) )
	    {
		std::istringstream in(line);    
		std::string type;
        	in >> metric_sum[i];
		//std::cout <<"Metric result of "<<k<<" patient: "<< line << '\n';

		i++;
	    }
	    myfile.close();
	  }
	  else {std::cout << "Unable to open file"<< '\n';}

	mmedian[0]=metric_sum[2];
	mmedian[1]=metric_sum[2];
	median->PushBack(mmedian);

	Hausd_metric_sum=dice_metric_sum+metric_sum[4];
	ACD_metric_sum=dice_metric_sum+metric_sum[3];
	dice_metric_sum=dice_metric_sum+metric_sum[2];
	specificity_metric_sum=specificity_metric_sum+metric_sum[1];
	sensitivity_metric_sum=sensitivity_metric_sum+metric_sum[0];
	std::cout <<"Metric result  of "<<k+1<<" patient: "<<'\n';
	std::cout <<"Dice:  "<<metric_sum[2]<<'\n';
	std::cout <<"Sensitivity: "<<metric_sum[0]<<  '\n';
	std::cout <<"Specificity: "<<metric_sum[1] <<  '\n';
	std::cout <<"ACD: "<<metric_sum[3] <<  '\n';
	std::cout <<"Hausdorff: "<<metric_sum[4] <<  '\n';
	//store the vectors
	mean_patient_hausd.push_back(metric_sum[4]);
	mean_patient_ACD.push_back(metric_sum[3]);
	mean_patient_dice.push_back(metric_sum[2]);
	mean_patient_sen.push_back(metric_sum[0]);
	mean_patient_spec.push_back(metric_sum[1]);
	//extract SD



}
//compute total metrics

float mean_dice=dice_metric_sum/patient_folders.size();
float mean_spec=specificity_metric_sum/patient_folders.size(); 
float mean_sens=sensitivity_metric_sum/patient_folders.size();
float mean_hausd=Hausd_metric_sum/patient_folders.size();
float mean_ACD=ACD_metric_sum/patient_folders.size();

for ( unsigned int k1 = 0; k1 < patient_folders.size(); k1++)
{
	std_sum[0]=std_sum[0]+(mean_patient_dice[k1]-mean_dice)*(mean_patient_dice[k1]-mean_dice);
	std_sum[1]=std_sum[1]+(mean_patient_sen[k1]-mean_sens)*(mean_patient_sen[k1]-mean_sens);
	std_sum[2]=std_sum[2]+(mean_patient_spec[k1]-mean_spec)*(mean_patient_spec[k1]-mean_spec);
	std_sum[3]=std_sum[3]+(mean_patient_ACD[k1]-mean_ACD)*(mean_patient_ACD[k1]-mean_ACD);
	std_sum[4]=std_sum[4]+(mean_patient_hausd[k1]-mean_hausd)*(mean_patient_hausd[k1]-mean_hausd);
}

std[0]=std::sqrt(std_sum[0]/(patient_folders.size()-1));
std[1]=std::sqrt(std_sum[1]/(patient_folders.size()-1));
std[2]=std::sqrt(std_sum[2]/(patient_folders.size()-1));
std[3]=std::sqrt(std_sum[3]/(patient_folders.size()-1));
std[4]=std::sqrt(std_sum[4]/(patient_folders.size()-1));

std::shared_ptr<rebuild_myo_mask > rmm=std::make_shared<rebuild_myo_mask >(root);
double median_total=rmm->compute_median( median);

std::cout <<"Metric total result  of"<<patient_folders.size()<<" patients: "<<'\n';
std::cout <<"Dice:  "<<mean_dice<< "+/-"<<std[0]<< '\n';
std::cout <<"Sensitivity: "<<  mean_sens<<"+/-"<<std[1]<< '\n';
std::cout <<"Specificity: "<<mean_spec<<"+/-"<<std[2]<< '\n';
std::cout <<"ACD: "<<mean_ACD<<"+/-"<<std[3]<< '\n';
std::cout <<"Hausdorff: "<<mean_hausd<<"+/-"<<std[4]<< '\n';
std::cout <<"Median_total: "<<median_total<< '\n';

ofstream myfile;// (json_roi_store_path+"roi.txt");
std::string file_path=txt_path_store+("/results_sensitivity.txt");
myfile.open (file_path, std::ofstream::out | std::ofstream::app);
if (myfile.is_open())
	{
	  myfile <<label<<" segmentation results:" <<"\n";
	  myfile <<" median: " <<std::to_string(median_total) <<"\n";
	  myfile <<std::to_string(mean_dice*100) <<"\n";
	  myfile <<std::to_string(mean_sens*100) <<"\n";
	  myfile <<std::to_string(mean_spec*100) <<"\n";
	  myfile <<std::to_string(mean_ACD) <<"\n";
	  myfile <<std::to_string(mean_hausd) <<"\n";
	  myfile <<std::to_string(std[0]*100) <<"\n";
	  myfile <<std::to_string(std[1]*100) <<"\n";
	  myfile <<std::to_string(std[2]*100) <<"\n";
	  myfile <<std::to_string(std[3]) <<"\n";
	  myfile <<std::to_string(std[4]) <<"\n";
	  myfile.close();
	}

}


void Evaluation::preprocessing_align_match_images( std::string predictpath, std::string manualpath,  std::vector<string> patient_folders, std::string label,std::string ground_truth_case )
{

std::string name;
int cases_label_loop=1;
std::vector<string> vector_label;
std::string store_label=label;


//TODO last smoothing and remove object for scars and myo regions:



if ((label=="all")||(label=="scar")){cases_label_loop=2; vector_label.push_back("myo"); vector_label.push_back("scar");} 
if ((label=="scar_bz")){cases_label_loop=2; vector_label.push_back("scar");vector_label.push_back("border_zone");}

for ( unsigned int vk = 0; vk < cases_label_loop; vk++){

	if ((store_label=="all")||(store_label=="scar")){
		label=vector_label[vk];
		std::cout << "Preprocessing step align and match manual and automatic semented images. "<<vector_label[0]<<", "<<vector_label[1] <<'\n'; 
		if (label=="scar"){name="myo";} else {name="ROItest";}
	}
	if ((store_label=="scar_bz")){
		label=vector_label[vk];
		std::cout << "Preprocessing step align and match manual and automatic semented images. "<<vector_label[0]<<", "<<vector_label[1] <<'\n'; 
		if (label=="scar"||label=="border_zone"){name="myo";} else {name="ROItest";}
	}

	for ( unsigned int k = 0; k < patient_folders.size(); k++){
		bool delete_on=true;
		std::string path_predict_masks= (predictpath+patient_folders[k]+"/"+label+"/");
		std::string path_manual_ground_masks= (manualpath+label+"/"+patient_folders[k]+"/");
		std::shared_ptr<unbiased_ground_trouth > UBGT01=std::make_shared<unbiased_ground_trouth >(root);
		vector<string> auto_patient_images= UBGT01->read_images_folder((path_predict_masks), 1, false);
		vector<string> manual_patient_images= UBGT01->read_images_folder((path_manual_ground_masks), 1, false);
		int auto_num=auto_patient_images.size();
		if (auto_num==0)
		{
			std::cout<<"Delete patient "<<patient_folders[k]<<" no automatic results for this patient"<<'\n';			
			std::string commandwr2r14;
			commandwr2r14 += "rm -rf ";
			commandwr2r14 += (manualpath+label+"/"+patient_folders[k]);		
			int wrf2r14= system(commandwr2r14.c_str());

			std::string commandwr2r141;
			commandwr2r141 += "rm -rf ";
			commandwr2r141 += (predictpath+patient_folders[k]);		
			int wrf2r141= system(commandwr2r141.c_str());

			delete_on=false;
		}	
		int manual_num=manual_patient_images.size();
		bool stop_delete_process =false;
		if (delete_on==true){
			//didect the number of predict :
			int length= std::max(manual_num,auto_num);
			int count_predict=0;
			for ( unsigned int km = 0; km <std::max(manual_num,auto_num); km++){				
			std::cout<<"Manual:  "<<manual_num<<" auto: "<<auto_num<<'\n';
					int man_contour_case;
					int pre_contour_case;
					if ((manual_num>auto_num)){
						man_contour_case=km;
						pre_contour_case=count_predict;
					}
					else if ((manual_num==auto_num)){
						man_contour_case=km;
						pre_contour_case=km;
					}
					else{pre_contour_case=km; man_contour_case=count_predict;}

					std::string manual_image_str=path_manual_ground_masks+manual_patient_images[man_contour_case];
					std::string predict_image_str=path_predict_masks+auto_patient_images[pre_contour_case];
					std::size_t pos_pred=predict_image_str.find("/"+label+"_");
					std::size_t pos_man=manual_image_str.find("/"+name);
					if (label=="border_zone" ){pos_pred=predict_image_str.find("/scarbz_");}					
					std::cout << "ditect "<<pos_man<<"/"<<manual_image_str.length() <<'\n'; 
					int shift_pos_m=0;
					int shift_pos_p=0;
					if (label=="border_zone" ){shift_pos_p=8; shift_pos_m=4;}				
					if (label=="scar" ){shift_pos_p=6; shift_pos_m=4;}
					if (label=="myo" ){shift_pos_p=5;shift_pos_m=8;}	
					std::string sub_p=predict_image_str.substr(pos_pred);
					sub_p=sub_p.substr(shift_pos_p, sub_p.length());
					std::string sub_m=manual_image_str.substr(pos_man);	
					sub_m=sub_m.substr(shift_pos_m, sub_m.length());
					std::string manual_number=sub_m.substr(0,sub_m.length()-5);
					std::string predict_number=sub_p.substr(0,sub_p.length()-4);
					std::cout<<"Patients images numbers are, i) predict image: "<<predict_number<<" ii) manual image: "<< manual_number<<'\n';

					//delete images with no match
					if (stop_delete_process==false){
							if (manual_number!=predict_number){
								std::stringstream manual(manual_number);
								std::stringstream predict(predict_number);
								int m_number=0;
								int p_number=0;
								manual>>m_number;
								predict>>p_number;
								if (m_number<p_number){
									std::cout<<"Delete patient "<<patient_folders[k]<<" manual image: "<< manual_number<<'\n';			
									std::string commandwr2r14;
									commandwr2r14 += "rm -rf ";
									commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+manual_number+".jpeg");		
									int wrf2r14= system(commandwr2r14.c_str());

								}
		
								if (m_number>p_number){
									std::cout<<"Delete patient "<<patient_folders[k]<<" predict image: "<< predict_number<<'\n';			
									std::string commandwr2r141;
									commandwr2r141 += "rm -rf ";
									commandwr2r141 += (path_predict_masks+label+"_"+predict_number+".jpg");		
									int wrf2r141= system(commandwr2r141.c_str());
								}

								if (km==std::max(manual_num,auto_num)-1){
									std::cout<<"Delete patient "<<patient_folders[k]<<" predict image: "<< predict_number<<'\n';			
									std::string commandwr2r141;
									commandwr2r141 += "rm -rf ";
									commandwr2r141 += (path_predict_masks+label+"_"+predict_number+".jpg");		
									int wrf2r141= system(commandwr2r141.c_str());
									std::cout<<"Delete patient "<<patient_folders[k]<<" manual image: "<< manual_number<<'\n';			
									std::string commandwr2r14;
									commandwr2r14 += "rm -rf ";
									commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+manual_number+".jpeg");		
									int wrf2r14= system(commandwr2r14.c_str());
								}
								else{

									//length=length-1;
									km=km-1;
									}	
									
							  }//end if
							  else{
								count_predict++;
							   }//end else
								
					     }//edn if
					else{
							if (manual_number!=predict_number){
								std::stringstream manual(manual_number);
								std::stringstream predict(predict_number);
								int m_number=0;
								int p_number=0;
								manual>>m_number;
								predict>>p_number;
								std::cout << "stop delete process " <<m_number<<", "<<p_number<<", "<<count_predict<<'\n';
								if (auto_num==count_predict+1){
									std::cout<<"Delete patient "<<patient_folders[k]<<" manual image: "<< manual_number<<'\n';			
									std::string commandwr2r14;
									commandwr2r14 += "rm -rf ";
									commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+manual_number+".jpeg");		
									int wrf2r14= system(commandwr2r14.c_str());

								}
		
								if (manual_num==count_predict+1){
									std::cout<<"Delete patient "<<patient_folders[k]<<" predict image: "<< predict_number<<'\n';			
									std::string commandwr2r141;
									commandwr2r141 += "rm -rf ";
									commandwr2r141 += (path_predict_masks+label+"_"+predict_number+".jpg");		
									int wrf2r141= system(commandwr2r141.c_str());
								}

									//length=length-1;
									km=km-1;
						            }//end if
					}//end else


					if (std::min(manual_num,auto_num)==(count_predict))
					{
						count_predict=std::min(manual_num,auto_num)-1; 
						stop_delete_process=true; 
						std::cout << "stop delete process " <<'\n';

					}
				        if (auto_num!=0){
						auto_patient_images= UBGT01->read_images_folder((path_predict_masks), 1, false);
					        auto_num=auto_patient_images.size();
					}
					else{auto_num=0;count_predict=0;}

					manual_patient_images= UBGT01->read_images_folder((path_manual_ground_masks), 1, false);
					manual_num=manual_patient_images.size();
				}//end for
					
			}//end if

}//end for


} //end for 

for ( unsigned int k = 0; k < patient_folders.size(); k++)
	{
	int up=0;
	int up2=0;
	label="myo";
	std::string path_predict_masks= (predictpath+patient_folders[k]+"/"+label+"/");
	std::string path_predict_masks2= (predictpath+patient_folders[k]+"/"+"scar"+"/");
	std::string path_manual_ground_masks= (manualpath+label+"/"+patient_folders[k]+"/");
	std::string path_manual_ground_masks2= (manualpath+"scar"+"/"+patient_folders[k]+"/");
	std::shared_ptr<unbiased_ground_trouth > UBGT=std::make_shared<unbiased_ground_trouth >(root);
	vector<string> auto_new= UBGT->read_images_folder((path_predict_masks), 1, false);
	vector<string> auto_new2= UBGT->read_images_folder((path_predict_masks2), 1, false);
	vector<string> manual_new= UBGT->read_images_folder((path_manual_ground_masks), 1, false);
	vector<string> manual_new2= UBGT->read_images_folder((path_manual_ground_masks2), 1, false);
	
	std::string path_predict_masks3;
	std::string path_manual_ground_masks3;
	vector<string> auto_new3;
	vector<string> manual_new3;

if (store_label=="scar_bz"){

	path_predict_masks3= (predictpath+patient_folders[k]+"/"+"border_zone"+"/");
	path_manual_ground_masks3= (manualpath+"border_zone"+"/"+patient_folders[k]+"/");
	std::shared_ptr<unbiased_ground_trouth > UBGTsb=std::make_shared<unbiased_ground_trouth >(root);
	auto_new3= UBGTsb->read_images_folder((path_predict_masks3), 1, false);
	manual_new3= UBGTsb->read_images_folder((path_manual_ground_masks3), 1, false);




}



if((store_label=="scar_bz"))	
	{
	for ( unsigned int o = 0; o< manual_new3.size(); o++)
		{
		std::cout << "Patient number: "<<patient_folders[k]<< '\n';
		std::cout << "Preprocessing image number "<<o<< '\n';
//change space of images
		std::shared_ptr<Change_information > CI=std::make_shared<Change_information >();
		CI->Change_information_run((path_manual_ground_masks+manual_new[o]).c_str(),(path_predict_masks+auto_new[o]).c_str());

		std::shared_ptr<Change_information > CI1=std::make_shared<Change_information >();
		CI1->Change_information_run((path_manual_ground_masks+manual_new[o]).c_str(),(path_predict_masks2+auto_new2[o]).c_str());

		std::shared_ptr<Change_information > CI2=std::make_shared<Change_information >();
		CI2->Change_information_run((path_manual_ground_masks+manual_new[o]).c_str(),(path_manual_ground_masks2+manual_new2[o]).c_str());

		std::shared_ptr<Change_information > CI11=std::make_shared<Change_information >();
		CI11->Change_information_run((path_manual_ground_masks+manual_new[o]).c_str(),(path_predict_masks3+auto_new3[o]).c_str());

		std::shared_ptr<Change_information > CI21=std::make_shared<Change_information >();
		CI21->Change_information_run((path_manual_ground_masks+manual_new[o]).c_str(),(path_manual_ground_masks3+manual_new3[o]).c_str());

		//copy initial an align images
		boost::filesystem::copy_file((path_predict_masks+auto_new[o]).c_str(), (path_predict_masks+"/old_center"+to_string(o)+".jpg").c_str(),boost::filesystem::copy_option::overwrite_if_exists);
		boost::filesystem::copy_file((path_manual_ground_masks+manual_new[o]).c_str(), (path_manual_ground_masks+"/old_center"+to_string(o)+".jpeg").c_str(),boost::filesystem::copy_option::overwrite_if_exists);


		std::cout << "Preprocessing auto mask image number "<<o<< '\n';
		//align images
		std::shared_ptr<deformable_registration > drw=std::make_shared<deformable_registration>(root);
		drw->init_reg((path_predict_masks+auto_new[o]).c_str(),(path_predict_masks+auto_new[o]).c_str());//just cenetr the image


		std::cout << "Preprocessing manual mask image number "<<o<< '\n';
		std::shared_ptr<deformable_registration > dr1w=std::make_shared<deformable_registration >(root);
		dr1w->init_reg((path_manual_ground_masks+manual_new[o]).c_str(),(path_manual_ground_masks+manual_new[o]).c_str());

		std::cout << "Preprocessing manual scar image number "<<o<< '\n';
std::shared_ptr<deformable_registration > dr2=std::make_shared<deformable_registration >(root);
		dr2->init_reg((path_manual_ground_masks+"/old_center"+to_string(o)+".jpeg").c_str(), (path_manual_ground_masks2+manual_new2[o]).c_str());
		std::cout << "Preprocessing auto scar image number "<<o<< '\n';
std::shared_ptr<deformable_registration > dr3=std::make_shared<deformable_registration >(root);
		dr3->init_reg((path_predict_masks+"/old_center"+to_string(o)+".jpg").c_str(),(path_predict_masks2+auto_new2[o]).c_str());

		std::cout << "Preprocessing manual scar bz image number "<<o<< '\n';
std::shared_ptr<deformable_registration > dr21=std::make_shared<deformable_registration >(root);
		dr21->init_reg((path_manual_ground_masks+"/old_center"+to_string(o)+".jpeg").c_str(), (path_manual_ground_masks3+manual_new3[o]).c_str());
		std::cout << "Preprocessing auto scarbz  image number "<<o<< '\n';
std::shared_ptr<deformable_registration > dr31=std::make_shared<deformable_registration >(root);
		dr31->init_reg((path_predict_masks+"/old_center"+to_string(o)+".jpg").c_str(),(path_predict_masks3+auto_new3[o]).c_str());
			// smooth

		//delete initial copy images
		std::string commandwr2r14;
		commandwr2r14 += "rm -rf ";
		commandwr2r14 += (path_manual_ground_masks+"old_center"+to_string(o)+".jpeg");		
		int wrf2r14= system(commandwr2r14.c_str());

		std::string commandwr2r141;
		commandwr2r141 += "rm -rf ";
		commandwr2r141 += (path_predict_masks+"old_center"+to_string(o)+".jpg");		
		int wrf2r141= system(commandwr2r141.c_str());


			//smooth_images( (path_manual_ground_masks2+manual_new2[o]), 20, 200 );
			//smooth_images( (path_manual_ground_masks3+manual_new3[o]),  20, 200 );//1 200
		//	smooth_images( (path_predict_masks2+auto_new2[o]),  20, 200 );//10
			//smooth_images( (path_predict_masks3+auto_new3[o]),  20, 200 );//20 200
std::shared_ptr<deformable_registration > dr=std::make_shared<deformable_registration >(root);



			// smooth
			smooth_images( (path_manual_ground_masks+manual_new[o]),10, 200 );
			smooth_images( (path_manual_ground_masks3+manual_new3[o]),20, 200 );
			smooth_images( (path_manual_ground_masks2+manual_new2[o]),  20, 200 );//1 200
			smooth_images( (path_predict_masks3+auto_new3[o]),  20, 200 );//10
			smooth_images( (path_predict_masks2+auto_new2[o]),  20, 200 );//20 200
			smooth_images( (path_predict_masks+auto_new[o]),  10, 200 );//20 200

			//leave only 3 big regions scar

			for ( unsigned int i = 0; i< 1; i++)
			{
				std::shared_ptr<remove_image_object> rbio3 =std::make_shared<remove_image_object>(root);
				rbio3->remove_object((path_manual_ground_masks2+manual_new2[o]).c_str() , (path_manual_ground_masks2+manual_new2[o]).c_str() ,  20, "None");
				std::shared_ptr<remove_image_object> rbio213 =std::make_shared<remove_image_object>(root);
				rbio213->remove_object((path_predict_masks2+auto_new2[o]).c_str() , (path_predict_masks2+auto_new2[o]).c_str() ,   20, "None");

				smooth_images( (path_manual_ground_masks2+manual_new2[o]),  1, 1 );
				smooth_images( (path_predict_masks2+auto_new2[o]),  1, 1 );



				// smooth
				smooth_images( (path_manual_ground_masks2+manual_new2[o]),  6, 245 ); //45

				smooth_images( (path_predict_masks2+auto_new2[o]),  3, 240 ); //15 240 

				//leave only 3 big regions scar bbzone

				std::shared_ptr<remove_image_object> rbio33 =std::make_shared<remove_image_object>(root);
				rbio33->remove_object((path_manual_ground_masks3+manual_new3[o]).c_str() , (path_manual_ground_masks3+manual_new3[o]).c_str() ,  20, "None");
				std::shared_ptr<remove_image_object> rbio2133 =std::make_shared<remove_image_object>(root);
				rbio2133->remove_object((path_predict_masks3+auto_new3[o]).c_str() , (path_predict_masks3+auto_new3[o]).c_str() ,   20, "None");

				smooth_images( (path_manual_ground_masks3+manual_new3[o]),  1, 1 );
				smooth_images( (path_predict_masks3+auto_new3[o]),  1, 1 );

				// smooth
				smooth_images( (path_manual_ground_masks3+manual_new3[o]),  6, 245 ); //1
				smooth_images( (path_predict_masks3+auto_new3[o]),  3, 240 ); //2  
			}//end for this loop 5 times

			//dr->affine_registartion((path_manual_ground_masks2+manual_new2[o]).c_str(), (path_predict_masks2+auto_new2[o]).c_str(),  "None" ,(path_predict_masks2+auto_new2[o]).c_str());

		//dr->affine_registartion((path_manual_ground_masks3+manual_new3[o]).c_str(), (path_predict_masks3+auto_new3[o]).c_str(),  "None" ,(path_predict_masks3+auto_new3[o]).c_str());

					
		}
}




//if(patient_folders[k]=="37"||patient_folders[k]=="38"||patient_folders[k]=="39"||patient_folders[k]=="40"||patient_folders[k]=="41"||patient_folders[k]=="42"||patient_folders[k]=="43"||patient_folders[k]=="44"||patient_folders[k]=="45"||patient_folders[k]=="6"||patient_folders[k]=="7"||patient_folders[k]=="8")

//{
	if (store_label=="myo"){
		for ( unsigned int o = 0; o< manual_new.size(); o++)
		    {

			std::shared_ptr<deformable_registration > dr=std::make_shared<deformable_registration >(root);
			std::cout << "Patient number: "<<patient_folders[k]<< '\n';
			std::cout << "Preprocessing step align images: "<<manual_new[o]<<" , "<<auto_new[o]<< '\n';
			std::shared_ptr<Change_information > CI=std::make_shared<Change_information >();
			CI->Change_information_run((path_manual_ground_masks+manual_new[o]).c_str(),(path_predict_masks+auto_new[o]).c_str());
			//CI->Change_information_run((path_manual_ground_masks2+manual_new2[o]).c_str(),(path_predict_masks2+auto_new2[o]).c_str());

			dr->similarity_registartion((path_manual_ground_masks+manual_new[o]).c_str(), (path_predict_masks+auto_new[o]).c_str(), "None",(path_predict_masks+auto_new[o]).c_str());


			// smooth

			smooth_images( (path_manual_ground_masks+manual_new[o]),  0, 200 );
			smooth_images( (path_predict_masks+auto_new[o]),  10, 200 );

			}// end for
	}// end if
//if((patient_folders[k]=="8")||(patient_folders[k]=="9"))	{	
	
	if((store_label=="all"))	
	{
	for ( unsigned int o = 0; o< manual_new.size(); o++)
		{
		std::cout << "Patient number: "<<patient_folders[k]<< '\n';
		std::cout << "Preprocessing image number "<<o<< '\n';

//change space of images
		std::shared_ptr<Change_information > CI=std::make_shared<Change_information >();
		CI->Change_information_run((path_manual_ground_masks+manual_new[o]).c_str(),(path_predict_masks+auto_new[o]).c_str());
		std::shared_ptr<Change_information > CI1=std::make_shared<Change_information >();
		CI1->Change_information_run((path_manual_ground_masks+manual_new[o]).c_str(),(path_predict_masks2+auto_new2[o]).c_str());
		std::shared_ptr<Change_information > CI2=std::make_shared<Change_information >();
		CI2->Change_information_run((path_manual_ground_masks+manual_new[o]).c_str(),(path_manual_ground_masks2+manual_new2[o]).c_str());

		//copy initial an align images
		boost::filesystem::copy_file((path_predict_masks+auto_new[o]).c_str(), (path_predict_masks+"/old_center"+to_string(o)+".jpg").c_str(),boost::filesystem::copy_option::overwrite_if_exists);
		boost::filesystem::copy_file((path_manual_ground_masks+manual_new[o]).c_str(), (path_manual_ground_masks+"/old_center"+to_string(o)+".jpeg").c_str(),boost::filesystem::copy_option::overwrite_if_exists);


		std::cout << "Preprocessing auto mask image number "<<o<< '\n';
		//align images
		std::shared_ptr<deformable_registration > dr=std::make_shared<deformable_registration>(root);
		dr->init_reg((path_predict_masks+auto_new[o]).c_str(),(path_predict_masks+auto_new[o]).c_str());//just cenetr the image


		std::cout << "Preprocessing manual mask image number "<<o<< '\n';
		std::shared_ptr<deformable_registration > dr1=std::make_shared<deformable_registration >(root);
		dr1->init_reg((path_manual_ground_masks+manual_new[o]).c_str(),(path_manual_ground_masks+manual_new[o]).c_str());

		std::cout << "Preprocessing manual scar image number "<<o<< '\n';
std::shared_ptr<deformable_registration > dr2=std::make_shared<deformable_registration >(root);
		dr2->init_reg((path_manual_ground_masks+"/old_center"+to_string(o)+".jpeg").c_str(), (path_manual_ground_masks2+manual_new2[o]).c_str());
		std::cout << "Preprocessing auto scar image number "<<o<< '\n';
std::shared_ptr<deformable_registration > dr3=std::make_shared<deformable_registration >(root);
		dr3->init_reg((path_predict_masks+"/old_center"+to_string(o)+".jpg").c_str(),(path_predict_masks2+auto_new2[o]).c_str());

		//delete initial copy images
		std::string commandwr2r14;
		commandwr2r14 += "rm -rf ";
		commandwr2r14 += (path_manual_ground_masks+"old_center"+to_string(o)+".jpeg");		
		int wrf2r14= system(commandwr2r14.c_str());

		std::string commandwr2r141;
		commandwr2r141 += "rm -rf ";
		commandwr2r141 += (path_predict_masks+"old_center"+to_string(o)+".jpg");		
		int wrf2r141= system(commandwr2r141.c_str());

		if(store_label=="scar"){
			// smooth

			smooth_images( (path_manual_ground_masks+manual_new[o]), 1, 200 );
			smooth_images( (path_manual_ground_masks2+manual_new2[o]),  20, 200 );//1 200
			smooth_images( (path_predict_masks+auto_new[o]),  10, 200 );//10
			smooth_images( (path_predict_masks2+auto_new2[o]),  20, 200 );//20 200

			dr->affine_registartion((path_manual_ground_masks2+manual_new2[o]).c_str(), (path_predict_masks2+auto_new2[o]).c_str(),  "None" ,(path_predict_masks2+auto_new2[o]).c_str());

			// smooth
			smooth_images( (path_manual_ground_masks+manual_new[o]),5, 200 );
			smooth_images( (path_manual_ground_masks2+manual_new2[o]),  10, 200 );//1 200
			smooth_images( (path_predict_masks+auto_new[o]),  5, 200 );//10
			smooth_images( (path_predict_masks2+auto_new2[o]),  10, 200 );//20 200

			//leave only 3 big regions scar
			std::shared_ptr<remove_image_object> rbio =std::make_shared<remove_image_object>(root);
			rbio->remove_object((path_manual_ground_masks2+manual_new2[o]).c_str() , (path_manual_ground_masks2+manual_new2[o]).c_str() ,  3, "None");
			std::shared_ptr<remove_image_object> rbio21 =std::make_shared<remove_image_object>(root);
			rbio21->remove_object((path_predict_masks2+auto_new2[o]).c_str() , (path_predict_masks2+auto_new2[o]).c_str() ,   3, "None");

			smooth_images( (path_manual_ground_masks2+manual_new2[o]),  1, 1 );
			smooth_images( (path_predict_masks2+auto_new2[o]),  1, 1 );

			// smooth
			smooth_images( (path_manual_ground_masks2+manual_new2[o]),  45, 250 ); //45

			smooth_images( (path_predict_masks2+auto_new2[o]),  10, 240 ); //15 240 


					
		}

		if(store_label=="all"){

			// smooth

			smooth_images( (path_manual_ground_masks+manual_new[o]), 10, 200 );
			smooth_images( (path_manual_ground_masks2+manual_new2[o]),  20, 200 );//1 200
			smooth_images( (path_predict_masks+auto_new[o]),  10, 200 );//10
			smooth_images( (path_predict_masks2+auto_new2[o]),  20, 200 );//20 200
		
			dr->affine_registartion((path_manual_ground_masks+manual_new[o]).c_str(), (path_predict_masks+auto_new[o]).c_str(), (path_predict_masks2+auto_new2[o]).c_str(),(path_predict_masks+auto_new[o]).c_str());
		
			// smooth

			smooth_images( (path_manual_ground_masks+manual_new[o]), 10, 200 );
			smooth_images( (path_manual_ground_masks2+manual_new2[o]),  10, 200 );//1 200
			smooth_images( (path_predict_masks+auto_new[o]),  10, 200 );//10
			smooth_images( (path_predict_masks2+auto_new2[o]),  10, 210 );//20 200

			//leave only the 3 and 1 biggest regions
			std::shared_ptr<remove_image_object> rbio =std::make_shared<remove_image_object>(root);
			rbio->remove_object((path_manual_ground_masks2+manual_new2[o]).c_str() , (path_manual_ground_masks2+manual_new2[o]).c_str() ,  1000, "None");
			std::shared_ptr<remove_image_object> rbio21 =std::make_shared<remove_image_object>(root);
			rbio21->remove_object((path_predict_masks2+auto_new2[o]).c_str() , (path_predict_masks2+auto_new2[o]).c_str() ,   1000, "None");
			std::shared_ptr<remove_image_object> rbio5 =std::make_shared<remove_image_object>(root);
			rbio5->remove_object((path_manual_ground_masks+manual_new[o]).c_str() , (path_manual_ground_masks+manual_new[o]).c_str() ,  1, "None");
			std::shared_ptr<remove_image_object> rbio215 =std::make_shared<remove_image_object>(root);
			rbio215->remove_object((path_predict_masks+auto_new[o]).c_str() , (path_predict_masks+auto_new[o]).c_str() ,   1, "None");
			// smooth
			//scar
			smooth_images( (path_manual_ground_masks2+manual_new2[o]),  1, 1 );
			smooth_images( (path_predict_masks2+auto_new2[o]),  1, 1 );
			smooth_images( (path_manual_ground_masks2+manual_new2[o]),  2, 100 );//2 100
			smooth_images( (path_predict_masks2+auto_new2[o]),  10, 240 );



// 3 V 45,245 10,245 = 16.15 
// 3 V 50,250, 10,240 = 15.97
// 3 V 50,200, 10,230 = 16.17

//3v 2 100, 10,240 = 16.18
//4v 2 100, 10,240 = 17.25
//5v 2 100, 10,240 = 17.55
//6v 2 100, 10 240 = 17.81
//7v 2 100, 10 240 = 18.07
//8v 2 100, 10 240 = 18.25
//10v 2 100, 10 240 = 18.43
//12v 2 100, 10 240 = 18.54
//16v 2 100, 10 240 = 18.81
//26v 2 100, 10 240 = 19.14
//50v 2 100, 10 240= 19.45
// 75v 2 100, 10 240 = 19.68
// 100v 2 100, 10 240 = 19.92
// 200v 2 100, 10 240 =20.53
// 500v 2 100, 10 240 =21.08

// 1000v 100, 10 240 = 21.2 (smooth change) 21.75 
// 1000v affine ..... = 25.4 


// 10000v 100,10,240= 21.18


//None 2 100, 10 240 = 17.65



			//myo
			smooth_images( (path_manual_ground_masks+manual_new[o]),  1, 1 );
			smooth_images( (path_predict_masks+auto_new[o]),  1, 1 );
			smooth_images( (path_manual_ground_masks+manual_new[o]),  45, 250 );
			smooth_images( (path_predict_masks+auto_new[o]),  10, 240 );

	
			}

		}// end for
//}//end if delete it 

	}// end if













}//end for





}

void Evaluation::smooth_images( std::string predictim,  int loop, int thres )
{
					for ( unsigned int i = 0 ; i < loop ; ++i ){
						std::shared_ptr<BinaryImage> BI1 =std::make_shared<BinaryImage>();
						std::shared_ptr<denoising> SM =std::make_shared<denoising>(root);
						BI1->Image_binary((predictim).c_str(),(predictim).c_str(), thres,255);
						SM->Median_smoothing((predictim).c_str(),(predictim).c_str(),1,1);
						
					}

					
						
}



//old delete version of evaluation


/*
if (store_label=="scar" )
{
if (delete_on==true){

		//inter TODO
		if (patient_folders[k]=="3"){	
				if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 1 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");		
				int wrf2r14= system(commandwr2r14.c_str());

				//std::string commandwr2r142;
				//commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				//int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete inter images patient 1 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");	
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");	
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());	
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());	

			}
		}
		if (patient_folders[k]=="1"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 1 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");		
				int wrf2r14= system(commandwr2r14.c_str());

				//std::string commandwr2r142;
				//commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				//int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				//std::cout<<"delete inter images patient 1 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");		
				int wrf2r14= system(commandwr2r14.c_str());
			}	
		}


		if (patient_folders[k]=="2"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 2 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");//intraf
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");//intraf
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");//intraf
				//commandwr2r142 += (path_predict_masks+label+"_9.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 2 "<< '\n';			
					std::cout<<"delete intra images patient 2 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first
				int wrf2r14= system(commandwr2r14.c_str());
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				int wrf2r142= system(commandwr2r142.c_str());
			}	
		}


		if (patient_folders[k]=="4"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 4 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

			
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 4 "<< '\n';	
				std::cout<<"delete intra images patient 2 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//basic
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");//basic
				int wrf2r14= system(commandwr2r14.c_str());		
			}

		}

		if (patient_folders[k]=="5"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 5 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 5 "<< '\n';
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";	
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");	
				int wrf2r14= system(commandwr2r14.c_str());	
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_6.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());				
			}
		}
		if (patient_folders[k]=="6"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 6 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

			
			}
			if (ground_truth_case=="inter"){			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first basic
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");//first basic
				int wrf2r14= system(commandwr2r14.c_str());


			}
		}

		if (patient_folders[k]=="7"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 7 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 7 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first basic
				int wrf2r14= system(commandwr2r14.c_str());
			}



		}

		if (patient_folders[k]=="8"){
	
			if (ground_truth_case=="intra"){
				std::cout<<"delete images patient intra 8 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");//intra
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");//intra
				int wrf2r14= system(commandwr2r14.c_str());

		
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 8 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first basic
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");//first asic
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}	
			}

		if (patient_folders[k]=="9"){
				if (ground_truth_case=="intra"){
				std::cout<<"delete images patient intra 9 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

		
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 9 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first basic
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"6.jpeg ");//first basic
				
				int wrf2r14= system(commandwr2r14.c_str());
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_5.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}

	
		}


		if (patient_folders[k]=="10"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 10 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_6.jpg ");//second
				int wrf2r142= system(commandwr2r142.c_str());

			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 10 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first basic
				int wrf2r142= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_7.jpg ");//second
				int wrf2r1425= system(commandwr2r142.c_str());


			}
		}

		if (patient_folders[k]=="11"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 11"<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_6.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 11 "<< '\n';			
					std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first basic
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");//first basic
				int wrf2r14= system(commandwr2r14.c_str());

				
			}
		}

		if (patient_folders[k]=="12"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 12 "<< '\n';			
				
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 12 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first basic
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");//first
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");//second
				int wrf2r1425= system(commandwr2r142.c_str());

			}
		}

		if (patient_folders[k]=="13"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete images patient 13 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"2.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

			
			}

			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 13 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");// basic first
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");// basic first
				int wrf2r14= system(commandwr2r14.c_str());
			}
		}

		if (patient_folders[k]=="14"){
			if (ground_truth_case=="intra"){	



				std::cout<<"delete images patient 14 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");

				int wrf2r14= system(commandwr2r14.c_str());

			std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_6.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			
			}
			if (ground_truth_case=="inter"){	
								std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"2.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");

				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_6.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			
		}

		if (patient_folders[k]=="15"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 15 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());


				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_3.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_4.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 15 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first ba
			
				int wrf2r14= system(commandwr2r14.c_str());


			}
		}

		if (patient_folders[k]=="16"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 16 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//intra
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");//intra

				int wrf2r14= system(commandwr2r14.c_str());
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_6.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 16 "<< '\n';			

			}
		}

		if (patient_folders[k]=="17"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 17 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				
			}

		
			if (ground_truth_case=="inter"){	
				std::cout<<"delete images patient 17 "<<'\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first basic
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");//first basic
				int wrf2r14= system(commandwr2r14.c_str());
			
			}
		}

		if (patient_folders[k]=="18"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 18 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
				
			}
		if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 18 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");	
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="19"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 19 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");//intraf
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");//intraf
				int wrf2r142= system(commandwr2r142.c_str());
				
			}

			if (ground_truth_case=="inter"){
				std::cout<<"delete images inter patient 19 "<<'\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first basic
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");//first basic
				int wrf2r14= system(commandwr2r14.c_str());
			}

				
		}

		if (patient_folders[k]=="20"){
			if (ground_truth_case=="intra"){

				std::cout<<"delete images intra patient 20 "<<'\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				

			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images inter patient 20 "<<'\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//first
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"6.jpeg ");//first
				int wrf2r14= system(commandwr2r14.c_str());
				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_6.jpg ");

	
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}


	}
}// end if
*/
/*
if (store_label=="all" )
{
	if (delete_on==true){
		//inter TODO
		if (patient_folders[k]=="3"){	
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 3 "<< '\n';			
				std::string commandwr2r1;
				commandwr2r1 += "rm -rf ";
				commandwr2r1 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r1 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r1 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				commandwr2r1 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
			int wrf2r= system(commandwr2r1.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_2.jpg ");
	
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete inter images patient 3 "<< '\n';			
				std::string commandwr2r1;
				commandwr2r1 += "rm -rf ";
				commandwr2r1 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r1 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r1 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				commandwr2r1 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
			int wrf2r= system(commandwr2r1.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_2.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}
		if (patient_folders[k]=="1"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete myo intra images patient 1 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//no need
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg ");//no need 
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete inter images patient 1 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}	
		}


		if (patient_folders[k]=="2"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 2 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 2 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg ");

				int wrf2r142= system(commandwr2r142.c_str());
			}	
		}


		if (patient_folders[k]=="4"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 4 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");

				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 4 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}

		}

		if (patient_folders[k]=="5"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 5 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_9.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 5 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");//basic
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
			int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_9.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}
		if (patient_folders[k]=="6"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 6 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_9.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 6 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_9.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="7"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 7 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");//basic
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg" );
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 7 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//basic
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");//basic
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");//basic
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg" );
		
				int wrf2r142= system(commandwr2r142.c_str());
			}

		}

		if (patient_folders[k]=="8"){
	
			if (ground_truth_case=="intra"){
				std::cout<<"delete images patient intra 8 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"2.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg ");//
				//commandwr2r142 += (path_predict_masks+label+"_9.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 8 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");//basic
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_9.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}	
			}

		if (patient_folders[k]=="9"){
	

				std::cout<<"delete images patient 9 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"6.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_7.jpg " );
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			
		}


		if (patient_folders[k]=="10"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 10 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");

				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 10 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="11"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 11"<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_9.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());

			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 11 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_9.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="12"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 12 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 12 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());


				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				int wrf2r1426= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="13"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete images patient 13 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}

			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 13 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="14"){
			if (ground_truth_case=="intra"){	



				std::cout<<"delete images patient 14 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg");

				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){	
		
				std::cout<<"delete images patient 14 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg");

				int wrf2r142= system(commandwr2r142.c_str());
			}
			
		}

		if (patient_folders[k]=="15"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 15 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 15 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"6.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_6.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");

				int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="16"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 16 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");
				int wrf2r1425= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 16 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"6.jpeg");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_8.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="17"){
			
				std::cout<<"delete intra images patient 17 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"1.jpeg ");//third
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_2.jpg ");
				int wrf2r142= system(commandwr2r142.c_str());

			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 18 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());
			}

			
		
		}

		if (patient_folders[k]=="18"){
			if (ground_truth_case=="intra"){
				std::cout<<"delete intra images patient 18 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");

				int wrf2r142= system(commandwr2r142.c_str());
			}
		if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 18 "<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				commandwr2r142 += (path_predict_masks+label+"_7.jpg ");

				int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="19"){

			if (ground_truth_case=="intra"){
				std::cout<<"delete images patient 19"<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");//
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");//v2
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");//
				//commandwr2r142 += (path_predict_masks+label+"_7.jpg ");//
				int wrf2r142= system(commandwr2r142.c_str());
			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images patient 19"<< '\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");//
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");//
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"9.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				commandwr2r142 += (path_predict_masks+label+"_0.jpg ");//v2
				commandwr2r142 += (path_predict_masks+label+"_1.jpg ");//v2
				int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		if (patient_folders[k]=="20"){
			if (ground_truth_case=="intra"){

				std::cout<<"delete images intra patient 20 "<<'\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"8.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");

				//int wrf2r142= system(commandwr2r142.c_str());

			}
			if (ground_truth_case=="inter"){
				std::cout<<"delete images inter patient 20 "<<'\n';			
				std::string commandwr2r14;
				commandwr2r14 += "rm -rf ";
				//commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"0.jpeg ");
				commandwr2r14 += (manualpath+label+"/"+patient_folders[k]+"/"+name+"7.jpeg ");
				int wrf2r14= system(commandwr2r14.c_str());

				std::string commandwr2r142;
				commandwr2r142 += "rm -rf ";
				//commandwr2r142 += (path_predict_masks+label+"_0.jpg ");
				//commandwr2r142 += (path_predict_masks+label+"_1.jpg ");

						//commandwr2r142 += (path_predict_masks+label+"_2.jpg ");
				//int wrf2r142= system(commandwr2r142.c_str());
			}
		}

		}	
					
			

	}//end if
*/

