#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk


from __future__ import division, print_function
import cv2
from auto_segm import create_net, config, datasetnet, store_model
from auto_segm.utils import vision_outputs
import argparse
import logging
import os
import numpy as np
import matplotlib.cm as cm
import pylab

class parser_data:

	def __init__ (self, rmn,mmn,data_return='off',save_mode='off') :
		args = config.parse_arguments()
		self.roi_model_name=rmn
		self.main_model_name=mmn
		self.data_return=data_return
		self.save_mode=save_mode
		self.STORE_PATH=args.store_data_test
		self.STORE_PATH1=self.STORE_PATH +'/ROI/train/'
		self.STORE_PATH2=self.STORE_PATH+'/ROI/test/'
		self.STORE_PATH_main1=self.STORE_PATH+'/MAIN/train/'
		self.STORE_PATH_main2=self.STORE_PATH+'/MAIN/test/'
		# if the paths does not excist create automatic
#		if not os.path.exists(self.STORE_PATH1):
#			os.makedirs(self.STORE_PATH1)
#		if not os.path.exists(self.STORE_PATH2):
#			os.makedirs(self.STORE_PATH2)
#		if not os.path.exists(self.STORE_PATH_main1):
#			os.makedirs(self.STORE_PATH_main1)
#		if not os.path.exists(self.STORE_PATH_main2):
#			os.makedirs(self.STORE_PATH_main2)
		self.data_extention=args.data_extention
		self.counter_extention=args.counter_extention
		self.restore_from_jpg_tif=args.restore_image

		self.label_classes=args.label_classes
		if self.label_classes=='both':
			self.classes=2
		if self.label_classes=='three':
			self.classes=3
		if self.label_classes=='four':
			self.classes=4
		if self.label_classes=='first':
			self.classes=1
		if self.label_classes=='second':
			self.classes=1
		if self.label_classes=='third':
			self.classes=1
		if self.label_classes=='fourth':
			self.classes=1	
		# add more if if need it more classes define (TODO extend in more than 2 classes)

	def pass_train_roi_data(self,phase,code):	
		'''
			pass the X and Y dimmensions  
		'''
		#test in train datasets
		prediction_image, prediction_mask_total=[],[]
		sm=store_model.store_model()
		dsn=datasetnet.datasetnet("train_pre","roi")
		Xtest, X_total, Y, contour_mask, storetxt = dsn.create_dataset()


		dsn_roi_mask=datasetnet.datasetnet(phase,code) #The size that the data will be store depend in "test", "train" "roi" "main"
		if Xtest==[]:
			print(" no training ... ")
			print(" Continue ... ")
		else:
			pred, metric=sm.load_best_callback(Xtest,Y,'roi',self.roi_model_name)
			
			print('Final train accuracy:', (metric[1]*100.0))
			print (pred.shape)
			# store images 
			length=len(Xtest)
			classes=self.classes	
			o=0

			while o <classes:
				prediction_mask=[]		
				#output, mask_roi, label_mask = dsn_roi_mask.compute_pred_roi(X_total[0], pred[0],contour_mask[0,:,:,0])
				#print(np.array(label_mask))
				for i in range(0,int(length)):
					output, mask_roi, label_mask = dsn_roi_mask.compute_pred_roi(X_total[i], pred[i],contour_mask[i,:,:,o])
					#save to images the results
					if (self.save_mode=='on'):
						
						str2=self.STORE_PATH1 + 'ROImasks%s.%s' % (i+((o)*(length)),self.counter_extention)		
						#cv2.imwrite(str2,  label_mask)
						pylab.imsave(str2, label_mask, format=self.data_extention, cmap = cm.Greys_r)
						if o==0:
							str1=self.STORE_PATH1 + 'ROItrain%s.%s' % (i,self.data_extention)
							cv2.imwrite(str1,  output)
							#pylab.imsave(str1, output, format=self.data_extention, cmap = cm.Greys_r)

					
					prediction_mask.append(label_mask)
					#print(np.array(prediction_mask).shape)
					if o==0:
						prediction=np.array(output)	
						prediction_image.append(prediction)

				prediction_mask=np.array(prediction_mask)
				if o==0:
					prediction_mask=np.reshape((prediction_mask),[(prediction_mask).shape[0], 1 ,(prediction_mask).shape[1],(prediction_mask).shape[2]])	
					prediction_mask_total=prediction_mask
					prediction_image=np.reshape(np.array(prediction_image),[np.array(prediction_image).shape[0],np.array(prediction_image).shape[1],np.array(prediction_image).shape[2],1])
				else:
					prediction_mask=np.reshape((prediction_mask),[(prediction_mask).shape[0], 1 ,(prediction_mask).shape[1],(prediction_mask).shape[2]])
					prediction_mask_total=np.append(prediction_mask_total, prediction_mask,axis=1)
					prediction_mask_total=np.reshape(np.array(prediction_mask_total),[np.array(prediction_mask_total).shape[0],np.array(prediction_mask_total).shape[1],np.array(prediction_mask_total).shape[2],np.array(prediction_mask_total).shape[3]])	
				o=o+1
		if (self.data_return=='on'):
			return prediction_image, prediction_mask_total
			


	def pass_test_roi_data(self,phase,code,second='off'):	
		'''
			pass the X and Y dimmensions  
		'''
	
		#test in test dataset
		sm=store_model.store_model()
		prediction_image, prediction_mask_total=[],[]
		dsn2=datasetnet.datasetnet("test","roi",second)
		Xtest2, X_total2,Y2, contour_mask2, storetxt2 = dsn2.create_dataset()
		dsn_roi_mask_test=datasetnet.datasetnet(phase,code)

		if Xtest2==[]:

			print(" no testing ... ")
			print(" Continue ... ")
		else:
			pred2, metric2=sm.load_best_callback(Xtest2,Y2,'roi',self.roi_model_name)
			print('Final test accuracy:', (metric2[1]*100.0))
			print (pred2.shape)
			length2=len(Xtest2)
			classes2=self.classes	
			o=0
			while o <classes2:
				prediction_mask=[]		
				for i in range(0,length2):
					#print(pred[0].shape)	
					#print(Xtest[i].shape)	
					output2, mask_roi2, label_mask2 = dsn_roi_mask_test.compute_pred_roi(X_total2[i], pred2[i],contour_mask2[i,:,:,o])

					if (self.save_mode=='on'):
						str2=self.STORE_PATH2 + 'ROImasks%s.%s' % (i+((o)*(length2)),self.counter_extention)		
						#cv2.imwrite(str2,  label_mask2)	
						pylab.imsave(str2, label_mask2, format=self.data_extention, cmap = cm.Greys_r)
						if o==0:
							str1=self.STORE_PATH2 + 'ROItest%s.%s' % (i,self.data_extention)
							#pylab.imsave(str1, output2, format=self.data_extention, cmap = cm.Greys_r)
							cv2.imwrite(str1,  output2)
						#print(str2)
					prediction_mask.append(label_mask2)
					#print(np.array(prediction_mask).shape)
					if o==0:
						prediction=np.array(output2)	
						prediction_image.append(prediction)

				prediction_mask=np.array(prediction_mask)
				if o==0:
					prediction_mask=np.reshape((prediction_mask),[(prediction_mask).shape[0], 1 ,(prediction_mask).shape[1],(prediction_mask).shape[2]])	
					prediction_mask_total=(prediction_mask)
					prediction_image=np.reshape(np.array(prediction_image),[np.array(prediction_image).shape[0],np.array(prediction_image).shape[1],np.array(prediction_image).shape[2],1])
				else:
					prediction_mask=np.reshape((prediction_mask),[(prediction_mask).shape[0], 1 ,(prediction_mask).shape[1],(prediction_mask).shape[2]])
					prediction_mask_total=np.append(prediction_mask_total,prediction_mask,axis=1)
					prediction_mask_total=np.reshape(np.array(prediction_mask_total),[np.array(prediction_mask_total).shape[0],np.array(prediction_mask_total).shape[1],np.array(prediction_mask_total).shape[2],np.array(prediction_mask_total).shape[3]])	
				o=o+1
		print(np.array(prediction_mask_total).shape,np.array(prediction_image).shape)
		if (self.data_return=='on'):
			return prediction_image, prediction_mask_total

	def pass_train_main_data(self):
		#prediction_image,prediction_mask_total=[],[]
		sm=store_model.store_model()
		dsn_main_train=datasetnet.datasetnet('train','main')
		if (self.restore_from_jpg_tif=='on'):
			mainXtest2,  mainX_total,mainY, maincontour_mask, mainstoretxt = dsn_main_train.create_dataset()
			mainXtest=mainX_total
		else:
			data_return_store=self.data_return
			save_mode_store=self.save_mode
			self.data_return='on'
			#self.save_mode='off'
			mainXtest,mainY=self.pass_train_roi_data('train','main')
			self.data_return=data_return_store
			self.save_mode=save_mode_store
			mainXtest2=mainXtest

		if mainXtest==[]:
			print(" no testing ... ")
			print(" Continue ... ")
		else:
			mainpred, mainmetric=sm.load_best_callback(mainXtest,mainY,'main',self.main_model_name)
			print('Final train loss:', (mainmetric[0]))
			print('Final train accuracy:', (mainmetric[1]*100.0))
			print('Final train accuracy:', (mainmetric[2]*100.0))
			print('Final train accuracy:', (mainmetric[3]*100.0))
			print(mainXtest.shape)
			print(mainpred.shape)
			# store images 
			# store images 
			length=len(mainXtest)
			classes=self.classes	
			o=1
			#mainpred=np.array(mainpred,dtype='uint8')
			while o <classes+1:	
				prediction_mask=[]
				#print(np.array(mainXtest[0,:,:,0]))
				#print(np.array(mainXtest[0,:,:,0],dtype='uint8'))
				#print(np.array(mainY[0,:,:,0]))
				#print(np.array(mainY[0,:,:,0],dtype='uint8'))	
				print(np.array(mainpred[0,:,:,0]))	
				print(np.array(mainpred[0,:,:,1]))
				for i in range(0,length):
					#print(Xtest[i])	
					mainlabel_mask= mainpred[i,:,:,o-1]
					mainoutput=dsn_main_train.compute_pred_main(mainXtest[i],mainpred[i])
					if (self.save_mode=='on'):
						str2=self.STORE_PATH_main1 + 'mainmasks%s.%s' % (i+((o-1)*(length)),self.counter_extention)
						#cv2.imwrite(str2,  mainlabel_mask)			
						pylab.imsave(str2, mainlabel_mask, format=self.data_extention, cmap = cm.Greys_r)
						if o==1:
							str1=self.STORE_PATH_main1 + 'maintrain%s.%s' % (i,self.data_extention)
							pylab.imsave(str1, mainoutput, format=self.data_extention, cmap = cm.Greys_r)#mainoutput
							#cv2.imwrite(str1,mainoutput)	
				o=o+1


	def pass_test_main_data(self):
		prediction_image,prediction_mask_total=[],[]
		sm=store_model.store_model()
		dsn_main_test=datasetnet.datasetnet('test','main')
		if (self.restore_from_jpg_tif=='on'):
			mainXtest2,  mainX_total2,mainY2, maincontour_mask2, mainstoretxt2 = dsn_main_test.create_dataset()
			mainXtest=mainX_total2
		else:
			data_return_store=self.data_return
			save_mode_store=self.save_mode
			self.data_return='on'
			self.save_mode='on'
			mainXtest,mainY2=self.pass_test_roi_data('test','main')
			self.data_return=data_return_store
			self.save_mode=save_mode_store
			mainXtest2=mainXtest

		if mainXtest2==[]:
			print(" no testing ... ")
			print(" Continue ... ")
		else:
			mainpred2, mainmetric2=sm.load_best_callback(mainXtest2,mainY2,'main',self.main_model_name)
			# store images 
			vo=vision_outputs('main')
			vo.store_test_results(mainmetric2)
			print('Final test loss:', (mainmetric2[0]))
			print('Final test accuracy:', (mainmetric2[1]*100.0))
			print('Final test accuracy:', (mainmetric2[2]*100.0))
			print('Final test accuracy:', (mainmetric2[3]*100.0))
			print(mainpred2[1,10,50,0], mainpred2[10,100,50,0], mainpred2[19,10,50,1], mainpred2[50,20,100,1])
			# store images 
			length2=len(mainXtest2)
			classes2=self.classes
			o=1
			while o <classes2+2: 
				prediction_mask,prediction_mask_old=[],[]		
				for i in range(0,length2):
					#print(pred[0].shape)	
					#print(Xtest[i].shape)	
					mainoutput2  = mainXtest[i]
					mainlabel_mask2=255*mainpred2[i,:,:,o-1]
					if (self.save_mode=='on'):
						str2=self.STORE_PATH_main2 + 'mainmasks%s_rgb' % (i+((o-1)*(length2)))		
						str3=self.STORE_PATH_main2 + 'mainmask_%s.%s' % (i+((o-1)*(length2)),self.counter_extention)
						#cv2.imwrite(str3,  mainlabel_mask2)	
						pylab.imsave(str3, mainlabel_mask2, format=self.data_extention, cmap = cm.Greys_r)
						if o==1:
							str1=self.STORE_PATH_main2 + 'maintest%s.%s' % (i,self.data_extention)
							str4=self.STORE_PATH_main2 + 'maintest%s_rgb' % (i)

							cv2.imwrite(str1,  mainoutput2)
							
				if (self.data_return=='on'):
					prediction_mask.append(mainlabel_mask2)
					print(np.array(prediction_mask).shape)
					if o==1:
						prediction=np.array(mainoutput2)	
						prediction_image.append(prediction)
				if (self.data_return=='on'):
					prediction_mask=np.array(prediction_mask)
					if o==1:
						prediction_mask=np.reshape((prediction_mask),[(prediction_mask).shape[0], 1 ,(prediction_mask).shape[1],(prediction_mask).shape[2]])	
						prediction_mask_total=(prediction_mask)
						prediction_image=np.reshape(np.array(prediction_image),[np.array(prediction_image).shape[0],np.array(prediction_image).shape[1],np.array(prediction_image).shape[2],1])
					else:
						print(np.array(prediction_mask_total).shape, np.array(prediction_mask).shape)
						prediction_mask=np.reshape((prediction_mask),[(prediction_mask).shape[0], 1 ,(prediction_mask).shape[1],(prediction_mask).shape[2]])
						prediction_mask_total=np.append(prediction_mask_total,prediction_mask,axis=1)
						prediction_mask_total=np.reshape(np.array(prediction_mask_total),[np.array(prediction_mask_total).shape[0],np.array(prediction_mask_total).shape[1],np.array(prediction_mask_total).shape[2],np.array(prediction_mask_total).shape[3]])	
				o=o+1

			if (self.data_return=='on'):
				return prediction_image, prediction_mask_total
