#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk
#Acknowledgement: https://github.com/chuckyee/cardiac-segmentation

from __future__ import division, print_function

import os
import argparse
import configparser
import logging
from typing import List

definitions = [

######################################## Solver of Optimization #############################################################################################
	('ram',            (str,   'GPU', "CPU or GPU processes.")),
	('ngpu',            (int,   '1', "Number of GPU processors (keras at most 8).")),
	('metrics',            (str,   'accuracy', "Images input shape.")),
	('metrics1',            (str,   'dice', "Images input shape.")),
	('metrics2',            (str,   '', "Images input shape.")),
	('batch_size',            (int,   16, "Batch size of train.")),
	('batch_size_test',            (int,   8, "Batch size of test.")),
	('epochs_pre',            (int,   16, "Epochs of pre analysis.")),
	('epochs_roi',            (int,   16, "Epochs of roi analysis.")),
	('epochs_main',            (int,   16, "Epochs of main analysis.")),
	('num_cores',            (int,   1, "Numer of cores.")),
	('store_results',       (str, 'on',  " (on,off)  Store in txt all the mean and SD of each image if a big data set does not regomented ")),
	('validation',            (str,   'off', "If the data split with validation data.")),
	('validation_split',            (float,   0.4, "The rate of validation data.")),
	('crossval_cycle',            (int,   4, "Numerber of cycle of cross validation.")),
	('early_stop',            (str,   'False', "Use the early stop analysis.")),
	('label_case_extension_1',            (str,   '-icontour-manual.txt', "write the extention of the contour label here i for endo o for epi cardium.")),
	('label_case_extension_2',            (str,   '-ocontour-manual.txt', "write the extention of the contour label here i for endo o for epi cardium.")),
	('label_classes',            (str,   'both', "define the number of classes to simulate both: i, first: only i, second: only o.")),
	('monitor_callbacks',            (str,   'loss', "Callbacks save metric acc, loss, val_loss, val_acc.")),
	('mode_convert',            (str,   'min', "Callbacks mode convert max, min, auto.")),
	('gan_synthetic',            (str,   'False', "Gan synthtetic images.")),
	('num_synthetic_images',            (int,   100, "Number of synthetic images.")),
	('fft_convert_data',            (str,   'on', "fft convert of the data images.")),
######################################## Path and Data ######################################################################################################

    # Path
	('store_txt',        (str,	'/home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/Initial_segmentation/Initial_segmentation_Keras/code/python_source/Model', "Path of store models, solvers.")),
	('store_data_test',        (str, '/home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/Initial_segmentation/Initial_segmentation_Keras/code/python_source/Data', "Path of store models, solvers.")),
	('datapath',          (str,	'/home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/Initial_segmentation/Initial_segmentation_Keras/code/python_source/Data/Input',    "Root of images-labels for segmentation.")), 
	('gan_train_directory',          (str,	'/home/michail/Desktop/PHD_BASIC_WORK_FILES/PHD/case_study/scedule_work/feasibility_of_project/SEGMENTATION_PIPELINE/CODE/Initial_segmentation/Initial_segmentation_Keras/code/python_source/Data/GAN',    "Root of Gan synthetic images.")),
   # Data
	('patient_list',             (str,   'classification_of_data.json', "dataset .json file the name of patient list")),
	('patient_store_style',             (str,   'MICCAI_2009', "Its the way the data of patient's mask and dcm are stored choices: MICCAI_2009, MICCAI_2017, use( mask and dcm in the same path of the specific patient see example in the /Data/ROI path ")),
	#pre dataset
	('counter_extention_pre',             (str,   'txt', "Write the extention of counter txt, vtk ,jpeg...")),
	('data_extention_pre',             (str,   'dcm', "Write the extention of image txt, vtk ,jpeg...")),
	('image_shape_pre',            (int,   64, "images input shape")),
	('original_image_shape_pre',            (int,   256,      "Original shape of images.")),
	('roi_shape_pre',      (int, 32,    "Shape of the ROI image")),
	('pretrain_window',      (int, 11,    "Shape of window for the pre-train ROI image")),
	#roi dataset
	('counter_extention_roi',             (str,   'txt', "Write the extention of counter txt, vtk ,jpeg...")),
	('data_extention_roi',             (str,   'dcm', "Write the extention of image txt, vtk ,jpeg...")),
	('image_shape_roi',            (int,   64, "images input shape")),
	('original_image_shape_roi',            (int,   256,      "Original shape of images.")),
	('roi_shape_roi',      (int, 32,    "Shape of the ROI image")),
	#main dataset
	('counter_extention',             (str,   'txt', "Write the extention of counter txt, vtk ,jpeg...")),
	('data_extention',             (str,   'jpeg', "Write the extention of image txt, vtk ,jpeg...")),
	('image_shape',            (int,   284, "images input shape")),
	('original_image_shape',            (int,   284,      "Original shape of images.")),
	('roi_shape',      (int, 100,    "Shape of the ROI image")),
	# main restore data
	('restore_image',      (str, 'off',    "Run the main simulation base the stored image in data_extention choice")),
######################################## Data Augmentation######################################################################################################
	('data_augm',            (str,   'True', "Data augmentation classic, elastic deformation, or noise true or false.")),
	('random_apply_in_batch',            (str,   'False', "apply random the Data augmentation in each batch true or false.")),
	('data_augm_classic',            (str,   'True', "Data augmentation translation etc. true or false.")),
	('rotation_range',     (int, 1,    "Rotation range (0-180 degrees)")),
	('width_shift_range',  (float, 0.1,    "Width shift range, as a float fraction of the width")),
	('height_shift_range', (float, 0.1,    "Height shift range, as a float fraction of the height")),
#	('shear_range',        (float, 0.1,    "Shear intensity (in radians)")),
	('zca_whitening',            (str, 'False', "Apply ZCA whitening.")),
	('featurewise_center',            (str, 'False', "Set input mean to 0 over the dataset.")),
	('samplewise_center',            (str, 'False', "Set each sample mean to 0.")),
	('featurewise_std_normalization',            (str, 'False', "Divide inputs by std of the dataset.")),
	('samplewise_std_normalization',            (str, 'False', "Divide each input by its std.")),
	('horizontal_flip',            (str, 'False', "Randomly flip images.")),
	('vertical_flip',            (str, 'False', "Randomly flip images.")),
#TODO write the code for this augmented data
	('zoom_range',         (float, 0.05,   "Amount of zoom. If a scalar z, zoom in [1-z, 1+z]. Can also pass a pair of floats as the zoom range.")),
	('fill_mode',          (str,   'nearest', "Points outside boundaries are filled according to mode: constant, nearest, reflect, or wrap")),
	('alpha',              (float, 500,    "Random elastic distortion: magnitude of distortion")),
	('sigma',              (float, 20,     "Random elastic distortion: length scale")),
	('normalize', (str, False,  "Subtract mean and divide by std dev from each image.")),
	('noise', (str, False,  "Add salt and pepper noise to the image.")),
	('max_loops',              (int, 1,     "The loop of augmented data that will be created")),

	('shuffle', (str, False, "Shuffle images in each epoch.")),
####################################### Model net ###############################################################################################################

#	('model_exist', (str,'false' ,    "will you use your own prototxt")),
	('load_weights_roi', (str, '', "Model roi weights to initialize training( /Model dir`).")),
	('load_weights_main', (str, '', "Model main weights to initialize training(/Model dir).")),
	('loss_weights',     (list,  [0.1, 0.9], "When using dice or jaccard loss, how much to weight each output class.")),
	('loss_pre',  (str, 'customized_loss',    "Loss type.")),
	#roi
	('roi_model',  (str, 'deep_conv',    "Roi model of analysis.")),
	('roi_activation',  (str, 'relu',    "Kind of activation of net.")),	
	('loss_roi',  (str, 'mean_squared_error',    "Loss type.")),	
	('pre_optimizer',  (str, 'adam',    "pre optimizer.")),	
	('roi_optimizer',  (str, 'adam',    "roi optimizer.")),
	('roi_learning_rate',  (float, None ,    "Optimizer learning rate.")),
	('roi_momentum',  (float, None ,    "Momentum for SGD optimizer.")),
	('roi_decay',  (float, None ,    "Learning rate decay (not applicable for nadam).")),
	('roi_seed',  (int, None ,    "Seed for numpy RandomState")),
	#main
	('main_model',     (str, 'u_net',    "Main model of analysis.")),
	('max_norm_const',     (str, 'off',    "U-net parameter of constarin max norm on, off.")),
	('max_norm_value',     (float, 3,    "U-net parameter of constarin max norm value.")),
	('main_activation',  (str, 'relu',    "activation of main of unet.")),
	('loss_main',  (str, 'customized_loss',   "Loss type.")),	
	('m_optimizer',  (str, 'adam',   "Optimizer: sgd, rmsprop, adagrad, adadelta, adam, adamax, or nadam..")),	
	('height',  (int, 284,    "height of unet.")),
	('width',  (int, 284,    "width of unet.")),
	('classes',  (int, 2,    "classes of unet.")),
	('features',  (int, 128,    "Input size of features of unet.")),
	('depth',  (int, 4,    "Depth of unet.")),	
	('channels',  (int, 1,    "channels of unet RGB=3 Grey=1 unet.")),
	('dropout',  (float, 0.0,    "Dropout of unet.")),
	('batchnorm',  (str, 'False',    "Batch normalization of unet")),	
	('padding',  (str, 'valid' ,    "Padding of unet")),	
	('learning_rate',  (float, None ,    "Optimizer learning rate.")),
	('momentum',  (float, None ,    "Momentum for SGD optimizer.")),
	('decay',  (float, None ,    "Learning rate decay (not applicable for nadam).")),
	('seed',  (int, None ,    "Seed for numpy RandomState")),
]


noninitialized = {
	'learning_rate': 'getfloat',
	'momentum': 'getfloat',
	'decay': 'getfloat',
	'seed': 'getint',
	'roi_learning_rate': 'getfloat',
	'roi_momentum': 'getfloat',
	'roi_decay': 'getfloat',
	'roi_seed': 'getint',
}

def update_from_configfile(args, default, config, section, key):
    # Point of this function is to update the args Namespace.
	value = config.get(section, key)
	if value == '' or value is None:
		return

    # Command-line arguments override config file values
	if getattr(args, key) != default:
		return

    # Config files always store values as strings -- get correct type
	if isinstance(default, bool):
		value = config.getboolean(section, key)
	elif isinstance(default, int):
		value = config.getint(section, key)
	elif isinstance(default, float):
		value = config.getfloat(section, key)
	elif isinstance(default, str):
		value = config.get(section, key)
	elif isinstance(default, list):
        # special case (HACK): loss_weights is list of floats
		string = config.get(section, key)
		value = [float(x) for x in string.split()]
	elif default is None:
        # values which aren't initialized
		getter = getattr(config, noninitialized[key])
		value = getter(section, key)
	setattr(args, key, value)

def parse_arguments():
	parser = argparse.ArgumentParser(
	description="Train U-Net to segment left ventricle from cardiac "
	"MRI images.")

	for argname, kwargs in definitions:
		d = kwargs
		if isinstance(kwargs, tuple):
			d = dict(zip(['type', 'default', 'help'], kwargs))
			parser.add_argument('--' + argname, **d)

    # allow user to input configuration file
	parser.add_argument(
		'configfile', nargs='?', type=str, help="Load options from config "
		"file (command line arguments take precedence).")

	args = parser.parse_args()

	if args.configfile:
		logging.info("Loading options from config file: {}".format(args.configfile))
		config = configparser.ConfigParser(
		inline_comment_prefixes=['#', ';'], allow_no_value=True)
		config.read(args.configfile)
		for section in config:
			for key in config[section]:
				if key not in args:
					raise Exception("Unknown option {} in config file.".format(key))
				update_from_configfile(args, parser.get_default(key),
						config, section, key)
	for k,v in vars(args).items():
		logging.info("{:25s} = {}".format(k, v))

	return args
