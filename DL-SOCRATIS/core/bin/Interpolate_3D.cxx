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


#include "Interpolate_3D.h"




void Interpolate_3D::threeD_to_2D_slicer(const char * inputFileName, const char *outputFileName, int slices_numb)
{

{
//measurement evaluate
vector<string> input;
//read all the files in myo and roi directory
struct dirent *entry;
DIR *pDIR;


if( pDIR=opendir(inputFileName) ){
	while(entry = readdir(pDIR)){
		if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
		std::stringstream i;
		i<<inputFileName<<entry->d_name;
                input.push_back(i.str());
		cout << i.str() << "\n";
		}
		}
	closedir(pDIR);
}


std::cout<<" The slice of 3D to the "<<slices_numb<< " files starts now ..."<<std::endl;

	itk::NumericSeriesFileNames::Pointer fileNamesCreator =itk::NumericSeriesFileNames::New();
	fileNamesCreator->SetStartIndex( 0 );
	fileNamesCreator->SetEndIndex( 1 );
	fileNamesCreator->SetSeriesFormat( input[0]);
	const std::vector<std::string> & shapeModeFileNamesm = fileNamesCreator->GetFileNames();
const char * threeDFile;	
    	threeDFile=shapeModeFileNamesm[0].c_str(); 

	
	std::shared_ptr<threeDvtk2Dslice > threed2d =std::make_shared<threeDvtk2Dslice>();
	threed2d->slice_conv(threeDFile, outputFileName,slices_numb);
gap=threed2d->GetGap();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


}



void Interpolate_3D::twoD_to_3D_interpolater(string predict_2D_path, string store3D_path, int inb_images_patient, const char * outputname,bool vision)
{
vtk_vision=vision;
vector<string> names;
//read all the files in myo and roi directory
struct dirent *entry;
DIR *pDIR;


if( pDIR=opendir(predict_2D_path.c_str()) ){
	while(entry = readdir(pDIR)){
		if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ){
		std::stringstream i;
		i<<predict_2D_path<<entry->d_name;
                names.push_back(i.str());
		cout << i.str() << "\n";
		}
		}
	closedir(pDIR);
}
std::sort(names.begin(),names.end());
using PixelType = unsigned char;
constexpr unsigned int Dimension = 3;
using ImageType = itk::Image< PixelType, Dimension >;
using ImageType2d = itk::Image< PixelType, 2> ;

using ReaderType = itk::ImageSeriesReader< ImageType >;
using ReaderType2 = itk::ImageFileReader< ImageType2d>;
using WriterType = itk::ImageFileWriter<   ImageType >;
ReaderType::Pointer reader = ReaderType::New();

WriterType::Pointer writer = WriterType::New();

const unsigned int first =  0 ;
const unsigned int last  = inb_images_patient-1 ;
auto my_cstr = ".mhd";
std::string output(outputname);       
std::string s(my_cstr); 
auto my_cstr2 = "%d.jpg";
std::string s2(my_cstr2); 
string result=store3D_path+output+s; 
string store=predict_2D_path+s2; 
const char * outputFilename = result.c_str();

using NameGeneratorType = itk::NumericSeriesFileNames;
NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();

nameGenerator->SetSeriesFormat( store );
nameGenerator->SetStartIndex( first );
nameGenerator->SetEndIndex( last );
nameGenerator->SetIncrementIndex( 1 );
//reader->SetImageIO( itk::PNGImageIO::New() );
reader->SetFileNames( nameGenerator->GetFileNames()  );


using JoinSeriesImageFilterType = itk::JoinSeriesImageFilter<ImageType2d, ImageType>;
JoinSeriesImageFilterType::Pointer joinFilter = JoinSeriesImageFilterType::New();

std::cout<<" The evaluation of the files start now ..."<<std::endl;
  for (int i=0; i<inb_images_patient; ++i){
	std::cout<<" The evaluation of the files start now ..."<<(double)gap*i<<std::endl;
		cout << nameGenerator->GetFileNames()[i] << "\n";
	ReaderType2::Pointer reader2 = ReaderType2::New();
	
	reader2->SetFileName( nameGenerator->GetFileNames()[i]  );
	reader2->Update();
	joinFilter->SetInput(i,reader2->GetOutput() );
	joinFilter->SetOrigin(0);
	joinFilter->SetSpacing(gap);
	joinFilter->Update();

	
}


writer->SetFileName( outputFilename ); 
writer->SetInput( joinFilter->GetOutput() );
writer->Update();
std::cout<<" The evaluation of the files start now ..."<<std::endl;
//interpolate the 3d image http://insight-journal.org/browse/publication/881

if(outputname=="3dScars"){
Set_input_image(outputFilename);
m_DelineationRatio= inb_images_patient ;
Generate3D_interpolation();
}


//VTK process
// Create all of the classes we will need
vtkSmartPointer<vtkMetaImageReader> readervtk =vtkSmartPointer<vtkMetaImageReader>::New();
vtkSmartPointer<vtkImageAccumulate> histogram =vtkSmartPointer<vtkImageAccumulate>::New();
vtkSmartPointer<vtkDiscreteMarchingCubes> discreteCubes =vtkSmartPointer<vtkDiscreteMarchingCubes>::New();
vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother =vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
vtkSmartPointer<vtkThreshold> selector =vtkSmartPointer<vtkThreshold>::New();
vtkSmartPointer<vtkMaskFields> scalarsOff =vtkSmartPointer<vtkMaskFields>::New();
vtkSmartPointer<vtkGeometryFilter> geometry =vtkSmartPointer<vtkGeometryFilter>::New();
vtkSmartPointer<vtkXMLPolyDataWriter> writervtk =vtkSmartPointer<vtkXMLPolyDataWriter>::New();
// Define all of the variables
unsigned int startLabel = first;
unsigned int endLabel = last;
std::string filePrefix = "Label";
unsigned int smoothingIterations = 15;
double passBand = 0.0001;
double featureAngle = 120.0;

// Generate models from labels
readervtk->SetFileName(result.c_str());
histogram->SetInputConnection(readervtk->GetOutputPort());
histogram->SetComponentExtent(0, endLabel, 0, 0, 0, 0);
histogram->SetComponentOrigin(0, 0, 0);
histogram->SetComponentSpacing(1, 1, 1);
histogram->Update();

discreteCubes->SetInputConnection(readervtk->GetOutputPort());
discreteCubes->GenerateValues(endLabel - startLabel + 1, startLabel, endLabel);

smoother->SetInputConnection(discreteCubes->GetOutputPort());
smoother->SetNumberOfIterations(smoothingIterations);
smoother->BoundarySmoothingOn();
smoother->FeatureEdgeSmoothingOn();
smoother->SetFeatureAngle(featureAngle);
smoother->SetPassBand(passBand);
smoother->NonManifoldSmoothingOn();
smoother->NormalizeCoordinatesOn();
smoother->Update();

selector->SetInputConnection(smoother->GetOutputPort());
selector->SetInputArrayToProcess(0, 0, 0,vtkDataObject::FIELD_ASSOCIATION_CELLS, vtkDataSetAttributes::SCALARS);

// Strip the scalars from the output
scalarsOff->SetInputConnection(selector->GetOutputPort());
scalarsOff->CopyAttributeOff(vtkMaskFields::POINT_DATA, vtkDataSetAttributes::SCALARS);
scalarsOff->CopyAttributeOff(vtkMaskFields::CELL_DATA, vtkDataSetAttributes::SCALARS);
geometry->SetInputConnection(scalarsOff->GetOutputPort());

vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
surfaceFilter->SetInputConnection(geometry->GetOutputPort());
surfaceFilter->Update();

writervtk->SetInputConnection(surfaceFilter->GetOutputPort());
auto my_cstr3 = ".vtp";        
std::string s3(my_cstr3); 
string store_vtk=store3D_path+output+s3; 
writervtk->SetFileName(store_vtk.c_str());
writervtk->Write();

vtkSmartPointer<vtkXMLPolyDataReader> fran =  vtkSmartPointer<vtkXMLPolyDataReader>::New();
fran->SetFileName(store_vtk.c_str());
vtkSmartPointer<vtkDecimatePro> deci =vtkSmartPointer<vtkDecimatePro>::New();
deci->SetInputConnection(fran->GetOutputPort());
deci->SetTargetReduction(0.9);
deci->PreserveTopologyOn();

vtkSmartPointer<vtkSmoothPolyDataFilter> smoother_vtu =vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
smoother_vtu->SetInputConnection(deci->GetOutputPort());
smoother_vtu->BoundarySmoothingOn();
smoother_vtu->FeatureEdgeSmoothingOn(); 
smoother_vtu->SetNumberOfIterations(40);
smoother_vtu->SetRelaxationFactor(0.001);
vtkSmartPointer<vtkXMLPolyDataWriter> ugWritersmooth =  vtkSmartPointer<vtkXMLPolyDataWriter>::New();
 
ugWritersmooth->SetInputConnection(smoother_vtu->GetOutputPort());
auto my_cstrsmooth = "smoothed.vtp";        
std::string smoothvtu(my_cstrsmooth); 
ugWritersmooth->SetFileName((store3D_path+output+smoothvtu).c_str());
ugWritersmooth->Write();

Generate3D_mesh((store3D_path+output+smoothvtu).c_str(),store3D_path+output );
}






void Interpolate_3D::Generate3D_interpolation()
{
std::cout<<" Generate 3D interpolation between the slices ..."<<std::endl;
using ReaderType1 = itk::ImageFileReader< InputImageType >;
ReaderType1::Pointer reader1 = ReaderType1::New();
reader1->SetFileName( Get_input_image()  );
reader1->Update();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef InputImageType::ConstPointer      InputImageConstPointer; 
InputImageConstPointer inputImage = reader1->GetOutput() ;
InputRegionType inputRegion = inputImage->GetLargestPossibleRegion();
    
ConstSliceItType sliceIt( inputImage, inputRegion );
sliceIt.SetFirstDirection(0);
sliceIt.SetSecondDirection(1);
sliceIt.GoToBegin(); 
    
InputIndexType inputStart;   
inputStart = sliceIt.GetIndex();    
InputPixelType pixel;
InputPixelType pixelSum;
  

while( !sliceIt.IsAtEnd() )
    {
    pixelSum = 0;
    while( !sliceIt.IsAtEndOfSlice() )
      {
      while( !sliceIt.IsAtEndOfLine() )
        {
        pixel = sliceIt.Get();
        pixelSum += pixel;
        ++sliceIt;
        }
      sliceIt.NextLine();
      }
    if ( pixelSum > 0 )   
      {
      m_DelineationZCoordinateArray.push_back( sliceIt.GetIndex()[2] );
      }   
    sliceIt.NextSlice();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
    
//Define the spacing of the intermediate image 
const InputImageType::SpacingType& inputSpacing = inputImage->GetSpacing();
IntermediateImageType::SpacingType intermediateSpacing;
intermediateSpacing[0] = inputSpacing[0];
intermediateSpacing[1] = inputSpacing[1];
intermediateSpacing[2] = m_DelineationRatio * inputSpacing[2];
    
//Define the starting index of the intermediate regions 
IntermediateIndexType intermediateStart;
IntermediateIndexType physicalIntermediateStart;
intermediateStart.Fill(0);
    
physicalIntermediateStart[0] = 0;
physicalIntermediateStart[1] = 0;
physicalIntermediateStart[2] = m_DelineationZCoordinateArray.front();
    
//Define the size of the intermediate regions 
InputRegionType inputRegion2 = inputImage->GetLargestPossibleRegion();
InputSizeType inputSize = inputRegion2.GetSize();
IntermediateSizeType intermediateSize;
intermediateSize[0] = inputSize[0];
intermediateSize[1] = inputSize[1];
intermediateSize[2] = m_DelineationZCoordinateArray.size();
  
IntermediateRegionType intermediateRegion( intermediateStart, intermediateSize );
    
//Define the origin of the intermediate image 
const InputPointType& inputOrigin = inputImage->GetOrigin();
IntermediatePointType intermediateOrigin;
    
for(unsigned int i=0; i< 3; i++)
{
    intermediateOrigin[i] = inputOrigin[i] + inputSpacing[i] * physicalIntermediateStart[i];
}
    
m_IntermediateImage->SetRegions( intermediateRegion );
m_IntermediateImage->SetSpacing( intermediateSpacing );
m_IntermediateImage->SetOrigin( intermediateOrigin );
m_IntermediateImage->Allocate();  
    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
InputSizeType inputSizeout = inputRegion2.GetSize();
const InputSpacingType& inputSpacingout = inputImage->GetSpacing();
const InputPointType& inputOriginout = inputImage->GetOrigin();
ConstRegionItType inputIt( inputImage, inputRegion2 );
    
IntermediateRegionType intermediateRegion2 = m_IntermediateImage->GetLargestPossibleRegion();
RegionItType intermediateIt( m_IntermediateImage, intermediateRegion2 );
     
OutputImagePointer outputImage = reader1->GetOutput() ; 
    
    
// Copy the slices that are delineated into the m_IntermediateImage 
for( inputIt.GoToBegin(), intermediateIt.GoToBegin(); !inputIt.IsAtEnd(); ++inputIt )
    {
    if( std::find(m_DelineationZCoordinateArray.begin(), m_DelineationZCoordinateArray.end(), inputIt.GetIndex()[2]) != m_DelineationZCoordinateArray.end() )
      {
      intermediateIt.Set( inputIt.Get() );
      ++intermediateIt;
      }
}
    
using InterpolatorType = itk::NearestNeighborInterpolateImageFunction <IntermediateImageType2, double >;
using InterpolatorType2 = itk::BSplineInterpolateImageFunction <IntermediateImageType2, double >;
InterpolatorType2::Pointer interpolator = InterpolatorType2::New();
if (interpolation_type==0){
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
}
if (interpolation_type==1){
   InterpolatorType2::Pointer interpolator = InterpolatorType2::New();
}
  // Resample the m_IntermediateImage   
m_SliceBySliceFilter->SetFilter( m_DistanceMapImageFilter );
m_SliceBySliceFilter->SetInput( m_IntermediateImage );    
m_ResampleFilter->SetInput( m_SliceBySliceFilter->GetOutput() );
m_ResampleFilter->SetTransform( m_Transform );
m_ResampleFilter->SetInterpolator( interpolator );
m_ResampleFilter->SetSize( inputSizeout ); 
m_ResampleFilter->SetOutputSpacing( inputSpacingout ); 
m_ResampleFilter->SetOutputOrigin( inputOriginout );   
m_ResampleFilter->Update();
    
IntermediateRegionItType itr( m_ResampleFilter->GetOutput(), m_ResampleFilter->GetOutput()->GetLargestPossibleRegion() );
/*
OutputRegionType outputRegion = m_ResampleFilter->GetOutput()->GetLargestPossibleRegion();
outputImage->SetRegions( outputRegion );
outputImage->CopyInformation( m_ResampleFilter->GetOutput() );
outputImage->Allocate();
OutputRegionItType outItr( outputImage, outputRegion );
// Threshold the output image to get a binary interpolated image
for( itr.GoToBegin(), outItr.GoToBegin(); !itr.IsAtEnd(); ++itr, ++outItr )
    {
    if( itr.Get() > 0 ) 
      {
      if( floor(10*itr.Get())/10 > 0)
        {
        outItr.Set(0);
        }
      else
        {
        outItr.Set(1);
        }      }
    else
      {
      outItr.Set(1);
      }
}
*/
}





void Interpolate_3D::Generate3D_mesh(const char * metadatafilevtp, string name)
{
vtkSmartPointer<vtkXMLPolyDataReader> reader =  vtkSmartPointer<vtkXMLPolyDataReader>::New();
reader->SetFileName(metadatafilevtp);
reader->Update();
//vtkSmartPointer<vtkCleanPolyData> cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
 //cleaner->SetInputConnection (reader->GetOutputPort());
// Create the convex hull of the pointcloud
vtkSmartPointer<vtkDelaunay3D> delaunay = vtkSmartPointer< vtkDelaunay3D >::New();
delaunay->SetInputData(reader->GetOutput());
delaunay->SetAlpha(alpha);//0
delaunay->SetTolerance (0.0001);//0.001
delaunay->Update();
vtkSmartPointer<vtkNamedColors> color =
vtkSmartPointer<vtkNamedColors>::New();
vtkSmartPointer<vtkUnsignedCharArray> cellData =
vtkSmartPointer<vtkUnsignedCharArray>::New();
cellData->SetNumberOfComponents(3);

int numTetras = 0;
int numLines = 0;
int numTris = 0;
int numVerts = 0;

auto it = delaunay->GetOutput()->NewCellIterator();
for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextCell())
  {
    if (it->GetCellType() == VTK_TETRA)
    {
      numTetras++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Banana").GetData());
    }
    else if (it->GetCellType() == VTK_LINE)
    {
      numLines++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Peacock").GetData());
    }
    else if (it->GetCellType() == VTK_TRIANGLE)
    {
      numTris++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Tomato").GetData());
    }
    else if (it->GetCellType() == VTK_VERTEX)
    {
      numVerts++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Lime").GetData());
    }
}
it->Delete();

std::stringstream ss;
ss << "numTetras: " << numTetras << std::endl;
ss << "numLines: " << numLines << std::endl;
ss << "numTris: " << numTris << std::endl;
ss << "numVerts: " << numVerts;

// Set the cell color depending on cell type
delaunay->GetOutput()->GetCellData()->SetScalars(cellData);
vtkSmartPointer<vtkXMLUnstructuredGridWriter> ugWriter =  vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
ugWriter->SetInputConnection(delaunay->GetOutputPort());
auto my_cstr3 = ".vtu";        
std::string s3(my_cstr3); 
ugWriter->SetFileName((name+s3).c_str());
ugWriter->Write();


if (vtk_vision==true){
	//fromItk_to_vtk_vision(outputFilename);
	create_anatomical_map((name+s3));
}



	
  /*
  vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
  surfaceFilter->SetInputConnection(reader->GetOutputPort());
  surfaceFilter->Update();  
 
  vtkSmartPointer<vtkXMLPolyDataWriter> outputWriter = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  outputWriter->SetFileName((name+"edge.vtp").c_str());
  outputWriter->SetInputData(surfaceFilter->GetOutput());
  outputWriter->Write();

  // Setup actor and mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(surfaceFilter->GetOutputPort());
  
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetInterpolationToFlat();

  // Setup render window, renderer, and interactor
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(actor);
  renderer->SetBackground(.1, .2, .3);

  renderWindow->Render();
  renderWindowInteractor->Start();

// if you want vision with slider to change the alpha
 */ 
}
 



 
void Interpolate_3D::create_LV(std::string myo, std::string scar,std::string store){

//vtkSmartPointer<vtkPolyData> polyData;
vtkSmartPointer<vtkPolyData> input1;
vtkSmartPointer<vtkPolyData> input2;
//vtkSmartPointer<vtkPolyData> input3;

//cout << "Directory created" <<epi<< std::endl;
//cout << "Directory created" <<endo<< std::endl;
vtkSmartPointer<vtkXMLPolyDataReader> reader =  vtkSmartPointer<vtkXMLPolyDataReader>::New();

reader->SetFileName(myo.c_str());
reader->Update();
input1=reader->GetOutput();


vtkSmartPointer<vtkXMLPolyDataReader> reader2 =  vtkSmartPointer<vtkXMLPolyDataReader>::New();
reader2->SetFileName(scar.c_str());
reader2->Update();
input2=reader2->GetOutput();


//vtkSmartPointer<vtkAppendPolyData> appendFilter =  vtkSmartPointer<vtkAppendPolyData>::New();
//appendFilter->AddInputData(input1);
//appendFilter->AddInputData(input2);
//appendFilter->Update();
//polyData = appendFilter->GetOutput();

//vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();


vtkSmartPointer<vtkBooleanOperationPolyDataFilter> boolFilter =vtkSmartPointer<vtkBooleanOperationPolyDataFilter>::New();
boolFilter->SetOperationToDifference();
boolFilter->SetInputData( 0, input1 );
boolFilter->SetInputData( 1, input2 );

vtkSmartPointer<vtkXMLPolyDataWriter> outputWriter = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
outputWriter->SetFileName((store+"myo.vtu").c_str());
outputWriter->SetInputData(boolFilter->GetOutput());
outputWriter->Write();

}



void Interpolate_3D::create_anatomical_map(std::string vtpdatafile ){

std::string fileName = vtpdatafile;
vtkSmartPointer<vtkNamedColors> colors =
vtkSmartPointer<vtkNamedColors>::New();

// Create the rendering window, renderer, and interactive renderer.
vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
vtkSmartPointer<vtkRenderWindow> renWin =
vtkSmartPointer<vtkRenderWindow>::New();
renWin->SetSize(780, 780);
renWin->AddRenderer(ren);
vtkSmartPointer<vtkRenderWindowInteractor> iren =
vtkSmartPointer<vtkRenderWindowInteractor>::New();
iren->SetRenderWindow(renWin);

// Make an annotated cube actor with axes and then add it into an orientation
// marker widget.
// Three of these need to be made.

// Right Posterior Superior
std::array<std::string, 3> xyzLabels{{"X", "Y", "Z"}};
std::array<double, 3> scale{{1.5, -1.5, 1.5}};
vtkSmartPointer<vtkPropAssembly> axes =MakeCubeActor(scale, xyzLabels, colors);
vtkSmartPointer<vtkOrientationMarkerWidget> om =vtkSmartPointer<vtkOrientationMarkerWidget>::New();
om->SetOrientationMarker(axes);
// Position upper left in the viewport.
om->SetViewport(0.0, 0.8, 0.2, 1.0);
om->SetInteractor(iren);
om->EnabledOn();
om->InteractiveOn();

// Right, Anterior, Superior.
std::array<double, 3> scale1{{1.5, 1.5, 1.5}};
vtkSmartPointer<vtkPropAssembly> axes1 =MakeCubeActor(scale1, xyzLabels, colors);
vtkSmartPointer<vtkOrientationMarkerWidget> om1 =
vtkSmartPointer<vtkOrientationMarkerWidget>::New();
om1->SetOrientationMarker(axes1);
// Position lower left in the viewport.
om1->SetViewport(0, 0, 0.2, 0.2);
om1->SetInteractor(iren);
om1->EnabledOn();
om1->InteractiveOn();

// Left, Posterior, Superior.
std::array<double, 3> scale2{{-1.5, -1.5, 1.5}};
vtkSmartPointer<vtkPropAssembly> axes2 =
MakeCubeActor(scale2, xyzLabels, colors);
vtkSmartPointer<vtkOrientationMarkerWidget> om2 =
vtkSmartPointer<vtkOrientationMarkerWidget>::New();
om2->SetOrientationMarker(axes2);
// Position lower right in the viewport.
om2->SetViewport(0.8, 0, 1.0, 0.2);
om2->SetInteractor(iren);
om2->EnabledOn();
om2->InteractiveOn();
// Finally create an annotated cube actor adding it into an orientation marker
// widget.
vtkSmartPointer<vtkAnnotatedCubeActor> axes3 = MakeAnnotatedCubeActor(colors);
vtkSmartPointer<vtkOrientationMarkerWidget> om3 =
vtkSmartPointer<vtkOrientationMarkerWidget>::New();
om3->SetOrientationMarker(axes3);
// Position upper right in the viewport.
om3->SetViewport(0.8, 0.8, 1.0, 1.0);
om3->SetInteractor(iren);
om3->EnabledOn();
om3->InteractiveOn();
// Read in the model.
vtkSmartPointer<vtkXMLPolyDataReader> reader =vtkSmartPointer<vtkXMLPolyDataReader>::New();
reader->SetFileName(fileName.c_str());
reader->Update();

vtkSmartPointer<vtkPolyDataMapper> humanMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
humanMapper->SetInputConnection(reader->GetOutputPort());
humanMapper->SetScalarModeToUsePointFieldData();
humanMapper->SelectColorArray("Color");
humanMapper->SetColorModeToDirectScalars();

vtkSmartPointer<vtkActor> humanActor = vtkSmartPointer<vtkActor>::New();
humanActor->SetMapper(humanMapper);
std::vector<double> bounds(6, 0);
humanActor->GetBounds(&bounds[0]);
std::vector<double>::iterator maxElt =std::max_element(bounds.begin(), bounds.end());
// Scale the actor
humanActor->SetScale(1.0 / *maxElt);
ren->AddActor(humanActor);

// Make the planes.
std::vector<vtkSmartPointer<vtkActor>> actors = MakePlanesActors(colors);
for (auto actor : actors)
{
    ren->AddViewProp(actor);
}
// Label them.
std::vector<vtkSmartPointer<vtkActor>> textActors = AddTextToPlanes();
for (auto actor : textActors)
{
    ren->AddViewProp(actor);
}

// Interact
ren->SetBackground2(colors->GetColor3d("OldLace").GetData());
ren->SetBackground(colors->GetColor3d("MistyRose").GetData());
ren->GradientBackgroundOn();
ren->ResetCamera();
ren->GetActiveCamera()->Zoom(1.6);
ren->GetActiveCamera()->SetPosition(-2.3, 4.1, 4.2);
ren->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);
ren->ResetCameraClippingRange();
renWin->Render();
//  Call SetWindowName after renWin.Render() is called.
renWin->SetWindowName("Anatomical Orientation");
iren->Initialize();
iren->Start();

}
