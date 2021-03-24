#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk

from __future__ import division, print_function
import keras
from keras.preprocessing.image import ImageDataGenerator
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten, Reshape
from keras.layers import Conv2D, MaxPooling2D, AveragePooling2D
from keras.preprocessing.image import ImageDataGenerator
from keras import regularizers
from keras.losses import mean_squared_error
from keras import backend as K
import keras.utils.vis_utils
import glob
import matplotlib.patches as patches
import json
import numpy as np
from matplotlib.path import Path
import cv2
import matplotlib.pyplot as plt
from auto_segm import config, run_model, main
import argparse

class roi_net:

	def __init__ (self , init1, init2) :
		args = config.parse_arguments()
		self.roi_activation=args.roi_activation
		self.init_f=init1
		self.init_b=init2
		self.loss_pre=args.loss_pre
		self.roi_shape=args.roi_shape_roi
		self.roi_optimizer=args.pre_optimizer
		self.get_history=False
		self.verbose=1 
		self.units_ed=64	
		self.pretrain_window=args.pretrain_window
		self.epoch_pre=args.epochs_pre
		self.monitor=args.monitor_callbacks
		self.store_model=args.store_txt
		self.weight_name='first'

	def deep_conv(self,input_shape=(64, 64)):
		"""
		"""
		depth=64
		model = Sequential()
		model.add(Conv2D(depth, (11,11), strides=(1, 1), padding='valid', activation=self.roi_activation, input_shape=(input_shape[0], input_shape[1], 1)))
		model.add(AveragePooling2D((2,2)))
		model.add(Conv2D(128, (10, 10), strides=(1, 1), padding='valid',activation=self.roi_activation ))
		model.add(AveragePooling2D((2,2)))
		model.add(Reshape([-1, 128*9*9]))
		model.add(Dense(1024, activation='sigmoid', kernel_regularizer=regularizers.l2(0.0001)))
		model.add(Reshape([-1, self.roi_shape, self.roi_shape]))
		if self.init_f!=[]:
			F=np.reshape(self.init_f,(11,11,1,depth))
			weights=[]
			weights.append(F)
			weights.append(self.init_b)
			print(F.shape, self.init_b.shape)
			model.layers[0].set_weights(weights)
		return model

	def pre_encoder(self, X_train, param_reg ):
		"""
		First part of the stacked AE.
		Train the AE on the ROI input images.
		:param X_train: ROI input image
		:param get_history: boolean to return the loss history
		:param loss: if "customized_loss" -> customized_loss
		:return: encoded ROI image
		"""
		K.set_epsilon(0.1)
		rm=run_model.run_model('pre')
		#define callbacks
		callbacks=rm.callbacks_define(self.monitor,self.weight_name)
		autoencoder_0 = Sequential()	
		dim=self.pretrain_window*self.pretrain_window
		#print(dim)
		encoder_0 = Dense(input_dim=dim, units=self.units_ed, kernel_regularizer=regularizers.l2(param_reg))
		decoder_0 = Dense(input_dim=self.units_ed, units=dim, kernel_regularizer=regularizers.l2(param_reg))
		autoencoder_0.add(encoder_0)
		autoencoder_0.add(decoder_0)
		rm=run_model.run_model('roi') 
		loss=rm.load_loss(self.loss_pre)
		autoencoder_0.compile(loss = loss, optimizer=self.roi_optimizer, metrics=['accuracy'])
		h = autoencoder_0.fit(X_train, X_train, epochs=self.epoch_pre, verbose=self.verbose,callbacks=callbacks)
		autoencoder_0.load_weights(self.store_model+ "/weights_%s_%s.hdf5" % ('pre',self.weight_name))
		weights=autoencoder_0.layers[0].get_weights()
		temp_0 = Sequential()
		temp_0.add(encoder_0)
		temp_0.compile(loss=loss, optimizer=self.roi_optimizer, metrics=['accuracy'])
		encoded_X = temp_0.predict(X_train, verbose=self.verbose)
	    
		if self.get_history:
			return   h.history['loss'], encoded_X, encoder_0, weights
		else:
			return  encoded_X, encoder_0 , weights
