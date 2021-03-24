#include <stdlib.h> 
#include <string.h> 
#include <iostream>
#include <bits/stdc++.h> 
#include <cstdlib>
#include <stdio.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <exception>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <array>
#include <iomanip>
#include <vector>
#include <memory>  
#include <algorithm>
#include <boost/filesystem.hpp>

//ITK thresholder
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkThresholdImageFilter.h"
#include "itkImage.h"

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
//sudo g++ -o parser parser.cpp

void Image_based_Thresholding(const char * m_inputFileName, const char * m_outputFileName, float m_lowerThreshold, float m_upperThreshold);
std::vector<std::string> read_the_data(const char * path);
void create_data(std::string path, std::string patient);


int main( int argc, char* argv[] )
{

//1)Inputs of code:
std::string create_path(argv[1]); //the path of data
std::string store_path(argv[2]); // where to store
std::string frame_cmr(argv[3]); // which time frame of CMR is? ES or ED

std::vector<std::string> data_folders=read_the_data((create_path).c_str());
int number_folder=data_folders.size();
std::string path=store_path;

if (mkdir((store_path).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				std::cerr << "Error :  " << strerror(errno) << std::endl; 
			}
			else{
				std::cout << "Directory created" << std::endl;
			}


for (int i=0;i<number_folder;i++)
{
	std::string patient= "patient"+std::to_string(i)+frame_cmr;

	if (mkdir((store_path+"/"+patient).c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
					std::cerr << "Error :  " << strerror(errno) << std::endl; 
				}
				else{
					std::cout << "Directory created" << std::endl;
				}
	std::vector<std::string> data=read_the_data((data_folders[i]+"/").c_str());
	std::string copy_from_path=data_folders[i]+"/";
	std::string specific_patient=store_path+"/"+patient+"/";

		if (frame_cmr=="ES")
		{	
			std::string copy_from_path1=data[2];
			std::string specific_patient1 = specific_patient+"image.nii.gz";
			boost::filesystem::copy_file(copy_from_path1, specific_patient1,boost::filesystem::copy_option::overwrite_if_exists);

			std::cout << "ES data:" << "\n";
			std::cout << data[2] << "\n";
			std::cout << data[3] << "\n";

			std::string copy_from_path2=data[3];
			std::string specific_patient2=specific_patient+"contour.nii.gz";
			boost::filesystem::copy_file(copy_from_path2, specific_patient2,boost::filesystem::copy_option::overwrite_if_exists);

			create_data(path, patient);
		}

		if (frame_cmr=="ED")
		{	
			std::string copy_from_path1=data[4];
			std::string specific_patient1=specific_patient+ "image.nii.gz";
			boost::filesystem::copy_file(copy_from_path1, specific_patient1,boost::filesystem::copy_option::overwrite_if_exists);


			std::string copy_from_path2=data[5];
			std::string specific_patient2=specific_patient+"contour.nii.gz";
			boost::filesystem::copy_file(copy_from_path2, specific_patient2,boost::filesystem::copy_option::overwrite_if_exists);

			create_data(path, patient);


		}

	
}



}

void Image_based_Thresholding(const char * m_inputFileName, const char * m_outputFileName, float m_lowerThreshold, float m_upperThreshold)
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
  filter->ThresholdOutside(  m_lowerThreshold,m_upperThreshold );

  filter->Update();

  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( m_outputFileName );
  writer->Update();
}


std::vector<std::string> read_the_data(const char * path)
{
	std::vector<std::string> input;
	//read all the files in myo and roi directory
	struct dirent *entry;
	DIR *pDIR;
	std::string    myo_path;

	if( pDIR=opendir(path )){
		while(entry = readdir(pDIR)){
			if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
			std::stringstream i;
			i<<(path)<<entry->d_name;
		        input.push_back(i.str());
			//std::cout << i.str() << "\n";
			}
			}
		closedir(pDIR);
	}
std::sort(input.begin(),input.end());
for (int i=0;i<input.size();i++)
{
std::cout << input[i] << "\n";
}
return input;
}


void create_data(std::string path, std::string patient)

{

//int o=3;
//o=o+1;
//char buffer[4];
//char buffer2[4];
//snprintf(buffer, sizeof(buffer), "%03d", i+1);
//snprintf(buffer2, sizeof(buffer2), "%02d", o);
//std::string c_s(buffer);
//std::string c_s2(buffer2);
//std::cout<<"create "<<i<<" patient data\n"<<std::endl;

if (mkdir((path+patient+"/contour").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				std::cerr << "Error :  " << strerror(errno) << std::endl; 
			}
			else{
				std::cout << "Directory created" << std::endl;
			}
if (mkdir((path+patient+"/contour"+"ED").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				std::cerr << "Error :  " << strerror(errno) << std::endl; 
			}
			else{
				std::cout << "Directory created" << std::endl;
			}
if (mkdir((path+patient+"/contour"+"ES").c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
				std::cerr << "Error :  " << strerror(errno) << std::endl; 
			}
			else{
				std::cout << "Directory created" << std::endl;
			}
std::string commandgz = "gzip -d "+path+patient+"/"+ "image.nii.gz";
int f0= system(commandgz.c_str());

std::string commandgz2 = "gzip -d "+path+patient+"/"+ "contour.nii.gz";
int f01= system(commandgz2.c_str());
std::string command1 = "cd bin/; med2image -i ""../"+path+patient+"/image.nii"" -o ../../Image/"+patient+"/"+patient+".jpeg"+ " -s -1";
int f1= system(command1.c_str());

std::string command2 = "cd bin/; med2image -i ""../"+path+patient+"/contour.nii"+ " -o ../../Image/"+patient+"/contour/"+patient+".jpeg"+ " -s -1";
int f2= system(command2.c_str());
//

std::vector<std::string> input=read_the_data((path+patient+"/contour/").c_str());


for (int k=0;k<input.size();k++)
{
	char buffer3[4];
	snprintf(buffer3, sizeof(buffer3), "%03d", k);
	std::string input1=input[k];
	std::string output1=path+patient+"/contourED/"+patient+"-"+buffer3+"_endo.jpeg";
	std::string input2= input[k];//"../Image/"+patient+"/contour/"+patient+"-"+buffer3+".jpeg";
	std::string output2=path+patient+"/contourES/"+patient+"-"+buffer3+"_epi.jpeg";
	Image_based_Thresholding(input1.c_str(), output1.c_str(), 240, 255);
	Image_based_Thresholding(input2.c_str(), output2.c_str(), 125, 255);
}


}
