//Copyright (c) 2009-2013 ConsortiumOfANTS
//All rights reserved.


#ifndef _interparameters_h_
#define _interparameters_h_

//V15 //from scratc the atlases part new preprosessing step smoth thresholding binary

#define PORTION_CYCLE_MASK_endo 0.2//depend of overestimate or underestimate of u-net or atlas(check it) 0.15 //0.2
#define PORTION_CYCLE_MASK_epi 0.75 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 0.5 //unet default 0.75
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 50 //45

#define max_radious_threshold 160
#define min_radious_threshold 25

#define FWHM_threshold_up 0.45 //scar core

#define FWHM_threshold_low 0.25 // boarder zone ditection 0.10 65.34% 0.05 65.18% lgescarv2 0,25

#define combine_FWHM_threshold_low 0.005 //change 0.1(16.7 v15), 0.5(12 v10), 0.25(9 v9), 0.75 (11 v8), 0.15(15.7 v1) 0.05 (16.7 v2) 0.005 (17.7 v3)

#define kmeanFWHM_threshold 0.45 //fix for myo 0.75 atlases.. dl 0.25
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 80 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 0.95 //1 67.23% myo 0.75
//PCA deformation strain image analysis
#define max_register_capacity 18

#endif

//Sensitivity tests

//LGE_v25

/*
atlas 40
LGE_40 : MASK_epi=0.75 70% (rerun with correct masks order)(run again atlases)
LGE_40b: MASK_epi=0.5  64 % 
LGE_40c:MASK_epi=0.25 68.61 (run now)
 
dl intra:
0.75 60% (here again with thickness constrain)
Dice:  0.605516+/-0.0849855
Sensitivity: 0.707748+/-0.157645
Specificity: 0.925242+/-0.0258681
ACD: 0.571512+/-0.586359
Hausdorff: 0.571512+/-0.586359
Median_total: 57.7661
Metric total result  of20 patients: 
Dice:  0.120742+/-0.0573969
Sensitivity: 0.160379+/-0.0857366
Specificity: 0.970542+/-0.0105863
ACD: 0.113179+/-0.116119
Hausdorff: 0.113179+/-0.116119
Median_total: 12.3992


0.5 53%
1 %    


#define PORTION_CYCLE_MASK_endo 0.2//depend of overestimate or underestimate of u-net or atlas(check it) 0.15 //0.2
#define PORTION_CYCLE_MASK_epi 0.75 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 0.25 //0.3
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 50 //45

#define max_radious_threshold 160
#define min_radious_threshold 25

#define FWHM_threshold_up 0.45 //scar core

#define FWHM_threshold_low 0.25 // boarder zone ditection 0.10 65.34% 0.05 65.18% lgescarv2 0,25

#define combine_FWHM_threshold_low 0.005 //change 0.1(16.7 v15), 0.5(12 v10), 0.25(9 v9), 0.75 (11 v8), 0.15(15.7 v1) 0.05 (16.7 v2) 0.005 (17.7 v3)

#define kmeanFWHM_threshold 0.45 //fix for myo 0.75 atlases.. dl 0.25
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 80 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 0.95 //1 67.23% myo 0.75
//PCA deformation strain image analysis
#define max_register_capacity 18

#endif
etric total result  of20 patients: 
Dice:  0.461578+/-0.180721
Sensitivity: 0.524205+/-0.243908
Specificity: 0.937226+/-0.0272936
ACD: 0.436679+/-0.448023
Hausdorff: 0.436679+/-0.448023
Median_total: 49.7979

Results:
Metric total result  of20 patients: 
Dice:  0.0679099+/-0.0379866
Sensitivity: 0.144505+/-0.135209
Specificity: 0.972576+/-0.0146554
ACD: 0.0634237+/-0.0650714
Hausdorff: 0.0634237+/-0.0650714
Median_total: 5.3697
Time to finish the segmentation: 126503


*/
//LGE_v30,
/*

#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it) 0.15 //0.2
#define PORTION_CYCLE_MASK_epi 0.50 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 0.25 //0.3
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 60 //45

#define max_radious_threshold 160
#define min_radious_threshold 25

#define FWHM_threshold_up 0.45 //scar core

#define FWHM_threshold_low 0.25 // boarder zone ditection 0.10 65.34% 0.05 65.18% lgescarv2 0,25

#define combine_FWHM_threshold_low 0.005 //change 0.1(16.7 v15), 0.5(12 v10), 0.25(9 v9), 0.75 (11 v8), 0.15(15.7 v1) 0.05 (16.7 v2) 0.005 (17.7 v3)

#define kmeanFWHM_threshold 0.45 //fix for myo 0.75 atlases.. dl 0.25
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 80 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 0.95 //1 67.23% myo 0.75
//PCA deformation strain image analysis
#define max_register_capacity 18

Results:
median value:   5.45362
Metric total result  of20 patients: 
Dice:  0.0693835+/-0.0491826
Sensitivity: 0.135316+/-0.0973157
Specificity: 0.972265+/-0.0133847
ACD: 0.0636325+/-0.0652855
Hausdorff: 0.0636325+/-0.0652855
Median_total: 5.45362
Time to finish the segmentation: 117476
median value:   45.5694
Metric total result  of20 patients: 
Dice:  0.4424+/-0.186662
Sensitivity: 0.469986+/-0.225229
Specificity: 0.944631+/-0.0279979
ACD: 0.422338+/-0.43331
Hausdorff: 0.422338+/-0.43331
Median_total: 45.5694





LGE_1
Metric total result  of20 patients: 
Dice:  0.0639674+/-0.0382794
Sensitivity: 0.155379+/-0.103855
Specificity: 0.967922+/-0.0180521
ACD: 0.0582105+/-0.0597227
Hausdorff: 0.0582105+/-0.0597227
Median_total: 5.95218

Metric total result  of20 patients: 
Dice:  0.526086+/-0.0981021
Sensitivity: 0.651834+/-0.123779
Specificity: 0.921862+/-0.0318958
ACD: 0.498979+/-0.511942
Hausdorff: 0.498979+/-0.511942
Median_total: 53.0599



*/




























//Sensitivity tests


//LGE_v24, 
/*

#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it) 0.15 //0.2
#define PORTION_CYCLE_MASK_epi 0.50 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 0.25 //0.3
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 60 //45

#define max_radious_threshold 180
#define min_radious_threshold 25

#define FWHM_threshold_up 0.45 //scar core

#define FWHM_threshold_low 0.25 // boarder zone ditection 0.10 65.34% 0.05 65.18% lgescarv2 0,25

#define combine_FWHM_threshold_low 0.005 //change 0.1(16.7 v15), 0.5(12 v10), 0.25(9 v9), 0.75 (11 v8), 0.15(15.7 v1) 0.05 (16.7 v2) 0.005 (17.7 v3)

#define kmeanFWHM_threshold 0.45 //fix for myo 0.75 atlases.. dl 0.25
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 80 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 0.95 //1 67.23% myo 0.75
//PCA deformation strain image analysis
#define max_register_capacity 18

results:

Metric total result  of20 patients: 
Dice:  0.060874+/-0.0502863
Sensitivity: 0.129236+/-0.130411
Specificity: 0.9744+/-0.012358
ACD: 0.0515692+/-0.0529089
Hausdorff: 0.0515692+/-0.0529089
Median_total: 4.06608
median value:   47.0928
Metric total result  of20 patients: 
Dice:  0.435724+/-0.183336
Sensitivity: 0.478153+/-0.216017
Specificity: 0.937939+/-0.0382383
ACD: 0.411463+/-0.422153
Hausdorff: 0.411463+/-0.422153
Median_total: 47.0928


Time to finish the segmentation: 126896

*/

//LGE_v21, 
/*
#define PORTION_CYCLE_MASK_endo 0.35//depend of overestimate or underestimate of u-net or atlas(check it) 0.15 //0.2
#define PORTION_CYCLE_MASK_epi 0.25 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 0.25 //0.3
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 60 //45

#define max_radious_threshold 170
#define min_radious_threshold 25

#define FWHM_threshold_up 0.45 //scar core

#define FWHM_threshold_low 0.25 // boarder zone ditection 0.10 65.34% 0.05 65.18% lgescarv2 0,25

#define combine_FWHM_threshold_low 0.005 //change 0.1(16.7 v15), 0.5(12 v10), 0.25(9 v9), 0.75 (11 v8), 0.15(15.7 v1) 0.05 (16.7 v2) 0.005 (17.7 v3)

#define kmeanFWHM_threshold 0.45 //fix for myo 0.75 atlases.. dl 0.25
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 80 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 0.95 //1 67.23% myo 0.75
//PCA deformation strain image analysis
#define max_register_capacity 18


results:

Metric total result  of20 patients: 
Dice:  0.0668831+/-0.0450602
Sensitivity: 0.14041+/-0.131139
Specificity: 0.973662+/-0.0118348
ACD: 0.0627456+/-0.0643756
Hausdorff: 0.0627456+/-0.0643756
Median_total: 7.69187
Time to finish the segmentation: 115155

median value:   38.2863
Metric total result  of20 patients: 
Dice:  0.393591+/-0.15988
Sensitivity: 0.394974+/-0.20303
Specificity: 0.947023+/-0.037816
ACD: 0.375022+/-0.384764
Hausdorff: 0.375022+/-0.384764
Median_total: 38.2863


*/


//LGE_v22 
/*

#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it) 0.15 //0.2
#define PORTION_CYCLE_MASK_epi 0.50 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 0.25 //0.3
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 50 //45

#define max_radious_threshold 170
#define min_radious_threshold 25

#define FWHM_threshold_up 0.45 //scar core

#define FWHM_threshold_low 0.25 // boarder zone ditection 0.10 65.34% 0.05 65.18% lgescarv2 0,25

#define combine_FWHM_threshold_low 0.005 //change 0.1(16.7 v15), 0.5(12 v10), 0.25(9 v9), 0.75 (11 v8), 0.15(15.7 v1) 0.05 (16.7 v2) 0.005 (17.7 v3)

#define kmeanFWHM_threshold 0.45 //fix for myo 0.75 atlases.. dl 0.25
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 80 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 0.95 //1 67.23% myo 0.75
//PCA deformation strain image analysis
#define max_register_capacity 18


results:
Dice:  0.0825037+/-0.0609292
Sensitivity: 0.16764+/-0.125389
Specificity: 0.9712+/-0.0146889
ACD: 0.0739757+/-0.0758975
Hausdorff: 0.0739757+/-0.0758975
Median_total: 6.64433
Time to finish the segmentation: 122407

Metric total result  of20 patients: 
Dice:  0.453193+/-0.184435
Sensitivity: 0.488676+/-0.220401
Specificity: 0.941013+/-0.0282049
ACD: 0.428693+/-0.43983
Hausdorff: 0.428693+/-0.43983
Median_total: 49.0003



*/


//LGE_v23, 
/*
#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it) 0.15 //0.2
#define PORTION_CYCLE_MASK_epi 0.50 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 0.25 //0.3
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 40 //45

#define max_radious_threshold 180
#define min_radious_threshold 25

#define FWHM_threshold_up 0.45 //scar core

#define FWHM_threshold_low 0.25 // boarder zone ditection 0.10 65.34% 0.05 65.18% lgescarv2 0,25

#define combine_FWHM_threshold_low 0.005 //change 0.1(16.7 v15), 0.5(12 v10), 0.25(9 v9), 0.75 (11 v8), 0.15(15.7 v1) 0.05 (16.7 v2) 0.005 (17.7 v3)

#define kmeanFWHM_threshold 0.45 //fix for myo 0.75 atlases.. dl 0.25
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 80 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 0.95 //1 67.23% myo 0.75
//PCA deformation strain image analysis
#define max_register_capacity 18

#endif



results:
Dice:  0.0690293+/-0.0523052
Sensitivity: 0.143163+/-0.124617
Specificity: 0.97338+/-0.0137523
ACD: 0.0592929+/-0.0608332
Hausdorff: 0.0592929+/-0.0608332
Median_total: 5.95494
Time to finish the segmentation: 123325
Metric total result  of20 patients: 
Dice:  0.445156+/-0.160543
Sensitivity: 0.475875+/-0.213191
Specificity: 0.943008+/-0.033404
ACD: 0.422579+/-0.433557
Hausdorff: 0.422579+/-0.433557
Median_total: 45.904





*/

//LGE_v4,
/*eep best result of epi ,change endo
/*
#define PORTION_CYCLE_MASK_endo 1 //depe
#define PORTION_CYCLE_MASK_endo 0 //depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 0.5 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.8 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 50
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.5
#define kmeanFWHM_threshold 0.45
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18
*/



//LGE_v5,
/*
#define PORTION_CYCLE_MASK_endo 0.5 //depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 1 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.8 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 50
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.5
#define kmeanFWHM_threshold 0.45
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18
*/

/*
//LGE_v6,
#define PORTION_CYCLE_MASK_epi 1 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.8 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 50
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.5
#define kmeanFWHM_threshold 0.45
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18
*/

//LGE_v7, 
/*
#define PORTION_CYCLE_MASK_endo 1.5 //depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 1 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.8 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 50
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.5
#define kmeanFWHM_threshold 0.45
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18
*/

//LGE_v8, 
/*
#define PORTION_CYCLE_MASK_endo 2 //depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 1 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.8 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 50
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.5
#define kmeanFWHM_threshold 0.45
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18
*/

/*
LGE_v9
rebuild myo
#define PORTION_CYCLE_MASK_endo 0//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 1.0 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
define PORTION_OF_MIDDLE_CICLE_CROP 0.8 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 50
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.5
#define kmeanFWHM_threshold 0.25
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18

*/




/*
LGE_v10 build, ED
//rebuild myo
#define PORTION_CYCLE_MASK_endo 0//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 1.0 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.1 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 35
#define FWHM_threshold_low 0.4
#define combine_FWHM_threshold_low 0.4
#define kmeanFWHM_threshold 0.6
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
#define max_register_capacity 18

*/



/*
LGE_v11
//rebuild myo
#define PORTION_CYCLE_MASK_endo 0//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 1.0 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 40
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.35
#define kmeanFWHM_threshold 0.45
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18

*/


////////////////////////////////////////////

/*
LGE_v12 build3, ES
//rebuild myo
#define PORTION_CYCLE_MASK_endo 0//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 1.0 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.1 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 35
#define FWHM_threshold_low 0.65
#define combine_FWHM_threshold_low 0.65
#define kmeanFWHM_threshold 0.6
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
#define max_register_capacity 18

*/

//v13
/*
#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 2.5 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 65
#define percent_Threshold_initial_thin_of_myo 40
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.35
#define kmeanFWHM_threshold 0.45
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18
*/

/* 
V14


#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 2.5 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 45
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.4
#define kmeanFWHM_threshold 0.3
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 80 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18

#endif
*/





/* 
V15 //from scratc the atlases part new preprosessing step smoth thresholding binary

#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 2.5 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 45
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.4
#define kmeanFWHM_threshold 0.7
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 80 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18

#endif
*/

//scar_v1 TR and TG no rrg
/*
#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 2.5 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 45
#define FWHM_threshold_low 0.55
#define combine_FWHM_threshold_low 0.3
#define kmeanFWHM_threshold 0.7
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18

#endif
*/
//scar_v2
/*
#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 2.5 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 45
#define FWHM_threshold_low 0.75
#define combine_FWHM_threshold_low 0.3
#define kmeanFWHM_threshold 0.7
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 230 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18

#endif
*/

//scar_v3
/*
#define PORTION_CYCLE_MASK_endo 0.25//depend of overestimate or underestimate of u-net or atlas(check it)
#define PORTION_CYCLE_MASK_epi 2.5 //depend of overestimate or underestimate of u-net or atlas(check it) //atlas default 2.8 //unet default 1
#define PORTION_OF_MIDDLE_CICLE_CROP 0.15 //depend of overestimate or underestimate of u-net or atlas(check it)
#define loop_rebuild 0        // if 0 the THRESHOLD_Intensity_rebuild_loop does not used it has to be !=0
#define THRESHOLD_Intensity_rebuild_loop 80
#define percent_Threshold_initial_thin_of_myo 45
#define FWHM_threshold_low 0.45
#define combine_FWHM_threshold_low 0.3
#define kmeanFWHM_threshold 0.7
//segmentation
#define PRE_PROCESSING_BINARY_THRESHOLD 50 
#define PORTION_OF_SCARS_SEGMENT_SEEDPOINTS 1
//PCA deformation strain image analysis
#define max_register_capacity 18

#endif
*/

