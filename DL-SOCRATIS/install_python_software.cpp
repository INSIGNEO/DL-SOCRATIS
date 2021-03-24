#include <stdlib.h> 
#include <string.h> 
#include <iostream>
#include <bits/stdc++.h> 
#include <cstdlib>


int main( int argc, char* argv[] )
{


	//std::string command_path2 = "ls ";
	//int f_path2= system(command_path2.c_str());
	//std::cout<<"Install the keras-contrib.git "<<std::endl;
	//std::string command1 = "pip install git+https://www.github.com/keras-team/keras-contrib.git";
	//int f1= system(command1.c_str());

	std::string command_path2 = "ls ";
	int f_path2= system(command_path2.c_str());
	std::cout<<"Install the keras-contrib.git "<<std::endl;
	std::string command1 = "cd ../Initial_Segmentation/keras-contrib-master/; pip install .";
	int f1= system(command1.c_str());

	std::string command_path3 = "ls ";
	int f_path3= system(command_path3.c_str());
	std::cout<<"Install the deep-belief-network.git "<<std::endl;
	std::string command3 = "cd ../Initial_Segmentation/deep-belief-network-master/; pip install .";
	int f3= system(command3.c_str());

	std::cout<<"Install the auto_segm python version of the Deep learning Initial Part"<<std::endl;
	std::string command = "cd ../Initial_Segmentation/; pip install .";
	int f= system(command.c_str());





return 0;
}
