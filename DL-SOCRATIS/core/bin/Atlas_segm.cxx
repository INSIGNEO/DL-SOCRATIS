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


#include "Atlas_segm.h"
#include "Initial_Segmentation.h"




void Atlas_segm::Run(std::string data_path, std::string landmarks_path){



//measurement evaluate
vector<string> input;
struct dirent *entry;
DIR *pDIR;
std::string  path_of_atlas;
path_of_atlas="../../Initial_Segmentation/multi_atlas/CIMAS/";
if (root=="./"){path_of_atlas="../Initial_Segmentation/multi_atlas/CIMAS/";}

if( pDIR=opendir(data_path.c_str()) ){
	while(entry = readdir(pDIR)){
		if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
		std::stringstream i;
		std::stringstream name;
		name<<entry->d_name;
		i<<data_path<<entry->d_name;
                input.push_back(i.str());
		cout << i.str() << "\n";
		folders_name.push_back(name.str());
		}
		}
	closedir(pDIR);
}
std::sort(input.begin(), input.end());
std::sort(folders_name.begin(), folders_name.end());
write_json_roi_file(folders_name,"roi.json");

vector<string> output;
struct dirent *entry2;
DIR *pDIR1;

if( pDIR1=opendir(landmarks_path.c_str()) ){
	while(entry2 = readdir(pDIR1)){
		if( strcmp(entry2->d_name, ".") != 0 && strcmp(entry2->d_name, "..") != 0 ){
		std::stringstream io;
		io<<landmarks_path<<entry2->d_name;
                output.push_back(io.str());
		cout << io.str() << "\n";
		}
		}
	closedir(pDIR1);
}
std::sort(output.begin(), output.end());

if (mkdir((store_image_path).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
	cerr << "Error :  " << strerror(errno) << endl; 
	}
else{
	cout << "Directory created" << std::endl;
	}


for ( unsigned int k = 0; k < folders_name.size(); k++ )
{


	if (type_of_input_data=="dcm"){
		std::string command = path_of_atlas;
		command +="dcm2niix";
		command += " ";
		command += "-o ";
		command += input[k];
		command += " ";
		command += "-f ";
		command += "3dmrri";
		command += " ";
		command += "-z y ";
		command += input[k];		
		int f= system(command.c_str());
		}

	if (mkdir((store_image_path+heart_phase).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}


	if (mkdir((store_image_path+heart_phase+"/image/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}
	if (mkdir((store_image_path+heart_phase+"/image/"+folders_name[k]).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}
}

for ( unsigned int k = 0; k < folders_name.size(); k++ )
{

	std::cout << " Run the multi-atlas fit model for the " << k <<" patient."<<std::endl;

	std::string command2 = "python3 ";
	command2 += path_of_atlas;
	command2 +="seg.p";
	command2 += " ";
	command2 += input[k]+"/patient"+folders_name[k]+"_LGE.nii.gz";
	command2 += " ";
	command2 += output[k]+"/"+folders_name[k]+"*";
	command2 += " ";
	command2 += store_image_path+heart_phase+"/image"+"/"+folders_name[k]+"/";
		
	int f= system(command2.c_str());
}//end for

for ( unsigned int k = 0; k < folders_name.size(); k++ )
{	
slice2dimages(store_image_path+heart_phase+"/image"+"/"+folders_name[k], input[k]+"/*.nii.gz");

preprocessing(store_image_path+heart_phase+"/image"+"/"+folders_name[k]);

std::string commandwr1;
commandwr1 += "rm -rf ";
commandwr1 += input[k]+"/3dmrri*";
int wrf1= system(commandwr1.c_str());


}//end for

run_roiDL_model();


}

void Atlas_segm::slice2dimages(std::string store_data, std::string imagenii){

cout << "Directory imagenii: " <<imagenii<< std::endl;

std::string save_image=store_data;

	if (mkdir((save_image+"/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}

	if (mkdir((save_image+"/contour/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}

if (heart_phase=="ES")
{
	std::string commandgz2 = "gzip -d "+store_data+"/"+ "seg_ES.nii.gz";
	int f01= system(commandgz2.c_str());
	std::string command3;
	command3 +=path_of_med2jpg;
	command3 += "med2image";
	command3 += " -i ";
	command3 += store_data;
	command3 += "/seg_ES.nii";
	command3 += " -d ";
	command3 += save_image+"/contour/";
	command3 += " -o seg_image.jpeg";
	command3 += " -s -1";	
		
	int f= system(command3.c_str());
}

if (heart_phase=="ED")
{
	std::string commandgz2 = "gzip -d "+store_data+"/"+ "seg_ED.nii.gz";
	int f01= system(commandgz2.c_str());
	std::string command3;
	command3 +=path_of_med2jpg;
	command3 += "med2image";
	command3 += " -i ";
	command3 += store_data;
	command3 += "/seg_ED.nii";
	command3 += " -d ";
	command3 += save_image+"/contour/";
	command3 += " -o seg_image.jpeg";
	command3 += " -s -1";	
		
	int f= system(command3.c_str());
}

std::string command4;
command4 +=path_of_med2jpg;
command4 += "med2image";
command4 += " -i ";
command4 += imagenii;
command4 += " -d ";
command4 += save_image+"/";
command4 += " -o image.jpeg";	
command4 += " -s -1";	
int f= system(command4.c_str());

}

void Atlas_segm::preprocessing(std::string images_path){

std::string segm_path= images_path +"/contour/";
vector<string> seg_images;
//read all the files in myo and roi directory
struct dirent *entry2;
DIR *pDIR1;

if( pDIR1=opendir(segm_path.c_str()) ){
	while(entry2 = readdir(pDIR1)){
		if( strcmp(entry2->d_name, ".") != 0 && strcmp(entry2->d_name, "..") != 0 ){
		std::stringstream io;
		io<<segm_path<<entry2->d_name;
                seg_images.push_back(io.str());
		cout << io.str() << "\n";
		}
		}
	closedir(pDIR1);
}
std::sort(seg_images.begin(), seg_images.end());

vector<string> images;
//read all the files in myo and roi directory
struct dirent *entry;
DIR *pDIR;
if( pDIR=opendir((images_path+"/").c_str()) ){
	while(entry = readdir(pDIR)){
		if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
		std::stringstream i;
		i<<(images_path+"/")<<entry->d_name;
                images.push_back(i.str());
		cout << i.str() << "\n";
		}
		}
	closedir(pDIR);
}
std::sort(images.begin(), images.end());
std::sort(seg_images.begin(), seg_images.end());
for ( unsigned int k = 0; k < seg_images.size(); k++ )
{
std::string seg=seg_images[k];
Image_segment_Thresholding(seg.c_str(), seg.c_str(), 20, 220, false);
//Image_segment_Thresholding(seg.c_str(), seg.c_str(), 0, 100, true);

}

patient_number.push_back(seg_images.size());
std::cout <<seg_images.size()<< endl;
}



void Atlas_segm::write_json_roi_file(std::vector<string> filename, std::string jsonname){

std::cout << " Write the roi.json file..."<<std::endl;
std::cout << " Location: "<<std::endl;
std::cout << json_roi_store_path+"roi.json" <<std::endl;
std::cout << "Number of patient: " <<filename.size()<<std::endl;
std::string commandwr;
commandwr += "rm -rf ";
commandwr += json_roi_store_path+jsonname;
int wrf= system(commandwr.c_str());

ofstream myfile;// (json_roi_store_path+"roi.txt");
myfile.open (json_roi_store_path+jsonname, std::ofstream::out | std::ofstream::app);
if (myfile.is_open())
{
	  myfile << "{";
	  myfile << '"'<< "test"<< '"'<<" :\n";
	  myfile << "{\n";

	for ( unsigned int k = 0; k < filename.size()-1; k++ )
	{
	  myfile << '"';
	  myfile << filename[k];
	  myfile << '"';
	  myfile << " : ";
	  myfile << '"';
	  myfile << '"';
	  myfile << ",\n";
	}

	  myfile << '"';
	  myfile << filename[filename.size()-1];
	  myfile << '"';
	  myfile << " : ";
	  myfile << '"';
	  myfile << '"';
	  myfile << "}\n";
	  myfile << "}";
	  myfile.close();
}
else
{
	std::cout << " Error file cannot open... "<<std::endl;
}


}


void Atlas_segm::run_roiDL_model()
{

	if (mkdir((store_roi_image_path).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}
	if (mkdir((store_roi_image_path+"ROI/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}
	if (mkdir((store_roi_image_path+"ROI/test/").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		cerr << "Error :  " << strerror(errno) << endl; 
		}
	else{
		cout << "Directory created" << std::endl;
		}
	if (heart_phase=="ES"){
		std::string command = "python3 ";
		command += atlas_roi_run+"roi.py";
		command += " ";
		command += atlas_roi_run+"roi_ES.config";
		int f= system(command.c_str());
	}
	if (heart_phase=="ED"){
		std::string command = "python3 ";
		command += atlas_roi_run+"roi.py";
		command += " ";
		command += atlas_roi_run+"roi_ED.config";
		int f= system(command.c_str());
	}
	std::shared_ptr< Initial_Segmentation > init_segmentation =std::make_shared< Initial_Segmentation >(root);
	vector<int> nb_images= patient_number;
	vector<string>  patients=folders_name;
	init_segmentation->Set_patients(patients);
	init_segmentation->Set_images_per_patient(nb_images);
	init_segmentation->Copy(store_roi_image_path+"ROI/test/", store_myo_path, store_roi_path, 0);


	for ( unsigned int k = 0; k < patients.size(); k++ )
	{
		std:string myo_path=(store_myo_path+patients[k])+'/';
		vector<string> images;
		vector<string> myoname;
		//read all the files in myo and roi directory
		struct dirent *entry;
		DIR *pDIR;
		if( pDIR=opendir(myo_path.c_str()) ){
			while(entry = readdir(pDIR)){
				if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
				std::stringstream i;
				i<<myo_path<<entry->d_name;
				std::stringstream io;
				io<<entry->d_name;
				myoname.push_back(io.str());
				images.push_back(i.str());
				cout << i.str() << "\n";
				cout << io.str() << "\n";
				}
				}
			closedir(pDIR);
		}
		std::sort(images.begin(), images.end());
		std::sort(myoname.begin(), myoname.end());
		for ( unsigned int k = 0; k < images.size(); k++ )
		{
			std::string im=images[k];
			Image_segment_Thresholding(im.c_str(), (myo_path+"endo_"+myoname[k]).c_str(), 20, 220, false);


		}

	}
}

void Atlas_segm::Image_segment_Thresholding(const char * m_inputFileName, const char * m_outputFileName, float m_lowerThreshold, float m_upperThreshold, bool inverse)
{
  using InputPixelType = unsigned char;
  using OutputPixelType = unsigned char;
  constexpr unsigned int Dimension = 2;
  using InputImageType = itk::Image< InputPixelType,  Dimension >;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;
  using FilterType = itk::ThresholdImageFilter<InputImageType >;
  using ReaderType = itk::ImageFileReader< InputImageType >;
  using WriterType = itk::ImageFileWriter< OutputImageType >;
  ReaderType::Pointer reader = ReaderType::New();
  FilterType::Pointer filter = FilterType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName(m_inputFileName );
  filter->SetInput( reader->GetOutput() );
  const OutputPixelType outsideValue = m_upperThreshold ;
  const OutputPixelType insideValue  = m_lowerThreshold ;
  filter->SetOutsideValue( 0 );
  if (inverse==true){  filter->SetOutsideValue( 255 );}
  filter->ThresholdOutside(  m_lowerThreshold,m_upperThreshold );

  filter->Update();

  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( m_outputFileName );
  writer->Update();
}





