#Author: Michail Mamalakis
#Version: 0.2
#Licence:
#email:mmamalakis1@sheffield.ac.uk

from __future__ import division, print_function
from auto_segm import main



mn=main.main('test','test')
# run roi define
mn.cycle_gan(['--dataset_dir=../Initial_Segmentation/auto_segm/CycleGAN/datasets/LGE2cine_trans/', '--checkpoint_dir=../Initial_Segmentation/auto_segm/CycleGAN/', '--save_point_path=ckeckpoint', '--phase=test', '--which_direction=AtoB',  '--use_resnet=True','--test_dir=../data_internal/Input/cgan_images/image/'])



