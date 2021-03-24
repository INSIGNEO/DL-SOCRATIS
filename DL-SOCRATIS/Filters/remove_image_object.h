////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef remove_image_object_h
#define remove_image_object_h


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryFillholeImageFilter.h"
#include "itkLabelObject.h"
#include "itkLabelMap.h"
#include "itkSimpleFilterWatcher.h"
#include "itkLabelShapeKeepNObjectsImageFilter.h"
#include "itkBinaryImageToShapeLabelMapFilter.h"
#include "itkShapeOpeningLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "denoising.h"
#include "BinaryImage.h"

class remove_image_object
{
public:
const char * m_outputImage_RH;

remove_image_object(std::string m_root){root=m_root;};
~remove_image_object(){};

void Image_removeholes(const char * m_inputFileName, const char * m_outputFileName);
void remove_object( const char * i_image, const char * out_image, int number, const char * type);
void remove_holles( const char * init_image);
void remove_binary_object( const char * i_image, const char * out_image, int number, const char * type,  std::string remove_type);
void remove_pixel_object( const char * i_image, int number_pixel, int nbr_object);


//Set Get functions
inline const char * GetOutput(){return m_outputImage_RH;}
private:
std::string root="./1/";
void smooth_images( std::string predictim,  int loop, int thres );

};





#endif
