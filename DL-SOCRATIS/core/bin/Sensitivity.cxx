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


#include "Sensitivity.h"
#define THREADS 2
#define CPU 1
float Sensitivity::uniform_real_generator(double max, double min, double SD){

  	std::default_random_engine generator;	
	std::uniform_real_distribution<double> distribution(0.0,1.0);
	float init_number;
	double number = distribution(generator);
	float interval=(max-min);
	if (number>0.5){
		init_number=number*interval+min+SD;
		if (init_number>max){init_number=max;}
	}
	if (number<=0.5){
		init_number=number*interval+min-SD;
		if (init_number<min){init_number=min;}
	}

	return init_number;
}


float Sensitivity::normal_real_generator(double max, double min, double SD){

  	std::default_random_engine generator1;	
	 std::normal_distribution<double> distribution1(0.0,1.0);
	float init_number1;
	double number1 = distribution1(generator1);
	float interval1=(max-min);
	if (number1>0.5){
		init_number1=number1*interval1+min +SD;
		if (init_number1>max){init_number1=max;}
	}
	if (number1<=0.5){
		init_number1=number1*interval1+min -SD;
		if (init_number1<min){init_number1=min;}
	}

	return init_number1;
}




Sensitivity::Sensitivity(std::vector<bool> parameters, std::string sensitivity_mode){
parameter_endo=parameters[0];
parameter_epi=parameters[1];
parameter_kmean=parameters[2];
parameter_rgmm=parameters[3];
parameter_rgmm_kmean=parameters[4];

if (sensitivity_mode=="GL")
{
	Global_mode=true;
}
else
{
	Local_mode=true;
}

}


void Sensitivity::RUN( const char *confg_file, std::string m_storepath_results)
{

storepath_results=m_storepath_results;
float init_endo=0;
float init_epi=0;
float init_kmean=0;
float init_rgmm=0;
float init_rgmm_kmean=0;

int interval_of_active[5];
interval_of_active[0]=0;
interval_of_active[1]=0;
interval_of_active[2]=0;
interval_of_active[3]=0;
interval_of_active[4]=0;

std::vector<float> five_parameters_value;
if (Global_mode==true)
{
	cout << "Global sensitivity CUDA GP" << std::endl;
}
else
{
	cout << "local sensitivity..." << std::endl;
	if (parameter_endo==true)
	{
		init_endo=(higher_value_endo-lower_value_endo)/2+lower_value_endo;
		five_parameters_value.push_back(init_endo);
		interval_of_active[0]=0+(int)(higher_value_endo-lower_value_endo)/SD_endo;
	}
	else {five_parameters_value.push_back(0);}

	if (parameter_epi==true)
	{
		init_epi=(higher_value_epi-lower_value_epi)/2+lower_value_epi;
		five_parameters_value.push_back(init_epi);
		interval_of_active[1]=0+(int)(higher_value_epi-lower_value_epi)/SD_epi;
	}
	else {five_parameters_value.push_back(0);}

	if (parameter_kmean==true)
	{

		init_kmean=(higher_value_kmean-lower_value_kmean)/2+lower_value_kmean;
		five_parameters_value.push_back(init_kmean);
		interval_of_active[2]=0+(int)(higher_value_kmean-lower_value_kmean)/SD_kmean;
	}
	else {five_parameters_value.push_back(0);}

	if (parameter_rgmm==true)
	{

		init_rgmm=(higher_value_rgmm-lower_value_rgmm)/2+lower_value_rgmm;
		five_parameters_value.push_back(init_rgmm);
		interval_of_active[3]=0+(int)(higher_value_rgmm-lower_value_rgmm)/SD_rgmm;
	}
	else {five_parameters_value.push_back(0);}

	if (parameter_rgmm_kmean==true)
	{

		init_rgmm_kmean=(higher_value_rgmm_kmean-lower_value_rgmm_kmean)/2+lower_value_rgmm_kmean;
		five_parameters_value.push_back(init_rgmm_kmean);
		interval_of_active[4]=0+(int)(higher_value_rgmm_kmean-lower_value_rgmm_kmean)/SD_rgmm_kmean;
	}
	else {five_parameters_value.push_back(0);}

	num_sensitivity=interval_of_active[0]+interval_of_active[1]+interval_of_active[2]+interval_of_active[3]+interval_of_active[4];
	//copy(confg_file, 0,five_parameters_value);


	std::vector<float> parameters_value=five_parameters_value;

	//#pragma omp parallel num_threads(THREADS+3) 
	//{
		//omp_set_nested(1);
		//#pragma omp parallel for num_threads(5) //schedule(static,1) //collapse(THREADS)//reduction(+: k)//schedule(static) 
			for ( unsigned int o = 0; o < 5; o++ )
				{	
					parameter_exacute(o,  interval_of_active[o], parameters_value, confg_file);
				}
	//}
}


}



void Sensitivity::parameter_exacute(int parametr_number, int interval_of_active, std::vector<float> parameters_value, const char * confg_file){

int o=parametr_number;
//#pragma omp parallel for num_threads(THREADS) schedule(static,1)
for ( unsigned int ko = 0; ko < interval_of_active; ko++ )
						{
						cout << "local sensitivity thread outer loop: " <<o<<"/5" <<std::endl;
						cout << "local sensitivity thread inner loop: " <<ko<<"/"<<interval_of_active <<std::endl;

						float value=uniform_real_generator(high[o], low[o], SD[o]);
						parameters_value[o]=value;//.insert(parameters_value.begin()+o, value);
						
						//std::replace (parameters_value.begin()+o, o, parameters_value[o], value);
						std::string file_path=(storepath_results)+"results"+"_sensitivity.txt";

						ofstream myfile;
						myfile.open(file_path, std::ofstream::out |std::ofstream::app);

						if (myfile.is_open())
							{
							
							  myfile << " Sensitivity analysis of parameters --> "<< (o*10)+ko <<"/"<<num_sensitivity<<"\n";
							  myfile <<" parameters endo ratio value: "<<parameters_value[0]  <<"\n";
							  myfile <<" parameters epi ratio value: "<<parameters_value[1]   <<"\n";
							  myfile << " parameters kmean threshold ratio value: "<<parameters_value[2] <<"\n";
							  myfile <<" parameters rgmm threshold ratio value: "<<parameters_value[3]  <<"\n";
							  myfile << " parameters rgmm-kmean threshold ratio value: "<<parameters_value[4] <<"\n";
							  myfile.close();

							}

							if (mkdir(("./"+std::to_string(o)+"/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
								cerr << "Error :  " << strerror(errno) << endl; 
								}
							else{
								cout << "Directory created" << std::endl;
								}
						copy(confg_file, (o*10)+ko, parameters_value, ("./"+std::to_string(o)+"/") );
						//#pragma omp atomic
						//k++;
						}

}
void Sensitivity::copy(const char *confg_file, int number_of_sensitivity, std::vector<float> parameters_value, std::string root){

std::shared_ptr<configuration > CONFG =std::make_shared<configuration>();
CONFG-> Set_segm(true);
CONFG-> Set_eval(true);
CONFG-> read(confg_file);


//segmentation run 
	vector<int> nb_images= CONFG->Get_patients_images_number();
	int num_patient= CONFG->Get_number_of_patients();
	vector<string> segmentation_paths= CONFG->Get_segmentation_paths();
	vector<string>  patients=CONFG->Get_patients_paths();
	std::string    myo_path=segmentation_paths[0];
	std::string    roi_path=segmentation_paths[1];
	std::string  scar_path=segmentation_paths[2]+std::to_string(number_of_sensitivity)+"_case/";
	std::string  segm_case=segmentation_paths[3];
	std::string scar_path_patient;
	bool bz=CONFG->Get_segmentation_border_zone_bool();
	int tosi=CONFG->Get_TH_Scar_intens();
	if (mkdir(segmentation_paths[2].c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		cout << "Directory created" << std::endl;
	}
	if (mkdir(scar_path.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		cout << "Directory created" << std::endl;
	}
	// segmentation

	//#pragma omp parallel for //num_threads(4) //schedule(static,1)
	if (segm_case=="scar"){
		std::shared_ptr<Segmentation_Pipeline > segmentation =std::make_shared<Segmentation_Pipeline>(root);
		segmentation->Set_segmentation_case( segm_case);
		segmentation->Run(segmentation_paths[0].c_str(),segmentation_paths[1].c_str(),segmentation_paths[2], num_patient);
	}
	else
	{
		for ( unsigned int k = 0; k < num_patient; k++ )
		{
			std::cout << " PATIENT: "<< k <<std::endl;
		  	myo_path=segmentation_paths[0];
			roi_path=segmentation_paths[1];
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
			//std::cout << myo_path << std::endl;
			//std::cout << roi_path << std::endl;
			//std::cout << scar_path_patient  << std::endl;
			int nb_images_patient=nb_images[k];
			std::shared_ptr<Segmentation_Pipeline > segmentation_scar =std::make_shared<Segmentation_Pipeline>(root);
			segmentation_scar->SetBorderZone( bz);
			segmentation_scar->SetThreshold_of_scar_intens( tosi);
			segmentation_scar->Set_segmentation_case( segm_case);
			segmentation_scar->SetSensitivity1_param( parameter_kmean,parameters_value[2]);
			segmentation_scar->SetSensitivity2_param( parameter_endo, parameters_value[0]);
			segmentation_scar->SetSensitivity3_param( parameter_epi, parameters_value[1]);
			segmentation_scar->SetSensitivity5_param( parameter_rgmm_kmean, parameters_value[3]);
			segmentation_scar->SetSensitivity4_param( parameter_rgmm, parameters_value[4]);
			segmentation_scar->Run(myo_path.c_str(),roi_path.c_str(),scar_path_patient,nb_images_patient);

		}
	}//end else
//evaluation

	std::shared_ptr<configuration > CONFG2 =std::make_shared<configuration>();
	CONFG2-> Set_eval(true);
	CONFG2-> Set_segm(true);
	CONFG2-> Set_init_segm(true);
	CONFG2-> read(confg_file);
	
	std::shared_ptr<unbiased_ground_trouth >UNGT =std::make_shared<unbiased_ground_trouth>(root);
	std::string heart_phase= CONFG2->Get_heart_phase(); //take from config

	vector<string> evaluation_paths= CONFG2->Get_evaluation_paths();
	int number_experts=CONFG2->Get_evaluation_number_experts();
	std::string    manual_path=evaluation_paths[0]+"/"+std::to_string(number_of_sensitivity)+"_case/";
	if (mkdir((evaluation_paths[0]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}

	if (mkdir((manual_path).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	if (mkdir((evaluation_paths[1]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
	}
	//store paths from config
	std::string    predict_labels_path=evaluation_paths[1];
	std::string manual_path_myo=manual_path +heart_phase+"/mask_manual/"+ "ground_truth"+"/";
	std::string predictpath=predict_labels_path+heart_phase+"/"+std::to_string(number_of_sensitivity)+"_case/";
	vector<string> segmentation_paths1= CONFG2->Get_segmentation_paths();
	std::string path_of_code_results=segmentation_paths1[2]; //take from config

	if( CONFG2->Get_copy()==true){
		//be sure that the folders are deleted

		delete_folder_localy(predict_labels_path+heart_phase+"/"+std::to_string(number_of_sensitivity)+"_case");
		delete_folder_localy(manual_path +heart_phase);
		delete_folder_localy(manual_path +"image");
		delete_folder_localy(segmentation_paths1[2] +"image");

		std::string    manual_path=evaluation_paths[0]+"/"+std::to_string(number_of_sensitivity)+"_case/";
		if (mkdir((evaluation_paths[0]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}

		if (mkdir((manual_path).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
			cerr << "Error :  " << strerror(errno) << endl; 
		}


		std::shared_ptr<unbiased_ground_trouth >UNGT =std::make_shared<unbiased_ground_trouth>(root);
		UNGT->Set_heart_phase(heart_phase);
		UNGT->Run(manual_path, manual_path, number_experts,heart_phase);


		std::shared_ptr<Evaluation > EV =std::make_shared<Evaluation>(root);
		EV->copy_pedrict_data( predictpath,  path_of_code_results,  "all" );
	}

//evaluate results


	if (mkdir((predict_labels_path).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		//cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		//cout << "Directory created" << std::endl;
	}
	if (mkdir((predict_labels_path+heart_phase+"/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		//cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		//cout << "Directory created" << std::endl;
	}
	if (mkdir(predictpath.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		//cerr << "Error :  " << strerror(errno) << endl; 
	}
	else{
		//cout << "Directory created" << std::endl;
	}

	std::shared_ptr<unbiased_ground_trouth > UBGT1 =std::make_shared<unbiased_ground_trouth >(root);
	if (segm_case=="both"){segm_case="all";}
	std::shared_ptr<Evaluation > EV =std::make_shared<Evaluation>(root);
	EV->copy_pedrict_data( predictpath,  path_of_code_results, segm_case );
	vector<string> patient_folders_myo= UBGT1->read_images_folder((predictpath), 1, true);
	//align match two images lists

	std::size_t found=manual_path.find("intra");
	std::string gtruthcase="inter";
	if (found!=std::string::npos){gtruthcase="intra";}
	//align match two images lists
	if (segm_case=="both"){segm_case="all";}


	EV->preprocessing_align_match_images(predictpath, manual_path_myo ,  patient_folders_myo, segm_case, gtruthcase );
	EV->Set_store_txt_path(storepath_results);
	if (segm_case=="all" || segm_case=="myo"){
		EV->compute_dice_sensitivity_specificity_metric(predictpath, manual_path_myo ,  patient_folders_myo, "myo" );
	}
	std::string manual_path_scar=manual_path +heart_phase+"/mask_manual/"+ "ground_truth"+"/";	
	std::shared_ptr<Evaluation > EV2 =std::make_shared<Evaluation>(root);
	if (segm_case=="all" || segm_case=="scar"){
		EV2->Set_store_txt_path(storepath_results);
		EV2->compute_dice_sensitivity_specificity_metric(predictpath, manual_path_scar ,  patient_folders_myo, "scar" );
	}


}

void Sensitivity::delete_folder_localy(std::string path)
{
std::cout<<"Delete folder at:  "<<path<<std::endl;

	std::string command2 = "rm -rf ";
	command2 +=
	command2 +=(path);
	int f= system(command2.c_str());

std::cout<<" END..."<<std::endl;
std::cout<<" ................................"<<std::endl;
}

/*

void Sensitivity::write_sh_files_multinode_running(std::string storepath, int number_of_sensitivity, std::vector<float> parameters_value){

ofstream myfile;// (json_roi_store_path+"roi.txt");
std::string file_path=(storepath)+"sensitivity_"+std::to_string(number_of_sensitivity)+"_job.sh";					myfile.open(file_path, std::ofstream::out |std::ofstream::app);
if (myfile.is_open())
{
	myfile << " Sensitivity analysis of parameters --> "<< k <<"/"<<num_sensitivity<<"\n";
	 myfile <<" parameters endo ratio value: "<<parameters_value[0]  <<"\n";
	 myfile <<" parameters epi ratio value: "<<parameters_value[1]   <<"\n";
	 myfile << " parameters kmean threshold ratio value: "<<parameters_value[2] <<"\n";
	myfile <<" parameters rgmm threshold ratio value: "<<parameters_value[3]  <<"\n";
	myfile << " parameters rgmm-kmean threshold ratio value: "<<parameters_value[4] <<"\n";
	myfile.close();
	}


}

*/



