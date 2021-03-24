from setuptools import setup
from setuptools import find_packages
#import pip


#pip.main(['install', 'git+https://www.github.com/keras-team/keras-contrib.git'])

setup(name='auto_segm',
      version='0.2',
      description='Deep Learning segmentation pipeline analysis in Python',
      url='http://github.com/ece7048',
      author='Michail Mamalakis',
      author_email='mmamalakis1@sheffield.ac.uk',
      license='GPL-3.0+',
      packages=['auto_segm','auto_segm.model', 'auto_segm.utils', 'auto_segm.CycleGAN','auto_segm.model.RGMMnet'],
      install_requires=[
	#  'niftynet',
          'numpy>=1.16.2',
          'scipy>=1.5.1',
          'matplotlib==3.0.2',
	  'dicom',
	  'pydicom==1.2.1',	
	  'opencv-python>=4.0.0.21',
	  'Pillow==7.2.0',
	  'vtk==8.1.1',	
          'future',
          'keras>=2.3.1',
          'tensorflow>=2.0.b1',
          #'tensorboard>=2.0.0',
	  #'tensorflow-gpu>=2.0.0b1',
	  'rq-scheduler==0.7.0',
	  'med2image',
	  'imageio',
          'gensim',
	  'SimpleITK',
	  'networkx',
	  'boost',
	  'sklearn',
 #          'SimpleItk',
# 	  'boost',
#          keras_contrib,
#	  'git+https://www.github.com/keras-team/keras-contrib.git',
#	  'pylab', 
      ],
	dependency_links=['https://www.github.com/keras-team/keras-contrib.git#egg=keras_contrib-2.0.8',


      ],
	zip_safe=False

)
