#Author: Michail Mamalakis
#Version: 0.1
#Licence:
#email:mmamalakis1@sheffield.ac.uk
from __future__ import division, print_function
from auto_segm import createhdf5data, datasetnet, config
import argparse
import subprocess
import numpy as np
import cv2
from keras import regularizers
from keras.preprocessing.image import ImageDataGenerator
from scipy.ndimage.interpolation import map_coordinates
from scipy.ndimage.filters import gaussian_filter
from scipy.stats import rice

#convert vtk to hdf5 files
def image_hdf():
	hdf5=createhdf5data()
	os.environ['HDF5_DISABLE_VERSION_CHECK']='2'
	hdf5.create_hdf5("Model/train.hdf", "Model/train_hdf5data.txt", X_train.reshape(1,495,128,128), counder_train.reshape(1,495,128,128), name=None, descr=None, shuffle=False)
	hdf5.create_hdf5("Model/test.hdf", "Model/test_hdf5data.txt", X_test.reshape(1,524,128,128), counder_test.reshae(1,524,128,128), name=None, descr=None, shuffle=False)


#data augmentation
class data_augmentation:
	
	def __init__(self,X,Y):
		self.mask=Y
		self.images=X
		args = config.parse_arguments()
		self.max=args.max_loops
		self.featurewise_center=args.featurewise_center
		self.samplewise_center=args.samplewise_center
		self.featurewise_std_normalization=args.featurewise_std_normalization
		self.samplewise_std_normalization=args.samplewise_std_normalization
		self.zca_whitening=args.zca_whitening
		self.rotation_range=args.rotation_range
		self.width_shift_range=args.width_shift_range
		self.height_shift_range=args.height_shift_range
		self.horizontal_flip=args.horizontal_flip
		self.vertical_flip = args.vertical_flip
		self.data_augm= args.data_augm_classic	
		self.alpha=args.alpha
		self.sigma=args.sigma
		self.normilize=args.normalize
		self.shuffle=args.shuffle
		self.batch_size=args.batch_size
		self.index = np.arange(len(self.images))
		self.noise=args.noise
		self.random_apply_in_batch=args.random_apply_in_batch
		
		if self.data_augm=='True':
			self.datagen = ImageDataGenerator(
				featurewise_center=self.featurewise_center,  # set input mean to 0 over the dataset
				samplewise_center=self.samplewise_center,  # set each sample mean o 0
				featurewise_std_normalization=self.featurewise_std_normalization,  # divide inputs by std of the dataset
				samplewise_std_normalization=self.samplewise_std_normalization,  # divide each input by its std
				zca_whitening=self.zca_whitening,  # apply ZCA whitening
				rotation_range=self.rotation_range,  # randomly rotate images in the range (degrees, 0 to 180)
				width_shift_range=self.width_shift_range,  # randomly shift images horizontally (fraction of total width)
				height_shift_range=self.height_shift_range,  # randomly shift images vertically (fraction of total height)
				horizontal_flip=self.horizontal_flip,  # randomly flip images
				vertical_flip=self.vertical_flip,
				zca_epsilon=1e-6
			)


	def __iter__(self):
		self.i=0
		return self
	def __next__(self):
		if self.i<self.max:
			mask2=self.mask		
			augmented_X = []
			augmented_Y = []
			resize_masks = []
			alpha=0.0
			sigma=0.0
			nrate=0.3
			dicision=4
			if self.shuffle:
				agstate = np.random.get_state()
				np.random.shuffle(self.images)
				np.random.set_state(agstate)
				np.random.shuffle(mask2)
			start = 0
			end =  len(self.images)
			mask2=np.array(mask2)
			# resize from roi_shape to image_shape as X vector images,  the Y label images
			#print(mask2.shape)
			mask_reshape=mask2.reshape(mask2.shape[0], mask2.shape[3], mask2.shape[2], mask2.shape[1])
			#print(mask_reshape.shape)
			for i in range(len(mask2)):
				resize_mask = cv2.resize(mask_reshape[i], (self.images.shape[1], self.images.shape[2]), interpolation=cv2.INTER_CUBIC)
				resize_masks.append(resize_mask.reshape(self.images.shape[1], self.images.shape[2], mask2.shape[1]))
			#print(np.array(resize_masks).shape)
			mask_resize=np.array(resize_masks)
			#print(mask_resize.shape)
			#mask_resize=np.array(mask2).reshape((len(mask2), mask2.shape[3], mask2.shape[2], mask2.shape[1]))
			#.reshape(self.mask.shape[2],self.mask.shape[3])
			#apply transformation data in each batch size
			for o in range(start,end,self.batch_size):
				#change only in each batch sample the mean and std of gaussian filter
				if self.random_apply_in_batch=='True':
					if self.alpha != 0 and self.sigma != 0:
						alpha =np.random.choice(int(self.alpha*0.2),1)
						sigma =np.random.choice(int(self.sigma*0.25),1)
						# disicion if +/- from the initial value of alpha, sigma
						dicision= np.random.choice(int(5),1)
					if self.noise=='True':
						rate =np.random.choice(int(50),1)
						nrate=rate*0.01	
					
				for co in self.index[o:o+self.batch_size]:      
		 	        	# stack X and Y together
					image=self.images[co]
					mask_res=mask_resize[co]
					_, _, channels = image.shape
					#print(image.shape, mask_res.shape)
					stack = np.concatenate((image, mask_res), axis=2)
					# change the initial values by random generator, affine transformation
					if self.data_augm=='True':				
						augment = self.datagen.random_transform(stack)
					else:
						augment=stack
					# apply elastic deformation
					if self.alpha != 0 and self.sigma != 0:
						if dicision>=3:
							alpha_final=self.alpha+float(alpha)
							sigma_final=self.sigma+float(sigma)
						else:
							alpha_final=self.alpha-float(alpha)
							sigma_final=self.sigma-float(sigma)												
						augment = elastic_transform(augment,sigma_final,alpha_final,augment.shape[1] * 0.08)
					if self.noise=='True':
						augment = noise(augment,nrate)
					# split image and mask back apart
					augmented_image = augment[:,:,:channels]
					augmented_X.append(augmented_image)
					augmented_mask = np.round(augment[:,:,channels:])
					augmented_mask=(cv2.resize(augmented_mask, (mask2.shape[2], mask2.shape[3])))
					augmented_Y.append(augmented_mask)
			self.i +=1
			
			augmented_Y=np.array(augmented_Y).reshape((len(mask2), mask2.shape[1], mask2.shape[2], mask2.shape[3] ))
			print(augmented_Y.shape)
			return (augmented_X), (augmented_Y)
		else:
			raise StopIteration

# Function to distort image
def elastic_transform(image, alpha, sigma, alpha_affine, random_state=None):
    """Elastic deformation of images as described in [Simard2003]_ (with modifications).
    .. [Simard2003] Simard, Steinkraus and Platt, "Best Practices for
         Convolutional Neural Networks applied to Visual Document Analysis", in
         Proc. of the International Conference on Document Analysis and
         Recognition, 2003.

     Based on https://gist.github.com/erniejunior/601cdf56d2b424757de5
    """
    if random_state is None:
        random_state = np.random.RandomState(None)

    shape = image.shape
    shape_size = shape[:2]
    
    # Random affine
    center_square = np.float32(shape_size) // 2
    square_size = min(shape_size) // 3
    pts1 = np.float32([center_square + square_size, [center_square[0]+square_size, center_square[1]-square_size], center_square - square_size])
    pts2 = pts1 + random_state.uniform(-alpha_affine, alpha_affine, size=pts1.shape).astype(np.float32)
    M = cv2.getAffineTransform(pts1, pts2)
    image = cv2.warpAffine(image, M, shape_size[::-1], borderMode=cv2.BORDER_REFLECT_101)

    dx = gaussian_filter((random_state.rand(*shape) * 2 - 1), sigma) * alpha
    dy = gaussian_filter((random_state.rand(*shape) * 2 - 1), sigma) * alpha
    dz = np.zeros_like(dx)

    x, y, z = np.meshgrid(np.arange(shape[1]), np.arange(shape[0]), np.arange(shape[2]))
    indices = np.reshape(y+dy, (-1, 1)), np.reshape(x+dx, (-1, 1)), np.reshape(z, (-1, 1))

    return map_coordinates(image, indices, order=1, mode='reflect').reshape(shape)

def noise(X,spr):
	# Noise salt and pepper noise
	X_copy = X.copy()
	row, col, _ = X_copy[0].shape
	salt_pepper_rate = 0.35
	amount = spr
	num_salt = np.ceil(amount * X_copy[0].size * salt_pepper_rate)
	num_pepper = np.ceil(amount * X_copy[0].size * (1.0 - salt_pepper_rate))
    
	for Xo in X_copy:
		# Add Salt noise
		coords = [np.random.randint(0, i - 1, int(num_salt)) for i in Xo.shape]
		Xo[coords[0], coords[1], :] = 255

		# Add Pepper noise
		coords = [np.random.randint(0, i - 1, int(num_pepper)) for i in Xo.shape]
		Xo[coords[0], coords[1], :] = 0

	if (spr>0.25):
		#add gaussian noise
		mean = 0.0   # some constant
		std = 1.0    # some constant (standard deviation)
		noisy_img = X_copy + np.random.normal(mean, std, X_copy.shape)
		noisy_img_clipped = np.clip(noisy_img, 0, 255)
	else:
		#add rician noise
		b = 0.775
		r = X_copy + rice.rvs(b, size=X_copy.shape)
		noisy_img_clipped = np.clip(r, 0, 255)

	return noisy_img_clipped

def normalize(x):
	axis=(1,2)
	epsilon=1e-7
	x -= np.mean(x, axis=axis, keepdims=True)
	x /= np.std(x, axis=axis, keepdims=True) + epsilon

