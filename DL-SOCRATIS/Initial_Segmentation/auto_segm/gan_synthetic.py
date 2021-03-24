# code from: https://github.com/sugyan

import tensorflow as tf
import numpy as np
from auto_segm import datasetnet

def linear(input_tensor, output_dim, scope=None, stddev=1.0):
	norm = tf.random_normal_initializer(stddev=stddev)
	const = tf.constant_initializer(0.0)
	with tf.variable_scope(scope or 'linear'):
		w = tf.get_variable('w', [input_tensor.get_shape()[1], output_dim], initializer=norm)
		b = tf.get_variable('b', [output_dim], initializer=const)
	return tf.matmul(input_tensor, w) + b
      
def minibatch(input_tensor, num_kernels=256, kernel_dim=4):
	in_vec = tf.reshape(input_tensor, [input_tensor.get_shape().as_list()[0], -1])
	x = linear(in_vec, num_kernels * kernel_dim, scope='minibatch', stddev=0.02)
	activation = tf.reshape(x, (-1, num_kernels, kernel_dim))
	diffs = tf.expand_dims(activation, 3) - tf.expand_dims(tf.transpose(activation, [1, 2, 0]), 0)
	abs_diffs = tf.reduce_sum(tf.abs(diffs), 2)
	minibatch_features = tf.reduce_sum(tf.exp(-abs_diffs), 2)
	#minibatch_features = tf.reshape(minibatch_features, (128, 4, 4, -1))
	test_out = tf.concat(axis=1, values=[in_vec, minibatch_features])
	test_out = tf.reshape(test_out, [input_tensor.get_shape().as_list()[0], input_tensor.get_shape().as_list()[1], input_tensor.get_shape().as_list()[2], -1])
	return test_out

def disc_conv2d(input_tensor, num_inputs, num_outputs, kernel_size, stride, dropout_ratio, scope):
	with tf.variable_scope(scope):
		w = tf.get_variable('w',[kernel_size, kernel_size, num_inputs, num_outputs],tf.float32, tf.truncated_normal_initializer(stddev=0.05))
		b = tf.get_variable('b', [num_outputs], tf.float32, tf.zeros_initializer())
		c = tf.nn.conv2d(input_tensor, w, [1, stride, stride, 1], 'SAME')
		mean, variance = tf.nn.moments(c, [0, 1, 2])
		outputs = leaky_relu(tf.nn.batch_normalization(c, mean, variance, b, None, 1e-5))
		#outputs = tf.nn.dropout(outputs, 0.5)
		return outputs;
		#out.append(outputs)

class Generator:
	def __init__(self, depths=[1024, 512, 256, 128, 64], f_size=4):
		self.reuse = False
		self.f_size = f_size
		self.depths = depths + [2]
		self.case="one"

	def model(self, inputs):
		i_depth = self.depths[0:(len(self.depths) - 1)]
		o_depth = self.depths[1:(len(self.depths))]
		out = []
		#print(self.reuse)
		with tf.variable_scope('g', reuse=self.reuse):
			# reshape from inputs
			#if (self.case=="one"):
			inputs = tf.convert_to_tensor(inputs)
			print(inputs.get_shape()[-1])
			print(inputs.get_shape())
			with tf.variable_scope('fc_reshape'):
				w0 = tf.get_variable('w', [inputs.get_shape()[-1], i_depth[0] * self.f_size * self.f_size], tf.float32, tf.truncated_normal_initializer(stddev=0.05))
				b0 = tf.get_variable('b',[i_depth[0]],tf.float32,tf.zeros_initializer())
				fc = tf.matmul(inputs, w0)
				reshaped = tf.reshape(fc, [-1, self.f_size, self.f_size, i_depth[0]])
				mean, variance = tf.nn.moments(reshaped, [0, 1, 2])
				outputs = tf.nn.relu(tf.nn.batch_normalization(reshaped, mean, variance, b0, None, 1e-5))
				out.append(outputs)
				# deconvolution (transpose of convolution) x 4
			#print("done")
            
			for i in range(len(self.depths) - 1):
		          
				with tf.variable_scope('conv%d' % (i + 1)):
					kernel_size = 5

		             
					w = tf.get_variable('w',[kernel_size, kernel_size, o_depth[i], i_depth[i]], tf.float32,  tf.truncated_normal_initializer(stddev=0.05))
					b = tf.get_variable( 'b',[o_depth[i]],tf.float32,tf.zeros_initializer())
					dc = tf.nn.conv2d_transpose(outputs,w,[int(outputs.get_shape()[0]),self.f_size * 2 ** (i + 1),
					self.f_size * 2 ** (i + 1),o_depth[i]], [1, 2, 2, 1])
					if i < len(self.depths) - 2:
						mean, variance = tf.nn.moments(dc, [0, 1, 2])
						outputs = tf.nn.relu(tf.nn.batch_normalization(dc, mean, variance, b, None, 1e-5))
					else:
						outputs = tf.nn.bias_add(dc, b)
						outputs = tf.nn.tanh(outputs)
					out.append(outputs)
                    
		self.reuse = True
		#print(self.reuse)
		self.variables = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, scope='g')
		#print("out")
		return out

	def __call__(self, inputs):
		return self.model(inputs)


class Discriminator:
	def __init__(self, depths=[64, 128, 256, 512]):
		self.reuse = False
		self.depths = [2] + depths
        


	def model(self, inputs, return_feature_vector=False):
		def leaky_relu(x, leak=0.2):
			return tf.maximum(x, x * leak)
		#print(self.reuse)
		i_depth = self.depths[0:(len(self.depths) - 1)]
		o_depth = self.depths[1:(len(self.depths))]
		out = []
		self.weights = []
		with tf.variable_scope('d', reuse=self.reuse):
			outputs = inputs
			print(outputs.get_shape())
			# convolution x 4

			for i in range(len(self.depths) - 1):
				with tf.variable_scope('conv%d' % i):
					w = tf.get_variable('w',[5, 5, outputs.get_shape()[3], o_depth[i]],tf.float32,tf.truncated_normal_initializer(stddev=0.05))
					b = tf.get_variable('b',[o_depth[i]],tf.float32, tf.zeros_initializer())
					c = tf.nn.conv2d(outputs, w, [1, 2, 2, 1], 'SAME')
					mean, variance = tf.nn.moments(c, [0, 1, 2])
					outputs = leaky_relu(tf.nn.batch_normalization(c, mean, variance, b, None, 1e-5)) 
					#print(outputs.get_shape())
					self.weights.append(w);
					out.append(outputs)         
			if(return_feature_vector == True):
				return out[3]      
				    
			with tf.variable_scope('classify'):
				dim = 1
				for d in outputs.get_shape()[1:].as_list():
					dim *= d
					#print(dim)
				w = tf.get_variable('w', [dim, 2], tf.float32, tf.truncated_normal_initializer(stddev=0.02))
				b = tf.get_variable('b', [2], tf.float32, tf.zeros_initializer())
				out.append(tf.nn.bias_add(tf.matmul(tf.reshape(outputs, [-1, dim]), w), b))
		self.reuse = True
		#print(self.reuse)
		self.variables = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, scope='d')
		#print("out")
		return out

	def __call__(self, inputs, return_feature_vector=False):
		return self.model(inputs, return_feature_vector)


class DCGAN:
	def __init__(self,batch_size=128, f_size=2, z_dim=400, gdepth1=512, gdepth2=256, gdepth3=128, gdepth4=128,ddepth1=128, ddepth2=128, ddepth3=256, ddepth4=512):
		self.batch_size = batch_size
		self.f_size = f_size
		self.z_dim = z_dim
		self.d = Discriminator(depths=[ddepth1, ddepth2, ddepth3, ddepth4, ddepth4, ddepth4])
		self.z = tf.random_uniform([self.batch_size, self.z_dim], minval=-1.0, maxval=1.0)
		self.losses = {'g': None, 'd': None }
		self.case_g="one"
		#if (self.case_g=="one"):
		self.g = Generator(depths=[gdepth1, gdepth2, gdepth3, gdepth4, gdepth4, gdepth4], f_size=2)
		#if (self.case_g=="two"):
		#self.g = Generator(depths=[gdepth1, gdepth2, gdepth3, gdepth4, gdepth4, gdepth4], f_size=4)
		self.g.case=self.case_g

	def build(self, input_images,learning_rate=0.0004, beta1=0.5, feature_matching=False):
		"""build model, generate losses, train op"""
		print('generate images for G')	
		generated_images = self.g_input(input_images)

		print('before d gen')
		outputs_from_g = self.d(generated_images)
		
		print('before d real')
		outputs_from_i = self.d(input_images)
		logits_from_g = outputs_from_g[-1]
		logits_from_i = outputs_from_i[-1] 
		tf.add_to_collection('g_losses',tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(logits=logits_from_g, labels=tf.ones([self.batch_size], dtype=tf.int64)))) # need probability one
		
		d_loss_real = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(logits=logits_from_i, labels=tf.ones([self.batch_size], dtype=tf.int64))) # need probability one		
		d_loss_fake = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(logits=logits_from_g, labels=tf.zeros([self.batch_size], dtype=tf.int64))) # need probability zero so no look like fake
		tf.add_to_collection('d_losses', d_loss_real)
		tf.add_to_collection('d_losses',d_loss_fake)

		if feature_matching:
			features_from_g = tf.reduce_mean(outputs_from_g[-2], axis=(0))
			features_from_i = tf.reduce_mean(outputs_from_i[-2], axis=(0))
			tf.add_to_collection('g_losses', tf.multiply(tf.nn.l2_loss(features_from_g - features_from_i), 0.1))
			mean_image_from_g = tf.reduce_mean(generated_images, axis=(0))
			mean_image_from_i = tf.reduce_mean(input_images, axis=(0))
			tf.add_to_collection('g_losses', tf.multiply(tf.nn.l2_loss(mean_image_from_g - mean_image_from_i), 0.01))
		self.losses['g'] = tf.add_n(tf.get_collection('g_losses'), name='total_g_loss')
		self.losses['d'] = tf.add_n(tf.get_collection('d_losses'), name='total_d_loss')
		g_opt = tf.train.AdamOptimizer(learning_rate=learning_rate, beta1=beta1)
		d_opt = tf.train.AdamOptimizer(learning_rate=learning_rate, beta1=beta1)
		self.g_opt_op = g_opt.minimize(self.losses['g'], var_list=self.g.variables)
		self.d_opt_op = d_opt.minimize(self.losses['d'], var_list=self.d.variables)
		with tf.control_dependencies([self.g_opt_op, self.d_opt_op]):
			self.train = tf.no_op(name='train')
		return self.train

	def sample_images(self, row=8, col=8, num_samples = None, inputs=None):
		if inputs is None:
			inputs = self.z
			    
		if num_samples is None:
			num_samples = self.batch_size

		#inputs = tf.random_uniform([num_samples, self.z_dim], minval=-1.0, maxval=1.0)
		images = tf.cast(tf.multiply(tf.add(self.g(inputs)[-1], 1.0), 127.5), tf.uint8)
			  
		return images
		images = [image for image in tf.split(axis=0, num_or_size_splits=self.batch_size, value=images)]
		rows = []
		for i in range(row):
			rows.append(tf.concat(axis=2, values=images[col * i + 0:col * i + col]))
		image = tf.concat(axis=1, values=rows)
		return tf.image.encode_jpeg(tf.squeeze(image, [0]))

	def test_images(self, row=8, col=8, num_samples = None, inputs=None, checkpoint_folder=None, num_synthetic_images=10, image_structure=None):

		itterations=int(num_synthetic_images/self.batch_size)
		total_images, total_inputs=[],[]
		#saver.restore(sess, checkpoint_folder)
		#graph=tf.get_default_graph()
		ckpt = tf.train.get_checkpoint_state(checkpoint_folder)
		print("Store weights of Generator network: ")
		print(ckpt.model_checkpoint_path)
		train_structure=self.build(image_structure)
		saver=tf.train.Saver()
		with tf.Session() as sess:
			summary_writer = tf.summary.FileWriter(checkpoint_folder+"/", graph=sess.graph)
			sess.run(tf.global_variables_initializer())          
			coord = tf.train.Coordinator()
			threads = tf.train.start_queue_runners(sess=sess, coord=coord)   
			_, g_loss_value, d_loss_value = sess.run([train_structure,self.losses['g'], self.losses['d']])
			_, g_loss_value, d_loss_value = sess.run([self.g_opt_op, self.losses['g'], self.losses['d']]) #which one to dicrease g_opt_op 
			print("extract synthetic data... ")
			saver.restore(sess, ckpt.model_checkpoint_path)
			for i in range(itterations):
				image=self.sample_images( row, col,  num_samples, inputs)
				imgs = sess.run(image)
				if (i==0):
					total_images=(imgs)
				if (itterations==1):
					total_images=imgs
				if (itterations!=1):
					#print(imgs.shape,np.array(total_images).shape)
					total_images=np.append(imgs,total_images,axis=0)
			total_images= np.asarray(total_images)
			coord.request_stop()
			coord.join(threads)
		return total_images 

	def retrieve_feature_vector(self, inputs):
		vector = tf.reshape(self.d(inputs, return_feature_vector=True), [-1]) 
		return vector

	def g_input(self,inputs):
		
		if (self.case_g=="one"):
			return self.g(self.z)[-1]
		if (self.case_g=="two"):
			image_size=128
			z_dim=image_size*image_size
			z_random = np.random.uniform(-1,1,(self.batch_size*z_dim))
			print(z_random.shape)
			z_random=np.array(z_random).reshape( 1,self.batch_size*image_size*image_size)
			print(z_random.shape)
			inputs_images=(inputs[:,:,:,0])
			inputs_images=tf.reshape(inputs_images,[1,self.batch_size*image_size*image_size])
			dsn=datasetnet.datasetnet('test','main') # need the test data from ROI to create synthetic images for endo and epi seperatly 
			dsn.image_shape= 128
			x, z_random=dsn.Gan_preprocessing_images(inputs_images,z_random)
			input_masks = tf.concat([inputs_images, z_random], 0)
			return self.g(input_masks)[-1]		


		
