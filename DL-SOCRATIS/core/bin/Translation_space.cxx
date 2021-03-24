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
//Version: 0.3

//////////////////////////////////////////////////////////////////////////////////

#include "Translation_space.h"





void Translation_space::Run_trans(std::string python_path, std::string m_init_image_space, std::string m_final_image_space, std::string store_path){

Store(store_path, " ", store_path);
		std::string commandwr2r;
		commandwr2r += "rm -rf ";
		commandwr2r += store_path;
		commandwr2r +="*";
		int wrf2r= system(commandwr2r.c_str());	
//measurement evaluate
vector<string> input;

vector<string> h_phase;
//read all the files in myo and roi directory
bool both_phases=false;
struct dirent *entry;
DIR *pDIR;
std::string    myo_path;
std::string init_image_space=m_init_image_space;
std::string final_image_space=m_final_image_space;

std::cout << "Run the CycleGAN model for the: " << init_image_space<<" to "<<final_image_space <<" translation."<<std::endl;

if( pDIR=opendir(python_path.c_str()) ){
	while(entry = readdir(pDIR)){
		if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, "Conf*") != 0){
			std::stringstream i;
			std::stringstream n;
			i<<python_path<<entry->d_name;
			n<<entry->d_name;
			if(n.str()==(init_image_space+"_"+final_image_space+".py"))
				{  
				     	input.push_back(i.str());
					cout << i.str() << "\n";
				}
			}
		}
		closedir(pDIR);
	}




std::cout << "input file: " << input[0].c_str() <<std::endl;

std::string command = "python3 ";
command += input[0];
command += " ";
int f= system(command.c_str());
Copy(store_data_path, store_myo_path, store_roi_path, -1);

				



}






