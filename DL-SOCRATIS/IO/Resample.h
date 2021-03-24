#ifndef Resample_h
#define Resample_h

#include "itkImage.h"
#include "itkIdentityTransform.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkImageFileReader.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"

class Resample
{
public:

Resample(){};
~Resample(){};

void resample(const char * m_inputFileName, int size, int interpolation_type);

};

#endif
