/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "Change_information.h"


void Change_information::Change_information_run(const char * m_inputImageFileName1, const char * m_inputImageFileName2 )

{
  using PixelType = unsigned char;
constexpr unsigned int Dimension = 2;

  using ImageType = itk::Image< PixelType,  Dimension >;
  using ReaderType = itk::ImageFileReader< ImageType >;
  using WriterType = itk::ImageFileWriter< ImageType >;
  ReaderType::Pointer reader1 = ReaderType::New();
ReaderType::Pointer reader2 = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  reader1->SetFileName( m_inputImageFileName1 );
reader2->SetFileName( m_inputImageFileName2 );
  writer->SetFileName( m_inputImageFileName2 );


  using FilterType = itk::ChangeInformationImageFilter< ImageType >;
  FilterType::Pointer filter = FilterType::New();

//std::cout <<"Reading ... " << '\n';


  reader1->Update();
  reader2->Update();

  ImageType::ConstPointer inputImage1 = reader1->GetOutput();
  ImageType::PointType     origin1    = inputImage1->GetOrigin();
  ImageType::SpacingType   spacing1   = inputImage1->GetSpacing();
  ImageType::DirectionType direction1 = inputImage1->GetDirection();
ImageType::SizeType size1 = reader1->GetOutput()->GetLargestPossibleRegion().GetSize();

  ImageType::ConstPointer inputImage2 = reader2->GetOutput();
  ImageType::PointType     origin2    = inputImage2->GetOrigin();
  ImageType::SpacingType   spacing2   = inputImage2->GetSpacing();
  ImageType::DirectionType direction2 = inputImage2->GetDirection();
ImageType::SizeType size2 = reader2->GetOutput()->GetLargestPossibleRegion().GetSize();
/*
std::cout <<"//////////////////////////////////////////////" <<'\n';
std::cout <<"Input info of two images:  " << '\n';
std::cout <<"//////////////////////////////////////////////" <<'\n';
std::cout <<"Origin of label image:  " <<origin1<< '\n';
std::cout <<"Origin of predict image:  " <<origin2<< '\n';
std::cout <<"Spacing of label image:  " <<spacing1<< '\n';
std::cout <<"Spacing of predict image:  " <<spacing2<< '\n';
std::cout <<"Direction of label image:  " <<direction1<< '\n';
std::cout <<"Direction of predict image:  " <<direction2<< '\n';
std::cout <<"Size of label image:  " <<size1<< '\n';
std::cout <<"Size of predict image:  " <<size2<< '\n';
std::cout <<"//////////////////////////////////////////////" <<'\n';
*/

//std::cout <<"Rescale of images... " << '\n';
    // rescale
 double scale[2];
    scale[0]= (double) spacing1[0]/spacing2[0];
    scale[1] = (double) spacing1[1]/spacing2[1];
    for(unsigned int i=0; i<Dimension; i++)
      {
      //spacing2[i] *= scale[i];
	spacing2[i] = spacing1[i];
      }
//std::cout <<"new spacing: "<<spacing2 << '\n';
    filter->SetOutputSpacing( spacing2 );
    filter->ChangeSpacingOn();
//std::cout <<"Recenter of images... " << '\n';
    //recenter
    ImageType::PointType::VectorType translation;
    for(unsigned int i=0; i<Dimension; i++)
      {
    translation[i] = origin1[i];
	}
    origin2 += translation;

//std::cout <<"new origin: "<<origin2 << '\n';
    filter->SetOutputOrigin( origin2 );
    filter->ChangeOriginOn();

    filter->SetOutputDirection( direction1 );
    filter->ChangeDirectionOn();

//std::cout <<"Writing ... " << '\n';

size2=size1;

filter->SetInput(reader2->GetOutput() );
writer->SetInput( filter->GetOutput() );
writer->Update();
}

void Change_information::Change_information_2D_similarity_transform(const char * m_inputImageFileName1, std::vector<int> origin, double scale, double m_angle )
{

std::cout <<" rotationCenter image: "<< '\n';
  using PixelType = unsigned char;
constexpr unsigned int Dimension = 2;
  using ImageType = itk::Image< PixelType,  Dimension >;
  using ReaderType = itk::ImageFileReader< ImageType >;
  using WriterType = itk::ImageFileWriter< ImageType >;
  ReaderType::Pointer reader1 = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  reader1->SetFileName( m_inputImageFileName1 );
  writer->SetFileName( m_inputImageFileName1 );

  using FilterType2 = itk::ResampleImageFilter<ImageType, ImageType >;
  FilterType2::Pointer filter2 = FilterType2::New();
  using TransformType = itk::Similarity2DTransform< double >;
  TransformType::Pointer transform = TransformType::New();
std::cout <<"///////////////////////////////////////// "<< '\n';
std::cout <<" Align of the counter with characteristics: "<< '\n';
  reader1->Update();
  ImageType::ConstPointer inputImage1 = reader1->GetOutput();
  ImageType::PointType     origin_old    = inputImage1->GetOrigin();
  ImageType::SpacingType   spacing_old   = inputImage1->GetSpacing();
  ImageType::DirectionType direction = inputImage1->GetDirection();
ImageType::SizeType size = reader1->GetOutput()->GetLargestPossibleRegion().GetSize();

std::cout <<" origin initial image: "<< origin_old << '\n';
std::cout <<" space of image: "<< spacing_old << '\n';
std::cout <<" size image: "<< size << '\n';
std::cout <<" new center image: [" <<origin[0]<<", "<<origin[1]<<"]" << '\n';
std::cout <<" rotated angle image: " <<m_angle << '\n';
std::cout <<" scale image: " <<scale << '\n';


  TransformType::InputPointType rotationCenter;

std::cout <<" space of image: "<< spacing_old << '\n';
  rotationCenter[0] = origin_old[0] + spacing_old[0]*origin[0]; //origin_old[0]+ spacing_old[0] * size[0] / 2.0;
  rotationCenter[1] = origin_old[1] + spacing_old[1]*origin[1]; //origin_old[1]+ spacing_old[0] * size[0] / 2.0;
  std::cout <<" rotationCenter image:  [" << rotationCenter<<"]"<< '\n';
  transform->SetCenter( rotationCenter );
  const double angleInDegrees = m_angle ;
  const double degreesToRadians = std::atan(1.0) / 45.0;
  const double angleInRadians = angleInDegrees * degreesToRadians;
  transform->SetAngle( angleInRadians );
 // transform->SetScale(scale);

TransformType::OutputVectorType translation2;

translation2[0]=rotationCenter[0]-(spacing_old[0] * size[0]*0.5);//(-origin[0]+size[0]*0.5);
translation2[1]=rotationCenter[1]-(spacing_old[1] * size[1]*0.5);//(-origin[1]+size[1]*0.5);



if (rotationCenter[0]<(spacing_old[0] * size[0]*0.5)){
	if (rotationCenter[1]<(spacing_old[1] * size[1]*0.5)){		translation2[0]=std::abs(translation2[0]);		translation2[1]=std::abs(translation2[1]);}
	if (rotationCenter[1]>(spacing_old[1] * size[1]*0.5)){		translation2[0]=std::abs(translation2[0]);		translation2[1]=-std::abs(translation2[1]);}
}

if (rotationCenter[0]>(spacing_old[0] * size[0]*0.5)){
	if (rotationCenter[1]<(spacing_old[1] * size[1]*0.5)){		translation2[0]=-std::abs(translation2[0]);		translation2[1]=std::abs(translation2[1]);}
	if (rotationCenter[1]>(spacing_old[1] * size[1]*0.5)){		translation2[0]=-std::abs(translation2[0]);		translation2[1]=-std::abs(translation2[1]);}
}

if (rotationCenter[0]==(spacing_old[0] * size[0]*0.5)){	
	if (rotationCenter[1]<(spacing_old[1] * size[1]*0.5)){		translation2[0]=0;		translation2[1]=std::abs(translation2[1]);}
	if (rotationCenter[1]>(spacing_old[1] * size[1]*0.5)){		translation2[0]=0;		translation2[1]=-std::abs(translation2[1]);}
}

if (rotationCenter[1]==(spacing_old[1] * size[1]*0.5)){	
	if (rotationCenter[0]<(spacing_old[0] * size[0]*0.5)){		translation2[1]=0;		translation2[0]=std::abs(translation2[0]);}
	if (rotationCenter[0]>(spacing_old[0] * size[0]*0.5)){		translation2[1]=0;		translation2[0]=-std::abs(translation2[0]);}
}

	
	translation2[1]=-translation2[1];
	translation2[0]=-translation2[0];
//if (rotationCenter[1]<std::abs(translation2[1])){translation2[1]=std::abs(translation2[1]);}
transform->Translate (translation2);
std::cout <<"translate image:  " <<translation2<< '\n';
//std::cout <<"origin image:  [" <<origin[0]<<", "<<origin[1]<<"]"<< '\n';
std::cout <<"///////////////////////////////////////// "<< '\n';

  filter2->SetOutputOrigin( origin_old );
//spacing_old[0]=1.0/spacing_old[0];
//spacing_old[1]=1.0/spacing_old[1];
  filter2->SetOutputSpacing( spacing_old );

  filter2->SetOutputDirection( direction );
  filter2->SetSize( size );
  filter2->SetTransform( transform );
//std::cout <<"Rotation angle: "<<angleInDegrees << '\n';

 
//std::cout <<"Writing and the rotation... " << '\n';

filter2->SetInput( reader1->GetOutput()  );
filter2->Update();
writer->SetInput( filter2->GetOutput() );
writer->Update();




}
