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

#ifndef threeDvtk2Dslice_h
#define threeDvtk2Dslice_h
#include <vtkMetaImageWriter.h>
#include "itkVTKPolyDataWriter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkPasteImageFilter.h"
#include "itkMedianImageFilter.h"
#include "vtkSmartPointer.h"
#include "vtkDataReader.h"
#include "itkVTKImageToImageFilter.h"
#include <vtkPolyDataReader.h>
#include <vtkVersion.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkPointData.h>
#include <vtkTransformPolyDataFilter.h>
#include <string>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
//visualize 
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkTextActor.h>
#include <vtkVectorText.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkRendererCollection.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkSphereSource.h>
#include <vtkRotationFilter.h>



class threeDvtk2Dslice 
{
public:

threeDvtk2Dslice(){};
~threeDvtk2Dslice(){};
double position_angle[3];
double position_point[3];
int gap_slice;
inline void SetGap(int m_gap){gap_slice=m_gap;};
inline int GetGap(){return gap_slice;};
void visualize(vtkSmartPointer<vtkPolyData> polydata);
void slice_conv(const char * m_inputImageFileName, const char * m_outputImageFileName, int sliceNum);

};


class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera 
{
  public:
    double pos[3];
double pos_ang[3];
    static MouseInteractorStyle2* New();
    vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);
    virtual void OnLeftButtonDown()
    {
      int* clickPos = this->GetInteractor()->GetEventPosition();

      // Pick from this location.
      vtkSmartPointer<vtkPropPicker>  picker =vtkSmartPointer<vtkPropPicker>::New();
      picker->Pick(clickPos[0], clickPos[1], clickPos[2], this->GetDefaultRenderer());

       double* m_pos = picker->GetPickPosition();

      std::cout << "Pick position (world coordinates) is: "
                << m_pos[0] << " " << m_pos[1]
                << " " << m_pos[2] << std::endl;

      std::cout << "Picked actor: " << picker->GetActor() << std::endl;

      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
     if (m_pos[0]!=0 && m_pos[1]!=0 && m_pos[2]!=0){ pos[0]=clickPos[0]; pos[1]=clickPos[1]; pos[2]=clickPos[2];
pos_ang[0]=m_pos[0]; pos_ang[1]=m_pos[1]; pos_ang[2]=m_pos[2];}   
	}

  private:


};



#endif
