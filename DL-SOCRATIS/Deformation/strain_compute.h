////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef strain_compute_h
#define strain_compute_h

#include <stdlib.h> 
#include <string.h> 
#include <Python.h>
#include <iostream>
#include "mergeimages.h"
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
#include "itkVector.h"
#include "itkListSample.h"
#include "Segmentation_Pipeline.h"
#include "itkInvertIntensityImageFilter.h"
#include "denoising.h"
#include "Sobel.h"
#include "BinaryImage.h"
#include "mergeimages.h"
#include "Initial_Segmentation.h"
#include "deformable_registration.h"
#include "kmeans.h"
#include "Region_grown.h"
#include "boundarysegm.h"
#include <itkBinaryShapeOpeningImageFilter.h>
#include "itkStrainImageFilter.h"
#include "itkTransformToStrainFilter.h"
#include "itkBSplineTransform.h"

#include <vtkJPEGWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkJPEGReader.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataReader.h>
#include <vtkXMLImageDataWriter.h>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


class strain_compute
{
public:
typedef float PixelType_strain;
typedef itk::Vector< PixelType_strain, 2> DisplacementVectorType_strain;
using InputImageType_strain = itk::Image< DisplacementVectorType_strain, 2 > ;
typedef double CoordRepresentationType;
typedef itk::BSplineTransform< CoordRepresentationType, 2, 3 > TransformType;

const char * m_outputImage_ST;

strain_compute(std::string m_root){root=m_root;};
~strain_compute(){};

void strain_length_run(std::string Deform_path, std::string store_path);
void from_jpg_to_vtk(std::string jpg_file, std::string vtk_file);
void from_vtk_to_jpg(std::string vtk2_file , std::string jpg2_file);
void strain_length_contour_compute(vector<string>  inp, std::string number_path,  std::string T, std::string dt,std::string strain_roi_store);
void determ_ED_ES(std::string inp, std::string patient_store_path, unsigned int o);
void strain_transformation_compute(TransformType::Pointer inputTransform,const char * m_outputFileName );
void strain_displacement_compute(InputImageType_strain::Pointer displacementImage,const char * m_outputFileName );

//Set Get functions
inline const char * GetOutput(){return m_outputImage_ST;}


private:
std::string root="./1/";
vector<string> folders_name;
vector<int> patient_number;
std::string outED;
std::string outES;
std::string path_of_strain= "../Deformation/";
};

#endif
