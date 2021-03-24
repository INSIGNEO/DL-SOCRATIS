#include "threeDvtk2Dslice.h"
#include <iostream>

vtkStandardNewMacro(MouseInteractorStyle2);

void threeDvtk2Dslice::visualize(vtkSmartPointer<vtkPolyData> polydata){
 // compute origin of polydata 

  double bounds[6];
  polydata->GetBounds(bounds);
  double spacing[3]; // desired volume spacing
  spacing[0] = 0.5;
  spacing[1] = 0.5;
  spacing[2] = 0.5;
  // compute dimensions
  int dim[3];
  for (int i = 0; i < 3; i++)
    {
    dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));
    }
  double origin[6];
  origin[0] = bounds[0] + spacing[0] / 2;
  origin[1] = bounds[2] + spacing[1] / 2;
  origin[2] = bounds[4] + spacing[2] / 2;

  vtkSmartPointer<vtkPolyDataMapper> mapper =vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(polydata);

  vtkSmartPointer<vtkActor> actor =vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(1,1,0);
  actor->SetOrigin(origin[0],origin[1],origin[2]);

  // a renderer and render window
  vtkSmartPointer<vtkRenderer> renderer =vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  // an interactor
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(.3, .6, .3); // Background color green

  renderWindow->Render();


  // Set the custom stype to use for interaction.
vtkSmartPointer<MouseInteractorStyle2> style =vtkSmartPointer<MouseInteractorStyle2>::New();
 style->SetDefaultRenderer(renderer);
 renderWindowInteractor->SetInteractorStyle( style );
  // begin mouse interaction
renderWindowInteractor->Initialize();
renderWindowInteractor->Start();

position_angle[0]=style->pos_ang[0];
position_angle[1]=style->pos_ang[1];
position_angle[2]=style->pos_ang[2];

position_point[0]=style->pos[0];
position_point[1]=style->pos[1];
position_point[2]=style->pos[2];

}




void threeDvtk2Dslice::slice_conv(const char * m_inputImageFileName, const char * m_outputImageFileName, int sliceNum )
{

//public
  
  const char * inputFilename  = m_inputImageFileName;
  const char * outputFilename = m_outputImageFileName;
  using InputPixelType = unsigned char;
  using OutputPixelType = unsigned char;
  using InputImageType = itk::Image< InputPixelType,  3 >;
  using OutputImageType = itk::Image< OutputPixelType, 2 >;
  using ReaderType = itk::ImageFileReader< InputImageType  >;
  using WriterType = itk::ImageFileWriter< OutputImageType >;


////////////////////////////////transformation of image///////////////////////////////////////////
   // Read all the data from the file
  vtkSmartPointer<vtkPolyDataReader> readerpoly = vtkSmartPointer<vtkPolyDataReader>::New();
  readerpoly->SetFileName(inputFilename);
  readerpoly->Update();
 vtkSmartPointer<vtkPolyData> pdinit = readerpoly->GetOutput();
// From vtk polydata to mhd metadata image store

visualize(pdinit);


std::cout <<"Angles of rotation with respect of global coordination " << '\n';
std::cout <<"Rz: "<< position_angle[2] << '\n';
std::cout <<"Ry: "<< position_angle[1] << '\n';
std::cout <<"Rx: "<< position_angle[0] << '\n';

std::cout <<"point of rotation with respect of global coordination " << '\n';
std::cout <<"Z: "<< position_point[2] << '\n';
std::cout <<"Y: "<< position_point[1] << '\n';
std::cout <<"X: "<< position_point[0] << '\n';


// Pick from the origin of the polydata.
double rx[4];
rx[0]=-position_angle[0];
rx[1]=1;
rx[2]=0;
rx[3]=0;

double ry[4];
ry[0]=-position_angle[1];
ry[1]=0;
ry[2]=1;
ry[3]=0;

double rz[4];
rz[0]=-position_angle[2];
rz[1]=0;
rz[2]=0;
rz[3]=1;
vtkSmartPointer<vtkTransform> transform_rt = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> transform_rz = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> transform_ry = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> transform_rx = vtkSmartPointer<vtkTransform>::New();
  //transform->RotateWXYZ(double angle, double x, double y, double z);
transform_rt->Translate(position_point[0],position_point[1],position_point[2]);
transform_rx->RotateX(position_angle[0]);
 transform_ry->RotateY(position_angle[1]);
  transform_rz->RotateZ(position_angle[2]);
 
  
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilterrt = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilterrz = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilterrx = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilterry = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
transformFilterrt->SetTransform(transform_rt);
  transformFilterrt->SetInputConnection(readerpoly->GetOutputPort());
  transformFilterrt->Update();
  transformFilterrz->SetTransform(transform_rx);
  transformFilterrz->SetInputConnection(transformFilterrt->GetOutputPort());
  transformFilterrz->Update();
  transformFilterrx->SetTransform(transform_ry);
  transformFilterrx->SetInputConnection(transformFilterrz->GetOutputPort());
  transformFilterrx->Update();
  transformFilterry->SetTransform(transform_rz);
  transformFilterry->SetInputConnection(transformFilterrx->GetOutputPort());
  transformFilterry->Update();

vtkSmartPointer<vtkPolyData> pdfinal=transformFilterry->GetOutput();
  visualize(pdfinal);
  vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New();    
  double bounds[6];
 pdfinal->GetBounds(bounds);
  double spacing[3]; // desired volume spacing
  spacing[0] = 0.25;
  spacing[1] = 0.25;
  spacing[2] = 0.25;
  whiteImage->SetSpacing(spacing);

  // compute dimensions
  int dim[3];
  for (int i = 0; i < 3; i++)
    {
    dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));
    }
  whiteImage->SetDimensions(dim);
  whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

  double origin[6];
  origin[0] = bounds[0] + spacing[0] / 2;
  origin[1] = bounds[2] + spacing[1] / 2;
  origin[2] = bounds[4] + spacing[2] / 2;

whiteImage->SetOrigin(origin);

#if VTK_MAJOR_VERSION <= 5
  whiteImage->SetScalarTypeToUnsignedChar();
  whiteImage->AllocateScalars();
#else
  whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);
#endif
  // fill the image with foreground voxels:
  unsigned char inval = 255;
  unsigned char outval = 0;
  vtkIdType count = whiteImage->GetNumberOfPoints();
  for (vtkIdType i = 0; i < count; ++i)
    {
    whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
    }

  // polygonal data --> image stencil:
  vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
#if VTK_MAJOR_VERSION <= 5
  pol2stenc->SetInput(pdfinal);
#else
  pol2stenc->SetInputData(pdfinal);
#endif
  pol2stenc->SetOutputOrigin(origin);
  pol2stenc->SetOutputSpacing(spacing);
  pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
  pol2stenc->Update();

  // cut the corresponding white image and set the background:
  vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
#if VTK_MAJOR_VERSION <= 5
  imgstenc->SetInput(whiteImage);
  imgstenc->SetStencil(pol2stenc->GetOutput());
#else
  imgstenc->SetInputData(whiteImage);
  imgstenc->SetStencilConnection(pol2stenc->GetOutputPort());
#endif
  imgstenc->ReverseStencilOff();
  imgstenc->SetBackgroundValue(outval);
  imgstenc->Update();

  vtkSmartPointer<vtkMetaImageWriter> writermhd = vtkSmartPointer<vtkMetaImageWriter>::New();
  writermhd->SetFileName("scars3D.mhd");
#if VTK_MAJOR_VERSION <= 5
  writermhd->SetInput(imgstenc->GetOutput());
#else
  writermhd->SetInputData(imgstenc->GetOutput());
#endif
  writermhd->Write();  


// slice the 3d to 2d
int gap;
int list[3]={dim[0],dim[1],dim[2]};


 for ( unsigned int i = 0 ; i < sliceNum ; ++i ){//atoi(sliceNum)
  ReaderType::Pointer reader = ReaderType::New();
 WriterType::Pointer writer = WriterType::New();
  reader->SetFileName( "scars3D.mhd" );
  //reader->Update();

  using FilterType = itk::ExtractImageFilter< InputImageType,OutputImageType >;
  FilterType::Pointer filter = FilterType::New();
  filter->InPlaceOn();
  filter->SetDirectionCollapseToSubmatrix();
 
  reader->UpdateOutputInformation();
  InputImageType::RegionType inputRegion =reader->GetOutput()->GetLargestPossibleRegion();

  InputImageType::SizeType size = inputRegion.GetSize();

  InputImageType::IndexType start = inputRegion.GetIndex();
  const unsigned int sliceNumber = ( sliceNum ); //std::stoi( sliceNum );
 if (i==0){gap=0;}
 if(i!=0){gap=*std::max_element(list,list+3)/( sliceNum );} //atoi(sliceNum);}


  size[std::distance(list,std::max_element(list,list+3))] = 0;
  std::cout << std::distance(list,std::max_element(list,list+3)) << '\n';

  start[std::distance(list,std::max_element(list,list+3))] = i*gap;
  std::string c=std::to_string(i);
  InputImageType::RegionType desiredRegion;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );
 
  filter->SetExtractionRegion( desiredRegion );
  
  filter->SetInput( reader->GetOutput() );
  writer->SetInput( filter->GetOutput() );
  std::string str;
 
str.append(outputFilename);
str.append(c);
str.append(".jpg");

std::cout << str << '\n';
  writer->SetFileName( str );
  writer->Update();

}

}



