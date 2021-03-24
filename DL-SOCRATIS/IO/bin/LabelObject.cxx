#include "LabelObject.h"

 
void  LabelObject::process(const char * m_inputFileName, const char * m_outputFileName, int objects, int lambda)
{
  const int dim = 2;
typedef unsigned char PType;
typedef itk::Image< PType, dim > IType;
typedef itk::AttributeLabelObject< unsigned long, dim, double > LabelObjectType;
typedef itk::LabelMap< LabelObjectType > LabelMapType;
  using RGBPixelType = itk::RGBPixel<unsigned char>;
  using RGBImageType = itk::Image<RGBPixelType, dim>;
typedef itk::ImageFileReader< IType > ReaderType;
ReaderType::Pointer reader = ReaderType::New();
reader->SetFileName( m_inputFileName );


typedef itk::LabelImageToLabelMapFilter< IType, LabelMapType > I2LType;
I2LType::Pointer i2l = I2LType::New();
i2l->SetInput( reader->GetOutput() );
i2l->SetBackgroundValue( itk::NumericTraits< PType >::min() );
i2l->Update();


LabelMapType::LabelObjectContainerType::const_iterator it;
LabelMapType::Pointer labelMap = i2l->GetOutput();
//const LabelMapType::LabelObjectContainerType & labelObjectContainer = labelMap->GetLabelObjectContainer();



typedef itk::AttributeKeepNObjectsLabelMapFilter< LabelMapType > KeepType;
KeepType::Pointer keep = KeepType::New();
keep->SetInput( labelMap );
keep->SetReverseOrdering( true );
keep->SetNumberOfObjects( objects );
keep->SetInPlace( false );
typedef itk::AttributeOpeningLabelMapFilter< LabelMapType > OpeningType;
OpeningType::Pointer opening = OpeningType::New();
opening->SetInput( labelMap );
opening->SetLambda( lambda);
opening->SetInPlace( false );
typedef itk::AttributeRelabelLabelMapFilter< LabelMapType > RelabelType;
RelabelType::Pointer relabel = RelabelType::New();
relabel->SetInput( labelMap );
relabel->SetInPlace( false );

typedef itk::LabelMapToAttributeImageFilter< LabelMapType, RGBImageType > A2IType;
A2IType::Pointer a2i = A2IType::New();
a2i->SetInput( labelMap );



typedef itk::ImageFileWriter< RGBImageType > WriterType;
WriterType::Pointer writer = WriterType::New();
writer->SetInput( a2i->GetOutput() );
writer->SetFileName( m_outputFileName );
writer->Update();
}


