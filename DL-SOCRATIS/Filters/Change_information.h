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
//  Software Guide : BeginLatex
//
//  This example illustrates the common task of extracting a 2D slice from a
//  3D volume. Perform some processing on that slice and then paste it on an
//  output volume of the same size as the volume from the input.

#ifndef Change_information_h
#define Change_information_h

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVersor.h"
#include "itkChangeInformationImageFilter.h"
#include "itkSimilarity2DTransform.h"
#include "itkResampleImageFilter.h"
#include "itkTranslationTransform.h"
#include <vector>

class Change_information
{
public:

Change_information(){};
~Change_information(){};
bool rotate=true;
void Set_turn(bool m_turn){turn=m_turn;}
void Change_information_run(const char * m_inputImageFileName1, const char * m_inputImageFileName2 );
void Change_information_2D_similarity_transform(const char * m_inputImageFileName1, std::vector<int> origin, double scale, double m_angle );

private:
bool turn=false;
};
#endif
