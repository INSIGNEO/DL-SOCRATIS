#!/bin/bash
# Shell Script to Update GeoIP Country database in each WebServer Jail defined by $TDIRS
# -------------------------------------------------------------------------
# Copyright (c) 2007 Vivek Gite <vivek@nixcraft.com>
# This script is licensed under GNU GPL version 2.0 or above
# -------------------------------------------------------------------------
# This script is part of nixCraft shell script collection (NSSC)
# Visit http://bash.cyberciti.biz/ for more information.
# -------------------------------------------------------------------------



e=0
 
#for gg in ./../../../DATA/LGE_test/LGE_manual_35_TestData/*;



let e=e+1
#mkdir "$gg"/contour
#for f in "$gg"/*.jpg;
for c in {001..700};
do
x=0;
for f in ~/Desktop/Sheffield_COVID/COVID19/Validation/"$c"/*;
	do
	
	
	#d="$gg"


	#for g in "$f"/ 
	#do 
	#echo "$f";
		#let  x=x+1
	#let x=x+1;
	#./dcm2niix  -o "$gg"/ -f dcm_nii_start"$c" -z y "$gg"/;
	 
  

	med2image -i "$f" -d ~/Desktop/Sheffield_COVID/COVID19/Validation/"$c" -o seg_"$e"_"$c"_"$x".jpg ;
 	done


done

#chmod +x hello.sh
#./hello.sh
