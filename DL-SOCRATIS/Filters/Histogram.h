////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef Histogram_h
#define Histogram_h

#include <vector>
#include <fstream>
#include <itkVector.h>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToHistogramFilter.h"
#include "itkImageRandomIteratorWithIndex.h"


//for the vision of histogram file

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
 
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkPlotPoints.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

class Histogram
{
public:
const char * m_outputImage_THR;
Histogram(){};
~Histogram(){};

void ImageHistogram(const char * m_inputFileName, const char * bins, const char * m_outputFileName);
//Set Get functions
std::vector<double> Get_histogramm(){return histogramm_frequency;}; 
private:
std::vector<double> histogramm_frequency;

};

#endif
