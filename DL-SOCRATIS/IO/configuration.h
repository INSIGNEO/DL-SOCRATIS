#ifndef configuration_h
#define configuration_h


//#include <string>
#include <vector>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

using namespace std;
using namespace libconfig;

class configuration {

public:
configuration(){};
~configuration(){};


//public variablies
bool init_segment=false;
bool atlas=false;
bool deeplearning=false;
bool segment=false;
bool evaluate=false;
bool interpolate=false;
bool deformation=false;
bool sensitivity=false;
bool translation=false;

bool copy=false;
bool slice=false;
bool interpolate3d=false;
bool deactivate_eval_user=false;
bool deactivate_segm_user=false;
bool deactivate_inter_user=false;
bool deactivate_init_segm_user=false;
bool deactivate_deform=false;
bool deactivate_sensitivity=false;
bool deform_dl_seg=false;
bool deactivate_translation=false;


std::string  data_path;
std::string  landmarks_path;
std::string  type_of_input_data;
std::string  heart_phase;

std::string  python_path_trans;
std::string  initial_space;
std::string  final_space;
std::string  store_trans_path;



std::string    store_data_path;
std::string  conf_path;
std::string    pyth_scr;
int num_classes;


vector<string>  patients;
std::string    myo_path;
std::string    roi_path;
std::string  scar_path;
vector<int> nb_images;
bool segm_border_zone;
int th_scar;
std::string segm_case;

vector<string>  epatients;
std::string    manual_path;
std::string    predict_path;
vector<int> enb_images;
std::string store_metric_path;
double rotation;
std::vector<int> center;
double scale;
int number_experts;


vector<string>  ipatients;
std::string    threeD_path;
std::string    store2dslice_path;
std::string    store3D_path;
vector<int> inb_images;
vector<bool>  threed_vision;
bool    threeD_scar;
bool    threeD_endo;
bool    threeD_epi;
int gap_mri;
double alpha_Delaunay3D;


int num_patient;
vector<string> segmentation_paths;
vector<string> evaluation_paths;
vector<string> interpolate_paths;
vector<string> init_segmentation_paths;
vector<string> atlas_paths;



std::vector<string> deform_images;
std::string    image_move;
std::string    image_fix;
std::string    store_def_path;


vector<bool> parameter_sens;
vector<float> low_border;
vector<float> high_border;
vector<float> initial_SD;
std::string    sensitivity_case;
std::string    store_path_sensit;
std::vector<string> sensitivity_string;
//public functions

void read(const char * configFile);





//Get functions
inline bool Get_translation_space()
{return translation;
}
inline bool Get_init_segm(){
return init_segment;
}

inline bool Get_dl_segm(){
return deeplearning;
}

inline bool Get_atlas_segm(){
return atlas;
}

inline bool Get_eval(){
return evaluate;
}
inline bool Get_segm(){
return segment;
}
inline bool Get_inter(){
return interpolate;
}

inline bool Get_deform_reg(){
return deformation;
}

inline bool Get_deform_dl_seg(){
return deform_dl_seg;
}

inline bool Get_sens(){
return sensitivity;
}

//Set functions
inline  void Set_eval(bool value){
deactivate_eval_user=true;
evaluate=value;
}
inline  void Set_segm(bool value){
deactivate_segm_user=true;
segment=value;
}

inline  void Set_init_segm(bool value){
deactivate_init_segm_user=true;
init_segment=value;
}

inline  void Set_init_segm_dl(bool value){
deactivate_init_segm_user=true;
init_segment=value;
deeplearning=value;
}

inline  void Set_inter(bool value){
deactivate_inter_user=true;
interpolate=value;
}

inline  void Set_deform(bool value){
deactivate_deform==true;
deformation=value;
}

inline  void Set_sensitivity(bool value){
deactivate_sensitivity==true;
sensitivity=value;
}

//sensitivity

inline vector<string> Get_sensitivity_strings(){
sensitivity_string.push_back(sensitivity_case);
sensitivity_string.push_back(store_path_sensit);
return sensitivity_string;
}

inline vector<float> Get_sensitivity_low(){
return low_border;
}

inline vector<float> Get_sensitivity_high(){
return high_border;
}

inline vector<float> Get_sensitivity_SD(){
return initial_SD;
}

inline vector<bool> Get_sensitivity_parameters(){
return parameter_sens;
}

//translation

inline  void Set_translation(bool value){
deactivate_translation==true;
translation=value;
}


inline std::string Get_init_space(){
return initial_space;
}


inline std::string Get_final_space(){
return final_space;
}

inline std::string Get_trans_paths(){
return python_path_trans;
}

inline std::string Get_store_paths(){
return store_trans_path;
}

//init segm

inline vector<string> Get_init_segmentation_paths(){
init_segmentation_paths.push_back(store_data_path);
init_segmentation_paths.push_back(conf_path);
init_segmentation_paths.push_back(pyth_scr);
return init_segmentation_paths;
}

inline vector<string> Get_atlas_paths(){
atlas_paths.push_back(data_path);
atlas_paths.push_back(landmarks_path);
return atlas_paths;
}

inline vector<string> Get_deform_images(){
deform_images.push_back(image_fix);
deform_images.push_back(image_move);
return deform_images;
}



inline string Get_store_deform_path(){
return store_def_path;
}


inline string Get_heart_phase(){return heart_phase;}
inline string Get_type_of_input_data(){return type_of_input_data;}

inline int Get_num_classes(){return num_classes;}

//segmentation
bool Get_segmentation_border_zone_bool(){
return segm_border_zone;
}
inline vector<string> Get_segmentation_paths(){
segmentation_paths.push_back(myo_path);
segmentation_paths.push_back(roi_path);
segmentation_paths.push_back(scar_path);
segmentation_paths.push_back(segm_case);
return segmentation_paths;
}

inline vector<string> Get_patients_paths(){
return patients;
}

inline vector<int> Get_patients_images_number(){
return nb_images;
}

inline int Get_number_of_patients(){
return num_patient;
}
inline int Get_TH_Scar_intens(){
return th_scar;
}
//evaluation
inline bool Get_copy(){
return copy;
}

inline double Get_eval_rotation(){
return rotation;
}
inline double Get_eval_scale(){
return scale;
}
inline std::vector<int>  Get_eval_center(){
return center;
}


inline int Get_evaluation_number_experts(){
return number_experts;
}

inline vector<string> Get_evaluation_paths(){
evaluation_paths.push_back(manual_path);
evaluation_paths.push_back(predict_path);
evaluation_paths.push_back(store_metric_path);
return evaluation_paths;
}

inline vector<string> Get_eval_patients_paths(){
return epatients;
}

inline vector<int> Get_eval_patients_images_number(){
return enb_images;
}

//interpolation
inline bool Get_slice(){
return slice;
}
inline bool Get_interpolate(){
return interpolate3d;
}

inline vector<string> Get_interpolate_paths(){
interpolate_paths.push_back(threeD_path);
interpolate_paths.push_back(store2dslice_path);
interpolate_paths.push_back(store3D_path);
return interpolate_paths;
}


inline vector<bool> Get_3D_vision(){
threed_vision.push_back(threeD_scar);
threed_vision.push_back(threeD_endo);
threed_vision.push_back(threeD_epi);
return threed_vision;
}

inline int Get_gap_mri(){return gap_mri;}

inline vector<string> Get_inter_patients_paths(){
return ipatients;
}

inline double Get_alpha(){
return alpha_Delaunay3D;
}


inline vector<int> Get_inter_patients_images_number(){
return inb_images;
}
};

#endif
