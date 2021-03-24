#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk


from __future__ import division, print_function
import sys
import cv2
import matplotlib.pyplot as plt
from auto_segm import run_model, config
from keras.losses import mean_squared_error
import numpy as np
import itertools
from keras.models import Sequential
from keras.layers import Dense
from keras.utils import to_categorical
from keras.wrappers.scikit_learn import KerasClassifier



class vision_outputs:

	def __init__ (self,case, history=None) :
		args = config.parse_arguments()
		self.h=history
		self.STORE_TXT=args.store_txt
		self.store_results=args.store_results
		self.validation=args.validation
		self.metrics=args.metrics
		self.metrics1=args.metrics1
		self.metrics2=args.metrics2
		self.case=case

	def plot_loss(self):	
		'''
			plot the loss results 
		'''
		print(self.h.history.keys())
		if (self.validation=="on"):
			# summarize history for accuracy
			plt.plot(self.h.history['acc'])
			plt.plot(self.h.history['val_acc'])
			plt.title('model accuracy')
			plt.ylabel('accuracy')
			plt.xlabel('epoch')
			plt.legend(['train', 'test'], loc='upper left')
			fig1 = plt.gcf()
			plt.show()
			plt.draw()
			fig1.savefig(self.STORE_TXT+'/acc_val_{}.png'.format(self.case), dpi=100)
			# summarize history for loss
			plt.plot(self.h.history['loss'])
			plt.plot(self.h.history['val_loss'])
			plt.title('model loss')
			plt.ylabel('loss')
			plt.xlabel('epoch')
			plt.legend(['train', 'test'], loc='upper left')
			fig2 = plt.gcf()
			plt.show()
			plt.draw()
			fig2.savefig(self.STORE_TXT+'/loss_val_{}.png'.format(self.case), dpi=100)
		else:
			metric = 'loss'
			plt.plot(self.h.history['loss'])
			plt.plot(self.h.history['acc'])
			plt.plot(self.h.history[self.metrics1])
			plt.plot(self.h.history[self.metrics2])
			plt.ylabel('loss-accur')
			plt.xlabel('epochs')
			plt.title("Learning curve")
			fig1 = plt.gcf()
			plt.show()
			plt.draw()
			fig1.savefig(self.STORE_TXT+'/loss_accur_{}.png'.format(self.case), dpi=100)		

	def plot_history(self):
		#https://www.kaggle.com/danbrice/keras-plot-history-full-report-and-grid-search#
		history=self.h
		loss_list = [s for s in history.history.keys() if 'loss' in s and 'val' not in s]
		val_loss_list = [s for s in history.history.keys() if 'loss' in s and 'val' in s]
		acc_list = [s for s in history.history.keys() if 'acc' in s and 'val' not in s]
		val_acc_list = [s for s in history.history.keys() if 'acc' in s and 'val' in s]
		    
		if len(loss_list) == 0:
			print('Loss is missing in history')
			return 
		    
		## As loss always exists
		epochs = range(1,len(history.history[loss_list[0]]) + 1)
		    
		## Loss
		for l in loss_list:
			plt.plot(epochs, history.history[l], 'b', label='Training loss (' + str(str(format(history.history[l][-1],'.5f'))+')'))
		if (self.validation=="on"):
			for l in val_loss_list:
				plt.plot(epochs, history.history[l], 'g', label='Validation loss (' + str(str(format(history.history[l][-1],'.5f'))+')'))
		    
		plt.title('Loss')
		plt.xlabel('Epochs')
		plt.ylabel('Loss')
		plt.legend()

		fig1 = plt.gcf()
		plt.show()
		plt.draw()
		fig1.savefig(self.STORE_TXT+'/Accuracy_{}.png'.format(self.case), dpi=100)

		## Accuracy

		for l in acc_list:
			plt.plot(epochs, history.history[l], 'b', label='Training accuracy (' + str(format(history.history[l][-1],'.5f'))+')')
		if (self.validation=="on"):
			for l in val_acc_list:    
				plt.plot(epochs, history.history[l], 'g', label='Validation accuracy (' + str(format(history.history[l][-1],'.5f'))+')')

		plt.title('Accuracy')
		plt.xlabel('Epochs')
		plt.ylabel('Accuracy')
		plt.legend()
		fig2 = plt.gcf()
		plt.show()
		plt.draw()
		fig2.savefig(self.STORE_TXT+'/Losses_{}.png'.format(self.case), dpi=100)


	def plot_images(self,ncol,X,Y):	
		'''
			plot the images based the ground truth
		'''
		f_one, ax_one = plt.subplots(ncols=4, figsize=(10,10))
		#n=0	
		#print(X.shape[0])	
		#for i in range(ncol):
		o,i =np.random.choice((X.shape[0]),2)
		#	print(o)
		ax_one[0].imshow(X[o])
		ax_one[1].imshow(Y[o])
		ax_one[2].imshow(X[i])
		ax_one[3].imshow(Y[i])
		#	n=i+2
		plt.show()

	def print_results(self,Yp,Yt,i):	
		'''
			print the accuracy based the ground truth
		'''
		print('Test results of '+str(i)+' image	of the Test data set is:\n')
		rm=run_model.run_model('roi')
		loss = rm.customized_loss(Yt,Yp) 
		print('Customized loss of image is :' +str(loss) + '\n')
		loss = rm.hard_jaccard(Yt,Yp)
		print('Hard jaccard of image is :' +str(loss) + '\n')
		loss = rm.soft_jaccard(Yt,Yp)
		print('Soft jaccard of image is :' +str(loss) + '\n')
		loss = rm.hard_sorensen_dice(Yt,Yp)
		print('Hard sorensen dice of image is :' +str(loss) + '\n')
		loss = rm.soft_sorensen_dice(Yt,Yp)
		print('Soft sorensen dice of image is :' +str(loss) + '\n')
		loss = rm.mean_squared_error(Yt,Yp)
		print('Mean squared error of image is :' +str(loss) + '\n')


	def store_test_results(self,metric):
		'''
			Compute store the mean and SD accuracy based the ground truth
		'''
		with open('{}/loss_results_{}.txt'.format(self.STORE_TXT,self.case), 'w') as f:
#			print(metric[1])
#			print(metric[1].itemsize)
#			f.write('The history of the loss metric are:\n')
#			for i in range(len(metric[1].itemsize)):
#				f.write('{} of image is : {}\n'.format(self.metrics, metric[0][i]*100))
#				f.write('{} of image is : {}\n'.format(self.metrics1, metric[1][i]*100))
##				f.write('{} of image is : {}\n'.format(self.metrics2, metric[2][i]*100))		
#			mean= np.mean(metric[0])
#			mean1= np.mean(metric[1])
#			mean2= np.mean(metric[2])
#			sd= np.std(metric[0])
#			sd1= np.std(metric[1])
#			sd2= np.std(metric[2])
			f.write('\n')
			f.write('\n')
			f.write('Mean values of errors of the Test data set are:\n')
			f.write('{} of image is : {}\n'.format(self.metrics, metric[0]*100))
			f.write('{} of image is : {}\n'.format(self.metrics1, metric[1]*100))
			f.write('{} of image is : {}\n'.format(self.metrics2, metric[2]*100))
#			f.write('SD of errors of the Test data set are:\n')
#			f.write('{} of image is : {}\n'.format(self.metrics, sd*100))
#			f.write('{} of image is : {}\n'.format(self.metrics1, sd1*100))
#			f.write('{} of image is : {}\n'.format(self.metrics2, sd2*100))






