#ifndef unbiased_ground_trouth_h
#define unbiased_ground_trouth_h


#include "kmeans.h"
#include "denoising.h"
#include "BinaryImage.h"
#include <vector>
#include <stdlib.h> 
#include <string.h> 
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

#include <memory>  
#include <algorithm>

#include <bits/stdc++.h> 
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
#include <boost/filesystem.hpp>

#include "Segmentation_Pipeline.h"
#include "Atlas_segm.h"
#include "Deformation.h"
#include "Resample.h"

#include "itkImageMomentsCalculator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

using namespace std;

class unbiased_ground_trouth {

public:
unbiased_ground_trouth(std::string m_root){root=m_root;};
~unbiased_ground_trouth(){};


//public variablies
bool init_segment=false;

std::string  data_path;
int num_classes;
vector<string>  patients;
double rotation;
std::vector<int> center;

//public functions

void Run(std::string manual_labels_path, std::string store_image_path, int size_of_experts, std::string store_label);

void preprocessing(std::string images_path, std::string store_path, std::string label);

void slice2dimages(std::string store_data, std::string imagenii);

void compute(std::string path_labels , int size_expert, std::string patient_names );

//void create_binary_Image(const char * m_inputFileName, const char * m_outputFileName);
inline void Set_heart_phase(std::string m_heart_phase){heart_phase=m_heart_phase;};
//inline void Set_type_of_input_data(std::string m_type_of_input_data){type_of_input_data=m_type_of_input_data;};	
//inline void Store(std::string m_store_myo_path,std::string m_store_roi_path){store_myo_path=m_store_myo_path; store_roi_path=m_store_roi_path;};
std:: vector<string> read_images_folder(std::string mlp_expert, int return_case,bool everything_read);
void Set_root_path(std::string path_root){root= path_root;};
std::vector<double> align_images(std::string image, double center[2], bool compute_center_only);
void prepare_run_ROI_DL(std::string store_image_path , std::string mask_compute_myo_new, std::string  mask_compute_roi_new);
private:


std::string root="";

//std::string store_myo_path;
//std::string store_roi_path;
std::string type_of_input_data="dcm";
std::string heart_phase;
vector<string> folders_name;
vector<int> patient_number;
std::string json_roi_store_path="../Initial_Segmentation/Model/";
std::string store_roi_image_path="../data_internal/Data/ROI/";
std::string store_image_path="../data_internal/Atlas/";
std::string atlas_roi_run="../Initial_Segmentation/atlas/";
std::string path_of_med2jpg="../Initial_Segmentation/multi_atlas/CIMAS/med2image-master/bin/";
std::string path_of_atlas="../Initial_Segmentation/multi_atlas/CIMAS/";
};






#endif
