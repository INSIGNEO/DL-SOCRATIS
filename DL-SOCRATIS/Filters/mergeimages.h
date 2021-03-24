////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////

#ifndef mergeimages_h
#define mergeimages_h

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSubtractImageFilter.h"
#include <itkImageToVTKImageFilter.h>
#include "seedpoints.h"
#include "itkMaskImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "Change_information.h"
#include <itkAddImageFilter.h>
#include <itkTestingComparisonImageFilter.h>

#include "itkImage.h"

#include <itkImageToVTKImageFilter.h>

#include "vtkVersion.h"
#include "vtkImageViewer.h"
#include "vtkImageMapper3D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkImageActor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderer.h"

class mergeimages
{
public:
const char * m_outputImage_merge;
bool inverse=false;
mergeimages(std::string m_root){root=m_root;};
~mergeimages(){};
void Set_root_path(std::string path_root){root= path_root;};
void Image_add_const(const char * m_inputFileName1,int constant, const char * m_outputFileName);
void Image_merge(const char * m_inputFileName1, const char * m_inputFileName2, const char * m_outputFileName);
void Image_add(const char * m_inputFileName1, const char * m_inputFileName2, const char * m_outputFileName);
//Set Get functions
inline const char * GetOutput(){return m_outputImage_merge;}
void Crop_binary(const char * inputFileName1, const char * inputFileName2, const char * outputFileName);
void Crop_cicle(const char * inputFileName1, int centerx, int centey,int radius, const char * outputFileName);
double Compare_images(const char * m_inputFileNameoriginal, const char * m_inputFileNametest);
void Image_overlap(const char * m_inputFileName1, const char * m_inputFileName2, const char * m_outputFileName);

private:
std::string root="./1/";
};

#endif
