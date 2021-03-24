#Author: Michail Mamalakis
#Version: 0.2
#Licence:
#email:mmamalakis1@sheffield.ac.uk

from __future__ import division, print_function
from auto_segm import main


mn=main.main('test','ESepi_unet_4')

# run test of main segmentation based on roi 'ESendo_unet_4' lge_unet_ss_myo_ft_nroi_nsd
mn.test_main()
#mn.test_roi('on')
