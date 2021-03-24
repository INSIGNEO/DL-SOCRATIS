#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk


from __future__ import division, print_function
import cv2
from keras import backend as K
from auto_segm import config, regularization, handle_data
from keras.preprocessing.image import ImageDataGenerator
from auto_segm.regularization import data_augmentation
from keras import losses, optimizers, utils, metrics
from keras.utils import np_utils
import tensorflow as tf
from math import ceil
from auto_segm import config, regularization, datasetnet
from auto_segm.gan_synthetic import *
import argparse
import logging
import os
import numpy as np
import matplotlib.cm as cm
import pylab
from pylab import *
from PIL import Image
from scipy.ndimage import zoom
from auto_segm.model.RGMMnet.coarsening import *

class handle_data:

	def __init__ (self, X,Y,case) :
		args = config.parse_arguments()
		self.case=case
		self.gan_train_directory=args.gan_train_directory
		self.data_augm=args.data_augm		
		self.batch_size=args.batch_size
		self.batch_size_test=args.batch_size_test
		self.num_cores=args.num_cores
		self.validation_split=args.validation_split
		self.validation=args.validation	
		self.shuffle=args.shuffle
		self.normalize_image=args.normalize
		self.gancheckpoint='checkpoint'
		self.gan_synthetic=args.gan_synthetic
		self.num_synthetic_images=args.num_synthetic_images
		self.batch_size=args.batch_size
		self.X=X
		self.Y=Y
		self.fourier=args.fft_convert_data
		self.STORE_PATH=args.store_data_test
		self.STORE_PATH10=self.STORE_PATH +'/ROI/train/'
		self.STORE_PATH1=self.STORE_PATH +'/FFT/train/'
		self.data_extention="jpeg"
		boolt='False'
		self.main_model=args.main_model
		if (args.main_model=='rgu_net' and self.case=='main'):
			A, nodes_coordinates = grid_graph(args.height)
			coarsening_levels=args.depth
			u_shape, u_rows, u_cols, u_val, perm = coarsen_mnist(A, coarsening_levels, nodes_coordinates)
			self.X=convert_train_data(X,perm,args.height)
			print('the modified shape of X input because of RGMM structure is: ')
			print(self.X.shape)


		if boolt=='True':
			self.threshold_bin=0.8
		else:
			self.threshold_bin=0.05

	def gan_synthetic_data_generate (self) :

		'''
			Run the Generative Adversarial Network train model synthesis images 
		'''
		print("Generate synthetic data")
		#if case=='main':
		image_size=128#self.original_image_shape_main, instead of 284(modify this so to take the image shape TODO )
		#else:
		#	image_size=self.original_image_shape_roi
		checkpoint_folder = self.gan_train_directory + "/" + self.gancheckpoint +"/"#+ '/checkpoint%d.ckpt' % self.stored_cycle_number
		#tf.reset_default_graph()
		dcgan = DCGAN(batch_size=self.batch_size)# define both classes of masks
		o=0
		dsn=datasetnet.datasetnet('test','main')# need the test data from ROI to create synthetic images for endo and epi seperatly 
		dsn.image_shape= 128 
		r_masks,resize_masks= [], []
		while o < 1:#self.classes:
			r_mask_store=resize_masks
			resize_masks,images_resize = [],[]
			
			for i in range(self.batch_size):
				image_resize = cv2.resize(self.X[i,:,:,0].reshape(self.X.shape[1],self.X.shape[2]), (image_size, image_size), cv2.INTER_NEAREST)
				resize_mask = cv2.resize(self.Y[i,o,:,:].reshape(self.Y.shape[2],self.Y.shape[3]), (image_size, image_size), cv2.INTER_NEAREST)
				resize_mask=np.array(resize_mask).reshape(1,image_size,image_size)
				resize_masks.append(resize_mask)				
				images_resize.append(image_resize)
				if o>0:	
					r_masks=np.append(r_mask_store,resize_masks,axis=1)
				else:
					r_masks=resize_masks
			o=o+1

		mask_resize=np.array(r_masks).reshape(self.batch_size, image_size, image_size, 1)#Y.shape[1]))
		images_resize=np.array(images_resize).reshape(self.batch_size, image_size, image_size, 1)#Y.shape[1]))

		X_pre, Y_pre=dsn.Gan_preprocessing_images(images_resize,mask_resize)
		#print(X_pre.shape, Y_pre.shape)
		image_structure = tf.concat([X_pre, Y_pre], 3)
		imgs=dcgan.test_images(row=8, col=8, num_samples = None, inputs=None, checkpoint_folder=checkpoint_folder, num_synthetic_images=self.num_synthetic_images,image_structure=image_structure)
			
		for k in range(self.num_synthetic_images):

			imgs_folder = os.path.join(self.gan_train_directory, 'synthetic_images/images/') 
			if not os.path.exists(imgs_folder):
				os.makedirs(imgs_folder)           
			segs_folder = os.path.join(self.gan_train_directory, 'synthetic_images/masks/') 
			if not os.path.exists(segs_folder):
				os.makedirs(segs_folder)
			img_channel = imgs[k][:, :, 0]
			img_seg = imgs[k][:, :, 1]


			imsave(os.path.join(imgs_folder, 'img_%d.jpeg') % k,img_channel.reshape(image_size, image_size), cmap = cm.Greys_r)
			imsave(os.path.join(segs_folder, 'segmented_%d.jpeg') % k,img_seg.reshape(image_size, image_size) , cmap = cm.Greys_r)      

		#X_synthetic, Y_synthetic= [], []
		#X_synthetic.append(imgs[:,:,:,0])
		#Y_synthetic.append(imgs[:,:,:,1])

		#if self.normalize_image == 'True':
		#	X_synthetic=regularization.normalize(X_synthetic)
		#return X_synthetic, Y_synthetic

	def gan_synthetic_data (self) :

		'''
			Run the Generative Adversarial Network train model synthesis images 
		'''
		print("Load synthetic data")
		image_size=128
		X_synthetic, Y_synthetic= [], []
		imgs_folder = os.path.join(self.gan_train_directory, 'synthetic_images/images/img_%d.jpeg' % 0)           
		segs_folder = os.path.join(self.gan_train_directory, 'synthetic_images/masks/segmented_%d.jpeg' % 0)      
		img = Image.open(imgs_folder).convert('L')
		seg = Image.open(segs_folder).convert('L')
		img=np.reshape(np.array(img),[1, image_size, image_size, 1])
		seg=np.reshape(np.array(seg),[1, image_size, image_size, 1])
		X_synthetic=np.array(img)
		Y_synthetic=np.array(seg)

		for k in range(1,self.num_synthetic_images):
			imgs_folder = os.path.join(self.gan_train_directory, 'synthetic_images/images/img_%d.jpeg' % k)           
			segs_folder = os.path.join(self.gan_train_directory, 'synthetic_images/masks/segmented_%d.jpeg' % k)      
			img = Image.open(imgs_folder).convert('L')
			seg = Image.open(segs_folder).convert('L')
			img=np.reshape(np.array(img),[1, image_size, image_size, 1])
			seg=np.reshape(np.array(seg),[1, image_size, image_size, 1])
			X_synthetic=np.append(np.array(img),X_synthetic,axis=0)
			Y_synthetic=np.append(np.array(seg),Y_synthetic,axis=0)

		print(X_synthetic.shape,Y_synthetic.shape)
		X_synthetic,Y_synthetic=self.resize_data(X_synthetic,Y_synthetic)
		print(X_synthetic.shape,Y_synthetic.shape)

		if self.normalize_image == 'True':
			X_synthetic=regularization.normalize(X_synthetic)
		return X_synthetic, Y_synthetic

	def validation_data (self,structure_case=None) :

		print("Start the Cross validation")
		pass_total_validation_augmen, validation_augment= [],[]
		split_index = int((1-self.validation_split) * len(self.X))
		train_steps_per_epoch = ceil(split_index / self.batch_size)
		val_steps_per_epoch = ceil((len(self.X) - split_index) / self.batch_size)
		val_Xaug, val_Yaug=[],[]
		print(self.X.shape,self.Y.shape)

		if self.gan_synthetic == 'False' and self.data_augm == 'True':
			Xtraining_augment,Ytraining_augment=self.augmented_data(split_index)
			print(self.X.shape,self.Y.shape, np.array(Xtraining_augment).shape,np.array(Ytraining_augment).shape)
			Xtraining_augment_total,Ytraining_augment_total=self.connect_data(self.X[:split_index],self.Y[:split_index],Xtraining_augment,Ytraining_augment)
			if self.fourier=="on":
				Xtraining_augment_total,Ytraining_augment_total=self.fourier_convert_data (Xtraining_augment_total,Ytraining_augment_total )
			Ytraining_augment_total=self.binary_masks(Ytraining_augment_total)
			training_augment=ImageDataGenerator().flow(Xtraining_augment_total,Ytraining_augment_total, batch_size=self.batch_size)
			print("Training data structure:")
			print(Ytraining_augment_total.shape, Xtraining_augment_total.shape)
			val_Xaug.append(self.X[split_index:])
			val_Yaug.append(self.Y[split_index:])
			#suffle the data
			if self.shuffle == 'True':
				valstate = np.random.get_state()
				np.random.shuffle(val_Xaug)
				np.random.set_state(valstate)
				np.random.shuffle(val_Yaug)
			#reshape to (total_image,height,weight,channels)
			val_Xaug1, val_Yaug2= np.asarray(val_Xaug) , np.asarray(val_Yaug)
			val_Xaug1=val_Xaug1.reshape((val_Xaug1.shape[0]*val_Xaug1.shape[1],val_Xaug1.shape[2],val_Xaug1.shape[3],val_Xaug1.shape[4]))
			val_Yaug2= val_Yaug2.reshape((val_Yaug2.shape[0]*val_Yaug2.shape[1],val_Yaug2.shape[2],val_Yaug2.shape[3],val_Yaug2.shape[4]))
				#create an Image data generator
			if self.fourier=="on":
				val_Xaug1,val_Yaug2=self.fourier_convert_data (val_Xaug1,val_Yaug2 )
			val_Yaug2=self.binary_masks(val_Yaug2)
			validation_augment=ImageDataGenerator().flow(val_Xaug1, val_Yaug2, batch_size=self.batch_size)
			print("Validating data structure:")
			print(val_Yaug2.shape, val_Xaug1.shape)
			# reset the epochs and split		
			train_steps_per_epoch = ceil(len(Xtraining_augment_total)/ self.batch_size)
			val_steps_per_epoch = ceil(len(val_Xaug1) / self.batch_size)

		elif self.gan_synthetic == 'True' and self.data_augm == 'False':
			Xtraining_gan,Ytraining_gan=self.gan_synthetic_data()
			Xtraining_augment_total,Ytraining_augment_total=self.connect_data(self.X[:split_index],self.Y[:split_index],Xtraining_gan,Ytraining_gan)
			if self.fourier=="on":
				Xtraining_augment_total,Ytraining_augment_total=self.fourier_convert_data (Xtraining_augment_total,Ytraining_augment_total )
			Ytraining_augment_total=self.binary_masks(Ytraining_augment_total)
			training_augment=ImageDataGenerator().flow(Xtraining_augment_total,Ytraining_augment_total, batch_size=self.batch_size)
			print("Training data structure:")
			print(Ytraining_augment_total.shape, Xtraining_augment_total.shape)
			val_Xaug.append(self.X[split_index:])
			val_Yaug.append(self.Y[split_index:])
			#suffle the data
			if self.shuffle == 'True':
				valstate = np.random.get_state()
				np.random.shuffle(val_Xaug)
				np.random.set_state(valstate)
				np.random.shuffle(val_Yaug)
			#reshape to (total_image,height,weight,channels)
			val_Xaug1, val_Yaug2= np.asarray(val_Xaug) , np.asarray(val_Yaug)
			val_Xaug1=val_Xaug1.reshape((val_Xaug1.shape[0]*val_Xaug1.shape[1],val_Xaug1.shape[2],val_Xaug1.shape[3],val_Xaug1.shape[4]))
			val_Yaug2= val_Yaug2.reshape((val_Yaug2.shape[0]*val_Yaug2.shape[1],val_Yaug2.shape[2],val_Yaug2.shape[3],val_Yaug2.shape[4]))
			#create an Image data generator
			if self.fourier=="on":
				val_Xaug1,val_Yaug2=self.fourier_convert_data (val_Xaug1,val_Yaug2 )
			val_Yaug2=self.binary_masks(val_Yaug2)
			validation_augment=ImageDataGenerator().flow(val_Xaug1, val_Yaug2, batch_size=self.batch_size)
			print("Validating data structure:")
			print(val_Yaug2.shape, val_Xaug1.shape)
			# reset the epochs and split		
			train_steps_per_epoch = ceil(len(Xtraining_augment_total)/ self.batch_size)
			val_steps_per_epoch = ceil(len(val_Xaug1) / self.batch_size)

		elif self.gan_synthetic == 'True' and self.data_augm == 'True':
			Xtraining_gan,Ytraining_gan=self.gan_synthetic_data()
			Xtraining_augment,Ytraining_augment=self.augmented_data(split_index)
			Xtraining_gan_total,Ytraining_gan_total=self.connect_data(self.X[:split_index],self.Y[:split_index],Xtraining_gan,Ytraining_gan)
			Xtraining_augment_total,Ytraining_augment_total=self. connect_data(Xtraining_augment,Ytraining_augment,Xtraining_gan_total,Ytraining_gan_total)
			if self.fourier=="on":
				Xtraining_total,Ytraining_total=self.fourier_convert_data (Xtraining_total,Ytraining_total )
			Ytraining_total=self.binary_masks(Ytraining_total)
			training_augment=ImageDataGenerator().flow(Xtraining_augment_total,Ytraining_augment_total, batch_size=self.batch_size)
			print("Training data structure:")
			print(Ytraining_total.shape, Xtraining_total.shape)
			val_Xaug.append(self.X[split_index:])
			val_Yaug.append(self.Y[split_index:])
			#suffle the data
			if self.shuffle == 'True':
				valstate = np.random.get_state()
				np.random.shuffle(val_Xaug)
				np.random.set_state(valstate)
				np.random.shuffle(val_Yaug)
			#reshape to (total_image,height,weight,channels)
			val_Xaug1, val_Yaug2= np.asarray(val_Xaug) , np.asarray(val_Yaug)
			val_Xaug1=val_Xaug1.reshape((val_Xaug1.shape[0]*val_Xaug1.shape[1],val_Xaug1.shape[2],val_Xaug1.shape[3],val_Xaug1.shape[4]))
			val_Yaug2= val_Yaug2.reshape((val_Yaug2.shape[0]*val_Yaug2.shape[1],val_Yaug2.shape[2],val_Yaug2.shape[3],val_Yaug2.shape[4]))
			#create an Image data generator

			if self.fourier=="on":
				val_Xaug1,val_Yaug2=self.fourier_convert_data (val_Xaug1,val_Yaug2 )
			val_Yaug2=self.binary_masks(val_Yaug2)
			validation_augment=ImageDataGenerator().flow(val_Xaug1, val_Yaug2, batch_size=self.batch_size)
			print("Validating data structure:")
			print(val_Yaug2.shape, val_Xaug1.shape)
			# reset the epochs and split		
			train_steps_per_epoch = ceil(len(Xtraining_total)/ self.batch_size)
			val_steps_per_epoch = ceil(len(val_Xaug1) / self.batch_size)

		else:
			if self.fourier=="on":
				self.X,self.Y=self.fourier_convert_data (self.X,self.Y )
			print(self.Y)
			print(np.array(self.Y).shape)
			Y=self.binary_masks(self.Y)
			Xtraining_augment_total=np.asarray(self.X[:split_index])
			Ytraining_augment_total=np.asarray(Y[:split_index])
			val_Xaug1=np.asarray(self.X[split_index:])
			val_Yaug2=np.asarray(Y[split_index:])
			#for i in range(0,int(np.array(self.Y).shape[0])):
			#	str2=self.STORE_PATH1 + 'FFTmasks_back%s.%s' % (i,self.data_extention)
			#	pylab.imsave(str2, Y[i,:,:,0], format=self.data_extention, cmap = cm.Greys_r)
			#	
			#	str3=self.STORE_PATH1 + 'FFTmasks_frond%s.%s' % (i,self.data_extention)    
			#	pylab.imsave(str3, Y[i,:,:,1], format=self.data_extention, cmap = cm.Greys_r)
			#	str1=self.STORE_PATH1 + 'FFTroi%s.%s' % (i,self.data_extention)
			#	cv2.imwrite(str1,  self.X[i])

			training_augment=ImageDataGenerator().flow(self.X[:split_index], Y[:split_index], batch_size=self.batch_size)
			validation_augment=ImageDataGenerator().flow(self.X[split_index:], Y[split_index:], batch_size=self.batch_size)
			# reset the epochs and split

		if structure_case==None:		
			return training_augment, train_steps_per_epoch,validation_augment, val_steps_per_epoch

		if structure_case=="dbn":
					
			return Xtraining_augment_total,Ytraining_augment_total,val_Xaug1, val_Yaug2			


	def no_validation_data (self) :
		print("No cross validation")
		pass_total_validation_augmen, validation_augment= [],[]
		split_index = len(self.X)
		train_steps_per_epoch = ceil(split_index / self.batch_size)
		if self.gan_synthetic == 'False' and self.data_augm == 'True':
			Xtraining_augment,Ytraining_augment=self.augmented_data(split_index)
			Xtraining_augment_total,Ytraining_augment_total=self.connect_data(self.X,self.Y,Xtraining_augment,Ytraining_augment)

			if self.fourier=="on":
				Xtraining_augment_total,Ytraining_augment_total=self.fourier_convert_data (Xtraining_augment_total,Ytraining_augment_total )
			Ytraining_augment_total=self.binary_masks(Ytraining_augment_total)
			training_augment=ImageDataGenerator().flow(Xtraining_augment_total,Ytraining_augment_total, batch_size=self.batch_size)
			print("Training data structure:")
			print(Ytraining_augment_total.shape, Xtraining_augment_total.shape)

		elif self.gan_synthetic == 'True' and self.data_augm == 'False':
			Xtraining_augment,Ytraining_augment=self.gan_synthetic_data()
			Xtraining_augment_total,Ytraining_augment_total=self.connect_data(self.X,self.Y,Xtraining_augment,Ytraining_augment)

			if self.fourier=="on":
				Xtraining_augment_total,Ytraining_augment_total=self.fourier_convert_data (Xtraining_augment_total,Ytraining_augment_total )
			Ytraining_augment_total=self.binary_masks(Ytraining_augment_total)
			training_augment=ImageDataGenerator().flow(Xtraining_augment_total,Ytraining_augment_total, batch_size=self.batch_size)
			print("Training data structure:")
			print(Ytraining_augment_total.shape, Xtraining_augment_total.shape)

		elif self.gan_synthetic == 'True' and self.data_augm == 'True':
			Xtraining_augment,Ytraining_augment=self.augmented_data(split_index)
			Xtraining_gan,Ytraining_gan=self.gan_synthetic_data()
			Xtraining_gan_total,Ytraining_gan_total=self.connect_data(self.X,self.Y,Xtraining_gan,Ytraining_gan)
			Xtraining_total,Ytraining_total=self. connect_data(self, Xtraining_augment,Ytraining_augment,Xtraining_gan_total,Ytraining_gan_total)

			if self.fourier=="on":
				Xtraining_total,Ytraining_total=self.fourier_convert_data (Xtraining_total,Ytraining_total )
			Ytraining_total=self.binary_masks(Ytraining_total)
			training_augment=ImageDataGenerator().flow(Xtraining_total,Ytraining_total, batch_size=self.batch_size)
			print("Training data structure:")
			print(Ytraining_total.shape, Xtraining_total.shape)
		else:
			Y=self.binary_masks(self.Y)		
			training_augment=ImageDataGenerator().flow(self.X, Y, batch_size=self.batch_size)
			# reset the epochs and split		

		return training_augment, train_steps_per_epoch

	def augmented_data (self,split_index ) :
		print("Create data augmented images")
		#training augmented data creation				
		Xaug, Yaug = [], []
		for i in data_augmentation(self.X[:split_index],self.Y[:split_index]):
				Xaug.append(i[0])
				Yaug.append(i[1])
		Xaug1, Yaug1= np.asarray(Xaug) , np.asarray(Yaug)
		Xaug1=Xaug1.reshape((Xaug1.shape[0]*Xaug1.shape[1],Xaug1.shape[2],Xaug1.shape[3],Xaug1.shape[4]))
		Yaug1= Yaug1.reshape((Yaug1.shape[0]*Yaug1.shape[1],Yaug1.shape[2],Yaug1.shape[3],Yaug1.shape[4]))
		return Xaug1, Yaug1

	def binary_masks(self,Y):
		print(np.array(Y).shape[1])
		mask_number=np.array(Y).shape[1]
		if mask_number>=8:
			mask_number=np.array(Y).shape[3]
			position_mask_end='true'

		if self.case!='main':
			Yout=np.array(Y)/np.max(Y)

		if self.case=='main':
			if (mask_number==2):
				if position_mask_end=='true':
					if (np.max(Y[:,:,:,0])>3):
						# be sure all the classes are equal to 1 TODO test this 2 classes 
						Y=np.array(Y[:,:,:,0])/np.max(Y[:,:,:,0])

					#first class
					pos_pred_mask = np.array(np.where(Y[:,:,:,0] >= self.threshold_bin))
					a,b,rows_mask=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2]
					Y[a,b,rows_mask,0]=1

					pos_pred_mask = np.array(np.where(Y[:,:,:,0] < self.threshold_bin))
					a1,b1,rows_mask1=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2]
					Y[a1,b1,rows_mask1,0]=0

					# be sure all the classes are equal to 1
					if (np.max(Y[:,:,:,1])>3):
						Y=np.array(Y[:,:,:,1])/np.max(Y[:,:,:,1])

					#second class
					pos_pred_mask = np.array(np.where(Y[:,:,:,1] >= self.threshold_bin))
					a,b,rows_mask=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2]
					Y[a,b,rows_mask,1]=1

					pos_pred_mask = np.array(np.where(Y[:,:,:,1] <self.threshold_bin))
					a2,b2,rows_mask2=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2]
					Y[a2,b2,rows_mask2,1]=0

					Y=np.asarray(Y[:,:,:,0])+np.asarray(Y[:,:,:,1])
					Yout = np_utils.to_categorical(np.asarray(Y),(mask_number+1))
					print(np.array(Yout).shape)
					Yout_final=[]
					Y1=[]
					Y2=[]
					Y1=Yout[:,:,:,1].reshape(np.array(Yout).shape[0],np.array(Yout).shape[1],np.array(Yout).shape[2],1)
					Y2=Yout[:,:,:,2].reshape(np.array(Yout).shape[0],np.array(Yout).shape[1],np.array(Yout).shape[2],1)

					Yout_final=np.append(Y1,Y2,axis=3)	
					Yout_final=np.array(Yout_final)
					Yout_final=Yout_final.reshape(np.array(Yout_final).shape[0],np.array(Yout_final).shape[1],np.array(Yout_final).shape[2],2)
					Yout=[]
					Yout=Yout_final
					print(np.array(Yout_final).shape)
					print(np.array(Yout).shape)
				else:
				
					if (np.max(Y[:,0,:,:])>3):
						# be sure all the classes are equal to 1 TODO test this 2 classes 
						Y=np.array(Y[:,0,:,:])/np.max(Y[:,0,:,:])

					#first class
					pos_pred_mask = np.array(np.where(Y[:,0,:,:] >= self.threshold_bin))
					a,b,rows_mask,cos_mask=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2],pos_pred_mask[3]
					Y[a,b,rows_mask,cos_mask]=1

					pos_pred_mask = np.array(np.where(Y[:,0,:,:] < self.threshold_bin))
					a1,b1,rows_mask1,cos_mask1=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2],pos_pred_mask[3]
					Y[a1,b1,rows_mask1,cos_mask1]=0

					# be sure all the classes are equal to 1
					if (np.max(Y[:,1,:,:])>3):
						Y=np.array(Y[:,1,:,:])/np.max(Y[:,1,:,:])

					#second class
					pos_pred_mask = np.array(np.where(Y[:,1,:,:] >= self.threshold_bin))
					a,b,rows_mask,cos_mask=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2],pos_pred_mask[3]
					Y[a,b,rows_mask,cos_mask]=1

					pos_pred_mask = np.array(np.where(Y[:,1,:,:] <self.threshold_bin))
					a2,b2,rows_mask2,cos_mask2=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2],pos_pred_mask[3]
					Y[a2,b2,rows_mask2,cos_mask2]=0

					Y=np.asarray(Y[:,0,:,:])+np.asarray(Y[:,1,:,:])
					Yout = np_utils.to_categorical(np.asarray(Y),(mask_number+1))
					Yout=Yout.reshape(np.array(Yout).shape[0],np.array(Yout).shape[2],np.array(Yout).shape[3],np.array(Yout).shape[4])
					
			if (mask_number<=1):
				if (np.max(Y)>2):
					Y=np.array(Y)/np.max(Y)
				# be sure all the classes are equal to 1 
				pos_pred_mask = np.array(np.where(Y > self.threshold_bin))
				a,b,rows_mask,cos_mask=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2],pos_pred_mask[3]
				Y[a,b,rows_mask,cos_mask]=1
				Yout = np_utils.to_categorical(np.asarray(Y),(mask_number+1))
				Yout=Yout.reshape(np.array(Yout).shape[0],np.array(Yout).shape[2],np.array(Yout).shape[3],np.array(Yout).shape[4])
		if self.main_model=="rg_net":
			Yout=Yout.reshape(np.array(Yout).shape[0],np.array(Yout).shape[2],np.array(Yout).shape[3],np.array(Yout).shape[1])
		return Yout	


	def connect_data(self, X1,Y1,X2,Y2) :
		print("Connect data_set")
		Xtotal, Ytotal=[],[]
		# add the initial data

		Xtotal=np.append(np.array(X1),np.array(X2),axis=0)
		Ytotal=np.append(np.array(Y1),np.array(Y2),axis=0)
		#reshape to (total_image,height,weight,channels)
		Xtotal1, Ytotal2= np.asarray(Xtotal) , np.asarray(Ytotal)

		#Xtotal1=Xtotal1.reshape((Xtotal1.shape[0]*Xtotal1.shape[1],Xtotal1.shape[2],Xtotal1.shape[3],Xtotal1.shape[4]))
		#Ytotal2= Ytotal2.reshape((Ytotal2.shape[0]*Ytotal2.shape[1],Ytotal2.shape[2],Ytotal2.shape[3],Ytotal2.shape[4]))
		#create an Image data generator
		#suffle the data
		state = np.random.get_state()
		np.random.shuffle(Xtotal1)
		np.random.set_state(state)
		np.random.shuffle(Ytotal2)
		print(Xtotal1.shape,Ytotal2.shape)		
		return Xtotal1,Ytotal2

	def transfer_learning_data (self ):
		print("Transfer_leraning")

		#combine more than one dataset and handle the transfer learning process



	def fourier_convert_data (self,X,Y ):
		print("Fourier transformation of data")
		masks_fft, images_fft=[],[]
		for i in range(len(Y)):

			fx = np.fft.fft2(X[i])
			fshiftx = np.fft.fftshift(fx)
			magnitude_spectrum_x = 20*np.log(np.abs(fshiftx))
			o=0
			print(np.array(X).shape, np.array(Y).shape)
			while o < Y.shape[1]:
				fy = np.fft.fft2(Y[i,o,:,:])
				fshifty = np.fft.fftshift(fy)
				magnitude_spectrum_y = 20*np.log(np.abs(fshifty)+1)
				mask = cv2.resize(magnitude_spectrum_y.reshape(Y.shape[2],Y.shape[3]), (Y.shape[2],Y.shape[3]), cv2.INTER_NEAREST)
				if o>0:	
					mask_y=np.append(mask_y,mask,axis=1)
				else:
					mask_y=mask
				o=o+1

			image_x = cv2.resize(magnitude_spectrum_x.reshape(X.shape[1],X.shape[2]), (X.shape[1],X.shape[2]), cv2.INTER_NEAREST)
			
			mask_y=np.array(mask_y).reshape(Y.shape[1],Y.shape[2],Y.shape[3])

			#STORE_PATH=self.STORE_PATH10
			#str2=STORE_PATH + 'R1masks%s.jpeg' % (i)
			#str3=STORE_PATH + 'R2masks%s.jpeg' % (i)		
			#pylab.imsave(str2, mask_y[0,:,:], format="jpeg", cmap = cm.Greys_r)
			#pylab.imsave(str3, mask_y[1,:,:], format="jpeg", cmap = cm.Greys_r)
			#str1=STORE_PATH + 'Rtrain%s.jpeg' % (i)
			#cv2.imwrite(str1,  image_x)

			masks_fft.append(mask_y)				
			images_fft.append(image_x)
		mask_fft_resize=np.array(masks_fft).reshape((np.array(masks_fft).shape[0], Y.shape[1], Y.shape[2], Y.shape[3]))#Y.shape[1]))
		images_fft_resize=np.array(images_fft).reshape((np.array(images_fft).shape[0], X.shape[1],X.shape[2], 1))#Y.shape[1]))

		return images_fft_resize, mask_fft_resize


	def resize_data(self,X,Y):
		r_masks,resize_masks= [], []
		X,Y=np.asarray(X),np.asarray(Y)
		o=0
		while o < 1:#self.classes:
			r_mask_store=resize_masks
			resize_masks, images_resize=[],[]	
			for i in range(len(Y)):
				image_resize = cv2.resize(X[i,:,:,0].reshape(X.shape[1],X.shape[2]), (self.X.shape[1],self.X.shape[2]), cv2.INTER_NEAREST)
				resize_mask = cv2.resize(Y[i,:,:,o].reshape(Y.shape[1],Y.shape[2]), (self.Y.shape[2],self.Y.shape[3]), cv2.INTER_NEAREST)
				resize_mask=np.array(resize_mask).reshape(1,self.Y.shape[2],self.Y.shape[3])
				resize_masks.append(resize_mask)				
				images_resize.append(image_resize)

			if o>0:	
				r_masks=np.append(r_mask_store,resize_masks,axis=1)
			else:
				r_masks=resize_masks
			o=o+1

		mask_resize=np.array(r_masks).reshape((np.array(r_masks).shape[0], 1,self.Y.shape[2],self.Y.shape[3]))#Y.shape[1]))
		images_resize=np.array(images_resize).reshape((np.array(images_resize).shape[0], self.X.shape[1],self.X.shape[2], 1))#Y.shape[1]))

		return images_resize, mask_resize


	def clipped_zoom(self):
		X=self.X
		Y=self.Y	
		r_masks=[]
		resize_masks=[]

		X=np.asarray(X)
		Y=np.asarray(Y)
		o=0
		h = X.shape[2]
		w = h
		zoom_offset=(h-Y.shape[2])//2

		while o < 2:
			r_mask_store=resize_masks
			images_resize=[]
			resize_masks=[]
			for i in range(len(Y)):
				resize_mask = cv2.resize(Y[i,:,:,o].reshape(Y.shape[1],Y.shape[2]), (self.Y.shape[2],self.Y.shape[2]), cv2.INTER_NEAREST)
				image_resize = cv2.resize(X[i,:,:,0].reshape(X.shape[1],X.shape[2]), (self.X.shape[1],self.X.shape[2]), cv2.INTER_NEAREST)

				# Zero-padding
				top=(zoom_offset)
				left=(zoom_offset)
				top2=(h-top)
				left2=(w-left)

				out = np.zeros_like(image_resize)
				out=np.array(out)
				out.astype(double)
				#print(top,top2,left,left2)
				out[top:top2, left:left2] = resize_mask[0:(Y.shape[2]),0:(Y.shape[2])] #zoom(resize_mask,1)

				out=np.array(out).reshape(1,np.array(out).shape[0],np.array(out).shape[1])
				resize_masks.append(out)
			if o>0:
				r_masks=np.append(r_mask_store,resize_masks,axis=1)
			else:
				r_masks=resize_masks
			o=o+1

		mask_resize=np.array(r_masks).reshape((np.array(r_masks).shape[0], 2, X.shape[1], X.shape[2]))	
		print(np.array(mask_resize).shape)
		print('clipped zoom finished...')
		return np.array(mask_resize)
