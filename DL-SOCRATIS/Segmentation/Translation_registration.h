////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef Translation_registration_h
#define Translation_registration_h

#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkCenteredRigid2DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkCommand.h"

class Translation_registration
{
public:

const char * m_outputImage_TR;

Translation_registration(){// you can initialize the parametersa of each code fix it better
};
~Translation_registration(){};

void Translation_registration_Filter(const char * m_fixedinputFileName,const char * m_movinginputFileName, const char * m_maskinputFileName, const char * m_outputFileName);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_TR;}

};




#endif
