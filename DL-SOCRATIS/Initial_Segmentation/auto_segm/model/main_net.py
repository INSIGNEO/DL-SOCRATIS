#Author: Michail Mamalakis
#Version: 0.3
#Licence:
#email:mmamalakis1@sheffield.ac.uk
#Acknowledgement: https://github.com/ece7048/cardiac-segmentation-1/tree/master/rvseg/models

from __future__ import division, print_function

from keras.layers import Input, Conv2D, Conv2DTranspose, Reshape, LeakyReLU
from keras.layers import MaxPooling2D, Cropping2D, Concatenate
from keras.layers import Layer, Dense, Flatten,Lambda, Activation, BatchNormalization, Dropout, Dense
from keras.models import Sequential
#from tensorflow.keras.layers import Layer, Dense, Flatten, Activation, Dropout
#from tensorflow.keras.layers import LeakyReLU
from keras.models import Model
from keras import backend as K
from keras import constraints
from keras import applications
import argparse
from auto_segm import config, run_model
# FCN
import numpy as np
import matplotlib.pyplot as plt
from pylab import *
import os
import sys
from keras_contrib.applications import densenet
#from keras.models import Model
from keras.regularizers import l2
from keras.layers import *
from keras.engine import Layer
from keras.applications.vgg16 import *
from keras.models import *
from keras_applications.imagenet_utils import _obtain_input_shape
import tensorflow as tf

 
from auto_segm.model.RGMMnet.RGMMNet import Gaussian_Weighting, Rician_Weighting, RGMMNet, MPool1, Attention, GAT
from auto_segm.model.RGMMnet import  coarsening, process_data, graph, utils2
from auto_segm.model.RGMMnet.coarsening import *
from auto_segm.utils import BilinearUpSampling, basics, resnet_helpers


# TODO batch shape in the models when you have a big data its better for paraller analysis so extend base of  https://github.com/aurora95/Keras-FCN/blob/master/models.py

class main_net(object):

	def __init__ (self, init1, init2, height,channels,classes,width ) :
		args = config.parse_arguments()
		self.height=height
		self.channels=channels
		self.classes=classes
		self.main_activation=args.main_activation
		self.init_w=init1
		self.init_b=init2
		self.features=args.features
		self.depth=args.depth 
		self.padding=args.padding 
		self.batchnorm=args.batchnorm	
		self.dropout=args.dropout
		self.width=width
		self.temperature=1.0 
		self.max_norm_const=args.max_norm_const
		self.max_norm_value=args.max_norm_value
		self.im_length=args.height
###########################################ordibary U-net Euclidian ##################################
	def downsampling_block(self,input_tensor):
		_, height, width, _ = K.int_shape(input_tensor)
		print(height,width)
		assert height % 2 == 0
		assert width % 2 == 0
		# add max norm constrain
		if self.max_norm_const=='on':
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding, kernel_constraint=constraints.max_norm(self.max_norm_value))(input_tensor)
		else:
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding)(input_tensor)

		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		# add max norm constrain
		if self.max_norm_const=='on':
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding, kernel_constraint=constraints.max_norm(self.max_norm_value))(x)
		else:
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding)(x)

		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		return MaxPooling2D(pool_size=(2,2))(x), x

	def upsampling_block(self,input_tensor, skip_tensor):
		x = Conv2DTranspose(self.features, kernel_size=(2,2), strides=(2,2))(input_tensor)
		# compute amount of cropping needed for skip_tensor
		_, x_height, x_width, _ = K.int_shape(x)
		_, s_height, s_width, _ = K.int_shape(skip_tensor)
		print(x_height,x_width)
		print(s_height,s_width)
		h_crop = s_height - x_height
		w_crop = s_width - x_width
		assert h_crop >= 0
		assert w_crop >= 0
		if h_crop == 0 and w_crop == 0:
			y = skip_tensor
		else:
			cropping = ((h_crop//2, h_crop - h_crop//2), (w_crop//2, w_crop - w_crop//2))
			y = Cropping2D(cropping=cropping)(skip_tensor)
		x = Concatenate()([x, y])
		# add max norm constrain
		if self.max_norm_const=='on':
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding, kernel_constraint=constraints.max_norm(self.max_norm_value))(x)
		else:
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding)(x)
		#
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		# add max norm constrain
		if self.max_norm_const=='on':
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding, kernel_constraint=constraints.max_norm(self.max_norm_value))(x)
		else:
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding)(x)
		#
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		return x

	def u_net(self):
		"""Generate U-Net model introduced in
		"U-Net: Convolutional Networks for Biomedical Image Segmentation"
		O. Ronneberger, P. Fischer, T. Brox (2015)
		Arbitrary number of input channels and output classes are supported.
		Arguments:
		height  - input image height (pixels)
		width   - input image width  (pixels)
		channels - input image features (1 for grayscale, 3 for RGB)
		classes - number of output classes (2 in paper)
		features - number of output features for first convolution (64 in paper)
		Number of features double after each down sampling block
		depth  - number of downsampling operations (4 in paper)
		padding - 'valid' (used in paper) or 'same'
		batchnorm - include batch normalization layers before activations
		dropout - fraction of units to dropout, 0 to keep all units
		Output:
		U-Net model expecting input shape (height, width, maps) and generates
		output with shape (output_height, output_width, classes). If padding is
		'same', then output_height = height and output_width = width.
		"""
		x = Input(shape=(self.height, self.width, self.channels))
		inputs = x
		skips = []
		print(self.height, self.width)
		for i in range(self.depth):

			x, x0 = self.downsampling_block(x)
			skips.append(x0)
			self.features *= 2
		# add max norm constrain
		if self.max_norm_const=='on':
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding, kernel_constraint=constraints.max_norm(self.max_norm_value))(x)
		else:
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding)(x)

		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		# add max norm constrain
		if self.max_norm_const=='on':
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding, kernel_constraint=constraints.max_norm(self.max_norm_value))(x)
		else:
			x = Conv2D(self.features, kernel_size=(3,3), padding=self.padding)(x)

		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		for i in reversed(range(self.depth)):
			self.features //= 2
			x = self.upsampling_block(x, skips[i])
		# add max norm constrain
		if self.max_norm_const=='on':
			x = Conv2D(filters=self.classes, kernel_size=(1,1), padding=self.padding, kernel_constraint=constraints.max_norm(self.max_norm_value))(x)
		else:
			x = Conv2D(filters=self.classes, kernel_size=(1,1), padding=self.padding)(x)

		logits = Lambda(lambda z: z/self.temperature)(x)

		probabilities = Activation('softmax')(logits)		
		return Model(inputs=inputs, outputs=probabilities)



##########################################MO-net Unet ################################################

	def RGMM_downsampling_block(self,input_tensor,batch_size,n_u, mm_case, portion):

		if mm_case=='GMM':
			self.n_mm=self.n_gauss
		if mm_case=='RMM':
			self.n_mm=self.n_rician

		_, height, width, _ = K.int_shape(input_tensor)
		x=input_tensor
		print(height,width)
		assert height % 2 == 0
		assert width % 2 == 0

		# RGMM layer
		x= RGMMNet(u=self.u[n_u], d=self.d, n_mixture_models=self.n_mm, n_hidden=self.features, batch_size=batch_size, trainable=True)(x)

		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		# RGMM layer
		x= RGMMNet(u=self.u[n_u], d=self.d, n_mixture_models=self.n_mm, n_hidden=self.features, batch_size=batch_size, trainable=True)(x)

		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		return MaxPooling2D(pool_size=(2,2))(x), x

	def RGMM_upsampling_block(self,input_tensor, batch_size, skip_tensor, n_u, mm_case, portion):
		if mm_case=='GMM':
			self.n_mm=self.n_gauss
		if mm_case=='RMM':
			self.n_mm=self.n_rician
		x = RGMMNet(u=self.u[n_u], d=self.d, n_mixture_models=self.n_mm, n_hidden=self.features, batch_size=batch_size, trainable=True)(input_tensor)
		# compute amount of cropping needed for skip_tensor
		_, x_height, x_width, _ = K.int_shape(x)
		_, s_height, s_width, _ = K.int_shape(skip_tensor)
		print(x_height,x_width)
		print(s_height,s_width)
		h_crop = s_height - x_height
		w_crop = s_width - x_width
		assert h_crop >= 0
		assert w_crop >= 0
		if h_crop == 0 and w_crop == 0:
			y = skip_tensor
		else:
			cropping = ((h_crop//2, h_crop - h_crop//2), (w_crop//2, w_crop - w_crop//2))
			y = Cropping2D(cropping=cropping)(skip_tensor)
		x = Concatenate()([x, y])
		# RGMM layer
		x= RGMMNet(u=self.u[0], d=self.d, n_mixture_models=25, n_hidden=64, batch_size=batch_size, trainable=True)(x)
#x= RGMMNet(u=self.u[n_u], d=self.d, n_mixture_models=self.n_mm, n_hidden=self.features, batch_size=batch_size, mm_case=mm_case, portion=portion, trainable=True)(x)
		print('pass_')

		#
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		# RGMM layer
		x= RGMMNet(u=self.u[n_u], d=self.d, n_mixture_models=self.n_mm, n_hidden=self.features, batch_size=batch_size, trainable=True)(x)

		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		return x

	def rg_net(self,d=2,portion=0.5,batch_size=1,n_mixturemodel=32,mm_case_i='RGMM'):
		dropout=0.5
		print('classical RG MO-net stracture')
		n_rows=28
		self.n_mm=n_mixturemodel
		A, nodes_coordinates = grid_graph(n_rows)
		coarsening_levels=self.depth
		u_shape, u_rows, u_cols, u_val, perm = coarsen_mnist(A, coarsening_levels, nodes_coordinates)
		self.u = []
		for level in range(coarsening_levels):
			self.u.append([u_shape[level], u_rows[level], u_cols[level], u_val[level]])
		p=4
		self.d=d	
		model = Sequential()
		model.add(RGMMNet(u=self.u[0], d=self.d, n_mixture_models=n_mixturemodel, n_hidden=self.features, batch_size=batch_size, trainable=True))
		model.add(Activation('relu'))
		model.add(MPool1(p=p))
		model.add(RGMMNet(u=self.u[2], d=self.d, n_mixture_models=n_mixturemodel, n_hidden=self.features, batch_size=batch_size, trainable=True))
		model.add(Activation('relu'))
		model.add(MPool1(p=p))
		model.add(Flatten())
		model.add(Dense(512, activation='relu'))
		model.add(Dropout(dropout))
		model.add(Dense(self.n_mm, activation='softmax'))
		return model


	def rgu_net(self,d=2,portion=0.5,batch_size=1, n_mixturemodel=32,mm_case_i='RGMM'):
		"""Generate U-Net model introduced in
		"RGU-Net"
		Michail Mamalakis (2020)
		Arbitrary number of input channels and output classes are supported.
		Arguments:
		height  - input image height (pixels)
		width   - input image width  (pixels)
		channels - input image features (1 for grayscale, 3 for RGB)
		classes - number of output classes (2 in paper)
		features - number of output features for first convolution (64 in paper)
		Number of features double after each down sampling block
		depth  - number of downsampling operations (4 in paper)
		padding - 'valid' (used in paper) or 'same'
		batchnorm - include batch normalization layers before activations
		dropout - fraction of units to dropout, 0 to keep all units
		Output:
		RGU-Net model expecting input shape (height, width, maps) and generates
		output with shape (output_height, output_width, classes). If padding is
		'same', then output_height = height and output_width = width.

		"""

		#create the grid and levelset
		print(' RGMM in u-net version')
		n_rows=28
		self.n_mm=n_mixturemodel
		A, nodes_coordinates = grid_graph(n_rows)
		coarsening_levels=self.depth
		u_shape, u_rows, u_cols, u_val, perm = coarsen_mnist(A, coarsening_levels, nodes_coordinates)

		self.u = []
		for level in range(coarsening_levels):
		    self.u.append([u_shape[level], u_rows[level], u_cols[level], u_val[level]])

		print(self.u)
		x = Input(shape=(self.height, self.width, self.channels))
		#x=convert_train_data(x,perm,self.im_length)
		print(x.shape)
		inputs = x
		skips = []
		self.d=d
		print(self.height, self.width)
		for i in range(self.depth):
			x, x0 = self.RGMM_downsampling_block(x, batch_size, (self.depth-i-1),mm_case_i,portion )
			skips.append(x0)
			self.features *= 2
		# add RGMMet layer
		x= RGMMNet(u=self.u[0], d=self.d, n_mixture_models=n_mixturemodel, n_hidden=self.features, batch_size=batch_size, trainable=True)(x)

		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		# add RGMMet layer
		x= RGMMNet(u=self.u[self.depth], d=self.d, n_mixture_models=n_mixturemodel, n_hidden=self.features, batch_size=batch_size, trainable=True)(x)

		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.main_activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x
		for i in reversed(range(self.depth)):
			self.features //= 2
			x = self.RGMM_upsampling_block(x,batch_size, skips[i], (self.depth-i), mm_case_i, portion)

		# add RGMMet layer
		x= RGMMNet(u=self.u[0], d=self.d, n_mixture_models=n_mixturemodel, n_hidden=self.features, batch_size=batch_size, trainable=True)(x)
		
		logits = Lambda(lambda z: z/self.temperature)(x)

		probabilities = Activation('softmax')(logits)
		
		return Model(inputs=inputs, outputs=probabilities)













######################################## FCN ####################################################################
#https://github.com/aurora95/Keras-FCN/blob/master/models.py

	


#def FCN_Vgg16_32s(self, input_shape=None, weight_decay=0., batch_momentum=0.9, batch_shape=None, classes=self.classes):
#    if batch_shape:
#        img_input = Input(batch_shape=batch_shape)
#        image_size = batch_shape[1:3]
#    else:
#        img_input = Input(shape=input_shape)
#        image_size = input_shape[0:2]

	def FCN_Vgg16_32s(self, weight_decay=0., batch_momentum=0.9):
		classes=self.classes
		img_input = Input(shape=(self.height, self.width, self.channels))
		image_size=[self.height, self.width, self.channels]
		# Block 1
		x = Conv2D(self.features, (3, 3), activation='relu', padding='same', name='block1_conv1', kernel_regularizer=l2(weight_decay))(img_input)
		x = Conv2D(self.features, (3, 3), activation='relu', padding='same', name='block1_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = MaxPooling2D((2, 2), strides=(2, 2), name='block1_pool')(x)

		# Block 2
		x = Conv2D(2*self.features, (3, 3), activation='relu', padding='same', name='block2_conv1', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(2*self.features, (3, 3), activation='relu', padding='same', name='block2_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = MaxPooling2D((2, 2), strides=(2, 2), name='block2_pool')(x)

		# Block 3
		x = Conv2D(4*self.features, (3, 3), activation='relu', padding='same', name='block3_conv1', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(4*self.features, (3, 3), activation='relu', padding='same', name='block3_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(4*self.features, (3, 3), activation='relu', padding='same', name='block3_conv3', kernel_regularizer=l2(weight_decay))(x)
		x = MaxPooling2D((2, 2), strides=(2, 2), name='block3_pool')(x)

		# Block 4
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block4_conv1', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block4_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block4_conv3', kernel_regularizer=l2(weight_decay))(x)
		x = MaxPooling2D((2, 2), strides=(2, 2), name='block4_pool')(x)

		# Block 5
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block5_conv1', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block5_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block5_conv3', kernel_regularizer=l2(weight_decay))(x)
		x = MaxPooling2D((2, 2), strides=(2, 2), name='block5_pool')(x)

		# Convolutional layers transfered from fully-connected layers
		x = Conv2D(64*self.features, (7, 7), activation='relu', padding='same', name='fc1', kernel_regularizer=l2(weight_decay))(x)
		x = Dropout(0.5)(x)
		x = Conv2D(64*self.features, (1, 1), activation='relu', padding='same', name='fc2', kernel_regularizer=l2(weight_decay))(x)
		x = Dropout(0.5)(x)
		#classifying layer
		x = Conv2D(classes, (1, 1), kernel_initializer='he_normal', activation='linear', padding='valid', strides=(1, 1), kernel_regularizer=l2(weight_decay))(x)

		x = BilinearUpSampling(target_size=tuple(image_size))(x) #(size=(0.5*self.features, 0.5*self.features))(x)
		model = Model(img_input, x)

		return model

	def AtrousFCN_Vgg16_16s(self, weight_decay=0., batch_momentum=0.9):
		classes=self.classes
		img_input = Input(shape=(self.height, self.width, self.channels))
		image_size=[self.height, self.width, self.channels]
		# Block 1
		x = Conv2D(self.features, (3, 3), activation='relu', padding='same', name='block1_conv1', kernel_regularizer=l2(weight_decay))(img_input)
		x = Conv2D(self.features, (3, 3), activation='relu', padding='same', name='block1_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = MaxPooling2D((2, 2), strides=(2, 2), name='block1_pool')(x)

		# Block 2
		x = Conv2D(2*self.features, (3, 3), activation='relu', padding='same', name='block2_conv1', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(2*self.features, (3, 3), activation='relu', padding='same', name='block2_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = MaxPooling2D((2, 2), strides=(2, 2), name='block2_pool')(x)

		# Block 3
		x = Conv2D(4*self.features, (3, 3), activation='relu', padding='same', name='block3_conv1', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(4*self.features, (3, 3), activation='relu', padding='same', name='block3_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(4*self.features, (3, 3), activation='relu', padding='same', name='block3_conv3', kernel_regularizer=l2(weight_decay))(x)
		x = MaxPooling2D((2, 2), strides=(2, 2), name='block3_pool')(x)

		# Block 4
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block4_conv1', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block4_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block4_conv3', kernel_regularizer=l2(weight_decay))(x)
		x = MaxPooling2D((2, 2), strides=(2, 2), name='block4_pool')(x)

		# Block 5
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block5_conv1', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block5_conv2', kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(8*self.features, (3, 3), activation='relu', padding='same', name='block5_conv3', kernel_regularizer=l2(weight_decay))(x)


		# Convolutional layers transfered from fully-connected layers
		x = Conv2D(64*self.features, (7, 7), activation='relu', padding='same', name='fc1', kernel_regularizer=l2(weight_decay))(x)
		x = Dropout(0.5)(x)
		x = Conv2D(64*self.features, (1, 1), activation='relu', padding='same', name='fc2', kernel_regularizer=l2(weight_decay))(x)
		x = Dropout(0.5)(x)
		#classifying layer
		x = Conv2D(classes, (1, 1), kernel_initializer='he_normal', activation='linear', padding='valid', strides=(1, 1), kernel_regularizer=l2(weight_decay))(x)

		x = BilinearUpSampling(target_size=tuple(image_size))(x) #0.5*self.features, 0.5*self.features))(x)
		model = Model(img_input, x)

		return model


	def FCN_Resnet50_32s(self, weight_decay=0., batch_momentum=0.9, batch_shape=None):

		classes=self.classes
		image_size=[self.height, self.width, self.channels]
		img_input = Input(shape=(self.height, self.width, self.channels))
		bn_axis = 3

		x = Conv2D(self.features, (7, 7), strides=(2, 2), padding='same', name='conv1', kernel_regularizer=l2(weight_decay))(img_input)
		x = BatchNormalization(axis=bn_axis, name='bn_conv1')(x)
		x = Activation('relu')(x)
		x = MaxPooling2D((3, 3), strides=(2, 2))(x)

		x = conv_block(3, [self.features, self.features, 4*self.features], stage=2, block='a', strides=(1, 1))(x)
		x = identity_block(3, [self.features, self.features, 4*self.features], stage=2, block='b')(x)
		x = identity_block(3, [self.features, self.features, 4*self.features], stage=2, block='c')(x)

		x = conv_block(3, [2*self.features, 2*self.features, 8*self.features], stage=3, block='a')(x)
		x = identity_block(3, [2*self.features, 2*self.features, 8*self.features], stage=3, block='b')(x)
		x = identity_block(3, [2*self.features, 2*self.features, 8*self.features], stage=3, block='c')(x)
		x = identity_block(3, [2*self.features, 2*self.features, 8*self.features], stage=3, block='d')(x)

		x = conv_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='a')(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='b')(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='c')(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='d')(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='e')(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='f')(x)

		x = conv_block(3, [8*self.features, 8*self.features, 32*self.features], stage=5, block='a')(x)
		x = identity_block(3, [8*self.features, 8*self.features, 32*self.features], stage=5, block='b')(x)
		x = identity_block(3, [8*self.features, 8*self.features, 32*self.features], stage=5, block='c')(x)
		    #classifying layer
		x = Conv2D(classes, (1, 1), kernel_initializer='he_normal', activation='linear', padding='valid', strides=(1, 1), kernel_regularizer=l2(weight_decay))(x)
		x = BilinearUpSampling(target_size=tuple(image_size))(x)

		model = Model(img_input, x)
		return model


	def AtrousFCN_Resnet50_16s(self, weight_decay=0., batch_momentum=0.9, batch_shape=None):

		classes=self.classes
		img_input = Input(shape=(self.height, self.width, self.channels))
		image_size=[self.height, self.width, self.channels]
		bn_axis = 3

		x = Conv2D(self.features, (7, 7), strides=(2, 2), padding='same', name='conv1', kernel_regularizer=l2(weight_decay))(img_input)
		x = BatchNormalization(axis=bn_axis, name='bn_conv1', momentum=batch_momentum)(x)
		x = Activation('relu')(x)
		x = MaxPooling2D((3, 3), strides=(2, 2))(x)

		x = conv_block(3, [self.features, self.features, 4*self.features], stage=2, block='a', weight_decay=weight_decay, strides=(1, 1), batch_momentum=batch_momentum)(x)
		x = identity_block(3, [self.features, self.features, 4*self.features], stage=2, block='b', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)
		x = identity_block(3, [self.features, self.features, 4*self.features], stage=2, block='c', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)

		x = conv_block(3, [2*self.features, 2*self.features, 8*self.features], stage=3, block='a', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)
		x = identity_block(3, [2*self.features, 2*self.features, 8*self.features], stage=3, block='b', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)
		x = identity_block(3, [2*self.features, 2*self.features, 8*self.features], stage=3, block='c', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)
		x = identity_block(3, [2*self.features, 2*self.features, 8*self.features], stage=3, block='d', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)

		x = conv_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='a', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='b', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='c', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='d', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='e', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)
		x = identity_block(3, [4*self.features, 4*self.features, 16*self.features], stage=4, block='f', weight_decay=weight_decay, batch_momentum=batch_momentum)(x)

		x = atrous_conv_block(3, [8*self.features, 8*self.features, 32*self.features], stage=5, block='a', weight_decay=weight_decay, atrous_rate=(2, 2), batch_momentum=batch_momentum)(x)
		x = atrous_identity_block(3, [8*self.features, 8*self.features, 32*self.features], stage=5, block='b', weight_decay=weight_decay, atrous_rate=(2, 2), batch_momentum=batch_momentum)(x)
		x = atrous_identity_block(3, [8*self.features, 8*self.features, 32*self.features], stage=5, block='c', weight_decay=weight_decay, atrous_rate=(2, 2), batch_momentum=batch_momentum)(x)
		#classifying layer
		#x = Conv2D(classes, (3, 3), dilation_rate=(2, 2), kernel_initializer='normal', activation='linear', padding='same', strides=(1, 1), kernel_regularizer=l2(weight_decay))(x)
		x = Conv2D(classes, (1, 1), kernel_initializer='he_normal', activation='linear', padding='same', strides=(1, 1), kernel_regularizer=l2(weight_decay))(x)
		x = BilinearUpSampling(target_size=tuple(image_size))(x)

		model = Model(img_input, x)

		return model


	def Atrous_DenseNet(self, weight_decay=1E-4, batch_momentum=0.9, batch_shape=None, include_top=True, activation='sigmoid'):

		classes=self.classes
		if include_top is True:
			return densenet.DenseNet(depth=None, nb_dense_block=3, growth_rate=32, nb_filter=-1, nb_layers_per_block=[6, 12, 24, 16], bottleneck=True, reduction=0.5, dropout_rate=0.2, weight_decay=1E-4,
		include_top=True, top='segmentation', weights=None, input_tensor=None, input_shape=input_shape, classes=classes, transition_dilation_rate=2, transition_kernel_size=(1, 1), transition_pooling=None)

		img_input = Input(shape=(self.height, self.width, self.channels))
		input_shape = _obtain_input_shape(init_img_input,   default_size=32, min_size=16,  data_format=K.image_data_format(), include_top=False)
		img_input = Input(shape=input_shape)

		x = densenet.__create_dense_net(classes, img_input, depth=None, nb_dense_block=3, growth_rate=32,  nb_filter=-1, nb_layers_per_block=[6, 12, 24, 16],  bottleneck=True, reduction=0.5, dropout_rate=0.2, weight_decay=1E-4, top='segmentation', input_shape=input_shape, transition_dilation_rate=2, transition_kernel_size=(1, 1), transition_pooling=None, include_top=include_top)

		x = self.top(x, input_shape, classes, activation, weight_decay)

		model = Model(img_input, x, name='Atrous_DenseNet')

		return model


	def DenseNet_FCN(self,  weight_decay=1E-4, batch_momentum=0.9, batch_shape=None, include_top=True, activation='sigmoid'):

		classes=self.classes
		if include_top is True:
			return densenet.DenseNetFCN(input_shape=input_shape,weights=None, classes=classes, nb_layers_per_block=[4, 5, 7, 10, 12, 15], growth_rate=16, dropout_rate=0.2)

		init_img_input = Input(shape=(self.height, self.width, self.channels))
		input_shape = _obtain_input_shape(init_img_input, default_size=0.5*self.features,  min_size=16, data_format=K.image_data_format(), include_top=False)
		img_input = Input(shape=input_shape)

		x = densenet.__create_fcn_dense_net(classes, img_input, input_shape=input_shape, nb_layers_per_block=[4, 5, 7, 10, 12, 15],  growth_rate=16,  dropout_rate=0.2, include_top=include_top)

		x = self.top(x, input_shape, classes, activation, weight_decay)

		model = Model(img_input, x, name='DenseNet_FCN')

		return model

	def top(self, x, input_shape, classes, activation, weight_decay):

		x = Conv2D(classes, (1, 1), activation='linear', padding='same', kernel_regularizer=l2(weight_decay), use_bias=False)(x)

		if K.image_data_format() == 'channels_first':
			channel, row, col = input_shape
		else:
			row, col, channel = input_shape

		  
		if activation is 'sigmoid':
			x = Reshape((row * col * classes,))(x)

		return x
