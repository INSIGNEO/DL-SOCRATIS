////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef EMGaussianRician_h
#define EMGaussianRician_h

#include "itkVector.h"
#include "itkListSample.h"
#include "itkGaussianMixtureModelComponent.h"
#include "itkRicianMixtureModelComponent.h"
#include "itkRicianMembershipFunction.h"

#include "itkExpectationMaximizationMixtureModelEstimator.h"
#include "itkNormalVariateGenerator.h"

//for the vision of example samples
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPen.h> 
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>


class EMGaussianRician
{
public:
  /**Standard class type aliases. */

const char * m_outputImage_THR;
;
EMGaussianRician(){};
~EMGaussianRician(){};



void Image_EMGaussianRician();
//Set Get functions
inline const char * GetOutput(){return m_outputImage_THR;}



};

#endif
