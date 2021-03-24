import os,sys,inspect
import os
import tensorflow as tf
import numpy as np
import scipy
import time
import pickle

import tensorflow as tf

from tensorflow.keras import activations
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Layer, Dense, Flatten, Activation, LeakyReLU, Softmax, ELU



import scipy.io as sio





class Gaussian_Weighting(Layer):

  def __init__(self, u, d, **kwargs):
    self.u = u
    self.d = d
    super(Gaussian_Weighting, self).__init__(**kwargs)

  def build(self, input_shape):
    self.mu = self.add_weight(name='mu',
                              shape=(1, self.d),
                              initializer='uniform',
                              trainable=True)
    self.sigma = self.add_weight(name='sigma',
                                 shape=(1, self.d),
                                 initializer='uniform',
                                 trainable=True)
    super(Gaussian_Weighting, self).build(input_shape)  

  def call(self, X):
    batch_size, n_nodes, n_features = X.shape
    u_shape, u_rows, u_cols, u_val = self.u

    diff =  tf.square(u_val - self.mu)
    factor = tf.square(self.sigma) + 1e-14
    weights = -0.5 * (diff / factor)
    weights = tf.math.reduce_sum(weights, axis=1)

    weights = tf.SparseTensor(indices=np.vstack([u_rows, u_cols]).T, 
                              values=weights, 
                              dense_shape=[u_shape]*2)
    weights = tf.sparse.reorder(weights)
    weights = tf.sparse.softmax(weights)


    X_t = tf.reshape(tf.transpose(X, [1,2,0]), [n_nodes, batch_size * n_features])
    
    D = tf.sparse.sparse_dense_matmul(weights, X_t)
    D = tf.transpose(tf.reshape(D, [n_nodes, n_features, batch_size]), [2,0,1])
    
    return D
    
  def compute_output_shape(self, input_shape):
    return (input_shape[0], self.output_dim)

class Rician_Weighting(Layer):

  def __init__(self, u, d, **kwargs):
    self.u = u
    self.d = d
  #  self.batch_size = batch_size
    super(Rician_Weighting, self).__init__(**kwargs)

  def build(self, input_shape):
    self.mu = self.add_weight(name='mu',
                              shape=(1, self.d),
                              initializer='uniform',
                              trainable=True)
    self.sigma = self.add_weight(name='sigma',
                                 shape=(1, self.d),
                                 initializer='uniform',
                                 trainable=True)
    super(Rician_Weighting, self).build(input_shape)  

  def call(self, X):
    batch_size, n_nodes, n_features = X.shape
 #   batch_size = self.batch_size
    u_shape, u_rows, u_cols, u_val = self.u

    diff =  tf.pow(u_val,2)- tf.pow(self.mu,2)
    factor = tf.pow(self.sigma,2) + 1e-14
    weights = -0.5 * (diff / factor)
    weights = tf.math.reduce_sum(weights, axis=1)
    weights = tf.SparseTensor(indices=np.vstack([u_rows, u_cols]).T, 
                              values=weights, 
                              dense_shape=[u_shape]*2)
    weights = tf.sparse.reorder(weights)
    weights = tf.sparse.softmax(weights)

    bassels = tf.math.bessel_i0e((u_val*self.mu)/tf.pow(self.sigma,2))
    bassels = tf.reshape(tf.transpose(bassels, [1,2,0]), [n_nodes, batch_size * n_features])

    beselD = tf.sparse.sparse_dense_matmul(weights, bassels)

    X_t = tf.reshape(tf.transpose(X, [1,2,0]), [n_nodes, batch_size * n_features])
    
    D = tf.sparse.sparse_dense_matmul(beselD, X_t)
    D = tf.transpose(tf.reshape(D, [n_nodes, n_features, batch_size]), [2,0,1])
    
    return D
    
  def compute_output_shape(self, input_shape):
    return (input_shape[0], self.output_dim)

  
# Create Gaussian (kernel) layer

class RGMMNet(Layer):

  def __init__(self, u, d, n_mixture_models, n_hidden, mm_case='GMM', portion=1, **kwargs):
    self.u = u
    self.d = d

    if self.case=='RGMM':
      self.n_gaussian = int(n_mixture_models*portion)
      self.n_rician=n_mixture_models-self.n_gaussian

    if self.case=='RMM':
      self.n_rician = n_mixture_models

    if self.case=='GMM':
      self.n_gaussian = n_mixture_models

    self.n_hidden = n_hidden
#    self.batch_size = batch_size
    self.case=mm_case
    self.weightings = []
    if mm_case=='GMM':
      for k in range(self.n_gaussian):
        weighting = Gaussian_Weighting(self.u, self.d, trainable=True)
        self.weightings.append(weighting)

    if mm_case=='RMM':
      for k in range(self.n_rician):
        weighting = Rician_Weighting(self.u, self.d, trainable=True)
        self.weightings.append(weighting)


    if mm_case=='RGMM':
      for k in range(self.n_gaussian):
        weighting = Gaussian_Weighting(self.u, self.d, trainable=True)
        self.weightings.append(weighting)
      for k in range(self.n_rician):
        weighting2 = Rician_Weighting(self.u, self.d, trainable=True)
        self.weightings.append(weighting2)



    super(RGMMNet, self).__init__(**kwargs)

  def build(self, input_shape):

    if self.case=='GMM':
      self.GW = self.add_weight(name='GW', 
                             shape=(input_shape[-1] * self.n_gaussian, self.n_hidden),
                             initializer='uniform',
                             trainable=True)

    if self.case=='RMM':

      self.RW = self.add_weight(name='RW', 
                             shape=(input_shape[-1] * self.n_rician, self.n_hidden),
                             initializer='uniform',
                             trainable=True)


    if self.case=='RGMM':
      self.RGW = self.add_weight(name='RGW', 
                             shape=(input_shape[-1] * (self.n_gaussian+self.n_rician), self.n_hidden),
                             initializer='uniform',
                             trainable=True)



    super(RGMMNet, self).build(input_shape)  # Be sure to call this at the end

  def call(self, X):
    weightings = []

    if self.case=='GMM':

      for k in range(self.n_gaussian):
        weighting = self.weightings[k](X)
        weightings.append(weighting)
    
      weights = tf.transpose(tf.stack(weightings), [1,2,3,0])
    
      batch_size, n_nodes, n_features = X.shape
      weights = tf.reshape(weights, [batch_size*n_nodes, n_features*self.n_gaussian])
    
      h = weights @ self.GW
      h = tf.reshape(h, [batch_size, n_nodes, self.n_hidden])

    if self.case=='RMM':

      for k in range(self.n_rician):
        weighting = self.weightings[k](X)
        weightings.append(weighting)
    
      weights = tf.transpose(tf.stack(weightings), [1,2,3,0])
    
      batch_size, n_nodes, n_features = X.shape
      weights = tf.reshape(weights, [batch_size*n_nodes, n_features*self.n_rician])
    
      h = weights @ self.RW
      h = tf.reshape(h, [batch_size, n_nodes, self.n_hidden])


    if self.case=='RGMM':

      for k in range(self.n_gaussian):
        weighting = self.weightings[k](X)
        weightings.append(weighting)    
      for k in range(self.n_gaussian,(self.n_gaussian+self.n_rician)):
        weighting2 = self.weightings[k](X)
        weightings.append(weighting2)
    
      weights = tf.transpose(tf.stack(weightings), [1,2,3,0])
    
      batch_size, n_nodes, n_features = X.shape
      weights = tf.reshape(weights, [batch_size*n_nodes, n_features*self.n_gaussian])
    
      h = weights @ self.RGW
      h = tf.reshape(h, [batch_size, n_nodes, self.n_hidden])



    return h

  def compute_output_shape(self, input_shape):
    return (input_shape[1], self.n_hidden)

  
class MPool1(Layer):
      def __init__(self, p, **kwargs):
         self.p = p
         super(MPool1, self).__init__(**kwargs)
     
      def call(self, X):
        """Max pooling of size p. Should be a power of 2 (this is possible thanks to the reordering we previously did)."""
        if self.p > 1:
          X = tf.expand_dims(X, 3)  # shape = N x M x F x 1
          X = tf.nn.max_pool(X, ksize=[1,self.p,1,1], strides=[1, self.p,1,1], padding='SAME')
          X = tf.squeeze(X, [3])  # shape = N x M/p x F
        return X
          
      def compute_output_shape(self, input_shape):
        return input_shape


#######################################################################################################################################

# Create attention

class Attention(Layer):
  
  def __init__(self, F_1, kernel_regularizer, **kwargs):
    self.F_1 = F_1
    self.kernel_regularizer = kernel_regularizer
    super(Attention, self).__init__(**kwargs)

  def build(self, input_shape):
    F = input_shape[0][-1]
    self.W = self.add_weight(name='W', 
                             shape=(self.F_1, F),
                             initializer='glorot_uniform',
                             regularizer=self.kernel_regularizer,
                             trainable=True)

    self.a = self.add_weight(name='a', 
                             shape=(2, self.F_1, 1),
                             initializer='glorot_uniform',
                             regularizer=self.kernel_regularizer,
                             trainable=True)

    super(Attention, self).build(input_shape)  # Be sure to call this at the end
   
  def call(self, inputs):
    X = inputs[0]
    A = inputs[1]

    x_features = X @ tf.transpose(self.W)
#     x_features = (N, F_1)
    att_self = x_features @ self.a[0]
    att_neighbours = x_features @ self.a[1]
#     both att = (N, 1)
    att = att_self + tf.transpose(att_neighbours)
#     att = (N, N)
    att = LeakyReLU(alpha=0.2)(att)

    mask = -10e9 * (1.0 - A)
    att_masked = att + mask
#     att_masked = att * A
    dense = Softmax(axis=0)(att_masked)
    
#     dense = (N, N)
    dense = dense @ x_features
#     dense = (N, F_1)

    return dense

  def compute_output_shape(self, input_shape):
    return (input_shape[0][0], self.output_dim)
    


# Create Gaussian (kernel) layer

class GAT(Layer):
  
  def __init__(self, F_1, K, kernel_regularizer, last=False, **kwargs):
    self.F_1 = F_1
    self.K = K
    self.kernel_regularizer = kernel_regularizer
    self.last = last
    
    self.attentions = []
    for k in range(self.K):
      attention = Attention(self.F_1, self.kernel_regularizer)
      self.attentions.append(attention)
      
    super(GAT, self).__init__(**kwargs)
  
  def build(self, input_shape):

    super(GAT, self).build(input_shape)  # Be sure to call this at the end
   
  def call(self, inputs):
    X = inputs[0]
    A = inputs[1]

    attentions = []
    for k in range(self.K):
      attention = self.attentions[k]([X, A])
      attentions.append(attention)

    if self.last:
#       h = (N, F_1)
      h = tf.reduce_mean(tf.stack(attentions), axis=0)
      h = Softmax()(h)
    else:
#       h = (N, F_1*k)
      h = tf.concat(attentions, axis=1)
      h = ELU()(h)
    return h

  def compute_output_shape(self, input_shape):
    return (input_shape[0][0], self.output_dim)

































