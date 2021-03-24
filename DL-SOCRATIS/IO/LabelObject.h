#ifndef LabelObject_h
#define LabelObject_h

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAttributeLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkAttributeKeepNObjectsLabelMapFilter.h"
#include "itkAttributeOpeningLabelMapFilter.h"
#include "itkAttributeRelabelLabelMapFilter.h"
#include "itkLabelMapToAttributeImageFilter.h"
#include "itkLabelMapToLabelImageFilter.h"

class LabelObject
{
public:

LabelObject(){};
~LabelObject(){};

void process(const char * m_inputFileName, const char * m_outputFileName, int objects, int lambda);

};

#endif
