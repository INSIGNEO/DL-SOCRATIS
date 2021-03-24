#ifndef kmeans_h
#define kmeans_h

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkScalarImageKmeansImageFilter.h>


class kmeans
{
public:
const char * m_outputImage_KM;

kmeans(){};
~kmeans(){};

void Image_kmeans(const char * m_inputImageFileName, const char * m_outputImageFileName, const unsigned int m_useNonContiguousLabels, const unsigned int m_numberOfInitialClasses, double m_argoffset[]);
//Set Get functions
using MeasurementVectorType = itk::Vector< float, 1 >;
using SampleType = itk::Statistics::ListSample< MeasurementVectorType >;
 SampleType::Pointer Means = SampleType::New();
inline SampleType::Pointer Getmeans(){return Means;}
};

#endif
