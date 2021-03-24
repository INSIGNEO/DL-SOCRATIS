#Author: Michail Mamalakis
#Version: 0.2
#Licence:
#email:mmamalakis1@sheffield.ac.uk

from __future__ import division, print_function
from auto_segm import main


mn=main.main('test','test')
# run roi define
mn.cycle_gan(['--dataset_dir=../auto_segm/CycleGAN/datasets/LGE2cine_trans', '--use_resnet=True', '--phase=test' ,'--which_direction=BtoA'])



