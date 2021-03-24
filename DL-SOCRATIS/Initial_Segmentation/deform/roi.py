#Author: Michail Mamalakis
#Version: 0.2
#Licence:
#email:mmamalakis1@sheffield.ac.uk

from __future__ import division, print_function
from auto_segm import main


mn=main.main('test','_insigneo_da')

# run test of main segmentation based on roi
mn.test_roi()

