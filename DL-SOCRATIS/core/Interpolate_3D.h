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
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#ifndef Interpolate_3D_h
#define Interpolate_3D_h

//C++
#include <memory>
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <algorithm>  
#include "configuration.h"
#include <dirent.h>
#include <array>
#include <iomanip>
#include <vector>

//auto_segm
#include "threeDvtk2Dslice.h"
#include "TestPipelineConfig.h"
#include "itkImageFileReader.h"  

//ITK
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include  <itkJoinSeriesImageFilter.h>
#include  <itkCastImageFilter.h>
#include "itkJoinImageFilter.h"

#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"

#include <itkImageToImageFilter.h>
#include <itkImage.h>

#include <itkImageSliceConstIteratorWithIndex.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

#include <itkSliceBySliceImageFilter.h>
#include <itkSignedDanielssonDistanceMapImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkInterpolateImageFunction.h>

//VTK
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkImageAccumulate.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkMaskFields.h>
#include <vtkThreshold.h>
#include <vtkGeometryFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
#include <sstream>
#include <vtkImageCast.h>

//VTK anatomical vision
#include <vtkAnnotatedCubeActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkNamedColors.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVectorText.h>
#include <vtkXMLPolyDataReader.h>
//vtk mesh
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkDelaunay3D.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkCellArray.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkParametricFunctionSource.h>
#include <vtkXMLDataSetWriter.h>
#include <vtkCleanPolyData.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkDelaunay3D.h>
#include <vtkSmartPointer.h>

#include <vtkCellArray.h>
#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkMath.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCleanPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCellIterator.h>
#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkNamedColors.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkActor2D.h>
#include <vtkAppendPolyData.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <sstream>
#include <vtkDecimatePro.h>
#include <vtkXMLUnstructuredGridReader.h>

namespace
{
double center[3]={0.0, 0.0, 0.0};
/**
 * @param scale Sets the scale and direction of the axes.
 * @param xyzLabels Labels for the axes.
 * @return The axes actor.
 */
vtkSmartPointer<vtkAxesActor> MakeAxesActor(
  std::array<double, 3>& scale, std::array<std::string, 3>& xyzLabels);

/**
 * @param colors Used to set the colors of the cube faces.
 * @return The annotated cube actor.
 */
vtkSmartPointer<vtkAnnotatedCubeActor> MakeAnnotatedCubeActor(
  vtkNamedColors* colors);

/**
 * @param scale Sets the scale and direction of the axes.
 * @param xyzLabels Labels for the axes.
 * @param colors Used to set the colors of the cube faces.
 * @return The combined axes and annotated cube prop.
 */
vtkSmartPointer<vtkPropAssembly> MakeCubeActor(
  std::array<double, 3>& scale, std::array<std::string, 3>& xyzLabels,
  vtkNamedColors* colors);

/**
 * Make the traverse, coronal and saggital planes.
 *
 * @param colors Used to set the color of the planes.
 * @return The planes actors.
 */
std::vector<vtkSmartPointer<vtkActor>> MakePlanesActors(vtkNamedColors* colors);

/**
* Generate text to place on the planes.
* Careful placement is needed here.
* @return The text actors.
*/
std::vector<vtkSmartPointer<vtkActor>> AddTextToPlanes();
}

namespace
{

vtkSmartPointer<vtkAxesActor> MakeAxesActor(
  std::array<double, 3>& scale, std::array<std::string, 3>& xyzLabels)
{
  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  axes->SetScale(scale[0], scale[1], scale[2]);
  axes->SetShaftTypeToCylinder();
  axes->SetXAxisLabelText(xyzLabels[0].c_str());
  axes->SetYAxisLabelText(xyzLabels[1].c_str());
  axes->SetZAxisLabelText(xyzLabels[2].c_str());
  axes->SetCylinderRadius(0.5 * axes->GetCylinderRadius());
  axes->SetConeRadius(1.025 * axes->GetConeRadius());
  axes->SetSphereRadius(1.5 * axes->GetSphereRadius());
  vtkTextProperty* tprop =
    axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty();
  tprop->ItalicOn();
  tprop->ShadowOn();
  tprop->SetFontFamilyToTimes();
  // Use the same text properties on the other two axes.
  axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(tprop);
  axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(tprop);
  return axes;
}

vtkSmartPointer<vtkAnnotatedCubeActor> MakeAnnotatedCubeActor(
  vtkNamedColors* colors)
{
  // A cube with labeled faces.
  vtkSmartPointer<vtkAnnotatedCubeActor> cube =
    vtkSmartPointer<vtkAnnotatedCubeActor>::New();
  cube->SetXPlusFaceText("R"); // Right
  cube->SetXMinusFaceText("L"); // Left
  cube->SetYPlusFaceText("A"); // Anterior
  cube->SetYMinusFaceText("P"); // Posterior
  cube->SetZPlusFaceText("S"); // Superior/Cranial
  cube->SetZMinusFaceText("I"); // Inferior/Caudal
  cube->SetFaceTextScale(0.5);
  cube->GetCubeProperty()->SetColor(colors->GetColor3d("Gainsboro").GetData());

  cube->GetTextEdgesProperty()->SetColor(
    colors->GetColor3d("LightSlateGray").GetData());

  // Change the vector text colors.
  cube->GetXPlusFaceProperty()->SetColor(
    colors->GetColor3d("Tomato").GetData());
  cube->GetXMinusFaceProperty()->SetColor(
    colors->GetColor3d("Tomato").GetData());
  cube->GetYPlusFaceProperty()->SetColor(
    colors->GetColor3d("DeepSkyBlue").GetData());
  cube->GetYMinusFaceProperty()->SetColor(
    colors->GetColor3d("DeepSkyBlue").GetData());
  cube->GetZPlusFaceProperty()->SetColor(
    colors->GetColor3d("SeaGreen").GetData());
  cube->GetZMinusFaceProperty()->SetColor(
    colors->GetColor3d("SeaGreen").GetData());
  return cube;
}

vtkSmartPointer<vtkPropAssembly> MakeCubeActor(
  std::array<double, 3>& scale, std::array<std::string, 3>& xyzLabels,
  vtkNamedColors* colors)
{
  // We are combining a vtk.vtkAxesActor and a vtk.vtkAnnotatedCubeActor
  // into a vtk.vtkPropAssembly
  vtkSmartPointer<vtkAnnotatedCubeActor> cube = MakeAnnotatedCubeActor(colors);
  vtkSmartPointer<vtkAxesActor> axes = MakeAxesActor(scale, xyzLabels);

  // Combine orientation markers into one with an assembly.
  vtkSmartPointer<vtkPropAssembly> assembly =
    vtkSmartPointer<vtkPropAssembly>::New();
  assembly->AddPart(axes);
  assembly->AddPart(cube);
  return assembly;
}

vtkSmartPointer<vtkTransformPolyDataFilter> MakePlane(
  std::array<int, 2>& resolution, std::array<double, 3>& origin,
  std::array<double, 3>& point1, std::array<double, 3>& point2,
  std::array<double, 4>& wxyz, std::array<double, 3>& translate)
{
  vtkSmartPointer<vtkPlaneSource> plane =
    vtkSmartPointer<vtkPlaneSource>::New();
  plane->SetResolution(resolution[0], resolution[1]);
  plane->SetOrigin(origin.data());
  plane->SetPoint1(point1.data());
  plane->SetPoint2(point2.data());
  vtkSmartPointer<vtkTransform> trnf = vtkSmartPointer<vtkTransform>::New();
  trnf->RotateWXYZ(wxyz[0], wxyz[1], wxyz[2], wxyz[3]);
  trnf->Translate(translate.data());
  vtkSmartPointer<vtkTransformPolyDataFilter> tpdPlane =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  tpdPlane->SetTransform(trnf);
  tpdPlane->SetInputConnection(plane->GetOutputPort());
  return tpdPlane;
}

std::vector<vtkSmartPointer<vtkActor>> MakePlanesActors(vtkNamedColors* colors)
{
  std::vector<vtkSmartPointer<vtkTransformPolyDataFilter>> planes;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> mappers;
  std::vector<vtkSmartPointer<vtkActor>> actors;

  // Parameters for a plane lying in the x-y plane.
  std::array<int, 2> resolution{{10, 10}};
  std::array<double, 3> origin{{center[0], center[1], center[2]}};
  std::array<double, 3> point1{{1, 0, 0}};
  std::array<double, 3> point2{{0, 1, 0}};

  std::array<double, 4> wxyz0{{0, 0, 0, 0}};
  std::array<double, 3> translate{{-0.5, -0.5, 0}};
  std::array<double, 4> wxyz1{{-90, 1, 0, 0}};
  std::array<double, 4> wxyz2{{-90, 0, 1, 0}};
  planes.push_back(MakePlane(resolution, origin, point1, point2, wxyz0,
                             translate)); // x-y plane
  planes.push_back(MakePlane(resolution, origin, point1, point2, wxyz1,
                             translate)); // x-z plane
  planes.push_back(MakePlane(resolution, origin, point1, point2, wxyz2,
                             translate)); // y-z plane
  for (size_t i = 0; i < planes.size(); ++i)
  {
    mappers.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
    mappers[i]->SetInputConnection(planes[i]->GetOutputPort());
    actors.push_back(vtkSmartPointer<vtkActor>::New());
    actors[i]->SetMapper(mappers[i]);
  }
  actors[0]->GetProperty()->SetColor(
    colors->GetColor3d("SeaGreen").GetData()); // Transverse plane
  actors[1]->GetProperty()->SetColor(
    colors->GetColor3d("DeepSkyBlue").GetData()); // Coronal plane
  actors[2]->GetProperty()->SetColor(
    colors->GetColor3d("Tomato").GetData()); // Saggital plane
  return actors;
}

std::vector<vtkSmartPointer<vtkActor>> AddTextToPlanes()
{
  std::vector<vtkSmartPointer<vtkActor>> textActors;
  std::array<double, 3> scale{{0.04, 0.04, 0.04}};

  vtkSmartPointer<vtkVectorText> text1 = vtkSmartPointer<vtkVectorText>::New();
  text1->SetText("Transverse\nPlane\n\nSuperior\nCranial");
  vtkSmartPointer<vtkTransform> trnf1 = vtkSmartPointer<vtkTransform>::New();
  trnf1->RotateZ(-90);
  vtkSmartPointer<vtkTransformPolyDataFilter> tpdPlane1 =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  tpdPlane1->SetTransform(trnf1);
  tpdPlane1->SetInputConnection(text1->GetOutputPort());
  vtkSmartPointer<vtkPolyDataMapper> textMapper1 =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  textMapper1->SetInputConnection(tpdPlane1->GetOutputPort());
  vtkSmartPointer<vtkActor> textActor1 = vtkSmartPointer<vtkActor>::New();
  textActor1->SetMapper(textMapper1);
  textActor1->SetScale(scale.data());
  textActor1->AddPosition(0.4, 0.49, 0.01);
  textActors.push_back(textActor1);

  vtkSmartPointer<vtkVectorText> text2 = vtkSmartPointer<vtkVectorText>::New();
  text2->SetText("Transverse\nPlane\n\nInferior\n(Caudal)");
  vtkSmartPointer<vtkTransform> trnf2 = vtkSmartPointer<vtkTransform>::New();
  trnf2->RotateZ(270);
  trnf2->RotateWXYZ(180, 0, 1, 0);
  vtkSmartPointer<vtkTransformPolyDataFilter> tpdPlane2 =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  tpdPlane2->SetTransform(trnf2);
  tpdPlane2->SetInputConnection(text2->GetOutputPort());
  vtkSmartPointer<vtkPolyDataMapper> textMapper2 =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  textMapper2->SetInputConnection(tpdPlane2->GetOutputPort());
  vtkSmartPointer<vtkActor> textActor2 = vtkSmartPointer<vtkActor>::New();
  textActor2->SetMapper(textMapper2);
  textActor2->SetScale(scale.data());
  textActor2->AddPosition(0.4, -0.49, -0.01);
  textActors.push_back(textActor2);

  vtkSmartPointer<vtkVectorText> text3 = vtkSmartPointer<vtkVectorText>::New();
  text3->SetText("Sagittal\nPlane\n\nLeft");
  vtkSmartPointer<vtkTransform> trnf3 = vtkSmartPointer<vtkTransform>::New();
  trnf3->RotateX(90);
  trnf3->RotateWXYZ(-90, 0, 1, 0);
  vtkSmartPointer<vtkTransformPolyDataFilter> tpdPlane3 =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  tpdPlane3->SetTransform(trnf3);
  tpdPlane3->SetInputConnection(text3->GetOutputPort());
  vtkSmartPointer<vtkPolyDataMapper> textMapper3 =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  textMapper3->SetInputConnection(tpdPlane3->GetOutputPort());
  vtkSmartPointer<vtkActor> textActor3 = vtkSmartPointer<vtkActor>::New();
  textActor3->SetMapper(textMapper3);
  textActor3->SetScale(scale.data());
  textActor3->AddPosition(-0.01, 0.49, 0.4);
  textActors.push_back(textActor3);

  vtkSmartPointer<vtkVectorText> text4 = vtkSmartPointer<vtkVectorText>::New();
  text4->SetText("Sagittal\nPlane\n\nRight");
  vtkSmartPointer<vtkTransform> trnf4 = vtkSmartPointer<vtkTransform>::New();
  trnf4->RotateX(90);
  trnf4->RotateWXYZ(-270, 0, 1, 0);
  vtkSmartPointer<vtkTransformPolyDataFilter> tpdPlane4 =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  tpdPlane4->SetTransform(trnf4);
  tpdPlane4->SetInputConnection(text4->GetOutputPort());
  vtkSmartPointer<vtkPolyDataMapper> textMapper4 =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  textMapper4->SetInputConnection(tpdPlane4->GetOutputPort());
  vtkSmartPointer<vtkActor> textActor4 = vtkSmartPointer<vtkActor>::New();
  textActor4->SetMapper(textMapper4);
  textActor4->SetScale(scale.data());
  textActor4->AddPosition(0.01, -0.49, 0.4);
  textActors.push_back(textActor4);

  vtkSmartPointer<vtkVectorText> text5 = vtkSmartPointer<vtkVectorText>::New();
  text5->SetText("Coronal\nPlane\n\nAnterior");
  vtkSmartPointer<vtkTransform> trnf5 = vtkSmartPointer<vtkTransform>::New();
  trnf5->RotateY(-180);
  trnf5->RotateWXYZ(-90, 1, 0, 0);
  vtkSmartPointer<vtkTransformPolyDataFilter> tpdPlane5 =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  tpdPlane5->SetTransform(trnf5);
  tpdPlane5->SetInputConnection(text5->GetOutputPort());
  vtkSmartPointer<vtkPolyDataMapper> textMapper5 =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  textMapper5->SetInputConnection(tpdPlane5->GetOutputPort());
  vtkSmartPointer<vtkActor> textActor5 = vtkSmartPointer<vtkActor>::New();
  textActor5->SetMapper(textMapper5);
  textActor5->SetScale(scale.data());
  textActor5->AddPosition(0.49, 0.01, 0.20);
  textActors.push_back(textActor5);

  vtkSmartPointer<vtkVectorText> text6 = vtkSmartPointer<vtkVectorText>::New();
  text6->SetText("Coronal\nPlane\n\nPosterior");
  vtkSmartPointer<vtkTransform> trnf6 = vtkSmartPointer<vtkTransform>::New();
  trnf6->RotateWXYZ(90, 1, 0, 0);
  vtkSmartPointer<vtkTransformPolyDataFilter> tpdPlane6 =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  tpdPlane6->SetTransform(trnf6);
  tpdPlane6->SetInputConnection(text6->GetOutputPort());
  vtkSmartPointer<vtkPolyDataMapper> textMapper6 =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  textMapper6->SetInputConnection(tpdPlane6->GetOutputPort());
  vtkSmartPointer<vtkActor> textActor6 = vtkSmartPointer<vtkActor>::New();
  textActor6->SetMapper(textMapper6);
  textActor6->SetScale(scale.data());
  textActor6->AddPosition(-0.49, -0.01, 0.3);
  textActors.push_back(textActor6);

  return textActors;
}
}


class Interpolate_3D
{
public:
//functions
Interpolate_3D(){};
~Interpolate_3D(){};
void copy_to_directory(); //TODO
void delete_images_localy(); //TODO
void threeD_to_2D_slicer(const char * inputFileName, const char *outputFileName, int slices_numb);
void twoD_to_3D_interpolater(string predict_2D_path, string store3D_path, int inb_images_patient, const char * outputname, bool vision);
void create_anatomical_map(std::string vtpdatafile );
//libraries itk
using PixelType = unsigned char;
using ImageType = itk::Image< PixelType, 2 >;
using ReaderType = itk::ImageFileReader< ImageType >;
int gap=5;
inline int SetGap(int m_gap){gap=m_gap;}
bool vtk_vision=false;
int interpolation_type=1;
double center[3]={255.0, 255.0, 255.0};
inline void Set_input_image(const char *outputFilename){m_outputFilename=outputFilename;};
void create_LV(std::string myo, std::string scar, std::string store);
inline const char  *Get_input_image(){return m_outputFilename;};
const char *m_outputFilename;
void Generate3D_interpolation(); 
void Generate3D_mesh(const char * metadatafilevtp, string name );
///definition variables
 typedef unsigned char InputPixelType;
  typedef double IntermediatePixelType;
    
  typedef itk::Image< InputPixelType, 3 >        InputImageType;
  typedef itk::Image< InputPixelType, 3 >        IntermediateImageType;
  typedef itk::Image< IntermediatePixelType, 3 > IntermediateImageType2;
  typedef itk::Image< InputPixelType,  2 >       InternalInImageType;
  typedef itk::Image< IntermediatePixelType, 2 > InternalOutImageType;
  typedef itk::Image< InputPixelType, 3 >        OutputImageType;
  typedef itk::InterpolateImageFunction< IntermediateImageType2, double>     InterpolatorType;
  /** Typedefs to access images. */
  typedef itk::ImageSliceConstIteratorWithIndex< InputImageType > ConstSliceItType;
  typedef itk::ImageRegionConstIterator< InputImageType >         ConstRegionItType;
  typedef itk::ImageRegionIterator< InputImageType >              RegionItType;
  typedef itk::ImageRegionIterator< IntermediateImageType2 >      IntermediateRegionItType;
  typedef itk::ImageRegionIterator< OutputImageType >             OutputRegionItType;

  typedef InputImageType::ConstPointer      InputImageConstPointer; 
  typedef InputImageType::RegionType        InputRegionType; 
  typedef InputImageType::SizeType          InputSizeType; 
  typedef InputImageType::SpacingType       InputSpacingType; 
  typedef InputImageType::IndexType         InputIndexType;
  typedef InputImageType::PointType         InputPointType;

  typedef IntermediateImageType::Pointer    IntermediateImagePointer;
  typedef IntermediateImageType::RegionType IntermediateRegionType;
  typedef IntermediateImageType::SizeType   IntermediateSizeType; 
  typedef IntermediateImageType::IndexType  IntermediateIndexType;
  typedef IntermediateImageType::PointType  IntermediatePointType;
   
  typedef OutputImageType::Pointer          OutputImagePointer;
  typedef OutputImageType::RegionType       OutputRegionType;


//private:
  unsigned int             m_DelineationRatio;    
  std::vector< int >        m_DelineationZCoordinateArray;
  IntermediateImagePointer   m_IntermediateImage= IntermediateImageType::New();


typedef itk::SliceBySliceImageFilter< InputImageType, IntermediateImageType2 >                    SliceBySliceFilterType;
  typedef itk::SignedDanielssonDistanceMapImageFilter< InternalInImageType, InternalOutImageType >  DistanceMapImageFilterType;   
  typedef itk::ResampleImageFilter< IntermediateImageType2, IntermediateImageType2 >                ResampleFilterType;
  typedef itk::IdentityTransform< double, 3 >                                                       TransformType;    

  SliceBySliceFilterType::Pointer     m_SliceBySliceFilter= SliceBySliceFilterType::New();
  DistanceMapImageFilterType::Pointer m_DistanceMapImageFilter= DistanceMapImageFilterType::New();
  ResampleFilterType::Pointer         m_ResampleFilter= ResampleFilterType::New();
  TransformType::Pointer              m_Transform= TransformType::New();

double alpha=3;
inline void Set_alpha(double a){alpha=a;};

};



#endif
