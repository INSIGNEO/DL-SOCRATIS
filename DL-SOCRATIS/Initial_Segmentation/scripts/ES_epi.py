#Author: Michail Mamalakis
#Version: 0.2
#Licence:
#email:mmamalakis1@sheffield.ac.uk

from __future__ import division, print_function
from auto_segm import main


mn=main.main('test','ESepi_unet_1') #batch

# run test of main segmentation based on roi  ESepi_unet_2  
mn.test_main()
#mm.test_roi('on') 
