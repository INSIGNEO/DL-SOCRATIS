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

#ifndef evaluate_h
#define evaluate_h

#include <itkLabelOverlapMeasuresImageFilter.h>
#include "itkCSVArray2DDataObject.h"
#include "itkCSVArray2DFileReader.h"
#include "itkCSVNumericObjectFileWriter.h"
#include "itkImage.h"
#include "itkLabelOverlapMeasuresImageFilter.h"
#include "read_writedata.h"
#include "Resample.h"
#include "Change_information.h"
#include "boundarysegm.h"

class evaluate 
{
public:
double m_angle;
std::vector<int> center;
double scale=1;
double dice;
double total;
double jaccard;
evaluate(){};
~evaluate(){};
void evaluate_results(const char * m_predictFileName, const char * m_labelFileName, std::string metric_store_path);
inline double GetDice(){return dice;};
inline double GetTotal(){return total;};
inline double GetJaccard(){return jaccard;};
inline void SetCenter(std::vector<int> m_center){center.push_back(m_center[0]);center.push_back(m_center[1]);};
};
/*
 class Cleanup_argv
  {
public:
    Cleanup_argv( char* * argv_, int argc_plus_one_ ) : argv( argv_ ), argc_plus_one( argc_plus_one_ )
    {
    }

    ~Cleanup_argv()
    {
      for( unsigned int i = 0; i < argc_plus_one; ++i )
        {
        delete[] argv[i];
        }
      delete[] argv;
    }

private:
    char* *      argv;
    unsigned int argc_plus_one;
  };


*/

#endif
