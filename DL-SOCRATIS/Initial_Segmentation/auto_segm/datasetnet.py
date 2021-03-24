#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk
from __future__ import division, print_function
import glob
import matplotlib.patches as patches
import json
import numpy as np
from matplotlib.path import Path
import pydicom
import pydicom.uid
import dicom
import cv2
import matplotlib.pyplot as plt
import os
import scipy.misc
from PIL import Image
from PIL.Image import fromarray
import tensorflow as tf
import vtk
from vtk.util import numpy_support
from vtk.util.numpy_support import vtk_to_numpy
from auto_segm import config
import argparse
from keras import utils
from pydicom.uid import ExplicitVRLittleEndian
from pydicom.uid import generate_uid
from PIL import Image, ImageDraw
#from med2image import med2image
from auto_segm.model.RGMMnet.coarsening import *


class datasetnet:

#####################################
##### INITIALIZATION ##############
####################

	def __init__ (self,analysis,path_case) :
		"""
		Initializare of the config file

		"""
		args = config.parse_arguments()
		self.path_case=path_case
		self.restore_from_jpg_tif=args.restore_image
		self.rotated="False"
		if self.path_case=='roi':
			self.image_shape=args.image_shape_roi
			self.original_image_shape=args.original_image_shape_roi
			self.roi_shape=args.roi_shape_roi
			self.data_path=args.datapath
			self.data_path2=args.datapath
			self.STORE_TXT=args.store_txt
			self.counter_path='/contour/'
			self.data_extention = args.data_extention_roi
			self.counter_extention = args.counter_extention_roi
			self.PATH_IMAGES='/image'
			self.PATH_IMAGES2='/image'	
		if self.path_case=='main':
			if self.restore_from_jpg_tif=='off':	
				self.image_shape=args.image_shape
				self.original_image_shape=args.original_image_shape
				self.roi_shape=args.roi_shape
				self.data_path=args.datapath
				self.data_path2=args.datapath
				self.STORE_TXT=args.store_txt
				self.counter_path='/contour/'
				self.data_extention = args.data_extention_roi
				self.counter_extention = args.counter_extention_roi
				self.PATH_IMAGES='/image'
				self.PATH_IMAGES2='/image'				
			else:
				self.image_shape=args.image_shape
				self.original_image_shape=args.original_image_shape
				self.roi_shape=args.roi_shape
				self.data_path=args.datapath
				self.data_path2=args.datapath
				self.STORE_TXT=args.store_txt
				self.counter_path='/contour/'
				self.data_extention = args.data_extention
				self.counter_extention = args.counter_extention
				self.PATH_IMAGES='/image'
				self.PATH_IMAGES2='/image'

		if self.path_case=='pre':
			self.image_shape=args.image_shape_pre
			self.original_image_shape=args.original_image_shape_pre
			self.roi_shape=args.roi_shape_pre
			self.data_path=args.datapath
			self.data_path2=args.datapath
			self.STORE_TXT=args.store_txt
			self.counter_path='/contour/'
			self.data_extention = args.data_extention_pre
			self.counter_extention = args.counter_extention_pre		
			self.PATH_IMAGES='/image'
			self.PATH_IMAGES2='/image'
			self.pretrain_window=args.pretrain_window
		
		# seperate the train of ROI with the train set for the u_net. Thus take the epi and endo seperate in u_net and train the ROI detection in both
		if (analysis=='train' or analysis=='test') :		
			self.n_set_pre=analysis
			self.n_set=analysis
		else:
			self.n_set_pre='train_prediction'
			self.n_set='train'

		self.patient_list=args.patient_list
		self.store_contour=args.store_data_test
		self.image_part = np.zeros([self.original_image_shape,self.original_image_shape])
		self.shuffle=args.shuffle
		self.num_preprocess_threads=args.num_cores
		self.batch_size=args.batch_size
		self.patient_store_style=args.patient_store_style
		self.STORE_PATH=args.store_data_test
		#if MICCAI_2009:
		self.label_case1=args.label_case_extension_1
		self.label_case2=args.label_case_extension_2
		self.label_classes=args.label_classes
		if self.label_classes=='both':
			self.classes=2
		elif self.label_classes=='three':
			self.classes=3
		elif self.label_classes=='four':
			self.classes=4
		elif self.label_classes=='first':
			self.classes=1
		elif self.label_classes=='second':
			self.classes=1
		elif self.label_classes=='third':
			self.classes=1
		elif self.label_classes=='fourth':
			self.classes=1	
		else:
			print("No accebtable label_classes!! ")
		# add more if if need it more classes define (TODO extend in more than 2 classes)


#####################################
##### BASE FUNCTION OF CLASS############
####################


	def create_dataset(self):
		"""
		Creating the dataset from the images and the contour for the CNN-unet-algorithm.
		
		mask: the matrix of labels in txt no visible (exception in main is the same with cmask)
 		cmask: the matrix of labels in image version visible (polygon version)

		"""  
		# Create dataset from determine json file
		series = json.load(open(self.STORE_TXT+'/'+self.patient_list))[self.n_set]

		if series=={}:
			print('The %s is empty...' %self.n_set)
			X, Y, cmask_total=[],[],[]
		else:
			self.count=1
			self.count4=1
			self.count1=0
			self.count3=0
			# call the way that the data are stored
			if self.patient_store_style=='use':
				images_total,images_total_full,mask_total,cmask_total=self.quick_use_data(series)

			elif self.patient_store_style=='MICCAI_2009':
				images_total,images_total_full,mask_total,cmask_total=self.MICCAI_2009_data_store_style(series)
			else:
				print("Please define stored style 'use' 'MICCAI_2009' 'MICCAI_2017' ")
			print('DATASET SHAPE BEFORE RESAMPLE:', np.array(images_total).shape,np.array(mask_total).shape)
			# reshape the outpouts
			X = np.reshape(np.array(images_total), [len(images_total), self.image_shape, self.image_shape, 1])
			# store the masks
			Y = np.reshape(np.array(mask_total), [len(mask_total), mask_total.shape[3], self.roi_shape, self.roi_shape])
			Xtotal = np.reshape(np.array(images_total_full), [len(images_total_full), self.original_image_shape, self.original_image_shape, 1])
		print('Dataset shape :', X.shape, Y.shape)
		return X,Xtotal, Y, cmask_total, self.STORE_TXT

	def load_images(self,contours_list_series,case=None,serie=None):
		"""
		load images base of the path dcm or nii, png, jpg
		"""
		images, images_fullsize = [], []
		print(len(contours_list_series))
		for o in range(0,len(contours_list_series)):	
			c=contours_list_series[o]
			# Get images path base the store style for ROI
			if self.patient_store_style=='MICCAI_2009':
				if (self.path_case=='roi' or self.path_case=='pre' or (self.path_case=='main' and self.restore_from_jpg_tif=='off')):
					count=c	
					#determine image path
					if self.n_set == 'train': 
						image_path =  self.data_path + self.PATH_IMAGES + '/%s/%s-%s.%s' %(case,serie,count, self.data_extention)
					elif self.n_set == 'test': 
						image_path =  self.data_path + self.PATH_IMAGES2 + '/%s/%s-%s.%s' %(case,serie,count, self.data_extention)
					else:
						print("Please define the analysis 'test' 'train' ")
			elif self.patient_store_style=='use':	
				image_path=c
				print(c)
			else:
				print("Please define stored style 'use' 'MICCAI_2009' 'MICCAI_2017' ")
			# The path of main is determine base the roi where it stored so no need of store style... Just overwrite the previous cases...
			#if self.path_case=='main' and self.restore_from_jpg_tif=="on":
				#determine image path
				#if self.n_set == 'train': 
					#image_path =  self.data_path + self.PATH_IMAGES + '/ROI%s%s.%s' %(self.n_set, self.count1, self.data_extention)
				#elif self.n_set == 'test': 
					#image_path =  self.data_path + self.PATH_IMAGES2 + '/ROI%s%s.%s' %(self.n_set, self.count1, self.data_extention)
				#else:
					#print("Please define the analysis 'test' 'train' ")

			# open image as numpy array 
			if self.data_extention=='jpeg' or self.data_extention=='png' or self.data_extention=='tif' or self.data_extention=='jpg':
				#print(image_path)                                       
				image_part_store = Image.open(image_path).convert('L')
				#print(image_part_store.bits) 
				self.image_part= np.array(image_part_store)


			elif self.data_extention=='dcm':
				# dicom and pydicom call 
				try:
					self.image_part = dicom.read_file(image_path).pixel_array
					#print("normal dicom")

				except dicom.errors.InvalidDicomError as exc:
					try:
						dcm_file = pydicom.dcmread(image_path)
						self.image_part =dcm_file.pixel_array.astype(float)
						#print("normal pydicom")
				
					except pydicom.errors.InvalidDicomError as exc:				
						
						print("general exception dicom")
						self._dcm = dicom.read_file(image_path,force=True)
						image = self._dcm.pixel_array
						self.image_part=image
						print(self.image_part.shape)
					except:
						pass
					finally:
						pass 
				finally:
					pass
				#self.image_part= ArrayDicom
			else:
				print("Please define the extension of image data 'dcm' 'jpeg' ")
			#check for rotate image
			height, width = np.array(self.image_part).shape[0],np.array(self.image_part).shape[1]
			if width < height:
				np.rot90(np.array(self.image_part)) 
				self.rotated="True"
			else:
				self.image_part=self.image_part
			#resize
			image_p=cv2.resize((self.image_part), (self.image_shape, self.image_shape),interpolation=cv2.INTER_NEAREST)
			image_p=np.reshape(np.array(image_p),[1, np.array(image_p).shape[0], np.array(image_p).shape[1]])
			full_image=cv2.resize((self.image_part), (self.original_image_shape, self.original_image_shape),interpolation=cv2.INTER_NEAREST)
			full_image=np.reshape(np.array(full_image),[1, np.array(full_image).shape[0], np.array(full_image).shape[1]])

			if o==0:
				images=image_p
				images_fullsize=full_image
				
			if o!=0:	
				images=np.append(image_p, images, axis=0) 
				images_fullsize=np.append(full_image, images_fullsize, axis=0)
			#print(len(np.array(images).shape))
			
			if (len(np.array(images).shape)==4): 
				images=np.reshape(np.array(images),[len(np.array(images))*np.array(images).shape[3], np.array(images).shape[1], np.array(images).shape[2]])
				images_fullsize=np.reshape(np.array(images_fullsize),[len(np.array(images_fullsize))*np.array(images_fullsize).shape[3], np.array(images_fullsize).shape[1], np.array(images_fullsize).shape[2]])
			#print(np.array(images).shape) 
         
		self.count1=self.count1+1
		#if (self.count1>9):
		#	self.count12="A"+str(self.count)
		#	self.count=self.count+1
		#case without dependence of mask with image in data slices etc)
		return images,images_fullsize

	def load_masks(self,contours_list_series,contour_path_base=None,label=None):
		"""
		load masks base of the path txt, vtk, jpeg, png

		"""
		contours_jpeg, contours,contours_mask  = [],[],[]
		#print(contours_list)
		for o in range(0,len(contours_list_series)):
			c=contours_list_series[o]
			#print('counder list :', c)
			# Get contours and images path
			#idx_contour = contours_list_series.index(c)
			#print(idx_contour)
			if self.patient_store_style=='MICCAI_2009':
				if (self.path_case=='roi' or self.path_case=='pre'):
					count=c	
					#determine contour path 
					label_image='IM-0001-'
					each_contour_path_l=contour_path_base +label_image
					each_contour_path_l1=each_contour_path_l+'%s' %c
					each_contour_path_list = sorted(glob.glob(each_contour_path_l1+'*')) 
					each_contour_path=each_contour_path_list[0]				
				if (self.path_case=='main' and self.restore_from_jpg_tif=='off'):
					count=c	
					#determine contour path 
					each_contour_path_list = sorted(glob.glob(contour_path_base + '*'+'-%s%s' %(c,label))) 
					each_contour_path=each_contour_path_list[0]
					#print(each_contour_path)
			elif self.patient_store_style=='use':	
				each_contour_path=c
			else:
				print("Please define stored style 'use' 'MICCAI_2009' 'MICCAI_2017' ")
				# The path of main is determine base the roi where it stored so no need of store style... Just overwrite the previous cases...
			#if self.path_case=='main' and self.restore_from_jpg_tif=="on":
				#determine  contour path
				#if self.n_set == 'train': 
					#each_contour_path =  self.data_path + self.PATH_IMAGES + '/ROImasks%s.%s' %( self.count3,self.counter_extention)
				#elif self.n_set == 'test': 
					#each_contour_path =  self.data_path + self.PATH_IMAGES2 + '/ROImasks%s.%s' %( self.count3, self.counter_extention)
				#else:
					#print("Please define the analysis 'test' 'train'  ")
				
			#convert label contour data to jpeg or data of images
			if self.counter_extention=='txt':
				contour,count_mask=self.txt_converter(each_contour_path)
				contours.append(contour)
				contours_mask.append(count_mask)
			elif self.counter_extention=='vtk':
				contour,count_mask=self.vtk_converter(each_contour_path)
				contours.append(contour)
				contours_mask.append(count_mask)
			elif self.counter_extention=='jpeg' or self.counter_extention=='png'or self.data_extention=='tif' or self.data_extention=='jpg':
				contour_mask_store = Image.open(each_contour_path).convert('L')
				contours_mask.append(cv2.resize(np.array(contour_mask_store), (self.original_image_shape, self.original_image_shape),interpolation=cv2.INTER_NEAREST) )
				contours_jpeg.append(cv2.resize(np.array(contour_mask_store), (self.roi_shape, self.roi_shape),interpolation=cv2.INTER_AREA) )    				
			
			# dicom and pydicom call
			elif self.counter_extention=='dcm':
				try:
					contour_mask_store= dicom.read_file(each_contour_path).pixel_array
					contours_mask.append(cv2.resize(np.array(contour_mask_store), (self.original_image_shape, self.original_image_shape),interpolation=cv2.INTER_NEAREST) )
					contours_jpeg.append(cv2.resize(np.array(contour_mask_store), (self.roi_shape, self.roi_shape),interpolation=cv2.INTER_AREA) )
				
				except dicom.errors.InvalidDicomError as exc:
					try:
						dcm_file = pydicom.dcmread(each_contour_path)
						contour_mask_store =dcm_file.pixel_array.astype(float)
						contours_mask.append(cv2.resize(np.array(contour_mask_store), (self.original_image_shape, self.original_image_shape),interpolation=cv2.INTER_NEAREST) )
						contours_jpeg.append(cv2.resize(np.array(contour_mask_store), (self.roi_shape, self.roi_shape),interpolation=cv2.INTER_AREA) )
					
                                               
					except pydicom.errors.InvalidDicomError as exc:
						print("general exception dicom")
						in_dcm = dicom.read_file(each_contour_path,force=True)
						contour_mask_store = in_dcm.pixel_array
						contours_mask.append(cv2.resize(np.array(contour_mask_store), (self.original_image_shape, self.original_image_shape),interpolation=cv2.INTER_NEAREST) )
						contours_jpeg.append(cv2.resize(np.array(contour_mask_store), (self.roi_shape, self.roi_shape),interpolation=cv2.INTER_AREA) )
						print(self.image_part.shape)
					except:
						pass
					finally:
						pass
				finally:
					pass


			else:
				print("Please define the extension of mask data 'dcm' 'txt' 'jpeg' 'vtk' ")	
			       
			self.count3=self.count3+1
			#if (self.count3>9):
			#	self.count3="A"+str(self.count4)
			#	self.count4=self.count4+1
		if self.counter_extention=='dcm' or self.counter_extention=='jpeg' or self.counter_extention=='png'or self.data_extention=='tif' or self.data_extention=='jpg':
			Ymask = (contours_jpeg)
		else:
			Ymask = (contours)

		Ymask= np.reshape(np.array(Ymask,dtype='uint8'), [len(Ymask), self.roi_shape, self.roi_shape, 1])
		contours_mask=np.reshape(np.array(contours_mask,dtype='uint8'), [len(contours_mask), self.original_image_shape, self.original_image_shape, 1])
		return Ymask,contours_mask


#####################################
#####STORE STYLE##############
####################


	def quick_use_data(self,series):
		"""
		quick use of small random dataset no dependence image with masks according the path.(mask file inside the image file of the patient)

		"""
		images_total,images_total_full,mask_total,cmask_total=[],[],[],[]
		o=0
		for case, serie in series.items():
			contour_path_base = self.data_path + self.PATH_IMAGES + '/%s' % (case)
			#print(contour_path_base)
			contours_list_single=[]			
			contours_list = sorted(glob.glob(contour_path_base +'/*.%s' %self.data_extention)) 
			#print(contours_list)	
			contours_list2 = sorted(glob.glob(contour_path_base + self.counter_path+'*.%s' %self.counter_extention))# need modification does not take in series but it takes the data randomly so different from images
			#print(contours_list2)
			#contours_list_single.append(contours_list[0])
			images,images_full= self.load_images(contours_list)
			mask,cmask= self.load_masks(list(contours_list2))
			if(o==0):
				cmask_total=cmask
				mask_total=mask
				images_total=images
				images_total_full=images_full
			print(np.array(images).shape)
			print(np.array(images_total).shape)
			print(np.array(mask).shape)
			print(np.array(mask_total).shape)
			if(o!=0):
				mask_total=np.append(mask,mask_total,axis=0)
				cmask_total=np.append(cmask,cmask_total,axis=0)
				images_total=np.append(images,images_total,axis=0)
				images_total_full=np.append(images_full,images_total_full,axis=0)
			o=o+1	
		Y_total=np.array(mask_total)
		cY_total=np.array(cmask_total)
		X_total=np.array(images_total)
		cX_total=np.array(images_total_full)
		print(X_total.shape,cX_total.shape)
		print(Y_total.shape,cY_total.shape)
		X_total=np.reshape(X_total, [X_total.shape[0], X_total.shape[1], X_total.shape[2], 1])
		cX_total=np.reshape(cX_total, [cX_total.shape[0], cX_total.shape[1], cX_total.shape[2], 1])
		Y_total=np.reshape(Y_total, [Y_total.shape[0], Y_total.shape[1], Y_total.shape[2], 1])
		cY_total=np.reshape(cY_total, [cY_total.shape[0], cY_total.shape[1], cY_total.shape[2], 1])
		if(len(X_total.shape)==5):
			X_total=np.reshape(X_total, [X_total.shape[0]*X_total.shape[1], X_total.shape[2], X_total.shape[3], 1])
			cX_total=np.reshape(cX_total, [cX_total.shape[0]*cX_total.shape[1], cX_total.shape[2], cX_total.shape[3], 1])
			Y_total=np.reshape(Y_total, [Y_total.shape[0]*Y_total.shape[1], Y_total.shape[2], Y_total.shape[3], 1])
			cY_total=np.reshape(cY_total, [cY_total.shape[0]*cY_total.shape[1], cY_total.shape[2], cY_total.shape[3], 1])
		return X_total,cX_total,Y_total,cY_total


			
	def MICCAI_2009_data_store_style(self,series):
		"""
		MICCAI 2009 dependence image with masks according the path.

		"""
		classes=self.classes	
		labels=[self.label_case1,self.label_case2] #define the string of deferent label masks from configuration file		
		o=0
		images_total,images_total_full,mask1,cmask1=[],[],[],[]
		while o <classes:
			label=labels[o]
			print(label)
			initialize=0
			mask_total, cmask_total= [],[]
			for case, serie in series.items():
				contour_path_base = self.data_path2 + self.counter_path + '%s/' % (case) 			
				if (self.path_case!='main' and self.n_set_pre=='train' and o==0):
					contours_list = sorted(glob.glob(contour_path_base + '*')) 
					contours_list_ser = ["0" for i in contours_list]
					count2=0
					for k in contours_list:	
						contour_list_subpart=k			
						countp = contour_list_subpart.count("/")		
						contours_list_ser.insert(count2,k.split('/')[countp].split('-')[2])	
						count2=count2+1 	
						# Loop over the contours/images
					contours_list_series = [contours_list_ser[contours_list.index(k)] for k in contours_list]
					images,images_full= self.load_images(list(contours_list_series),case,serie)
					mask,cmask= self.load_masks(list(contours_list_series),contour_path_base)
				else:
					# add more cases with respect tha label_classes (TODO extend in more than 2 classes)
					if  self.label_classes=='both':
						#create in both cases the lists
						#first list
						contours_list1 = sorted(glob.glob(contour_path_base + '*'+'%s' %(self.label_case1))) 
						contours_list_ser = ["0" for i in contours_list1]
						count21=0
						for k in contours_list1:	
							contour_list_subpart=k			
							countp = contour_list_subpart.count("/")		
							contours_list_ser.insert(count21,k.split('/')[countp].split('-')[2])	 	
							count21=count21+1	
					
						# Loop over the contours/images
						contours_list_series1 = [contours_list_ser[contours_list1.index(k)] for k in contours_list1]
						#second list
						contours_list2 = sorted(glob.glob(contour_path_base + '*'+'%s' %(self.label_case2)) )
						contours_list_ser = ["0" for i in contours_list2]
						count22=0
						for k in contours_list2:	
							contour_list_subpart=k			
							countp = contour_list_subpart.count("/")		
							contours_list_ser.insert(count22,k.split('/')[countp].split('-')[2])	 	
							count22=count22+1	
						# Loop over the contours/images
						contours_list_series2 = [contours_list_ser[contours_list2.index(k)] for k in contours_list2]
						# compair two list take only the one that exist in both
						contours_list_series0= sorted(set(contours_list_series1) & set(contours_list_series2))
						mask,cmask= self.load_masks(list(contours_list_series0), contour_path_base, label)
						mask=np.asarray(mask)
						cmask=np.asarray(cmask)
						# call the function of image only the first time(the images are dependend on the smaller length of classes
						if o==0:	
							images,images_full= self.load_images(list(contours_list_series0),case,serie)
					else:
						if  self.label_classes=='first':								
							contours_list1 = sorted(glob.glob(contour_path_base + '*'+'%s' %(self.label_case1))) #if work find a way to pass epi and endo
							label=	self.label_case1					
						elif  self.label_classes=='second':
							contours_list1 = sorted(glob.glob(contour_path_base + '*'+'%s' %(self.label_case2))) #if work find a way to pass epi and endo
							label=	self.label_case2
						# add more if if more classes are available (better structure TODO)
						else:								
							print('ERROR: label_classes must be first or second or both')
							contours_list1=None
						contours_list_ser = ["0" for i in contours_list1]
						count2=0
						# print(contours_list)
						for k in contours_list1:	
							contour_list_subpart=k			
							countp = contour_list_subpart.count("/")		
							contours_list_ser.insert(count2,k.split('/')[countp].split('-')[2])	 	
							count2=count2+1	
					
						# Loop over the contours/images
						contours_list_series1 = sorted([contours_list_ser[contours_list1.index(k)] for k in contours_list1])
						images,images_full= self.load_images(contours_list_series1,case,serie)
						mask,cmask= self.load_masks(contours_list_series1,contour_path_base, label)

				if initialize==0:
					cmask_total=cmask
					mask_total=mask
					if o==0:
						images_total=images
						images_total_full=images_full
					initialize=1
				else:
					cmask_total=np.append(cmask_total,cmask, axis=0)	
					mask_total=np.append(mask_total,mask, axis=0)
					if o==0: 
						images_total=np.append(images_total,images, axis=0)
						images_total_full=np.append(images_total_full,images_full, axis=0)	   				
				# store first class , if more than two need more store (TODO extend in more than 2 classes)
			if (classes!=1 and o==0):
				mask1=mask_total
				cmask1=cmask_total

			elif (classes!=1 and o!=0):		
				mask_total=np.append(mask1,mask_total,axis=3)
				cmask_total=np.append(cmask1,cmask_total,axis=3)
			else:
				print("Only one class of segmentation in dataset! ")
			o=o+1
			print('Dataset shape pre:', mask_total.shape, cmask_total.shape)
		return images_total,images_total_full,mask_total,cmask_total


	def create_pretrain_dataset(self):	 
		"""
		Creating the dataset for the initialize the weights of the roi analysis.
		""" 
		images_pre = []
		images,X,Y, self.contour_mask, self.STORE_TXT=self.create_dataset()
		# peak a random x,y in space
		for i in range(len(images)):	
			for a in range(21):
				x1,y1 =np.random.choice(self.image_shape-self.pretrain_window-1,2) 
				#sliding randomly the window
				single_image=images[i]
				crop_img = single_image[y1:y1+self.pretrain_window, x1:x1+self.pretrain_window]
				images_pre.append(cv2.resize(crop_img, (self.pretrain_window, self.pretrain_window)))
		X = np.reshape(np.array(images_pre), [len(images_pre), self.pretrain_window, self.pretrain_window])
		
		return X


#####################################
##### Compute ROI ##############
####################

	def roi(self, contour):
		"""
		Create a binary mask with ROI from contour. 
		Extract the maximum square around the contour.

		"""
		#print(contour[:,0].min())
		X_min, Y_min = contour[:,0].min(), contour[:,1].min()
		X_max, Y_max = contour[:,0].max(), contour[:,1].max()  
		w = X_max - X_min
		h = Y_max - Y_min
		mask_roi = np.zeros([self.original_image_shape,self.original_image_shape])
		if w > h :	
			mask_roi[int(Y_min - (w -h)/2):int(Y_max + (w -h)/2), int(X_min):int(X_max)] = 1.0
			
		else :
			mask_roi[int(Y_min):int(Y_max), int(X_min - (h-w)/2):int(X_max + (h -w)/2)] = 1.0
		return cv2.resize(mask_roi, (self.roi_shape, self.roi_shape),interpolation=cv2.INTER_NEAREST)

	def compute_pred_roi(self, X, y_pred, mask_contour):
		"""
		Computing the ROI from the original image 
		"""
		X_array = np.array(X)
		X_fullsize=cv2.resize(X_array.reshape((X_array.shape[0],X_array.shape[1])),(self.original_image_shape,self.original_image_shape),interpolation=cv2.INTER_NEAREST)
		#print(np.array(mask_contour).shape)
		# up sampling from 32x32 to original MR size
		pred = cv2.resize(y_pred.reshape((y_pred.shape[1], y_pred.shape[2])), (self.original_image_shape,self.original_image_shape), interpolation=cv2.INTER_NEAREST)
		mask_contour_original=cv2.resize(np.reshape(np.array(mask_contour), [np.array(mask_contour).shape[0],np.array(mask_contour).shape[1]]), (self.original_image_shape, self.original_image_shape), interpolation=cv2.INTER_NEAREST)
		#print(np.array(mask_contour_original).shape)
		pos_pred = np.array(np.where(pred > 0.5))
		# get the center of the mask
		X_min, Y_min = pos_pred[0, :].min(), pos_pred[1, :].min()
		X_max, Y_max = pos_pred[0, :].max(), pos_pred[1, :].max()  
		X_middle = X_min + (X_max - X_min) / 2
		Y_middle = Y_min + (Y_max - Y_min) / 2
		# Find ROI coordinates
		X_top = int(X_middle - 50)
		Y_top = int(Y_middle - 50)
		X_down = int(X_middle + 50)
		Y_down = int(Y_middle + 50)
		# crop ROI of size 100x100
		mask_roi = np.zeros((self.original_image_shape, self.original_image_shape))
		mask_roi = cv2.rectangle(mask_roi, (X_top, Y_top), (X_down, Y_down), 1, -1)#*255
		X_fullsize=X_fullsize[X_top:X_down, Y_top:Y_down]
		mask_contour_original=mask_contour_original[X_top:X_down, Y_top:Y_down]

		X_fullsize = cv2.resize(X_fullsize.reshape((X_fullsize.shape[0], X_fullsize.shape[1])), (self.original_image_shape,self.original_image_shape), interpolation=cv2.INTER_NEAREST)

		if ((self.n_set =='test') and (self.path_case =='roi')):
			mask_contour_original = cv2.resize(np.reshape(np.array(mask_contour_original), [np.array(mask_contour_original).shape[0], np.array(mask_contour_original).shape[1]]), (self.original_image_shape, self.original_image_shape), interpolation=cv2.INTER_NEAREST)
		else:
			mask_contour_original = cv2.resize(np.reshape(np.array(mask_contour_original), [np.array(mask_contour_original).shape[0], np.array(mask_contour_original).shape[1]]), (self.roi_shape,self.roi_shape), interpolation=cv2.INTER_NEAREST)

		# select the non null pixels
		#pos_pred_mask = np.array(np.where(mask_contour_original > 0.5))
		#rows_mask,cos_mask=pos_pred_mask[0],pos_pred_mask[1]
		#mask_contour_original[rows_mask,cos_mask]=1
		#pos_pred_mask2 = np.array(np.where(mask_contour_original <= 0.5))
		#rows_mask2,cos_mask2=pos_pred_mask2[0],pos_pred_mask2[1]
		#mask_contour_original[rows_mask2,cos_mask2]=0
		#mask_contour_original=np.array(mask_contour_original, dtype='uint8')
		return X_fullsize, mask_roi, mask_contour_original

	def compute_pred_main(self, X, y_pred):
		"""
		Computing the ROI from the original image 
		"""
		X_fullsize = cv2.resize(np.array(X), (self.original_image_shape,self.original_image_shape), interpolation=cv2.INTER_NEAREST)
		clone=np.array(X_fullsize)
		if  self.label_classes=='both':
			# up sampling from 32x32 to original MR size
			pred = cv2.resize(y_pred[:,:,2].reshape((y_pred[:,:,2].shape[0], y_pred[:,:,2].shape[1])), (self.original_image_shape,self.original_image_shape), interpolation=cv2.INTER_NEAREST)
			# select the non null pixels
			pred2 = cv2.resize(y_pred[:,:,1].reshape((y_pred[:,:,1].shape[0], y_pred[:,:,1].shape[1])), (self.original_image_shape,self.original_image_shape), interpolation=cv2.INTER_NEAREST)
			#print(pred)
			#print(pred2)
			pos_pred = np.array(np.where(pred < 1))
			#print(pos_pred)
			rows1,cols1=pos_pred[0],pos_pred[1]
			clone[rows1,cols1]=0
			#print(clone)
			pos_pred2 = np.array(np.where(pred2 > 1))
			rows2,cols2=pos_pred2[0],pos_pred2[1]
			clone[rows2,cols2]=0
			#print(clone)


		if  self.label_classes=='first':
			pred2 = cv2.resize(y_pred[:,:,0].reshape((y_pred[:,:,0].shape[0], y_pred[:,:,0].shape[1])), (self.original_image_shape,self.original_image_shape), interpolation=cv2.INTER_NEAREST)
			pos_pred2 = np.array(np.where(pred2 > 1))
			rows2,cos2=pos_pred2[0],pos_pred2[1]
			clone[rows2,cos2]=0
			#print(pos_pred2)


		if  self.label_classes=='second':
			# up sampling from 32x32 to original MR size
			pred = cv2.resize(y_pred[:,:,0].reshape((y_pred[:,:,0].shape[0], y_pred[:,:,0].shape[1])), (self.original_image_shape,self.original_image_shape), interpolation=cv2.INTER_NEAREST)
			pos_pred = np.array(np.where(pred > 1))
			rows1,cos1=pos_pred[0],pos_pred[1]
			clone[rows1,cos1]=0
			#print(pos_pred)

		return clone 

#####################################
##### Extention OF the Mask DATA##############
####################

	def txt_converter(self,each_contour_path):
		"""
		Convert the contour from txt extention to jpg

		"""

		contours,contour_mask=[],[]
		if self.path_case!='main':
			# modify path to take the name of contour
			#countverticle = each_contour_path.count("/")		
			#contours_name=each_contour_path.split('/')[countverticle]
			#con_name=contours_name[:-4]
			#print (each_contour_path)
			#open contours as numpy array
			contour = []
			file = open(each_contour_path, 'rb') 
			for line in file: 
				contour.append(tuple(map(float, line.split())))
			contour = np.array(contour)
			#print(each_contour_path)
			cont=self.roi(contour)
			contours.append(cont)
			# create mask contour with experts contours
			x, y = np.meshgrid(np.arange(self.original_image_shape), np.arange(self.original_image_shape)) # make a canvas with coordinates
			x, y = x.flatten(), y.flatten()
			points = np.vstack((x,y)).T 
			p = Path(contour) # make a polygon
			grid = p.contains_points(points)
			mask_contour = grid.reshape(self.original_image_shape, self.original_image_shape,1)
			mask_contour=mask_contour
			contour_mask.append(mask_contour)
			#cv2.imwrite( self.store_contour+'/Contour_Image/%s.jpeg' %con_name, cont)

		if self.path_case=='main':
			x, y = np.loadtxt(each_contour_path).T
			if self.rotated=="True":
				x, y = y, self.image_height - x
			BW_8BIT = 'L'
			polygon = list(zip(x, y))
			image_dims = (self.original_image_shape, self.original_image_shape)
			img = Image.new(BW_8BIT, image_dims, color=0)
			ImageDraw.Draw(img).polygon(polygon, outline=1, fill=1)
			norm=1
			contour_mask.append(norm * np.array(img, dtype='uint8'))
			contours.append(norm * np.array(img, dtype='uint8'))

		return  contours ,contour_mask
	
	def vtk_converter(self,each_contour_path):
		"""
		Convert the contour from vtk extention to jpg

		"""
		contours,contour_mask=[],[]
		vtkarray = self.GetPointData().GetArray(each_contour_path)
		if vtkarray:
			array = vtk_to_numpy(vtkarray)
		if array.dtype == np.int8:
			array = array.astype(np.bool) 
		if self.path_case!='main':
			contour = array	
			cont=self.roi(contour)	
			contours.append(cont)
			# modify path to take the name of contour		
			#countverticle = each_contour_path.count("/")		
			#contours_name=each_contour_path.split('/')[countverticle]
			#con_name=contours_name[:-4]
			# create mask contour with experts contours
			x, y = np.meshgrid(np.arange(self.original_image_shape), np.arange(self.original_image_shape)) # make a canvas with coordinates
			x, y = x.flatten(), y.flatten()
			points = np.vstack((x,y)).T 
			p = Path(contour) # make a polygon
			grid = p.contains_points(points)
			mask_contour = grid.reshape(self.original_image_shape,self.original_image_shape,1)
			mask_contour=mask_contour
			contour_mask.append(mask_contour)
			#print(contour_mask)
			#cv2.imwrite( self.store_contour+'/Contour_Image/%s.jpeg' %con_name, cont)
		if self.path_case=='main':
			x, y = np.loadtxt(each_contour_path).T
			if self.rotated=="True":
				x, y = y, self.image_height - x
			BW_8BIT = 'L'
			polygon = array
			image_dims = (self.original_image_shape, self.original_image_shape)
			img = Image.new(BW_8BIT, image_dims, color=0)
			ImageDraw.Draw(img).polygon(polygon, outline=1, fill=1)
			norm=1
			contour_mask.append(norm * np.array(img, dtype='uint8'))
			contours.append(norm * np.array(img, dtype='uint8'))
		return  contours,contour_mask

#####################################
##### GAN ##############
####################

	def Gan_preprocessing_images(self,image,mask, shift_param = [-128, -0.5], rescale_param = [128, 0.5]):
		min_queue_examples= 2500
		processed_image=tf.cast(image, tf.float32)
		processed_seg=tf.cast(mask, tf.float32)
		if shift_param != 0:
			processed_image = tf.add(processed_image, shift_param[0])
  
		if rescale_param != 0:
			processed_image = tf.multiply(processed_image, 1.0/rescale_param[0])

		if shift_param != 0:
			processed_seg = tf.add(processed_seg, shift_param[1])
  
		if rescale_param != 0:
			processed_seg = tf.multiply(processed_seg, 1.0/rescale_param[1])

		if self.shuffle == True:
			images, segmentations = tf.train.shuffle_batch([processed_image, processed_seg], batch_size=self.batch_size, num_threads=self.num_preprocess_threads, capacity=min_queue_examples + 3 * self.batch_size, enqueue_many=True, min_after_dequeue=min_queue_examples)
		else:
			images, segmentations = tf.train.batch([processed_image, processed_seg], batch_size=self.batch_size, num_threads=self.num_preprocess_threads, capacity=min_queue_examples + 3 * self.batch_size,   enqueue_many=True)
	    
		return images, segmentations
