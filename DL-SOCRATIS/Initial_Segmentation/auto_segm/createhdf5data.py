import h5py
import numpy as np
import os

class createhdf5data(object):

	def create_hdf5(hdf5_filename, txt_filename, data, labels, name=None, descr=None, shuffle=False):
		"""
		Creates a HDF5 file containing given data and labels as HDF5 datasets with names 'data' and 'label'.
		:param hdf5_filename: Filename of HDF5 file that is going to be created.
		:type hdf5_filename: str
		:param txt_filename: Filename of text file to write the HDF5 filename to.
		:type txt_filename: str
		:param data: Numpy array containing the data. Should be of type float.
		:type data: numpy.multiarray.ndarray
		:param labels: Numpy array containing the labels for the data.
		:type data: numpy.multiarray.ndarray
		:param name: Optional name to add as attribute to the HDF5 file.
		:type name: str
		:param descr: Optional description to add as attribute to the HDF5 file.
		:type descr: str
		:param shuffle: Shuffle data before writing. Default is False.
		:type shuffle: bool
		"""
		hdf_file = h5py.File(hdf5_filename, 'w')
		# store some metadata	
		if name is not None:
			hdf_file.attrs['name'] = name
		if descr is not None:
			hdf_file.attrs['description'] = descr
		num_data = len(data)
		datum_shape = data[0].shape
		# optionally shuffle data
		if shuffle:
			perm = np.random.permutation(num_data)
			data = data[perm]
			labels = labels[perm]
	
		# assuming that the rank of a datum is 3
		# setting chunk size to match a datum
		chunks = (1, datum_shape[0], datum_shape[1], datum_shape[2])
		hdf_file.create_dataset('data', dtype=np.float, data=data, chunks=chunks)
		hdf_file.create_dataset('label', dtype=np.float, data=labels)
		hdf_file.flush()
		hdf_file.close()
		# caffe expects a txt file containing a list of hdf5 file names
		with open(txt_filename, mode='w') as txt_file:
			txt_file.writelines([hdf5_filename])

