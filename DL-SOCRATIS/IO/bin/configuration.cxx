#include "configuration.h"
#include <fstream>

void configuration::read(const char * configFile) 
{
libconfig::Config cfg;
std::cout <<configFile<< endl;
cfg.readFile(configFile);
std::cout <<" ////////////////////////////////////////// "<< endl;
std::string name = cfg.lookup("title");
std::string author = cfg.lookup("author");
std::string email = cfg.lookup("email"); 
std::string license = cfg.lookup("license");

std::cout <<" "<< name <<" "<< endl;
std::cout <<" author: "<< author << endl;
std::cout <<" email: "<< email << endl; 
std::cout <<" license: "<< license << endl;
std::cout <<" ////////////////////////////////////////// "<< endl;
const Setting& root = cfg.getRoot();

if (deactivate_eval_user==false){
	cfg.lookupValue("evaluate_results",evaluate);
}
if (deactivate_segm_user==false){
	cfg.lookupValue("segment_data",segment);
}
if (deactivate_inter_user==false){
	cfg.lookupValue("interpolate_3D",interpolate);
}

if (deactivate_translation==false){
	cfg.lookupValue("translation",translation);
}


if (deactivate_init_segm_user==false){
	cfg.lookupValue("init_segment_data",init_segment);
	cfg.lookupValue("atlas",atlas);
	cfg.lookupValue("deeplearning",deeplearning);
}

if (deactivate_deform==false){
	cfg.lookupValue("deformation",deformation);
}

if (deactivate_sensitivity==false){
	cfg.lookupValue("sensitivity",sensitivity);
}

////////////////////evaluation///////////////////////////////////////////////
//const Setting& segmentation = root["segmentation"];




////////////////////translation///////////////////////////////////////////////
if (translation==true){

    const Setting& general = root["translation_space"]["general"];
	try{
	      general.lookupValue("initial_image_space",initial_space);
	      std::cout << "initial_image_space: " << initial_space << std::endl;
	      general.lookupValue("final_image_space",final_space);
	      std::cout << "final_image_space: " << final_space << std::endl;
	  }
	catch(const SettingNotFoundException &nfex)
	{}

    const Setting& paths = root["translation_space"]["paths"];
	try{
	      paths.lookupValue("trans_python_script",python_path_trans);
	      std::cout << "path python translation: " <<python_path_trans<< std::endl;

	      paths.lookupValue("store_path",store_trans_path);
	      std::cout << "store_path translation: " <<store_trans_path<< std::endl;


	  }
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}

}
////////////////////initial segmentation///////////////////////////////////////////////
if (init_segment==true){
const Setting& general = root["initial_segmentation"]["general"];
		try{
 general.lookupValue("heart_phase", heart_phase);
		  }
		catch(const SettingNotFoundException &nfex)
		{
		    // Ignore.
		}
	if (atlas==true){
	    const Setting& paths = root["initial_segmentation"]["atlas"];
		try{
		   // for(int i = 0; i < segmentation.getLength(); ++i)
		    //{
		      //const Setting &segmentation = segmentation[i];
		      paths.lookupValue("data_path",data_path);
		      std::cout << "data_results_path: " <<data_path<< std::endl;
		      paths.lookupValue("landmarks_path", landmarks_path);
		      std::cout  << "landmarks path: "<< landmarks_path  << std::endl;
		    //}

		  }
		catch(const SettingNotFoundException &nfex)
		{
		    // Ignore.
		}

	    const Setting& parameter = root["initial_segmentation"]["atlas"];
		try{
		     
		      parameter.lookupValue("type_of_input_data", type_of_input_data);

		  }
		catch(const SettingNotFoundException &nfex)
		{
		    // Ignore.
		}

	}

	if (deeplearning==true){
	    const Setting& paths = root["initial_segmentation"]["deeplearning"];
		try{
		   // for(int i = 0; i < segmentation.getLength(); ++i)
		    //{
		      //const Setting &segmentation = segmentation[i];
		      paths.lookupValue("store_data_path",store_data_path);
		      std::cout << "store_data_results_path: " <<store_data_path<< std::endl;
		      paths.lookupValue("configuration_file", conf_path);
		      std::cout  << "configuration_file: "<< conf_path  << std::endl;
		      paths.lookupValue("python_script", pyth_scr);
		      std::cout  << "python_script: "<< pyth_scr  << std::endl;
		 

		    //}

		  }
		catch(const SettingNotFoundException &nfex)
		{
		    // Ignore.
		}

	    const Setting& parameter = root["initial_segmentation"]["deeplearning"];
		try{
		   // for(int i = 0; i < segmentation.getLength(); ++i)
		    //{
		      //const Setting &segmentation = segmentation[i];
		      parameter.lookupValue("num_classes",num_classes);

		  }
		catch(const SettingNotFoundException &nfex)
		{
		    // Ignore.
		}

	}


  }

if (evaluate==true){


	const Setting& egeneral = root["evaluation"]["general"];
	try{

	      egeneral.lookupValue("number_of_experts",number_experts);
	      std::cout << "number_of_experts who participate to extract ground truth are: " <<number_experts<< std::endl;
	}
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}

	const Setting& epaths = root["evaluation"]["paths"];
	try{
	   // for(int i = 0; i < segmentation.getLength(); ++i)
	    //{


	      epaths.lookupValue("copy_from_store_scar_result",copy);
	      epaths.lookupValue("manual_labels_path",manual_path);
	      std::cout << "manual_labels_path: " <<manual_path<< std::endl;
	      epaths.lookupValue("predict_labels_path", predict_path);
	      std::cout << "predict_labels_path: " <<  predict_path << std::endl;
	      epaths.lookupValue("store_results_metric", store_metric_path);
	      std::cout  << "results store path: "<< store_metric_path  << std::endl;
	      const Setting & epatient = epaths.lookup("patient_folders");
	      const Setting & enb_image = epaths.lookup("number_of_images");


	 
	      num_patient=epatient.getLength();
		std::cout  << "Number of patient : "<< num_patient  << std::endl;
		if(num_patient==1){epatients.push_back(epatient[0]); enb_images.push_back(enb_image[0]);}
		if(num_patient>=1){
			      for (int n = 0; n < epatient.getLength(); ++n)
			      {
				  epatients.push_back(epatient[n]);
				  enb_images.push_back(enb_image[n]);
			      }

				}
	    //}

	  }
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}
      const Setting& eparameter = root["evaluation"]["parameter"];
	try{
	   // for(int i = 0; i < segmentation.getLength(); ++i)
	    //{

	      eparameter.lookupValue("rotation_pred_mask",rotation);
	      eparameter.lookupValue("scale_pred_mask",scale);
	      const Setting & ecenter = eparameter.lookup("center_pred_mask");
  	      for (int n = 0; n < ecenter.getLength(); ++n)
	      {
		  center.push_back(ecenter[n]);
	      }
	  }
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}
 }

////////////////////segmentation///////////////////////////////////////////////
if (segment==true){

    const Setting& general = root["segmentation"]["general"];
	try{
	      general.lookupValue("segmentation_case",segm_case);
	      std::cout << "segmentation_case: " << segm_case << std::endl;
	  }
	catch(const SettingNotFoundException &nfex)
	{}

    const Setting& paths = root["segmentation"]["paths"];
	try{
	   // for(int i = 0; i < segmentation.getLength(); ++i)
	    //{
	      //const Setting &segmentation = segmentation[i];
	      paths.lookupValue("myofolder_path",myo_path);
	      std::cout << "path myo exist: " <<myo_path<< std::endl;
	      paths.lookupValue("ROIfolder_path", roi_path);
	      std::cout << "path roi exist: " <<  roi_path << std::endl;
	      paths.lookupValue("store_scars_results_path", scar_path);
	      std::cout  << "results store path: "<< scar_path  << std::endl;
	      const Setting & patient = paths.lookup("patient_folders");
	      const Setting & nb_image = paths.lookup("number_of_images");


	 
	      num_patient=patient.getLength();
	      for (int n = 0; n < patient.getLength(); ++n)
	      {
		  patients.push_back(patient[n]);
		  nb_images.push_back(nb_image[n]);
	      }


	    //}

	  }
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}

    const Setting& parameter = root["segmentation"]["parameter"];
	try{
	   // for(int i = 0; i < segmentation.getLength(); ++i)
	    //{
	      //const Setting &segmentation = segmentation[i];
	      parameter.lookupValue("segm_border_zone_bool",segm_border_zone);
	      parameter.lookupValue("threshold_of_scar_intensity",th_scar);

	  }
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}

 }

if (interpolate==true){
    const Setting& ipaths = root["interpolation_3D"]["paths"];
	try{
	   // for(int i = 0; i < segmentation.getLength(); ++i)
	    //{
	      //const Setting &segmentation = segmentation[i];
	      ipaths.lookupValue("slice3D_manual_data",slice);

	      ipaths.lookupValue("manual_3Dlabel_path", threeD_path);
	      std::cout << "path 3D scars exist: " <<  threeD_path << std::endl;
	      ipaths.lookupValue("manual_labels_path", store2dslice_path);
	      std::cout  << "results store path: "<< store2dslice_path  << std::endl;
	      const Setting & ipatient = ipaths.lookup("patient_folders");
	      const Setting & inb_image = ipaths.lookup("number_of_images");
	      ipaths.lookupValue("interpolate_2d_to_3D_predict_data",interpolate3d);
	      ipaths.lookupValue("store_scars_results_path", store3D_path);
	      std::cout << "path store 3D volume exist: " <<  store3D_path << std::endl;

	 
	      num_patient=ipatient.getLength();
	      for (int n = 0; n < ipatient.getLength(); ++n)
	      {
		  ipatients.push_back(ipatient[n]);
		  inb_images.push_back(inb_image[n]);
	      }


	    //}

	  }
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}

 const Setting& ivparameter = root["interpolation_3D"]["parameter"];
	try{
	   // for(int i = 0; i < segmentation.getLength(); ++i)
	    //{
	      //const Setting &segmentation = segmentation[i];
	      ivparameter.lookupValue("visualize_3D_scars",threeD_scar);
	      ivparameter.lookupValue("visualize_3D_epi",threeD_epi);
	      ivparameter.lookupValue("visualize_3D_endo",threeD_endo);
	      ivparameter.lookupValue("gap_of_mri",gap_mri);
		ivparameter.lookupValue("alpha_vtkDelaunay3D",alpha_Delaunay3D);

	  }
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}



 }

if (deformation==true){

    const Setting& general2 = root["Deformation"]["general2"];
	try{
	      general2.lookupValue("deeplearning",deform_dl_seg);
	      std::cout << "deeplearning: " << deform_dl_seg << std::endl;
	  }
	catch(const SettingNotFoundException &nfex)
	{}
    const Setting& paths = root["Deformation"]["paths"];
	try{
	   // for(int i = 0; i < segmentation.getLength(); ++i)
	    //{
	      //const Setting &segmentation = segmentation[i];
	      paths.lookupValue("image_move",image_move);
	      std::cout << "image_move: " <<image_move<< std::endl;
	      paths.lookupValue("image_fix",image_fix);
	      std::cout << "image_fix: " <<image_fix<< std::endl;
	      paths.lookupValue("store_path",store_def_path);
	      std::cout << "store_path: " <<store_def_path<< std::endl;

	    //}

	  }
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}


 }

if (sensitivity==true){

    const Setting& general = root["Sensitivity"]["general"];
	try{

	      const Setting & iparameter = general.lookup("parameters");
	      const Setting & ilow_border = general.lookup("lower_border");
	      const Setting & ihigher_border = general.lookup("higher_border");	    
	      const Setting & iinitial_SD = general.lookup("initial_SD");


	      int num_param=iparameter.getLength();
	      for (int n = 0; n < num_param; ++n)
	      {
		  parameter_sens.push_back(iparameter[n]);
		  low_border.push_back(ilow_border[n]);
		  high_border.push_back(ihigher_border[n]);
		  initial_SD.push_back(iinitial_SD[n]);
	      }

	      general.lookupValue("sensitivity_mode",sensitivity_case);
	      std::cout << "sensitivity_mode: " << sensitivity_case << std::endl;

	  }
	catch(const SettingNotFoundException &nfex)
	{}
    const Setting& paths = root["Sensitivity"]["paths"];
	try{
	   // for(int i = 0; i < segmentation.getLength(); ++i)
	    //{
	      //const Setting &segmentation = segmentation[i];
	      paths.lookupValue("store_path_results",store_path_sensit);
	      std::cout << "store_path_results: " <<store_path_sensit<< std::endl;
	      

	    //}

	  }
	catch(const SettingNotFoundException &nfex)
	{
	    // Ignore.
	}


 }



std::cout <<" ////////////////////////////////////////// "<< endl;

}
