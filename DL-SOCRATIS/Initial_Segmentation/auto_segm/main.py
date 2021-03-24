
#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk

from __future__ import division, print_function
import numpy as np
import sys
import os
import cv2
import argparse
import logging
import tensorflow as tf
from auto_segm import  create_net, run_model, config, datasetnet, store_model, handle_data
from auto_segm.utils import vision_outputs, parser_data
from auto_segm.model import roi_net, main_net
from pylab import *
from auto_segm.regularization import data_augmentation
from auto_segm.gan_synthetic import *
import time
from keras import utils

from auto_segm.CycleGAN.CGAN_model import cyclegan
from auto_segm.model import crossovernet
#dbn
from scipy.ndimage import convolve
from sklearn import linear_model, datasets, metrics
from sklearn.model_selection import train_test_split
from sklearn.pipeline import Pipeline

from sklearn.datasets import load_digits
from sklearn.model_selection import train_test_split
from sklearn.metrics.classification import accuracy_score

#from dbn.models import UnsupervisedDBN # use "from dbn.tensorflow import SupervisedDBNClassification" for computations on TensorFlow
#from dbn.tensorflow import SupervisedDBNClassification


class main(object):

	def __init__ (self, rmn,mmn) :


		args = config.parse_arguments()
		self.param_reg=0.0001
		self.loss_main=args.loss_main
		self.loss_roi=args.loss_roi
		self.pretrain_window=args.pretrain_window
		self.roi_model_name=rmn
		self.main_model_name=mmn
		self.batch_size=args.batch_size
		self.gan_train_directory=args.gan_train_directory
		self.gancheckpoint='checkpoint'
		self.original_image_shape_roi=args.original_image_shape_roi
		self.original_image_shape_main=args.original_image_shape
		self.restore_from_jpg_tif=args.restore_image
		self.data_augm=args.data_augm
		self.data_augm_classic=args.data_augm_classic
		self.store_model_path=args.store_txt
		self.validation=args.validation	
		self.gan_synthetic=args.gan_synthetic
		self.gan_loops=args.num_synthetic_images
		self.main_activation=args.main_activation
	
	def roi(self):
		'''
			Run model to create the ROI of image
		'''
		#PRE-TRAINING -reduce overfitting 
		#create the datasheet of random 11x11 windows from the images to initialize the weights of roi net
		#TODO split pretrain by roi
		dsn=datasetnet.datasetnet('train','pre')
		X_fullsize=0
		Xpre_train1 = dsn.create_pretrain_dataset() 
		r=roi_net.roi_net([], [])	
		r.weight_name=self.roi_model_name
		Xpre_train=np.array(Xpre_train1).reshape((-1, self.pretrain_window*self.pretrain_window))
		param=self.param_reg
		encoded_X, encoder_0 , weights = r.pre_encoder(Xpre_train, param) 
		init1=weights[0]
		init2=weights[1]
		print(init1.shape, init2.shape)
		#Main-train 
		#cretate dataset	
		dsnr=datasetnet.datasetnet('train','roi')
		X, X_total,Y, contour_mask, storet = dsnr.create_dataset()
		cn=create_net.create_net() 
		if self.gan_synthetic == 'True':
			h_d=handle_data.handle_data(X,Y,'main')
			h_d.gan_synthetic_data_generate()
		#run the augmented data if there are any just to define the model parameters (height etc).
		# width ,channels,classes , set the roi_shape in mask
		Xaug, Yaug= [], []
		if ((self.data_augm == 'True') or (self.data_augm_classic == 'True')):
			for i in data_augmentation(X,Y):
				Xaug.append(i[0])
				Yaug.append(i[1])
			image, mask= np.asarray(Xaug) , np.asarray(Yaug)
			_, _, height, width, channels = image.shape
			_, _, classes, _, _ = mask.shape
		else:
			image, mask= np.asarray(X) , np.asarray(Y)
			_, height, width, channels = image.shape					
			_, classes, _ ,_ = mask.shape
		model_structure=cn.net(init1, init2, 'roi', height,channels,classes,width)

		#run training
		rm=run_model.run_model('roi')
		rm.weight_name=self.roi_model_name
		model_weights, history_roi=rm.run_training(self.loss_roi, model_structure, X, Y)

		#plot loss
		vo=vision_outputs('roi',history_roi)
		vo.plot_loss()
		vo.plot_history()

	def test_roi(self,second='off'):
		'''
			Test model to create the ROI of image
		'''
		pd=parser_data.parser_data(self.roi_model_name, self.main_model_name, data_return='off',save_mode='on')
		#pd.pass_train_roi_data('train_pred','roi')
		pd.pass_test_roi_data('test','roi',second)

##################################################################################################################################
	def main(self):
		'''
			Run model to create the main segmentation model of image
		'''
		#TODO tine learning-train model tha already stored with weigths$

		#Main-train
		#Crop the label of the myo in ROI shape to learn the main model
		#cretate dataset
		if (self.restore_from_jpg_tif=='on'):
			dsn=datasetnet.datasetnet('train','main')
			#set the shape of roi analysis for the output lab
			X, X_total, Y, contour_mask, storetxt = dsn.create_dataset()
		else:
			pd=parser_data.parser_data(self.roi_model_name, self.main_model_name, data_return='on',save_mode='on')
			X,Y=pd.pass_train_roi_data('train','main')

		if self.gan_synthetic == 'True':
			h_d=handle_data.handle_data(X,Y,'main')
			h_d.gan_synthetic_data_generate()

		cn=create_net.create_net()
		Xaug, Yaug= [], []
		if ((self.data_augm == 'True') or (self.data_augm_classic == 'True')):
			for i in data_augmentation(X,Y):
				Xaug.append(i[0])
				Yaug.append(i[1])
			image, mask= np.asarray(Xaug) , np.asarray(Yaug)
			_, _, height, width, channels = image.shape
			_, _, classes, _ ,_ = mask.shape
		else:
			image, mask= np.asarray(X) , np.asarray(Y)
			print(image.shape, mask.shape)
			_, height, width, channels = image.shape
			_, classes, _ ,_ = mask.shape
		print(height,width,channels,classes)
		#create the analysis net
		model_structure=cn.net([], [], 'main', height,channels,(classes+1),width) # classes +1 to take into account the background
		rm=run_model.run_model('main')
		rm.weight_name=self.main_model_name
		model_weights, history_main =rm.run_training(self.loss_main, model_structure, X, Y)



	def test_main(self):
	
		'''
			Test model to create the main segmentation model of image
		'''	
		pd=parser_data.parser_data(self.roi_model_name, self.main_model_name, data_return='off', save_mode='on')
		#pd.pass_train_main_data()
		pd.pass_test_main_data()



		
##########################################################################################################################



	def crossnet(self,main_net='on'):
#  gpu_options = tf.GPUOptions(allow_growth=True)
#		with tf.Session(config=tf.ConfigProto(gpu_options=gpu_options)) as sess:

		if main_net=='on':
			pd=parser_data.parser_data(self.roi_model_name, self.main_model_name, data_return='on', save_mode='on')
			X,Y=pd.pass_test_main_data()
			#create the analysis net
			c=crossovernet.CrossoverNet( height,channels,classes+1,width)
			model=c.crossover()
			rm=run_model.run_model('main')
			rm.weight_name=self.main_model_name
			model_weights, history_main =rm.run_training(self.loss_main, model, X, Y)		


		else:


			Xaug, Yaug= [], []
			if ((self.data_augm == 'True') or (self.data_augm_classic == 'True')):
				for i in data_augmentation(X,Y):
					Xaug.append(i[0])
					Yaug.append(i[1])
				image, mask= np.asarray(Xaug) , np.asarray(Yaug)
				_, _, height, width, channels = image.shape
				_, _, classes, _ ,_ = mask.shape
			else:
				image, mask= np.asarray(X) , np.asarray(Y)
				print(image.shape, mask.shape)
				_, height, width, channels = image.shape
				_, classes, _ ,_ = mask.shape
			print(height,width,channels,classes)
			#create the analysis net
			c=crossovernet.CrossoverNet( height,channels,classes+1,width)
			model=c.crossover()
			rm=run_model.run_model('main')
			rm.weight_name=self.main_model_name
			model_weights, history_main =rm.run_training(self.loss_main, model, X, Y)






	def gan(self,case):
		'''
			Run the Generative Adversarial Network based synthesis images 
		'''
		#TODO add to take the images of gan in the main datashet of roi main... 

		# https://github.com/ece7048/Generative-Adversarial-Network-based-Synthesis-for-Supervised-Medical-Image-Segmentation/blob/master/train.py

		#########Initialize parameters#################################################
		batch_size = self.batch_size
		learning_rate = 0.0004
		beta1 = 0.5

		r_masks,resize_masks= [], []
		if case=='main':
			image_size=128#self.original_image_shape_main, instead of 284(modify this so to take the image shape TODO )
		else:
			image_size=self.original_image_shape_roi

                ######################################################################################
		dsn=datasetnet.datasetnet('test',case) # need the test data from ROI to create synthetic images for endo and epi seperatly 
		dsn.image_shape= 128 #instead of 284 of initial image for the u-net we change to 128 as the generated images are craeated in the DCGAN (modify this so to take the image shape TODO )(set both classes TODO)
		#X,  X_total,Y, contour_mask, storetxt = dsn.create_dataset()
		pd=parser_data.parser_data(self.roi_model_name, self.main_model_name, data_return='on',save_mode='on')
		X,Y=pd.pass_train_roi_data('train','main')
		#Y = utils.to_categorical(Y,(0))
		#print(np.array(Y).shape)
		#Y=Y.reshape(np.array(Y).shape[0],np.array(Y).shape[2],np.array(Y).shape[3],np.array(Y).shape[4])
		#print(np.array(Y).shape)
		monitoring_batches =int(X.shape[0]/self.batch_size) #number of batches after which some samples are saved

		num_monitoring_cycles = self.gan_loops #int(whole_data_pass*monitoring_batches)
		if X==[]:
			print(" no DATA ... ")
			print(" END ... ")
		else:
			o=0
			while o < 1:#self.classes:
				r_mask_store=resize_masks
				resize_masks, images_resize=[],[]	
				print(X.shape)		
				for i in range(len(Y)):
					image_resize = cv2.resize(X[i,:,:,0].reshape(X.shape[1],X.shape[2]), (image_size, image_size), cv2.INTER_NEAREST)
					resize_mask = cv2.resize(Y[i,o,:,:].reshape(Y.shape[2],Y.shape[3]), (image_size, image_size), cv2.INTER_NEAREST)
					resize_mask=np.array(resize_mask).reshape(1,image_size,image_size)
					resize_masks.append(resize_mask)				
					images_resize.append(image_resize)
				if o>0:	
					r_masks=np.append(r_mask_store,resize_masks,axis=1)
				else:
					r_masks=resize_masks
				o=o+1
				print(np.array(r_masks).shape)					
				print(np.array(images_resize).shape)
			mask_resize=np.array(r_masks).reshape((len(Y), image_size, image_size, 1))#Y.shape[1]))
			images_resize=np.array(images_resize).reshape((len(Y), image_size, image_size, 1))#Y.shape[1]))
			z_dim_image = X.shape[2]
			dcgan = DCGAN(batch_size=batch_size)# define both classes of masks
			print(mask_resize.shape, np.array(images_resize).shape)
			X_pre, Y_pre=dsn.Gan_preprocessing_images(images_resize,mask_resize)
			print(np.array(Y_pre).shape, np.array(X_pre).shape)
			input_masks = tf.concat([X_pre, Y_pre], 3)
			
			train_op = dcgan.build(input_masks)
			sample_images = dcgan.sample_images()
			saver = tf.train.Saver()
			min_loss=10000
			with tf.Session() as sess:
				summary_writer = tf.summary.FileWriter(self.gan_train_directory+"/", graph=sess.graph)
				sess.run(tf.global_variables_initializer())
				coord = tf.train.Coordinator()
				threads = tf.train.start_queue_runners(sess=sess, coord=coord)   
				total_iters_max =  num_monitoring_cycles * monitoring_batches
				iters = 0
				total_duration = 0.0
				avg_iter_time = 0.0
				print("START cycles...")
				for cycle in range(num_monitoring_cycles):
					g_loss_total=0
					for batch in range(monitoring_batches):

						start_time = time.time()
						_, g_loss_value, d_loss_value = sess.run([train_op, dcgan.losses['g'], dcgan.losses['d']])
						_, g_loss_value, d_loss_value = sess.run([dcgan.g_opt_op, dcgan.losses['g'], dcgan.losses['d']]) # which one to dicrease g_opt_op        
						duration = time.time() - start_time
						total_duration = total_duration + duration
						iters = iters + 1
						avg_iter_time = (avg_iter_time * (iters - 1) + duration) / iters
						#ETA_seconds = (total_iters_max / iters) * total_duration - total_duration
						ETA_seconds = (0.95 * avg_iter_time + 0.05 * duration) * (total_iters_max - iters)
						format_str = 'cycle (%d / %d), batch (%d / %d) loss = (G: %.8f, D: %.8f) (%.3f sec/batch) (ETA: %d seconds)'
						print(format_str % (cycle, num_monitoring_cycles, batch, monitoring_batches, g_loss_value, d_loss_value, duration, int(ETA_seconds)))	
						g_loss_total+=g_loss_value
					mean_g_loss=g_loss_total/monitoring_batches
					print('mean_g_loss: ')
					print(mean_g_loss)
					checkpoint_folder = self.gan_train_directory + "/" + self.gancheckpoint
					checkpoint_folder_file = self.gan_train_directory + "/" + self.gancheckpoint + "/"+'checkpoint%d.ckpt' % cycle
					if not os.path.exists(checkpoint_folder):
						os.makedirs(checkpoint_folder)
					if cycle==0:
						saver.save(sess,checkpoint_folder_file)
					ckpt = tf.train.get_checkpoint_state(checkpoint_folder)
					saver.restore(sess, ckpt.model_checkpoint_path)#call the last store sess

					if (mean_g_loss<=min_loss or cycle==(num_monitoring_cycles-1)):
						saver.save(sess, checkpoint_folder_file)
						ckpt = tf.train.get_checkpoint_state(checkpoint_folder)
						saver.restore(sess, ckpt.model_checkpoint_path)#call the last store sess
						min_loss=mean_g_loss
						print("Smaller loss reach again...")
						imgs = sess.run(sample_images)
						print("batch_size sample_images: ")
						for k in range(batch_size):
							imgs_folder = os.path.join(self.gan_train_directory, 'out/imgs%d/') % cycle
							if not os.path.exists(imgs_folder):
								os.makedirs(imgs_folder)           
							segs_folder = os.path.join(self.gan_train_directory, 'out/segs%d/') % cycle
							if not os.path.exists(segs_folder):
								os.makedirs(segs_folder)
							img_channel = imgs[k][:, :, 0]
							img_seg = imgs[k][:, :, 1]
							print(np.array(img_channel).shape,np.array(img_seg).shape)
							imsave(os.path.join(imgs_folder, 'img_%d.jpeg') % k,img_channel.reshape(image_size, image_size), cmap = cm.Greys_r)
							imsave(os.path.join(segs_folder, 'imgrgb_%d.jpeg') % k,img_seg.reshape(image_size, image_size) ) # , cmap = cm.Greys_r)
							imsave(os.path.join(segs_folder, 'img_%d.jpeg') % k,img_seg.reshape(image_size, image_size) , cmap = cm.Greys_r)      
						imgs_in = sess.run(input_masks) 
						print("batch_size input_masks: ")   
						for k in range(batch_size):
							imgs_folder = os.path.join(self.gan_train_directory, 'in/imgs%d/') % cycle
							if not os.path.exists(imgs_folder):
								os.makedirs(imgs_folder)           
							segs_folder = os.path.join(self.gan_train_directory, 'in/segs%d/') % cycle
							if not os.path.exists(segs_folder):
								os.makedirs(segs_folder)
							img_channel = imgs_in[k][:, :, 0]
							img_seg = imgs_in[k][:, :, 1]
							print(np.array(img_channel).shape,np.array(img_seg).shape)
							imsave(os.path.join(imgs_folder, 'img_%d.jpeg') % k, img_channel.reshape(image_size, image_size), cmap = cm.Greys_r)
							imsave(os.path.join(segs_folder, 'img_%d.jpeg') % k, img_seg.reshape(image_size, image_size), cmap = cm.Greys_r)
				print("END cycles...")		      
				coord.request_stop()
				coord.join(threads)
	def cycle_gan(self,command_parser):


		parser = argparse.ArgumentParser(description='')
		parser.add_argument('--save_point_path', dest='save_point_path', default='LGE2cine_leeds_256', help='name of checkpoint folder')		
		parser.add_argument('--dataset_dir', dest='dataset_dir', default='LGE2cine', help='path of the dataset')
		parser.add_argument('--epoch', dest='epoch', type=int, default=400, help='# of epoch')
		parser.add_argument('--epoch_step', dest='epoch_step', type=int, default=100, help='# of epoch to decay lr')
		parser.add_argument('--batch_size', dest='batch_size', type=int, default=2, help='# images in batch')
		parser.add_argument('--train_size', dest='train_size', type=int, default=674, help='# images used to train')
		parser.add_argument('--load_size', dest='load_size', type=int, default=286, help='scale images to this size')
		parser.add_argument('--fine_size', dest='fine_size', type=int, default=256, help='then crop to this size')
		parser.add_argument('--ngf', dest='ngf', type=int, default=64, help='# of gen filters in first conv layer')
		parser.add_argument('--ndf', dest='ndf', type=int, default=64, help='# of discri filters in first conv layer')
		parser.add_argument('--input_nc', dest='input_nc', type=int, default=1, help='# of input image channels')
		parser.add_argument('--output_nc', dest='output_nc', type=int, default=1, help='# of output image channels')
		parser.add_argument('--lr', dest='lr', type=float, default=0.02, help='initial learning rate for adam')
		parser.add_argument('--beta1', dest='beta1', type=float, default=0.5, help='momentum term of adam')
		parser.add_argument('--which_direction', dest='which_direction', default='AtoB', help='AtoB or BtoA')
		parser.add_argument('--phase', dest='phase', default='train', help='train, test')
		parser.add_argument('--save_freq', dest='save_freq', type=int, default=10000, help='save a model every save_freq iterations')
		parser.add_argument('--print_freq', dest='print_freq', type=int, default=10000, help='print the debug information every print_freq iterations')
		parser.add_argument('--continue_train', dest='continue_train', type=bool, default=False, help='if continue training, load the latest model: 1: true, 0: false')
		parser.add_argument('--checkpoint_dir', dest='checkpoint_dir', default='./../Initial_Segmentation/auto_segm/CycleGAN/checkpoint', help='models are saved here')
		parser.add_argument('--sample_dir', dest='sample_dir', default='./../Initial_Segmentation/auto_segm/CycleGAN/sample', help='sample are saved here')
		parser.add_argument('--test_dir', dest='test_dir', default='./../Initial_Segmentation/auto_segm/CycleGAN/test', help='test sample are saved here')
		parser.add_argument('--L1_lambda', dest='L1_lambda', type=float, default=10.0, help='weight on L1 term in objective')
		parser.add_argument('--use_resnet', dest='use_resnet', type=bool, default=False, help='generation network using reidule block')
		parser.add_argument('--use_lsgan', dest='use_lsgan', type=bool, default=False, help='gan loss defined in lsgan')
		parser.add_argument('--max_size', dest='max_size', type=int, default=50, help='max size of image pool, 0 means do not use image pool')

		parser.parse_args()
		args=parser.parse_args(command_parser)
		#args = parser.parse_args()

		tfconfig = tf.ConfigProto(allow_soft_placement=True)
		tfconfig.gpu_options.allow_growth = True
		with tf.Session(config=tfconfig) as sess:
			model = cyclegan(sess, args)
			if args.phase == 'train':
				model.train(args)
			else:
				model.test(args)





#################################################################################################################################################################################
	def dbn(self,case):



#digits = datasets.load_digits()
#X = np.asarray(digits.data, 'float32')
#X, Y = nudge_dataset(X, digits.target)
#X = (X - np.min(X, 0)) / (np.max(X, 0) + 0.0001)  # 0-1 scaling
#X_train, X_test, Y_train, Y_test = train_test_split(X, Y,test_size=0.2, random_state=0)

		# Load Data
		pd=parser_data.parser_data(self.roi_model_name, self.main_model_name, data_return='on',save_mode='off')
		X,Y=pd.pass_train_roi_data('train','main')
		h_d=handle_data.handle_data(X,Y,'main')

		Xtrain, Ytrain, X_test, Y_test =h_d.validation_data("dbn")
	
		if (case=='unsupervised'):

			Xtrain = np.asarray(Xtrain, 'float32')
			X_test = np.asarray(X_test, 'float32')
			X_test = (X_test - np.min(X_test, 0)) / (np.max(X_test, 0) + 0.0001)
			Xtrain = (Xtrain - np.min(Xtrain, 0)) / (np.max(Xtrain, 0) + 0.0001)
			print((np.array(Xtrain)).shape, (Ytrain).shape)

			Xtrain=np.array(Xtrain)
			X_test=np.array(X_test)
			Xtrain= np.reshape((Xtrain), [(Xtrain).shape[1]*(Xtrain).shape[2]*(Xtrain).shape[3], (Xtrain).shape[0]])
			Ytrain=np.reshape((Ytrain), [(Ytrain).shape[1]*(Ytrain).shape[2]*(Ytrain).shape[3], (Ytrain).shape[0]])
			X_test= np.reshape((X_test), [(X_test).shape[1]*(X_test).shape[2]*(X_test).shape[3], (X_test).shape[0]])
			Y_test= np.reshape((Y_test), [(Y_test).shape[1]*(Y_test).shape[2]*(Y_test).shape[3], (Y_test).shape[0]])

		
			print(Xtrain.shape,Ytrain.shape)
			# Models we will use
			#logistic = linear_model.LogisticRegression()

			dbn = UnsupervisedDBN(hidden_layers_structure=[self.original_image_shape_main, self.original_image_shape_main], batch_size=10, learning_rate_rbm=0.06, n_epochs_rbm=20, activation_function=self.main_activation)
			classifier = Pipeline(steps=[('dbn', dbn)])

			# Training
			#logistic.C = 6000.0

			# Training RBM-Logistic Pipeline
			classifier.fit(Xtrain, Ytrain)

			# Training Logistic regression
			#logistic_classifier = linear_model.LogisticRegression(C=100.0)
			#logistic_classifier.fit(Xtrain, Ytrain)

			# Evaluation

			print()
			print("Logistic regression using RBM features:\n%s\n" % (
			    metrics.classification_report(Y_test,classifier.predict(X_test))))

			#print("Logistic regression using raw pixel features:\n%s\n" % (
			#    metrics.classification_report(Y_test,logistic_classifier.predict(X_test))))

###############################################################################
		if (case=='supervised'):

			X_test = (X_test / 16).astype(np.float32)
			Xtrain = (Xtrain / 16).astype(np.float32)
			print((np.array(Xtrain)).shape, np.array(Ytrain).shape)

						
						
			Xtrain=np.array(Xtrain)
			X_test=np.array(X_test)
			Xtrain= np.reshape((Xtrain), [(Xtrain).shape[1]*(Xtrain).shape[2]*(Xtrain).shape[3], (Xtrain).shape[0]])
			Ytrain=np.reshape((Ytrain), [(Ytrain).shape[1]*(Ytrain).shape[2]*(Ytrain).shape[3], (Ytrain).shape[0]])
			X_test= np.reshape((X_test), [(X_test).shape[1]*(X_test).shape[2]*(X_test).shape[3], (X_test).shape[0]])
			Y_test= np.reshape((Y_test), [(Y_test).shape[1]*(Y_test).shape[2]*(Y_test).shape[3], (Y_test).shape[0]])

			print(Xtrain.shape,Ytrain.shape)
			classifier = SupervisedDBNClassification(hidden_layers_structure=[ self.original_image_shape_main, self.original_image_shape_main], learning_rate_rbm=0.05,  learning_rate=0.1, n_epochs_rbm=10,  n_iter_backprop=100, batch_size=32, activation_function=self.main_activation, dropout_p=0.2)

			classifier.fit(Xtrain, Ytrain)
			# Test
			Y_pred = classifier.predict(X_test)
			print('Done.\nAccuracy: %f' % accuracy_score(Y_test, Y_pred))


