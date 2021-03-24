from niftynet.evaluation.pairwise_measures import PairwiseMeasures
from niftynet.evaluation.segmentation_evaluator import SegmentationEvaluator
from niftynet.evaluation.base_evaluator import CachedSubanalysisEvaluator
from niftynet.io.image_reader import ImageReader

import os
import sys
import glob
import math
import numpy as np
from PIL import Image, ImageDraw
from keras.utils import np_utils

def binary_images(Y):

	if (np.max(Y)>1):
		Y=np.array(Y)/np.max(Y)
	# be sure all the classes are equal to 1 
	pos_pred_mask = np.array(np.where(Y > 0.5))
	a,b,rows_mask=pos_pred_mask[0],pos_pred_mask[1],pos_pred_mask[2]
	Y[a,b,rows_mask]=1

	pos_pred_mask2 = np.array(np.where(Y <= 0.5))
	a2,b2,rows_mask2=pos_pred_mask2[0],pos_pred_mask2[1],pos_pred_mask2[2]
	Y[a2,b2,rows_mask2]=0.0
	
	Yout=np.array(Y, dtype='int')
	Yout=Yout.reshape(np.array(Yout).shape[0],np.array(Yout).shape[1],np.array(Yout).shape[2],1)
	print(Yout.shape)
	return Yout

class evaluation_metric:
	def __init__ (self, X,Y) :
		self.seg=X		
		self.ref=Y

	def test_sensitivity(self):
		pairwise_measures = PairwiseMeasures(seg_img=self.seg, ref_img=self.ref)
		#self.assertEqual(pairwise_measures.sensitivity(), 1.)
		return pairwise_measures.sensitivity()

	def test_specificity(self):
		pairwise_measures = PairwiseMeasures(seg_img=self.seg, ref_img=self.ref)
		#self.assertEqual(pairwise_measures.specificity(), 2./3)
		return pairwise_measures.specificity()

	def test_dice_score(self):
		pairwise_measures = PairwiseMeasures(seg_img=self.seg, ref_img=self.ref)
		#self.assertEqual(pairwise_measures.dice_score(), 1.)
		return pairwise_measures.dice_score()

	# distance metrices 
	def test_ACD(self):
		pairwise_measures = PairwiseMeasures(seg_img=self.seg, ref_img=self.ref)
		#self.assertEqual(pairwise_measures.dice_score(), 1.)
		return pairwise_measures.measured_average_distance()

	def test_hausdorff(self):
		pairwise_measures = PairwiseMeasures(seg_img=self.seg, ref_img=self.ref)
		#self.assertEqual(pairwise_measures.dice_score(), 1.)
		return pairwise_measures.measured_hausdorff_distance()


if __name__ == '__main__':
	if len(sys.argv) < 3:
		print('Usage: {0} segmentation_images, ground trouth, and measurement metrices'.format(sys.argv[0]))
		exit(1)
	# load images and save in array
	seg_img_init= sorted(glob.glob(sys.argv[1] + '/*.jp*'))
	ref_img_init= sorted(glob.glob(sys.argv[2] + '/*.jp*'))
	seg_img=[]
	ref_img=[]
	ref_up=[]
	seg_up=[]
	reduce_start=0
	reduce_end=0
	#the 3 case manually not follow the rule down:

	seg_img=seg_img_init
	ref_img=ref_img_init

	print('the old size of automatic segmentation images was : ')
	print(len(seg_img_init))
	print('the old size of manual segmentation images was : ')
	print(len(ref_img_init))

	#print((seg_img))

	#print(len(ref_img))


	#if len(seg_img)<=8:
	seg= Image.open(seg_img[0]).convert('L')
	ref= Image.open(ref_img[0]).convert('L')
	k=1
	#else:
		#seg= Image.open(seg_img[1]).convert('L')
		#ref= Image.open(ref_img[1]).convert('L')
		#k=2
	seg_up=np.array(seg).reshape(1,np.array(seg).shape[0],np.array(seg).shape[1],1)
	ref=np.resize(np.array(ref),(np.array(seg).shape[0],np.array(seg).shape[1]))
	ref_up=np.array(ref).reshape(1,np.array(seg).shape[0],np.array(seg).shape[1],1)
	print(ref_up.shape,seg_up.shape)
	for o in range(k,min(len(seg_img),(len(ref_img))-1)):
		seg= Image.open(seg_img[o]).convert('L')
		ref= Image.open(ref_img[o]).convert('L')
		seg=np.array(seg).reshape(1,np.array(seg).shape[0],np.array(seg).shape[1],1)
		ref=np.resize(np.array(ref),(np.array(seg).shape[1],np.array(seg).shape[2]))
		ref=np.array(ref).reshape(1,np.array(seg).shape[1],np.array(seg).shape[2],1)

		seg_up=np.append(seg, seg_up, axis=0)
		ref_up=np.append(ref, ref_up, axis=0)
		print(ref_up.shape,seg_up.shape)
	print('the new size of automatic segmentation images is : ')
	print(len(seg_up))
	print('the new size of manual segmentation images is : ')
	print(len(ref_up))

	seg2=binary_images(seg_up)
	ref2=binary_images(ref_up)

	my_list=[] 
	#run the metrics and save to txt 
	evalut=evaluation_metric(seg2,ref2)
	with open (sys.argv[1]+'/evaluation.txt','w') as f:
			my_list.append(evalut.test_sensitivity())
			my_list.append(evalut.test_specificity())
			my_list.append(evalut.test_dice_score())
			#my_list.append(evalut.test_ACD())
			#my_list.append(evalut.test_hausdorff())
			f.write(str(my_list[0])+'\n')
			f.write(str(my_list[1])+'\n')
			f.write(str(my_list[2])+'\n')
			#f.write(str(my_list[3])+'\n')
			#f.write(str(my_list[4])+'\n')
	#print('patient results are:')
	print(my_list[0])
	print(my_list[1])
	print(my_list[2])	
	f.close()




#class PairwiseEvaluator(CachedSubanalysisEvaluator):

#    def default_evaluation_list(self):

 #       return ['dice', 'sensitivity','specificity']





#def initialise_evaluator(seg_img, ref_img, measurements):
        
#	evaluator = PairwiseMeasures(seg_img, ref_img, measurements)
#	return evaluator

#if __name__ == '__main__':
#	if len(sys.argv) < 3:
#		print('Usage: {0} segmentation_images, ground trouth, and measurement metrices'.format(sys.argv[0]))
#		exit(1)
#	measurements1=['dice', 'sensitivity','specificity']
	#measurements2=['sensitivity']
	#measurements3=['specificity']

	
#	seg_img= sorted(glob.glob(sys.argv[1] + '/*'))
#	ref_img= sorted(glob.glob(sys.argv[2] + '/*'))
#	segmentation_param = None
#	reader_names = ('image', 'label', 'inferred')
#	data_param = None
#	task_param=None
#	file_list=None
#	readers = [ImageReader(reader_names).initialise( data_param, task_param, file_list)]

#	eval_param=initialise_evaluator(seg_img, ref_img, measurements1)
#	evaluator=PairwiseEvaluator(readers[0], segmentation_param, eval_param)

	#metric2=initialise_evaluator(seg_img, ref_img, measurements2)
	#metric3=initialise_evaluator(seg_img, ref_img, measurements3)

#	my_list=[] 
#	with open (sys.argv[1]+'/evaluation.txt','w') as f:
#			my_list.append(evaluator[0])
#			my_list.append(evaluator[1])
#			my_list.append(evaluator[2])
#			f.write(str(my_list[0])+'\n')
#			f.write(str(my_list[1])+'\n')
#			f.write(str(my_list[2])+'\n')
#	print('patient results are:')
#	print(my_list)
#	f.close()
