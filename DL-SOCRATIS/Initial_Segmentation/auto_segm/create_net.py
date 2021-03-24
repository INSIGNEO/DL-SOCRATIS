#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk


from __future__ import division, print_function

from auto_segm.model import main_net, roi_net, crossovernet
from auto_segm import regularization, config
import logging

class create_net:

	def net(self, init1, init2, case, height, channels, classes, width):

		if case=='main':
			m=main_net.main_net(init1, init2, height,channels,classes,width)
			if self.main_model=="u_net":
				model=m.u_net()
			elif self.main_model=="rgu_net":
				model=m.rgu_net(batch_size=self.batch_size)
			elif self.main_model=="rg_net":
				model=m.rg_net(batch_size=self.batch_size)
			elif self.main_model=="FCN_Vgg":
				model=m.FCN_Vgg16_32s()
			elif self.main_model=="AtrousFCN_Vgg":
				model=m.AtrousFCN_Vgg16_16s()
			elif self.main_model=="FCN_Resenet":
				model=m.FCN_Resnet50_32s()
			elif self.main_model=="Atrous_Dense":
				model=m.Atrous_DenseNet()
			elif self.main_model=="Dense_FCN":
				model=m.DenseNet_FCN()
			elif self.main_model=="AtrousFCNResnet":
				model=m.AtrousFCN_Resnet50_16s()
			else:			
				print('Error: no main model file')

			if self.load_weights_main:
				logging.info("Load main weights from: {}".format(self.store_model+self.load_weights_main))
				model.load_weights(self.store_model+self.load_weights_main)

		else:
			r=roi_net.roi_net(init1,init2)
			print('roi net ready')
			if self.roi_model=="deep_conv":
				model=r.deep_conv()
			else:
				print('Error: no roi model file')
			if self.load_weights_roi:
				logging.info("Load roi weights from: {}".format(self.store_model+self.load_weights_roi))
				model.load_weights(self.store_model+self.load_weights_roi)

		if self.main_model!="rgu_net" and self.main_model!="rg_net":
			for p in model.layers:
				print(p.name.title(), p.input_shape, p.output_shape)

		return model		

	def __init__ (self) :
		args = config.parse_arguments()
		self.roi_model=args.roi_model
		self.main_model=args.main_model
		self.load_weights_roi=args.load_weights_roi
		self.load_weights_main=args.load_weights_main		
		self.store_model=args.store_txt
		self.batch_size=args.batch_size

        #preprocessing layer

