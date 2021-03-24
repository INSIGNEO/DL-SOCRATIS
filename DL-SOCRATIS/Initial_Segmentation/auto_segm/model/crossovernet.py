from __future__ import division, print_function
from tensorflow.python.ops import control_flow_ops
from random import shuffle
import numpy as np
import tensorflow as tf
import keras
from keras.preprocessing.image import ImageDataGenerator
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten, Reshape, Concatenate
from keras.layers import Conv2D, MaxPooling2D, AveragePooling2D
from keras.preprocessing.image import ImageDataGenerator
from keras.models import Model


class CrossoverNet(object):	

	def __init__ (self, features, height, channels, classes, width):
		self.activation='relu'
		args = config.parse_arguments()
		self.height=height
		self.channels=channels
		self.classes=classes
		self.features=features
		self.padding=args.padding 
		self.batchnorm=args.batchnorm	
		self.dropout=args.dropout
		self.width=width
		self.temperature=1.0 
		self.max_norm_const=args.max_norm_const
		self.max_norm_value=args.max_norm_value
		self.portion_step=0.2
		self.portion_step2=0.1
		

	def deep_conv_height(self,input_tensor):
		"""
		"""

		x = Conv2D(self.features, kernel_size=(5,3), padding=self.padding)(input_tensor)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x = Conv2D(2*self.features, kernel_size=(5,3), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x= MaxPooling2D(pool_size=(2,2))(x)

		x = Conv2D(4*self.features, kernel_size=(5,3), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		crossover_loss_h=x

		x= MaxPooling2D(pool_size=(2,2))(x)

		x = Conv2D(4*self.features, kernel_size=(3,3), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x = Conv2D(4*self.features, kernel_size=(7,1), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x = Conv2D(4*self.features, kernel_size=(7,1), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x = Conv2D(8*self.features, kernel_size=(7,1), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x


		x=(Flatten(name="flatten"))(x)
		output_h=Dense(500, activation='sigmoid',kernel_regularizer=regularizers.l2(0.0001))(x)


		return output_h, crossover_loss_h

	def deep_conv_weight(self):
		"""
		"""
		x = Conv2D(self.features, kernel_size=(3,5), padding=self.padding)(input_tensor)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x = Conv2D(2*self.features, kernel_size=(3,5), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x= MaxPooling2D(pool_size=(2,2))(x)

		x = Conv2D(4*self.features, kernel_size=(3,5), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		crossover_loss_w=x

		x= MaxPooling2D(pool_size=(2,2))(x)

		x = Conv2D(4*self.features, kernel_size=(3,3), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x = Conv2D(4*self.features, kernel_size=(1,7), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x = Conv2D(4*self.features, kernel_size=(1,7), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x

		x = Conv2D(8*self.features, kernel_size=(1,7), padding=self.padding)(x)
		x = BatchNormalization()(x) if self.batchnorm else x
		x = Activation(self.activation)(x)
		x = Dropout(self.dropout)(x) if self.dropout > 0 else x


		x=(Flatten(name="flatten"))(x)
		output_w=Dense(500, activation='sigmoid', kernel_regularizer=regularizers.l2(0.0001))(x)


		return output_w, crossover_loss_w

	def create_grid(self,input_t):

		step_h=int(self.portion_step*self.height)
		step_w=int(self.portion_step2*self.width)

		patch_size_h = [1,step_w,step_h,self.channels]
		patch_size_w= [1,step_h,step_w,self.channels]

		patches_h=tf.extract_image_patches([input_t],patch_size_h,patch_size_h,[1,1,1,1],'VALID')
		patches_w=tf.extract_image_patches([input_t],patch_size_w,patch_size_w,[1,1,1,1],'VALID')
			
		height, cross_h=self.deep_conv_height(patches_h)
		weight, cross_w=self.deep_conv_weight(patches_w)
		merged=Concatenate([height,weight])
		merged=Reshape([1,1,1000])(merged)
		merged=Dense(1000, activation='relu', kernel_regularizer=regularizers.l2(0.0001))(merged)
		output=Dense(2, activation='sigmoid')(merged)
		output=Reshape([1,1,2])(output)
		logits = Lambda(lambda z: z/self.temperature)(output)
		out = Activation('softmax')(logits)
		cr_h=cross_h
		cr_w=cross_w

		for i in range(1,self.height-step_h):
			for j in range(1,self.width-step-w):
				patches_h=tf.extract_image_patches([input_t],patch_size_h,patch_size_h,[1,j,i,1],'VALID')
				patches_w=tf.extract_image_patches([input_t],patch_size_w,patch_size_w,[1,i,j,1],'VALID')
			
				height, cross_h=self.deep_conv_height(patches_h)
				weight, cross_w=self.deep_conv_weight(patches_w)
				merged=Concatenate([height,weight])
				merged=Reshape([1,1,1000])(merged)
				merged=Dense(1000, activation='relu', kernel_regularizer=regularizers.l2(0.0001))(merged)
				output=Dense(2, activation='sigmoid')(merged)
				output=Reshape([1,1,2])(output)
				logits = Lambda(lambda z: z/self.temperature)(output)
				probabilities = Activation('softmax')(logits)
				out=Concatenate([probabilities,out])
				cr_h=Concatenate([cross_h,cr_h])
				cr_w=Concatenate([cross_w,cr_w])
		return out,cr_h,cr_w


	def crossover(self):
		x = Input(shape=(self.height, self.width, self.channels))
		inputs=x
		out,cross_h,cross_w=create_grid(x)
		return Model(inputs=x, outputs=out)

