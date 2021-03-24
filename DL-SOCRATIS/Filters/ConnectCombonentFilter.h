////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef ConnectCombonentFilter_h
#define ConnectCombonentFilter_h

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkLabelToRGBImageFilter.h"
#include "itksys/SystemTools.hxx"
#include <sstream>
#include "QuickView.h"
#include "LabelObject.h"
#include "image_vision.h"
#include "BinaryImage.h"
#include "itkRGBToLuminanceImageFilter.h"
class ConnectCombonentFilter
{
public:
const char * m_outputImage_CCF;

ConnectCombonentFilter(){};
~ConnectCombonentFilter(){};

void Image_CCF(const char * m_inputFileName, const char * m_outputFileName, const char * m_maskfilter);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_CCF;}
};

#endif
