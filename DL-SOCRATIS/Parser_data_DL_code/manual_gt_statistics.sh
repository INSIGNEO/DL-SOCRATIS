#!/bin/bash
# Shell Script to Update GeoIP Country database in each WebServer Jail defined by $TDIRS
# -------------------------------------------------------------------------
# Copyright (c) 2007 Vivek Gite <vivek@nixcraft.com>
# This script is licensed under GNU GPL version 2.0 or above
# -------------------------------------------------------------------------
# This script is part of nixCraft shell script collection (NSSC)
# Visit http://bash.cyberciti.biz/ for more information.
# -------------------------------------------------------------------------



for f in {1..5};
do 

python3 evaluate_code.py ../../DATA/LGE_test/lgegt/"$f" ../../DATA/Results/LGE_test/myo/"$f"

done

#done

#chmod +x hello.sh
#./hello.sh
