////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#include "HistogramGaussianRician.h"
#include <vector>
#include <algorithm>

HistogramGaussianRician::HistogramGaussianRician(const char * inputFileName, const char * m_bins):m_inputFileName(inputFileName), bins(m_bins){

}
void HistogramGaussianRician::Update(){
ar=0.5;
sr=250;
ar2=0.13;
sr2=250;
xs=0;
mg=200;
mr=0;
mr2=60;
ag=0.25;
sg=250;
ag2=0.12;
sg2=250;
mg2=100;
xmax=0;
argmaxh=0;
if (RG==true){std::cout << "Risian Gaussian Mixture model analysis: "<< '\n' ; Image_HistogramRicianGaussian();RRG=false;RGG=false;RRGG=false; }
if (RRG==true){std::cout << "Risian Risian Gaussian Mixture model analysis: "<< '\n' ; Image_HistogramRician_R_Gaussian();RG=false;RRG=false;RRGG=false; }
if (RGG==true){ std::cout << "Risian Gaussian Gaussian Mixture model analysis: "<< '\n' ; Image_HistogramRician_G_Gaussian();RG=false;RGG=false;RRGG=false;}
if (RRGG==true){std::cout << "Risian Risian Gaussian Gaussian Mixture model analysis: "<< '\n' ; Image_HistogramRician_R_G_Gaussian();RG=false;RGG=false;RRG=false; }
}
void HistogramGaussianRician::Reset(){
detect_max=true;
RG=false;
RRG=false;
RGG=false;
RRGG=false;
print=true;

ar=0.5;
sr=250;
ar2=0.13;
sr2=250;
xs=0;
mg=200;
mr=0;
mr2=60;
ag=0.25;
sg=250;
ag2=0.12;
sg2=250;
mg2=100;
xmax=0;
argmaxh=0;
std::fill(observ.begin(), observ.end(), 0);
std::fill(freq.begin(), freq.end(), 0);


}
void HistogramGaussianRician::Image_HistogramRicianGaussian()
 {
// number of classes two Rician-Gaussian
unsigned int numberOfClasses = 2;
compute_histogram();
//define initial parameters
   ar=0.5;
   ag=0.5;



if (detect_max==true){ 
int sum=0; 
int offset=freq.size()*0; 

//my assumption
int indexmax;
int indexmax2;
int check=1;
int winner = offset;
for ( int pos = offset ; pos < freq.size() ; pos++ ) {
            if ( freq[winner] < freq[pos] ) {
                winner = pos;
              }//end if
             
          sum+=freq[pos];
    }//end for 
  

double max=*std::max_element(std::begin(observ),std::end(observ));
ar=freq[winner];
sr=observ[winner];
mr=0;
mg=sr+0.5*(max-observ[winner]);
sg=sqrt(0.33333*(max-mg));
ag=1-ar;


/*
indexmax=observ[winner];
double hmax=freq[winner];
int sd1= 50;//define sensitivity of code
int sd2= 20;//define sensitivity of code
int sd3= 30;//define sensitivity of code
int test=0;
int addsize=0;
 std::cout << "Maximum detection points initialization..." << '\n' ;
while(check==1){
         int winner = offset;
        for ( int pos = offset ; pos < freq.size() ; pos++ ) {
            if ( freq[winner] < freq[pos] ) {
                winner = pos;
              }//end if   

        }//end for
         //std::cout << "index : " <<observ[winner]<< '\n' ;
        indexmax2=observ[winner];
        freq.erase(freq.begin()+winner); //erase the max value
        observ.erase(observ.begin()+winner); //erase the max value
        addsize++;
	if (indexmax<=1*(255)/4){ 
		if ( (indexmax2>indexmax+sd1) && (indexmax+sd1<=255)){ check=0; test=2;}}

	if ((indexmax>1*(255)/4) && (indexmax<2*(255)/4))		{
		if ( (indexmax2>indexmax+sd1) && (indexmax+sd1<=255)){ check=0; test=2;} 
		if ((indexmax2<indexmax-sd2) && (indexmax-sd2>=0)){ check=0; test=1;}
		}
	if (indexmax>=2*(255)/4){
		if ((indexmax2<indexmax-sd3) && (indexmax-sd3>=0) ){ check=0; test=1;}}
       

}//end while


if (test==1){ ag=0.5;   ar=1-ag; mr=indexmax2-1; mg=indexmax-1; sr=10*mr; sg=1*mg;}
if (test==2){ ar=0.5;  ag=1-ar; mr=indexmax-1; mg=indexmax2-1; sr=1*mr; sg=5*mg;}

*/
}


 

  ParametersType params( 2 );
  // Create the first set of initial Gaussian parameters
  std::vector< ParametersType > initialParameters( numberOfClasses ); 
  // Create the second set of initial Rician parameters
  params[0] = mr; // mean of dimension 1
  params[1] = sr; //500.0; // covariance(0,0)
  initialParameters[0] = params;

  params[0] = mg; // mean of dimension 1
  params[1] = sg; //400.0; // covariance(0,0)
  initialParameters[1] = params;


 ///////////////////////////////////////////////////////////////////////////////////
  // Create the Rician components
  ComponentType2::Pointer  components2=ComponentType2::New();

  
    components2->SetSample( sample );
    components2->SetParameters( initialParameters[0] );



  // Create the Gausian components
  ComponentType::Pointer  components =ComponentType::New();
  

    components->SetSample( sample );
    components->SetParameters( initialParameters[1] );


 /////////////////////////////////////////////////////////////////////////////

  estimator->SetSample( sample );
  estimator->SetMaximumIteration( 2000 );
 
  itk::Array< double > initialProportions(numberOfClasses);
 
  initialProportions[0] = ar;
 initialProportions[1] = ag;

  estimator->SetInitialProportions( initialProportions );

  estimator->AddComponent( (ComponentType2::Superclass*) components2.GetPointer()) ;
  estimator->AddComponent( (ComponentType::Superclass*) components.GetPointer()) ;
  estimator->Update();
// write the mean results 
SampleType::Pointer mean1 = SampleType::New();
MeasurementVectorType mv3;

mv3=components2->GetFullParameters()[0];
mean1->PushBack(mv3);
mv3=components->GetFullParameters()[0];
mean1->PushBack(mv3);

mnR=mean1->GetMeasurementVector(0); //R
mnG=mean1->GetMeasurementVector(1); //G
if (print==true){

std::cout<<"//////////////////////"<<std::endl;
std::cout<<"Initial_values"<<std::endl;

std::cout<<"ar: "<<ar<<std::endl;
std::cout<<"SD_rician: "<<sr<<std::endl;
std::cout<<"mean_rician: "<<mr<<std::endl;



std::cout<<"ag: "<<ag<<std::endl;
std::cout<<"SD_gauss: "<<sg<<std::endl;
std::cout<<"mean_gauss: "<<mg<<std::endl;

std::cout<<"//////////////////////"<<std::endl;



   std::cout << "Cluster[" << 0<< "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components2)->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[0] << std::endl;

    std::cout << "Cluster[" << 1 << "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components)->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[1] << std::endl;



std::cout<<"//////////////////////"<<std::endl;

}

}



void HistogramGaussianRician::Image_HistogramRician_R_Gaussian()
 {
  // number of classes two Rician-Gaussian
 unsigned int numberOfClasses = 3;

 compute_histogram();

// two boundaris no need in computation
  //define initial parameters
ar=0.5;
ar2=0.25;
ag=0.25;
 
if (detect_max==true){ 
int sum=0; 
  int offset=freq.size()*0; 


//my assumption
int indexmax;
int indexmax2;
int check=1;
int winner = offset;
for ( int pos = offset ; pos < freq.size() ; pos++ ) {
            if ( freq[winner] < freq[pos] ) {
                winner = pos;
              }//end if
             
          sum+=freq[pos];
    }//end for   


double max=*std::max_element(std::begin(observ),std::end(observ));
ar=freq[winner];
sr=observ[winner];
mr=0;
mg=sr+0.5*(max-observ[winner]);
sg=sqrt(0.33333*(max-mg));
ag=1-ar-ar*0.5;

mr2=(mg-mg/2.0)-1; sr2=sr*0.8;

ar2=ar*0.5;


/*
indexmax=observ[winner];
double hmax=freq[winner];
int sd1= 50;//define sensitivity of code
int sd2= 20;//define sensitivity of code
int sd3= 30;//define sensitivity of code
int test=0;
int addsize=0;
 std::cout << "Maximum detection points initialization..." << '\n' ;
while(check==1){
         int winner = offset;
        for ( int pos = offset ; pos < freq.size() ; pos++ ) {
            if ( freq[winner] < freq[pos] ) {
                winner = pos;
              }//end if   

        }//end for
         //std::cout << "index : " <<observ[winner]<< '\n' ;
        indexmax2=observ[winner];
        freq.erase(freq.begin()+winner); //erase the max value
        observ.erase(observ.begin()+winner); //erase the max value
        addsize++;
	if (indexmax<=1*(255)/4){ 
		if ( (indexmax2>indexmax+sd1) && (indexmax+sd1<=255)){ check=0; test=2;}}

	if ((indexmax>1*(255)/4) && (indexmax<2*(255)/4))		{
		if ( (indexmax2>indexmax+sd1) && (indexmax+sd1<=255)){ check=0; test=2;} 
		if ((indexmax2<indexmax-sd2) && (indexmax-sd2>=0)){ check=0; test=1;}
		}
	if (indexmax>=2*(255)/4){
		if ((indexmax2<indexmax-sd3) && (indexmax-sd3>=0) ){ check=0; test=1;}}
       

}//end while


if (test==1){ ag=0.25;  ar2=0.25; ar=1-ag-ar2; mr=indexmax2-1; mg=indexmax-1; sr=10*mr; sg=1*mg; mr2=(indexmax2+std::abs(indexmax2-indexmax)/2)+1; sr2=5*mr2;}
if (test==2){ ar=0.25; ar2=0.25; ag=1-ar-ar2; mr=indexmax-1; mg=indexmax2-1; sr=1*mr; sg=5*mg; mr2=(indexmax+std::abs(indexmax-indexmax2)/2)+1; sr2=5*mr2;}

*/

}


  typedef itk::Array< double > ParametersType;
  ParametersType params( 2 );
 
  // Create the first set of initial Gaussian parameters
  std::vector< ParametersType > initialParameters( numberOfClasses );

 
  // Create the second set of initial Rician parameters
  params[0] = mr; // mean of dimension 1
  params[1] = sr; //500.0; // covariance(0,0)
  initialParameters[0] = params;

  params[0] = mr2; // mean of dimension 1
  params[1] = sr2; //400.0; // covariance(0,0)
  initialParameters[1] = params;

   params[0] = mg; // mean of dimension 1
  params[1] = sg; //400.0; // covariance(0,0)
  initialParameters[2] = params;



 ///////////////////////////////////////////////////////////////////////////////////

  // Create the Rician components
  std::vector<ComponentType2::Pointer>   components2;

  
  components2.push_back( ComponentType2::New() );
    components2[0]->SetSample( sample );
    components2[0]->SetParameters( initialParameters[0] );
    components2.push_back( ComponentType2::New() );
    components2[1]->SetSample( sample );
    components2[1]->SetParameters( initialParameters[1] );


  // Create the Rician components
  //ComponentType2::Pointer  componentsR2=ComponentType2::New();


   // componentsR2->SetSample( sample );
  //  componentsR2->SetParameters( initialParameters[1] );



  // Create the Gausian components
  ComponentType::Pointer  components =ComponentType::New();
  

    components->SetSample( sample );
    components->SetParameters( initialParameters[2] );



 /////////////////////////////////////////////////////////////////////////////


 
  estimator->SetSample( sample );
  estimator->SetMaximumIteration( 2000 );
 
  itk::Array< double > initialProportions(numberOfClasses);
 
  initialProportions[0] = ar;
 initialProportions[1] = ar2;
 initialProportions[2] = ag;

  estimator->SetInitialProportions( initialProportions );

  estimator->AddComponent( (ComponentType2::Superclass*) components2[0].GetPointer()) ;
  estimator->AddComponent( (ComponentType2::Superclass*) components2[1].GetPointer()) ;
  estimator->AddComponent( (ComponentType::Superclass*) components.GetPointer()) ;
  estimator->Update();
  // Output the results


 
SampleType::Pointer mean1 = SampleType::New();

MeasurementVectorType mv3;

mv3=components2[0]->GetFullParameters()[0];
mean1->PushBack(mv3);
mv3=components2[1]->GetFullParameters()[0];
mean1->PushBack(mv3);
mv3=components->GetFullParameters()[0];
mean1->PushBack(mv3);

mnR=mean1->GetMeasurementVector(0); //R
mnR2=mean1->GetMeasurementVector(1); //R2
mnG=mean1->GetMeasurementVector(2); //G

if (print==true){
std::cout<<"//////////////////////"<<std::endl;
std::cout<<"Initial_values"<<std::endl;
//std::cout<<"argmaxh: "<<argmaxh<<std::endl;
//std::cout<<"xmax: "<<xmax<<std::endl;
std::cout<<"ar: "<<ar<<std::endl;
std::cout<<"SD_rician: "<<sr<<std::endl;
std::cout<<"mean_rician: "<<mr<<std::endl;

std::cout<<"ar2: "<<ar2<<std::endl;
std::cout<<"SD_rician2: "<<sr2<<std::endl;
std::cout<<"mean_rician2: "<<mr2<<std::endl;

std::cout<<"ag: "<<ag<<std::endl;
std::cout<<"SD_gauss: "<<sg<<std::endl;
std::cout<<"mean_gauss: "<<mg<<std::endl;


std::cout<<"//////////////////////"<<std::endl;

  
    std::cout << "Cluster[" << 0<< "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components2[0])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[0] << std::endl;

    std::cout << "Cluster[" << 1 << "]" << std::endl;

    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components2[1])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[1] << std::endl;


    std::cout << "Cluster[" << 2 << "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components)->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[2] << std::endl;





}


}

void HistogramGaussianRician::Image_HistogramRician_G_Gaussian()
 {

// number of classes two Rician-Gaussian
unsigned int numberOfClasses = 3;
compute_histogram();
//define initial parameters
ar=0.5;

ag2=0.25;
ag=0.25;

if (detect_max==true){ 
int sum=0; 
int offset=freq.size()*0; 


//my assumption
int indexmax;
int indexmax2;
int check=1;
int winner = offset;
for ( int pos = offset ; pos < freq.size() ; pos++ ) {
            if ( freq[winner] < freq[pos] ) {
                winner = pos;
              }//end if
             
          sum+=freq[pos];
    }//end for   



double max=*std::max_element(std::begin(observ),std::end(observ));
ar=freq[winner];
sr=observ[winner];
mr=0;
mg=sr+0.5*(max-observ[winner]);
sg=sqrt(0.33333*(max-mg));
ag=1-ar-0.25;

mg2=(mg+(mg/2.0))-1; sg2=sg*0.8;

ag2=0.25;

/*
indexmax=observ[winner];
double hmax=freq[winner];
int sd1= 50;//define sensitivity of code
int sd2= 20;//define sensitivity of code
int sd3= 30;//define sensitivity of code
int test=0;
int addsize=0;
 std::cout << "Maximum detection points initialization..." << '\n' ;
while(check==1){
         int winner = offset;
        for ( int pos = offset ; pos < freq.size() ; pos++ ) {
            if ( freq[winner] < freq[pos] ) {
                winner = pos;
              }//end if   

        }//end for
         //std::cout << "index : " <<observ[winner]<< '\n' ;
        indexmax2=observ[winner];
        freq.erase(freq.begin()+winner); //erase the max value
        observ.erase(observ.begin()+winner); //erase the max value
        addsize++;
	if (indexmax<=1*(255)/4){ 
		if ( (indexmax2>indexmax+sd1) && (indexmax+sd1<=255)){ check=0; test=2;}}

	if ((indexmax>1*(255)/4) && (indexmax<2*(255/4))){
		if ( (indexmax2>indexmax+sd1) && (indexmax+sd1<=255)){ check=0; test=2;} 
		if ((indexmax2<indexmax-sd2) && (indexmax-sd2>=0)){ check=0; test=1;}
		}
	if (indexmax>=2*(255)/4){
		if ((indexmax2<indexmax-sd3) && (indexmax-sd3>=0) ){ check=0; test=1;}}
       

}//end while

if (test==1){ ag=0.25;  ag2=0.25; ar=1-ag-ag2; mr=indexmax2-1; mg=indexmax-1; sr=10*mr; sg=1*mg; mg2=(indexmax2+std::abs(indexmax2-indexmax)/2)-1; sg2=5*mg2;}
if (test==2){ ar=0.25; ag2=0.25; ag=1-ar-ag2; mr=indexmax-1; mg=indexmax2-1; sr=1*mr; sg=5*mg; mg2=(indexmax+std::abs(indexmax-indexmax2)/2)-1; sg2=5*mg2;}


*/
}



  typedef itk::Array< double > ParametersType;
  ParametersType params( 2 );
 
  // Create the first set of initial Gaussian parameters
  std::vector< ParametersType > initialParameters( numberOfClasses );

 
  // Create the second set of initial Rician parameters
  params[0] = mr; // mean of dimension 1
  params[1] = sr; //500.0; // covariance(0,0)
  initialParameters[0] = params;

  params[0] = mg2; // mean of dimension 1
  params[1] = sg2; //400.0; // covariance(0,0)
  initialParameters[1] = params;

   params[0] = mg; // mean of dimension 1
  params[1] = sg; //400.0; // covariance(0,0)
  initialParameters[2] = params;



 ///////////////////////////////////////////////////////////////////////////////////


  // Create the Rician components
  ComponentType2::Pointer  components2=ComponentType2::New();

  
    components2->SetSample( sample );
    components2->SetParameters( initialParameters[0] );



  // Create the Gaussian components

  //ComponentType::Pointer  componentsG2=ComponentType::New();


  //  componentsG2->SetSample( sample );
  //  componentsG2->SetParameters( initialParameters[1] );

  // Create the Gausian components
std::vector<ComponentType::Pointer> components;

components.push_back( ComponentType::New() );
    components[0]->SetSample( sample );
    components[0]->SetParameters( initialParameters[1] );

components.push_back( ComponentType::New() );
    components[1]->SetSample( sample );
    components[1]->SetParameters( initialParameters[2] );

 /////////////////////////////////////////////////////////////////////////////


 
  estimator->SetSample( sample );
  estimator->SetMaximumIteration( 2000 );
 
  itk::Array< double > initialProportions(numberOfClasses);
 
  initialProportions[0] = ar;
 initialProportions[1] = ag2;
 initialProportions[2] = ag;

  estimator->SetInitialProportions( initialProportions );

  estimator->AddComponent( (ComponentType2::Superclass*) (components2).GetPointer()) ;
  estimator->AddComponent( (ComponentType::Superclass*) (components[0]).GetPointer()) ;
  estimator->AddComponent( (ComponentType::Superclass*) (components[1]).GetPointer()) ;
  estimator->Update();
  // Output the results


 
SampleType::Pointer mean1 = SampleType::New();

MeasurementVectorType mv3;

mv3=components2->GetFullParameters()[0];
mean1->PushBack(mv3);
mv3=components[0]->GetFullParameters()[0];
mean1->PushBack(mv3);
mv3=components[1]->GetFullParameters()[0];
mean1->PushBack(mv3);

mnR=mean1->GetMeasurementVector(0); //R
mnG2=mean1->GetMeasurementVector(1); //R2
mnG=mean1->GetMeasurementVector(2); //G

if (print==true){
std::cout<<"//////////////////////"<<std::endl;
std::cout<<"Initial_values"<<std::endl;
//std::cout<<"argmaxh: "<<argmaxh<<std::endl;
//std::cout<<"xmax: "<<xmax<<std::endl;
std::cout<<"ar: "<<ar<<std::endl;
std::cout<<"SD_rician: "<<sr<<std::endl;
std::cout<<"mean_rician: "<<mr<<std::endl;

std::cout<<"ag2: "<<ag2<<std::endl;
std::cout<<"SD_gauss2: "<<sg2<<std::endl;
std::cout<<"mean_gauss2: "<<mg2<<std::endl;

std::cout<<"ag: "<<ag<<std::endl;
std::cout<<"SD_gauss: "<<sg<<std::endl;
std::cout<<"mean_gauss: "<<mg<<std::endl;


std::cout<<"//////////////////////"<<std::endl;
 
    std::cout << "Cluster[" << 0<< "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components2)->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[0] << std::endl;

    std::cout << "Cluster[" << 1 << "]" << std::endl;

    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components[0])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[1] << std::endl;

    std::cout << "Cluster[" << 2 << "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components[1])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[2] << std::endl;


}


}

void HistogramGaussianRician::Image_HistogramRician_R_G_Gaussian()
 {

// number of classes two Rician-Gaussian
unsigned int numberOfClasses = 4;
compute_histogram();
//define initial parameters
ar=0.5;
ar2=0.13;
ag=0.25;
ag2=0.12;

if (detect_max==true){ 
int sum=0; 
int offset=freq.size()*0; 
//my assumption
int indexmax;
int indexmax2;
int check=1;
int winner = offset;
for ( int pos = offset ; pos < freq.size() ; pos++ ) {
            if ( freq[winner] < freq[pos] ) {
                winner = pos;
              }//end if
             
          sum+=freq[pos];
    }//end for   


double max=*std::max_element(std::begin(observ),std::end(observ));
ar=freq[winner];
sr=observ[winner];
mr=0;
mg=sr+0.5*(max-observ[winner]);
sg=sqrt(0.33333*(max-mg));
ag=1-ar-ar*0.5-0.15;

mr2=(mg-mg/2.0)-1; sr2=sr*0.8; //+mg
mg2=(mg+mg/2.0)-1; sg2=sg*0.8;//2*mg
ar2=ar*0.5;
ag2=0.15;

/*
indexmax=observ[winner];
double hmax=freq[winner];
int sd1= 50;//define sensitivity of code
int sd2= 20;//define sensitivity of code
int sd3= 30;//define sensitivity of code
int test=0;
int addsize=0;
 std::cout << "Maximum detection points initialization..." << '\n' ;
while(check==1){
         int winner = offset;
        for ( int pos = offset ; pos < freq.size() ; pos++ ) {
            if ( freq[winner] < freq[pos] ) {
                winner = pos;
              }//end if   

        }//end for

//std::cout << "indexnorm : " <<indexmax<< '\n' ;
        indexmax2=observ[winner];
//std::cout << "indexnorm2 : " <<observ[winner]<< '\n' ;
        freq.erase(freq.begin()+winner); //erase the max value
        observ.erase(observ.begin()+winner); //erase the max value
        addsize++;
	if (indexmax<=1*(255)/4){ 
		if ( (indexmax2>indexmax+sd1) && (indexmax+sd1<=255)){ check=0; test=2;}}

	if ((indexmax>1*(255)/4) && (indexmax<2*(255)/4))		{
		if ( (indexmax2>indexmax+sd1) && (indexmax+sd1<=255)){ check=0; test=2;} 
		if ((indexmax2<indexmax-sd2) && (indexmax-sd2>=0)){ check=0; test=1;}
		}
	if (indexmax>=2*(255)/4){
		if ((indexmax2<indexmax-sd3) && (indexmax-sd3>=0) ){ check=0; test=1;}}
       

}//end while


if (test==1){ ag=0.25;  ar=1-ag-ar2-ag2; mr=indexmax2-1; mg=indexmax-1; sr=10*mr; sg=1*mg; 
mr2=(indexmax2+std::abs(indexmax2-indexmax)/3)-1; sr2=2.5*mg2;
mg2=(indexmax2+2*std::abs(indexmax2-indexmax)/3)-1; sg2=2.5*mg2;}
if (test==2){ ar=0.25;  ag=1-ar-ar2-ag2; mr=indexmax-1; mg=indexmax2-1; sr=1*mr; sg=5*mg; 
mr2=(indexmax+std::abs(indexmax2-indexmax)/3)-1; sr2=2.5*mg2;
mg2=(indexmax+2*std::abs(indexmax2-indexmax)/3)-1; sg2=2.5*mg2;
}

*/

}

if (print==true){
std::cout<<"//////////////////////"<<std::endl;
std::cout<<"Initial_values"<<std::endl;
//std::cout<<"argmaxh: "<<argmaxh<<std::endl;
//std::cout<<"xmax: "<<xmax<<std::endl;
std::cout<<"ar: "<<ar<<std::endl;
std::cout<<"SD_rician: "<<sr<<std::endl;
std::cout<<"mean_rician: "<<mr<<std::endl;

std::cout<<"ar2: "<<ar2<<std::endl;
std::cout<<"SD_rician2: "<<sr2<<std::endl;
std::cout<<"mean_rician2: "<<mr2<<std::endl;

std::cout<<"ag2: "<<ag2<<std::endl;
std::cout<<"SD_gauss2: "<<sg2<<std::endl;
std::cout<<"mean_gauss2: "<<mg2<<std::endl;

std::cout<<"ag: "<<ag<<std::endl;
std::cout<<"SD_gauss: "<<sg<<std::endl;
std::cout<<"mean_gauss: "<<mg<<std::endl;
}
  typedef itk::Array< double > ParametersType;
  ParametersType params( 2 );
 ParametersType params1( 2 );
ParametersType params2( 2 );
ParametersType params3( 2 );
  // Create the first set of initial Gaussian parameters
  std::vector< ParametersType > initialParameters( numberOfClasses );

 
  // Create the second set of initial Rician parameters
  params[0] = mr; // mean of dimension 1
  params[1] = sr; //500.0; // covariance(0,0)
  initialParameters[0] = params;
  params1[0] = mr2; // mean of dimension 1
  params1[1] = sr2; //400.0; // covariance(0,0)
  initialParameters[1] = params1;
  params2[0] = mg2; // mean of dimension 1
  params2[1] = sg2; //400.0; // covariance(0,0)
  initialParameters[2] = params2;

   params3[0] = mg; // mean of dimension 1
  params3[1] = sg; //400.0; // covariance(0,0)
  initialParameters[3] = params3;



 ///////////////////////////////////////////////////////////////////////////////////


  // Create the Rician components
 std::vector<ComponentType2::Pointer>  components2;

  components2.push_back( ComponentType2::New() );
    components2[0]->SetSample( sample );
    components2[0]->SetParameters( initialParameters[0] );
    components2.push_back( ComponentType2::New() );
    components2[1]->SetSample( sample );
    components2[1]->SetParameters( initialParameters[1] );
  // Create the Rician components
 // ComponentTypeR2::Pointer  componentsR2=ComponentTypeR2::New();


   // componentsR2->SetSample( sample );
   // componentsR2->SetParameters( initialParameters[1] );


  // Create the Gaussian components

 // ComponentTypeG2::Pointer  componentsG2=ComponentTypeG2::New();


  //  componentsG2->SetSample( sample );
  //  componentsG2->SetParameters( initialParameters[2] );

  // Create the Gausian components
  std::vector<ComponentType::Pointer>  components ;
  
components.push_back( ComponentType::New() );
    components[0]->SetSample( sample );
    components[0]->SetParameters( initialParameters[2] );

components.push_back( ComponentType::New() );
    components[1]->SetSample( sample );
    components[1]->SetParameters( initialParameters[3] );

 /////////////////////////////////////////////////////////////////////////////


 
  estimator->SetSample(frequency_measure );
  estimator->SetMaximumIteration( 2000 );
 
  itk::Array< double > initialProportions(numberOfClasses);
 
  initialProportions[0] = ar;
 initialProportions[1] = ar2;
 initialProportions[2] = ag2;
 initialProportions[3] = ag;

  estimator->SetInitialProportions( initialProportions );
  estimator->AddComponent( (ComponentType2::Superclass*) components2[0].GetPointer()) ;
 estimator->AddComponent( (ComponentType2::Superclass*) components2[1].GetPointer()) ;
 // estimator->AddComponent( (ComponentTypeR2::Superclass*) componentsR2.GetPointer()) ;
 // estimator->AddComponent( (ComponentTypeG2::Superclass*) componentsG2.GetPointer()) ;
  estimator->AddComponent( (ComponentType::Superclass*) components[0].GetPointer()) ;
 estimator->AddComponent( (ComponentType::Superclass*) components[1].GetPointer()) ;

std::cout<<"//////////////////////"<<std::endl;
  estimator->Update();
  // Output the results

std::cout<<"//////////////////////"<<std::endl;
 
SampleType::Pointer mean1 = SampleType::New();

MeasurementVectorType mv3;

mv3=components2[0]->GetFullParameters()[0];
mean1->PushBack(mv3);
mv3=components2[1]->GetFullParameters()[0];
//mv3=componentsR2->GetFullParameters()[0];
mean1->PushBack(mv3);
//mv3=componentsG2->GetFullParameters()[0];
mv3=components[0]->GetFullParameters()[0];
mean1->PushBack(mv3);
mv3=components[1]->GetFullParameters()[0];
mean1->PushBack(mv3);

mnR=mean1->GetMeasurementVector(0); //R
mnR2=mean1->GetMeasurementVector(1); //R2
mnG2=mean1->GetMeasurementVector(2); //R2
mnG=mean1->GetMeasurementVector(3); //G

if (print==true){
std::cout<<"//////////////////////"<<std::endl;
std::cout<<"Initial_values"<<std::endl;
//std::cout<<"argmaxh: "<<argmaxh<<std::endl;
//std::cout<<"xmax: "<<xmax<<std::endl;
std::cout<<"ar: "<<ar<<std::endl;
std::cout<<"SD_rician: "<<sr<<std::endl;
std::cout<<"mean_rician: "<<mr<<std::endl;

std::cout<<"ar2: "<<ar2<<std::endl;
std::cout<<"SD_rician2: "<<sr2<<std::endl;
std::cout<<"mean_rician2: "<<mr2<<std::endl;

std::cout<<"ag2: "<<ag2<<std::endl;
std::cout<<"SD_gauss2: "<<sg2<<std::endl;
std::cout<<"mean_gauss2: "<<mg2<<std::endl;

std::cout<<"ag: "<<ag<<std::endl;
std::cout<<"SD_gauss: "<<sg<<std::endl;
std::cout<<"mean_gauss: "<<mg<<std::endl;


std::cout<<"//////////////////////"<<std::endl;
   std::cout << "Cluster[" << 0<< "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components2[0])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[0] << std::endl;

    std::cout << "Cluster[" << 1 << "]" << std::endl;

    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components2[1])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[1] << std::endl;


    std::cout << "Cluster[" << 2 << "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components[0])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[2] << std::endl;


    std::cout << "Cluster[" << 3 << "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components[1])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[3] << std::endl;


}

}



void HistogramGaussianRician::compute_histogram()


{

double max_histogram_value=256;

//Compute the values of the histogram
  const auto binsPerDimension = static_cast< unsigned int >( atoi( bins ) );
  normalize=255.000/binsPerDimension;
int offset_black_inten=10*(1/normalize);
  reader->SetFileName( m_inputFileName );
  ImageType::Pointer image = reader->GetOutput();

  ImageToHistogramFilterType::HistogramType::MeasurementVectorType lowerBound(1); //Gray scale
  lowerBound.Fill(0);
  ImageToHistogramFilterType::HistogramType::MeasurementVectorType upperBound(1);
  upperBound.Fill(max_histogram_value);
  ImageToHistogramFilterType::HistogramType::SizeType size(MeasurementVectorSize);
  size.Fill(binsPerDimension);
  imageToHistogramFilter->SetInput( image );
  imageToHistogramFilter->SetHistogramBinMinimum( lowerBound );
  imageToHistogramFilter->SetHistogramBinMaximum( upperBound );
  imageToHistogramFilter->SetHistogramSize( size );
  try
    {
    imageToHistogramFilter->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;

    }


 // The values of the histogram can now be saved into a file by walking through
 // all of the histogram bins and pushing them into a std::ofstream.

  ImageToHistogramFilterType::HistogramType* histogram = imageToHistogramFilter->GetOutput();
 // std::ofstream histogramFile;
 // histogramFile.open( m_outputFileName );
   ImageToHistogramFilterType::HistogramType::ConstIterator itr = histogram->Begin();
   ImageToHistogramFilterType::HistogramType::ConstIterator end = histogram->End();
    const AbsoluteFrequencyType frequency_total = histogram->GetTotalFrequency();
int o=0;
float sum=0;
int sum_offset_freq=0;
 int frequency_val=0;
while(o !=  offset_black_inten ){++o;
 const AbsoluteFrequencyType frequency = itr.GetFrequency();
 frequency_val= frequency;
sum_offset_freq+=frequency_val;
++itr;
}
int freq_total=frequency_total;
int freq_total_value=freq_total-sum_offset_freq;
std::cout << "frequency Total initial Test:" <<freq_total << std::endl;
std::cout << "frequency Total after offset Test:" <<freq_total_value << std::endl;
  while( itr != end )
    {

    const AbsoluteFrequencyType frequency = itr.GetFrequency();
    //histogramFile.write( , sizeof(frequency) );
    if (frequency != 0)
      {
      ImageToHistogramFilterType::HistogramType::IndexType index;
      index = histogram->GetIndex(itr.GetInstanceIdentifier());

      float frequency_value=(frequency);
      
      frequency_measure->PushBack(frequency_value/freq_total_value);
      ImageToHistogramFilterType::HistogramType::MeasurementVectorType mvv;
      freq.push_back(frequency_value/freq_total_value);
       sum+=frequency_value/freq_total_value;
      observ.push_back(index[0]*normalize);
     mvv= histogram->GetMeasurementVector(itr.GetInstanceIdentifier());
      sample->PushBack(mvv[0]);  
//std::cout << "sample :" <<frequency_value << std::endl;
//std::cout << "sample2 :" <<freq_total_value << std::endl;
    }
    
    ++itr;
    }
std::cout << "Sum frequency Test:" <<sum << std::endl;

}





