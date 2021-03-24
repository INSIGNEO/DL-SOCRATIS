#!/bin/bash
# Shell Script to Update GeoIP Country database in each WebServer Jail defined by $TDIRS
# -------------------------------------------------------------------------
# Copyright (c) 2007 Vivek Gite <vivek@nixcraft.com>
# This script is licensed under GNU GPL version 2.0 or above
# -------------------------------------------------------------------------
# This script is part of nixCraft shell script collection (NSSC)
# Visit http://bash.cyberciti.biz/ for more information.
# -------------------------------------------------------------------------



for f in {1..20};
do 

python3 evaluate_code.py ../../DATA/Results/manual/inter/all_first/ED/mask_manual/ground_truth/scar/"$f" ../../DATA/Results/manual/inter/all/ED/mask_manual/ground_truth/scar/"$f";
done;

#done

#chmod +x hello.sh
#./hello.sh
