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

////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.3

//////////////////////////////////////////////////////////////////////////////////
#ifndef Translation_space_h
#define Translation_space_h


#include "Initial_Segmentation.h"


//TODO inheriate with Initial_Segmentation

class Translation_space : public Initial_Segmentation
{
public:

Translation_space(std::string m_root) 
{
	root=m_root; 
};

~Translation_space(){};

		
void Run_trans(std::string python_path, std::string m_init_image_space, std::string m_final_image_space, std::string store_path);



private:




};

#endif
