
#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk
#Acknowledgement: https://github.com/ece7048/cardiac-segmentation-1/blob/master/rvseg/loss.py


from __future__ import division, print_function
from keras.losses import mean_squared_error, categorical_crossentropy, sparse_categorical_crossentropy, binary_crossentropy, kullback_leibler_divergence
from keras import backend as K
from keras.optimizers import SGD, RMSprop, Adagrad, Adadelta, Adam, Adamax, Nadam
from auto_segm import config, regularization, handle_data
from keras.preprocessing.image import ImageDataGenerator
from auto_segm.regularization import data_augmentation
from keras import losses, optimizers, utils, metrics
from keras.utils import multi_gpu_model, np_utils
from keras.callbacks import ModelCheckpoint, EarlyStopping, Callback, LearningRateScheduler
from scipy.spatial.distance import directed_hausdorff
import tensorflow as tf
from math import ceil
import numpy as np
import cv2
import os
import argparse
from keras.objectives import *
from keras import utils
#from niftynet.evaluation.pairwise_measures import PairwiseMeasures
#from niftynet.evaluation.segmentation_evaluator import SegmentationEvaluator
#from niftynet.evaluation.base_evaluator import CachedSubanalysisEvaluator
from auto_segm.utils import losses_distance

def binary_images(Y):
	##########Y=np.array(Y.eval())
	if (np.max(Y)>1):
		Y=np.array(Y)/np.max(Y)
	# be sure all the classes are equal to 1 
	pos_pred_mask = np.array(np.where(Y > 0.5))
	a,b,rows_mask=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2]
	Y[a,b,rows_mask]=1

	pos_pred_mask2 = np.array(np.where(Y <= 0.5))
	a2,b2,rows_mask2=pos_pred_mask2[0],pos_pred_mask2[1],pos_pred_mask2[2]
	Y[a2,b2,rows_mask2]=0.0
	
	Yout=np.array(Y, dtype='int')
	Yout=Yout.reshape(np.array(Yout).shape[0],np.array(Yout).shape[1],np.array(Yout).shape[2],1)
	print(Yout.shape)
	return Yout

class run_model:
##################################################### INITIALIZATION ##########################################################
	def __init__ (self, path_case ) :
		
		args = config.parse_arguments()
		self.model_name=args.main_model
		self.ram= args.ram
		self.cross_validation_number=args.crossval_cycle
		self.ngpu=args.ngpu		
		self.metrics=args.metrics
		self.metrics1=args.metrics1
		self.metrics2=args.metrics2		
		self.batch_size=args.batch_size
		self.batch_size_test=args.batch_size_test
		self.epochs_roi= args.epochs_roi
		self.epochs_main= args.epochs_main
		self.num_cores=args.num_cores
		self.path_case=path_case
		self.validation_split=args.validation_split
		self.validation=args.validation	
		self.shuffle=args.shuffle
		self.optimizer=args.pre_optimizer
		self.weights=args.loss_weights
		self.normalize_image=args.normalize
		self.roi_shape=args.roi_shape_roi	
		self.store_model=args.store_txt
		self.weight_name='first'
		self.early_stopping=args.early_stop
		self.monitor=args.monitor_callbacks
		self.mode=args.mode_convert
		self.label_classes=args.label_classes
		self.exponential_decay='False'
		self.lrate=args.learning_rate
		self.store_model_path=args.store_txt
		self.fourier=args.fft_convert_data
		if(args.decay==666):
			self.exponential_decay='True'
			args.decay=0

		optimizer_args_roi = {
			'lr':       args.roi_learning_rate,
			'momentum': args.roi_momentum,
			'decay':    args.roi_decay,
			'seed':    args.roi_seed
			}
		optimizer_args = {
			'lr':       args.learning_rate,
			'momentum': args.momentum,
			'decay':   args.decay,
			'seed':    args.seed
			}
		if self.path_case=='roi':
			for k in list(optimizer_args_roi):
				if optimizer_args_roi[k] is None:
					del optimizer_args_roi[k]
			optimizer = self.pass_optimizer(args.roi_optimizer, optimizer_args_roi)
			self.optimizer=optimizer
			self.epochs=self.epochs_roi

		if self.path_case=='main':
			for k in list(optimizer_args):
				if optimizer_args[k] is None:
					del optimizer_args[k]
			optimizer = self.pass_optimizer(args.m_optimizer, optimizer_args)
			self.optimizer=optimizer
			self.epochs=self.epochs_main
		
################################################# data_augmentationTRAINING ##############################################################


	def run_training(self,loss_type, model_structure, X, Y):
		# MPU: data paraller mode deep learning 
		# GPU: model paraller mode deep learning 
		#one-hot encoder mask binary labels
		#if self.path_case=='main':
		#	Y = utils.to_categorical(Y,(np.array(Y).shape[1]+1))
		#	Y=Y.reshape(np.array(Y).shape[0],np.array(Y).shape[2],np.array(Y).shape[3],np.array(Y).shape[4])
		#	print(np.array(Y).shape)
		if self.ram=='GPU':
			model_structure = multi_gpu_model(model_structure, gpus=self.ngpu)
			self.batch_size=self.batch_size*self.ngpu
		#define metrics for training

		metrics_algorithm=self.metrics_generator()
		loss=self.load_loss(loss_type)
		model_structure.compile(optimizer=self.optimizer,loss=loss, metrics=metrics_algorithm)
		if self.normalize_image == 'True':
			X=regularization.normalize(X)
		if self.shuffle == 'True':
			# shuffle images and masks in parallel
			rng_state = np.random.get_state()
			np.random.shuffle(X)
			np.random.set_state(rng_state)
			np.random.shuffle(Y)
		#define callbacks
		self.callbacks=self.callbacks_define(self.monitor, self.weight_name)
		#validation data
		if self.validation=='on':
			for cross_val_num in range(int(self.cross_validation_number)):
				print("cross validation run: ", cross_val_num, "/", int(self.cross_validation_number))
				if cross_val_num!=0:
					#model_structure.load_weights(self.store_model_path + '/weights_%s_%s.hdf5' %(self.path_case, self.weight_name))
					rng_state = np.random.get_state()
					np.random.shuffle(X)
					np.random.set_state(rng_state)
					np.random.shuffle(Y)
					model_structure.compile(optimizer=self.optimizer,loss=loss, metrics=metrics_algorithm)
					self.callbacks=self.callbacks_define(self.monitor,( self.weight_name+'_'+str(cross_val_num)))

				h_d=handle_data.handle_data(X,Y,self.path_case)

				training_augment, train_steps_per_epoch, validation_augment, val_steps_per_epoch =h_d.validation_data()
				if self.ram=='CPU':				
					history = model_structure.fit_generator(training_augment, epochs=self.epochs, steps_per_epoch=train_steps_per_epoch,verbose=1, callbacks=self.callbacks, validation_data=validation_augment, validation_steps=val_steps_per_epoch  )  

				if self.ram=='MPU':  
					self.batch_size=self.batch_size
					history = model_structure.fit_generator(training_augment, epochs=self.epochs, steps_per_epoch=train_steps_per_epoch,verbose=1, callbacks=self.callbacks, validation_data=validation_augment, validation_steps=val_steps_per_epoch,workers=self.num_cores,use_multiprocessing=True)      

		else:
			h_d2=handle_data.handle_data(X,Y,self.path_case)
			training_augment, train_steps_per_epoch =h_d2.no_validation_data()
			if self.ram=='CPU':
				history = model_structure.fit_generator(training_augment, epochs=self.epochs, steps_per_epoch=train_steps_per_epoch, verbose=1, callbacks=self.callbacks)   
			if self.ram=='MPU':
				self.batch_size=self.batch_size 
				history = model_structure.fit_generator(training_augment, epochs=self.epochs, steps_per_epoch=train_steps_per_epoch, verbose=1, callbacks=self.callbacks,workers=self.num_cores,use_multiprocessing=True)   


		return model_structure, history

##################################################### TESTING ##################################################################
	def run_testing(self,loss_type,model_structure, X, Y):
		# set GPU or CPU
		if self.ram=='GPU':
			model_structure = multi_gpu_model(model_structure, gpus=self.ngpu)
			self.batch_size_test=self.batch_size_test*self.ngpu

		if self.normalize_image == 'True':
			X=regularization.normalize(X)

		if self.shuffle == 'True':
			# shuffle images and masks in parallel
			rng_state = np.random.get_state()
			np.random.shuffle(X)
			np.random.set_state(rng_state)
			np.random.shuffle(Y)
		if self.fourier=="on":
			hd=handle_data.handle_data(X,Y,self.path_case)
			X,Y=hd.fourier_convert_data (X,Y)
			Y=hd.binary_masks(Y)
			#Y=Y.reshape(Y.shape[0],Y.shape[3],Y.shape[2],Y.shape[1])
			print(np.array(Y).shape)

		if (self.path_case=='main' and self.fourier=="off"):
			#Y=Y.reshape(Y.shape[0],Y.shape[3],Y.shape[2],Y.shape[1]) # change for main case
			print(np.array(Y).shape)
			hd=handle_data.handle_data(X,Y,self.path_case)
			Y=hd.binary_masks(Y)
			#Y = np_utils.to_categorical(Y,np.array(Y).shape[1]+1)
			print(np.array(Y).shape)
	#				Y=Y.reshape(np.array(Y).shape[0],np.array(Y).shape[2],np.array(Y).shape[3],np.array(Y).shape[1])
		Xtest=X
		Ytest=Y
		#set metric and loss to model 
		#define metrics for testing
		metrics_algorithm=self.metrics_generator()

		loss=self.load_loss(loss_type)
		model_structure.compile(optimizer=self.optimizer,loss=loss, metrics=metrics_algorithm)
		#predict the outputs
		batch_size=self.batch_size_test
		test_generator=ImageDataGenerator().flow(x=Xtest, batch_size=batch_size, shuffle=False )
		y_pred = model_structure.predict_generator(test_generator,steps=np.ceil(len(X)/batch_size))
		
		hd1=handle_data.handle_data(Xtest,y_pred,self.path_case)

		y_pred=hd1.binary_masks(y_pred)

		hd_final=handle_data.handle_data(Xtest,y_pred,self.path_case)
		#if (self.path_case=='main' and self.fourier=="off"):
			#y_pred=hd_final.clipped_zoom()
			
		#evaluate the results
		metric=model_structure.evaluate(x=Xtest, y=Ytest, batch_size=batch_size, verbose=1, sample_weight=None, steps=None)

		return y_pred, metric 
############################################ metric, loss, optimizzer choices, callbacks################## 

	def callbacks_define(self,monitor,weight_name):
		# call backs of the weights
		filepath=str(self.store_model + "/weights_%s_%s.hdf5" % (self.path_case,weight_name))
		print(filepath)
		monitor=monitor
		checkpoint = ModelCheckpoint(filepath=filepath, monitor=monitor, verbose=1, save_best_only=True, save_weights_only=True, mode=str(self.mode))
		callbacks = [checkpoint]

		if self.early_stopping =='True':
			stop_points=EarlyStopping(monitor=monitor, min_delta=0.0000001, patience=10, verbose=1, mode=str(self.mode), baseline=None, restore_best_weights=True)
			callbacks = [checkpoint, stop_points]
		if self.exponential_decay=='True':
			 # loss_history =LossHistory()
			lrate= LearningRateScheduler(schedule=self.exp_decay,verbose=1)
			callbacks=[checkpoint,lrate]
		return callbacks


	def exp_decay(self,epoch,lr):
		k=7.5
		lrate=self.lrate*np.exp(-k*(epoch/self.epochs_main))
		#print(lrate)
		#print(epoch)
		#print(lr)
		#print(self.lrate)
		#print(self.epochs_main)
		return lrate

	def pass_optimizer(self,optimizer_name, optimizer_args):
		optimizers = {
		'sgd': SGD,
		'rmsprop': RMSprop,
		'adagrad': Adagrad,
		'adadelta': Adadelta,
		'adam': Adam,
		'adamax': Adamax,
		'nadam': Nadam,
		}
		if optimizer_name not in optimizers:
			raise Exception("Unknown optimizer ({}).".format(optimizer_name))
		return optimizers[optimizer_name](**optimizer_args)

	def pass_metric(self,metric_name):
		if (metric_name == "customized_loss"):
			return self.customized_loss 
		if (metric_name == "weighted_categorical_crossentropy"):				
			return self.weighted_categorical_crossentropy
		if (metric_name == "jaccard_loss"):
			return self.jaccard_loss
		if (metric_name == "hard_jaccard"):
			return self.hard_jaccard
		if (metric_name == "soft_jaccard"):
			return self.soft_jaccard
		if (metric_name == "sorensen_dice_loss"):
			return self.sorensen_dice_loss
		if (metric_name == "hard_sorensen_dices"):
			return self.hard_sorensen_dice
		if (metric_name == "soft_sorensen_dice"):
			return self.soft_sorensen_dice
		if (metric_name == "mean_squared_error"):
			return mean_squared_error
		if (metric_name == "kullback_leibler_divergence"):
			return kullback_leibler_divergence
		if (metric_name == "binary_crossentropy"):
			return binary_crossentropy
		if (metric_name == "sparse_categorical_crossentropy"):
			return sparse_categorical_crossentropy
		if (metric_name == "categorical_crossentropy"):
			return categorical_crossentropy
		if (metric_name == "accuracy"):
			return metrics.binary_accuracy
		if (metric_name=="categorical_accuracy"):
			return metrics.categorical_accuracy
		if (metric_name == "sparce_categorical_accuracy"):
			return metrics.sparce_categorical_accuracy
		if (metric_name == "dice"):
			return self.dice
		if (metric_name == "log_dice"):
			return self.log_dice
		if (metric_name == "log_jaccard"):
			return self.jaccard
		if (metric_name == "Hausdorff"):
			return self.loss_Hausdorff_distance
		if metric_name==" " :
			raise Exception("None metric ({}).".format(metric_name))


	def load_loss(self, loss_check):
		if (loss_check == "customized_loss"):
			def lossfunction(y_true, y_pred):
				return self.customized_loss(y_true, y_pred) 
		if (loss_check == "weighted_categorical_crossentropy"):
			def lossfunction(y_true, y_pred):				
				return self.weighted_categorical_crossentropy(y_true, y_pred)
		if (loss_check == "jaccard_loss"):
			def lossfunction(y_true, y_pred):
				return self.jaccard_loss(y_true, y_pred)
		if (loss_check == "hard_jaccard"):
			def lossfunction(y_true, y_pred):
				return self.hard_jaccard(y_true, y_pred)
		if (loss_check == "soft_jaccard"):
			def lossfunction(y_true, y_pred):
				return self.soft_jaccard(y_true, y_pred)
		if (loss_check == "sorensen_dice_loss"):
			def lossfunction(y_true, y_pred):
				return self.sorensen_dice_loss(y_true, y_pred)
		if (loss_check == "hard_sorensen_dices"):
			def lossfunction(y_true, y_pred):
				return self.hard_sorensen_dice(y_true, y_pred)
		if (loss_check == "soft_sorensen_dice"):
			def lossfunction(y_true, y_pred):
				return self.soft_sorensen_dice(y_true, y_pred)
		if (loss_check == "mean_squared_error"):
			def lossfunction(y_true, y_pred):
				return mean_squared_error(y_true, y_pred)
		if (loss_check == "kullback_leibler_divergence"):
			def lossfunction(y_true, y_pred):
				return kullback_leibler_divergence(y_true, y_pred)
		if (loss_check == "binary_crossentropy"):
			def lossfunction(y_true, y_pred):
				return binary_crossentropy(y_true, y_pred)
		if (loss_check == "sparse_categorical_crossentropy"):
			def lossfunction(y_true, y_pred):
				return sparse_categorical_crossentropy(y_true, y_pred)
		if (loss_check == "categorical_crossentropy"):
			def lossfunction(y_true, y_pred):
				return categorical_crossentropy(y_true, y_pred)
		if (loss_check == "Hausdorff_loss"):
			def lossfunction(y_true, y_pred):
				return self.loss_Hausdorff_distance(y_true, y_pred)
		if(loss_check=="dice_cross_entropy"):
			def lossfunction(y_true,y_pred):
				return self.dice_cross_entropy(y_true,y_pred)
		if(loss_check=="binary_dice_cross_entropy"):
			def lossfunction(y_true,y_pred):
				return self.binary_dice_cross_entropy(y_true,y_pred)
		if(loss_check=="dice_loss"):	
			def lossfunction(y_true,y_pred):
				return self.dice_loss(y_true,y_pred)
		if(loss_check=="log_dice_loss"):	
			def lossfunction(y_true,y_pred):
				return self.log_dice_loss(y_true,y_pred)

		if(loss_check=="jaccard_loss"):	
			def lossfunction(y_true,y_pred):
				return self.jaccard_loss(y_true,y_pred)

		if(loss_check=="sscill"):	
			def lossfunction(y_true,y_pred):
				return self.softmax_sparse_crossentropy_ignoring_last_label(y_true,y_pred)

		if(loss_check=="SS_DL"):
			def lossfunction(y_true,y_pred):
				return self.SS_DL(y_true,y_pred)

		if(loss_check=="SS_DCE"):
			def lossfunction(y_true,y_pred):
				return self.SS_DCE(y_true,y_pred)

		return lossfunction

	def metrics_generator(self):

		metrics_algorithm = [self.pass_metric(self.metrics), self.pass_metric(self.metrics1), self.pass_metric(self.metrics2)]
		return metrics_algorithm

###################################### METRICS MEASUREMENTS ####################################################################
	def dice(self,y_true, y_pred):
		batch_dice_coefs = self.hard_sorensen_dice(y_true, y_pred, axis=[1, 2])
		dice_coefs = K.mean(batch_dice_coefs, axis=0)
		#w = K.constant(self.weights)/sum(self.weights)
		return dice_coefs[0]  

	def log_dice(self,y_true, y_pred, smooth=100):
		log_batch_dice_coefs = self.log_dice_loss_core(y_true, y_pred, axis=[1, 2])
		log_dice_coefs = K.mean(log_batch_dice_coefs, axis=0)
		#w = K.constant(self.weights)/sum(self.weights)
		return log_dice_coefs[0] 

	def jaccard(self,y_true, y_pred):
		batch_jaccard_coefs = self.hard_jaccard(y_true, y_pred, axis=[1, 2])
		jaccard_coefs = K.mean(batch_jaccard_coefs, axis=0)
		#w = K.constant(self.weights)/sum(self.weights)
		return jaccard_coefs[0] 

	def metric_hausdorff(self,Y_tr,Y_pre):

		pairwise_measures = PairwiseMeasures(seg_img=Y_pre, ref_img=Y_tr)
		#self.assertEqual(pairwise_measures.dice_score(), 1.)
		return pairwise_measures.measured_hausdorff_distance()
###################################### LOSSES MEASUREMENTS #################################################################### 
	#def jaccard_loss(self,y_true, y_pred):
	#	batch_jaccard_coefs = self.hard_jaccard(y_true, y_pred, axis=[1,2])
	#	jaccard_coefs = K.mean(batch_jaccard_coefs, axis=0)
	#	w = K.constant(self.weights)/ sum(self.weights)
	#	return 1-K.sum(w * jaccard_coefs)

	def soft_sorensen_dice(self,y_true, y_pred, axis=None, smooth=100):
		intersection = K.sum(y_true * y_pred, axis=axis)
		area_true = K.sum(y_true, axis=axis)
		area_pred = K.sum(y_pred, axis=axis)
		return ( (2*(intersection + smooth) / (area_true + area_pred + smooth)))
	    
	def hard_sorensen_dice(self,y_true, y_pred, axis=None, smooth=100):
		y_true_int = K.round(y_true)
		y_pred_int = K.round(y_pred)
		return self.soft_sorensen_dice(y_true_int, y_pred_int, axis, smooth)

	def log_dice_loss_core(self,y_true, y_pred, axis=[1, 2],smooth=100):
		intersection = K.sum(y_true * y_pred, axis=axis)
		area_true = K.sum(y_true, axis=axis)
		area_pred = K.sum(y_pred,  axis=axis)
		batch_dice_coefs = K.log(2-( (intersection + smooth) / (area_true + area_pred + smooth)))
		return batch_dice_coefs

	def sorensen_dice_loss(self,y_true, y_pred, smooth=100):
		# Input tensors have shape (batch_size, height, width, classes)
		# User must input list of weights with length equal to number of classes
		#
		# Ex: for simple binary classification, with the 0th mask
		# corresponding to the background and the 1st mask corresponding
		# to the object of interest, we set weights = [0, 1]
		batch_dice_coefs = self.soft_sorensen_dice(y_true, y_pred, axis=[1, 2])
		dice_coefs = K.mean(batch_dice_coefs, axis=0)
		w = K.constant(self.weights) / sum(self.weights)
		return (1 - K.sum(w * dice_coefs))

	def log_dice_loss(self,y_true, y_pred, axis=[1, 2],smooth=1):
		# Input tensors have shape (batch_size, height, width, classes)
		# User must input list of weights with length equal to number of classes
		#
		# Ex: for simple binary classification, with the 0th mask
		# corresponding to the background and the 1st mask corresponding
		# to the object of interest, we set weights = [0, 1]
		batch_dice_coefs = self.log_dice_loss_core(y_true, y_pred, axis=[1, 2])
		dice_coefs = K.mean(batch_dice_coefs, axis=0)
		w = K.constant(self.weights) / sum(self.weights)
		return K.sum(w * (1-dice_coefs))

	def dice_loss(self,y_true,y_pred):
		return self.sorensen_dice_loss(y_true,y_pred)

	def dice_cross_entropy(self,y_true,y_pred,l1=0.4,l2=0.6,smooth=100):
		loss_dice=self.log_dice_loss(y_true,y_pred)
		loss_entropy=self.weighted_categorical_crossentropy(y_true,y_pred)
		return l1*loss_dice+l2*loss_entropy

	def binary_dice_cross_entropy(self,y_true,y_pred,l1=0.5,l2=0.5,smooth=100):
		loss_dice=self.log_dice_loss(y_true,y_pred)
		loss_entropy= binary_crossentropy(y_true,y_pred)
		return l1*loss_dice+l2*loss_entropy

	def soft_jaccard(self,y_true, y_pred, axis=None, smooth=100):
		intersection = K.sum(y_true * y_pred, axis=axis)
		area_true = K.sum(y_true, axis=axis)
		area_pred = K.sum(y_pred, axis=axis)
		union = area_true + area_pred - intersection
		return (intersection + smooth) / (union + smooth) 

	def hard_jaccard(self,y_true, y_pred, axis=None, smooth=1):
		y_true_int = K.round(y_true)
		y_pred_int = K.round(y_pred)
		return self.soft_jaccard(y_true_int, y_pred_int, axis, smooth)


	def jaccard_loss(self,y_true, y_pred, smooth=100):
		intersection = K.sum(K.abs(y_true * y_pred), axis=-1)
		sum_ = K.sum(K.abs(y_true) + K.abs(y_pred), axis=-1)
		jac = (intersection + smooth) / (sum_ - intersection + smooth)
		return (1 - jac) * smooth

	def weighted_categorical_crossentropy(self,y_true, y_pred):
		#weights = K.variable(np.array(self.weights))
		# scale predictions so that the class probas of each sample sum to 1
		#y_pred /= K.sum(y_pred, axis=-1, keepdims=True)
		#y_pred = K.clip(y_pred, K.epsilon(), 1 - K.epsilon())
		#loss = y_true * K.log(y_pred) * weights
		#loss = -K.sum(loss, -1)
		#return loss

		 ndim = K.ndim(y_pred)
		 ncategory = K.int_shape(y_pred)[-1]
		 print(ndim,ncategory)
		# scale predictions so class probabilities of each pixel sum to 1
		 y_pred /= K.sum(y_pred, axis=(ndim-1), keepdims=True)
		 y_pred = K.clip(y_pred, K.epsilon(), 1-K.epsilon())
		 w = K.constant(self.weights) * (ncategory / sum(self.weights))
		# first, average over all axis except classes
		 cross_entropies = K.mean(y_true * K.log(y_pred), axis=tuple(range(ndim-1)))
		# print(y_pred)
		 return -K.sum(w * cross_entropies)

	def customized_loss(self,y_true, y_pred, alpha=0.0001, beta=3):
		"""
		linear combination of MSE and KL divergence.
		"""
		loss1 = losses.mean_absolute_error(y_true, y_pred)
		loss2 = losses.kullback_leibler_divergence(y_true, y_pred)
		#(alpha/2) *
		return  loss1 + beta * loss2

	def loss_Hausdorff_distance(self,y_true, y_pred):
		loss_loc = losses_distance.Weighted_Hausdorff_loss(y_true, y_pred)
		return loss_loc



###################################################Nifty metrics##############################################################################

	def SS_DCE(self,y_true,y_pred):		
		loss_dice=self.dice_cross_entropy(y_true,y_pred)
		loss_sens=self.sensitivity(y_true,y_pred)
		loss_spec=self.specificity(y_true,y_pred) # self.sensitivity and self.specificity from evaluation file of auto
		loss_SS=2-loss_sens-loss_spec
		a=0.2
		b=0.8
		return a*loss_dice+b*loss_SS

	def SS_DL(self,y_true,y_pred):	
		loss_dice=self.dice_loss(y_true,y_pred)
		loss_sens=self.sensitivity(y_true,y_pred)
		loss_spec=self.specificity(y_true,y_pred) # self.sensitivity and self.specificity from evaluation file of auto
		loss_SS=2-loss_sens-loss_spec
		a=0.2
		b=0.8
		return a*loss_dice+b*loss_SS


	def specificity(self,y_pred, y_true):

		neg_y_true = 1 - y_true
		neg_y_pred = 1 - y_pred
		fp = K.sum(neg_y_true * y_pred)
		tn = K.sum(neg_y_true * neg_y_pred)
		specificity = tn / (tn + fp + K.epsilon())
		return specificity


	def sensitivity(self,y_pred, y_true):

		neg_y_pred = 1 - y_pred
		fn = K.sum(y_true * neg_y_pred)
		tp = K.sum(y_true * y_pred)
		sensitivity = tp / (tp + fn + K.epsilon())
		return sensitivity


##########//nifti version ####################################################################################################################################################################################

	def nifti_sensitivity(self,Y_tr,Y_pre):
		Y_tr=binary_images(Y_tr)
		Y_pre=binary_images(Y_pre)
		pairwise_measures = PairwiseMeasures(seg_img=Y_pre, ref_img=Y_tr)
		#self.assertEqual(pairwise_measures.sensitivity(), 1.)
		return pairwise_measures.sensitivity()
	def nifti_specificity(self,Y_tr,Y_pre):
		Y_tr=binary_images(Y_tr)
		Y_pre=binary_images(Y_pre)
		pairwise_measures = PairwiseMeasures(seg_img=Y_pre, ref_img=Y_tr)
		#self.assertEqual(pairwise_measures.specificity(), 2./3)
		return pairwise_measures.specificity()

###################################################FCN losses##############################################################################
#https://github.com/aurora95/Keras-FCN/blob/master/utils/loss_function.py

	def softmax_sparse_crossentropy_ignoring_last_label(y_true, y_pred):
		y_pred = K.reshape(y_pred, (-1, K.int_shape(y_pred)[-1]))
		log_softmax = tf.nn.log_softmax(y_pred)

		y_true = K.one_hot(tf.to_int32(K.flatten(y_true)), K.int_shape(y_pred)[-1]+1)
		unpacked = tf.unstack(y_true, axis=-1)
		y_true = tf.stack(unpacked[:-1], axis=-1)

		cross_entropy = -K.sum(y_true * log_softmax, axis=1)
		cross_entropy_mean = K.mean(cross_entropy)

		return cross_entropy_mean


# Softmax cross-entropy loss function for coco segmentation
# and models which expect but do not apply sigmoid on each entry
# tensorlow only
	def binary_crossentropy_with_logits(ground_truth, predictions):
		return K.mean(K.binary_crossentropy(ground_truth, predictions, from_logits=True),axis=-1)




