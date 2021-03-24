////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef deformable_registration_h
#define deformable_registration_h

#include <stdlib.h> 
#include <string.h> 
#include <Python.h>
#include <iostream>
#include <memory>  
#include <algorithm>

#include <bits/stdc++.h> 
#include <stdio.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <exception>
#include <fstream>
#include <sstream>
 
#include <dirent.h>
#include <array>
#include <iomanip>
#include <vector>
#include <boost/filesystem.hpp>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSymmetricForcesDemonsRegistrationFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkWarpImageFilter.h"
#include "BinaryImage.h"
#include "mergeimages.h"
#include "Initial_Segmentation.h"
#include "Resample.h"
#include "Atlas_segm.h"
#include "Region_grown.h"
#include <denoising.h>
#include "itkResampleImageFilter.h"
#include <configuration.h>

#include "itkCheckerBoardImageFilter.h"

#include "itkCommand.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkConjugateGradientLineSearchOptimizerv4.h"

#include "itkTranslationTransform.h"
#include "itkAffineTransform.h"
#include "itkCompositeTransform.h"
#include "itkTranslationTransform.h"


#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"

//#include "itkVTKTetrahedralMeshReader.h"
#include <itkBinaryMask3DMeshSource.h>
#include "itkImage.h"
#include "itkVector.h"
#include "itkPhysicsBasedNonRigidRegistrationMethod.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMetaImageIO.h"
#include "itkTestingMacros.h"
#include "itkMesh.h"
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"
#include <iostream>
#include "itkSimilarity2DTransform.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkRegistrationParameterScalesFromPhysicalShift.h"
#include "itkVectorImageToImageMetricTraitsv4.h"
 
#include "itkGaussianSmoothingOnUpdateDisplacementFieldTransform.h"

#include "strain_compute.h"

#include "itkBSplineTransform.h"
#include "itkTransformFileWriter.h" 

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


class deformable_registration
{
public:




const char * m_outputImage_DR;

deformable_registration(std::vector<bool> multistage_mode);
deformable_registration(std::string m_root){root=m_root;};
~deformable_registration(){};

void resample_reg(const char * m_input, const char * m_out, double space[2], double origin[2], double size[2]);

void init_reg(const char * m_inputfix, const char * m_inputmove);

void multi_stage_reg(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName);

void trans_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName);

void similarity_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName);

void affine_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName);
void rigid_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName);
void deform_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName);
void center_registartion(const char * m_inputfix, const char * m_inputmove,  const char * m_inputmove2, const char * m_outputFileName);

void compute_demons(const char * m_inputfix, const char * m_inputmove, const char * m_outputFileName, const char * vector_image, const char * m_outputFileName2);
void compute_b_splines(const char * m_inputfix, const char * m_inputmove, const char * m_outputFileName, const char * vector_image, const char * m_outputFileName2);
void FEM(const char * m_inputfix, const char * m_inputmove, const char * m_outputFileName, const char * vector_image,const char * m_outputFileName2 );
//Set Get functions
inline const char * GetOutput(){return m_outputImage_DR;}

void Image_segment_Thresholding(const char * m_inputFileName, const char * m_outputFileName, float m_lowerThreshold, float m_upperThreshold, bool inverse);


private:
bool trans_reg=false;
bool affine_reg=false;
bool rigid_reg=false;
bool deform_reg=false;
std::string root="./1/";
};

#endif
