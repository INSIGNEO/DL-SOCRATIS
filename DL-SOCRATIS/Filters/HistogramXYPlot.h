////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef HistogramXYPlot_h
#define HistogramXYPlot_h

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
 
#include <string>
 
#include <vtkActor.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkJPEGReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXYPlotActor.h>


class HistogramXYPlot
{
public:
const char * m_outputImage_XYPlot;
HistogramXYPlot(){};
~HistogramXYPlot(){};

void ImageHistogramXYPlot(const char * m_inputFileNameXYPlot, std::string m_ignore);

private:


};

#endif
