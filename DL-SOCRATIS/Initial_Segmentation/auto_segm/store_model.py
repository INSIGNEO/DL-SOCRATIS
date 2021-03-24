#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk
from __future__ import division, print_function
import argparse
from auto_segm import config, create_net, run_model
from keras.models import model_from_json
from auto_segm.regularization import data_augmentation
import numpy as np

class store_model:

	def __init__ (self) :
		args = config.parse_arguments()
		self.store_model_path=args.store_txt
		self.classes=args.classes
		self.loss_main=args.loss_main
		self.loss_roi=args.loss_roi
		self.width=args.width
		self.height=args.height

	def set_model(self,name_model_json, name_model_h5, model_weights):

		# serialize model to JSON
		model_json = model_weights.to_json()
		with open(self.store_model_path + name_model_json, "w") as json_file:
		    json_file.write(model_json)
		# serialize weights to HDF5
		model_weights.save_weights(self.store_model_path + name_model_h5)
		print("Saved model to disk")


	def get_model(self,name_model_json, name_model_h5):
		# load json and create model
		json_file = open(self.store_model_path + name_model_json, 'r')
		loaded_model_json = json_file.read()
		json_file.close()
		loaded_model = model_from_json(loaded_model_json)
		# load weights into new model
		loaded_model.load_weights(self.store_model_path + name_model_h5)
		print("Loaded model from disk")
		return loaded_model

	def load_best_callback(self,X,Y,case, weight_name):
		Xaug, Yaug= [], []
		for i in data_augmentation(X,Y):
			Xaug.append(i[0])
			Yaug.append(i[1])
		image, mask= np.asarray(Xaug) , np.asarray(Yaug)
		_, _, height, width, channels = image.shape					
		_, _, classes, _ ,_ = mask.shape
		cn=create_net.create_net()
		model_structure=cn.net([], [], case, self.height, channels,(classes+1),self.width)
		print('load weights')
		model_structure.load_weights(self.store_model_path + '/weights_%s_%s.hdf5' %(case,weight_name))
		print("Load model from: ")
		file_store=(self.store_model_path + '/weights_%s_%s.hdf5' %(case, weight_name))
		print(file_store)
		rm=run_model.run_model(case)
		if case=='roi':
			pred, metric=rm.run_testing(self.loss_roi,model_structure, X, Y)
		if case=='main':
			pred, metric=rm.run_testing(self.loss_main,model_structure, X, Y)				
		return pred, metric
