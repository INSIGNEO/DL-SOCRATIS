////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#include "EMGaussianRician.h"






void EMGaussianRician::Image_EMGaussianRician( )
{
  // number of classes two Rician-Gaussian
  unsigned int numberOfClasses = 2;
  typedef itk::Vector< double, 1 > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;
  SampleType::Pointer sample = SampleType::New();


 

//Test of code 

  typedef itk::Statistics::NormalVariateGenerator NormalGeneratorType;
  NormalGeneratorType::Pointer normalGenerator = NormalGeneratorType::New();
 
  // Create the first set of 1D Gaussian-Rician samples


  MeasurementVectorType mv;

  normalGenerator->Initialize( 1001 );
  double mean2 = 300;
  double standardDeviation2 = 40;
  double plot_gaussian2[100];
  for ( unsigned int i = 0 ; i < 100 ; ++i )
    {
    mv[0] = ( normalGenerator->GetVariate() * standardDeviation2 ) + mean2;
    plot_gaussian2[i]=( normalGenerator->GetVariate() * standardDeviation2 ) + mean2;
    sample->PushBack( mv );
    std::cout<<mv<<std::endl;
    }
 


  // Create the second set of 1D Rician samples
/*
// rician random values from R rrice (0,1)
double rician[100]={1.32595168, 0.79523587, 2.59137529, 0.87268470 ,1.00340144, 1.33291187,
1.63894369, 0.06776289, 1.82813895, 1.06842387, 1.95264240, 1.05034602, 1.24223891, 0.94968164, 1.48597489, 1.54346281, 0.29663535, 1.02269985, 1.05483959, 1.25625629, 0.99195153, 0.81130926, 0.10702484, 1.58678210, 0.49107290, 1.22695604, 1.74448249, 0.88059886, 0.55515030, 3.52463611, 1.33216412, 1.16967531, 1.81486649, 1.39203280, 0.67988854, 1.43971325, 1.13601666, 0.44968029, 0.90349463, 0.96094478, 0.52436055, 1.15263954, 1.70127752, 2.11401188, 1.32400322, 1.32399818, 0.60253188, 1.21673114, 1.67021605, 1.24561488, 1.80396108, 0.87253365, 1.75820043, 1.57296936, 1.44347780, 2.05198482, 1.16374947, 0.72310030, 2.01443191, 2.10528012, 2.55270383, 0.87903088, 0.49487460, 1.32542813, 0.47653167, 1.84750282,
0.51578235, 2.46453670, 1.15939450, 1.05306040, 1.88142746, 0.95734694, 0.44939267, 0.40580312, 0.79243354, 0.29250500, 1.24200882, 2.06759681, 2.07204079, 1.09612074, 1.51597589, 1.61274952, 1.40238873, 1.76125624, 1.58578425, 1.48518271, 1.69024593, 0.62644497, 2.39996074, 2.06074265, 1.23625016, 0.68873761, 0.61727106, 1.57962348, 0.79268484, 1.07916952,
0.97534468, 1.22397498, 0.30139368, 1.57409580};
*/
// rician random values from R rrice (100,30)
double rician2[100]= {114.658176,  85.993900,  78.242624, 110.863283, 127.910028, 126.160753,
   22.314676,  73.526580,  80.896671, 104.933499,  95.112322,  98.624777,
   92.986117,  87.764781, 133.252378, 112.473603,  64.585292,  62.035496,
  112.377000,  91.814963,  86.200023,  59.941395,  66.957696,  74.347343,
  151.274045, 122.870025,  36.252001, 136.589887, 123.476551, 158.162603,
   71.317008, 116.454262, 139.269862, 109.462639, 103.233896,  82.429538,
  118.611930, 137.360236, 112.202266, 100.072312,  76.533412, 100.007074,
  110.968913, 142.837773, 159.343555,  74.115372, 103.101288,  78.583994,
   80.039048,  49.482259, 128.231799, 108.001442, 127.529344,  59.325704,
   84.376548, 106.668548,  97.496740,  83.667866,  83.496226,  54.960123,
  130.500715, 124.720145,   8.808896, 133.989615,  96.978464, 105.770425,
   98.011364,  38.749382,  90.280326,  76.009409,  43.406708, 163.928955,
  126.877893,  85.527078, 127.574971, 122.297052, 124.871643, 117.270402,
   59.822380,  92.033397, 133.380749, 115.359724,  71.382727, 115.655050,
  105.113354,  98.417695,  63.572797, 121.026638, 143.108468,  94.266815,
  104.410879, 160.588355, 109.432177, 158.770455, 105.979943,  79.150782,
  125.387971, 113.714105,  98.752931,  82.658490};



  double plot_rician[100];
  for ( unsigned int i = 0 ; i < 100 ; ++i )
    {
    mv[0] = ( rician2[i]);
    plot_rician[i]=( rician2[i]);
    sample->PushBack( mv );
    }
/* 
 double mean = 0;
  double standardDeviation = 1;
  double plot_rician2[100];
  for ( unsigned int i = 0 ; i < 100 ; ++i )
    {
    mv[0] = ( rician[i]* standardDeviation ) + mean;
    plot_rician2[i]=( rician[i]*standardDeviation ) + mean;
    sample->PushBack( mv );
    }


*/

 
  typedef itk::Array< double > ParametersType;
  ParametersType params( 2 );
 
  // Create the first set of initial Gaussian parameters
  std::vector< ParametersType > initialParameters( numberOfClasses );
  params[0] = 310.0; // mean of dimension 1
  params[1] = 1050.0; // covariance(0,0)
  initialParameters[0] = params;
 
  // Create the second set of initial Rician parameters
  params[0] = 110.0; // mean of dimension 1
  params[1] = 800.0; // covariance(0,0)
  initialParameters[1] = params;
 

 ///////////////////////////////////////////////////////////////////////////////////
  typedef itk::Statistics::GaussianMixtureModelComponent<SampleType > ComponentType;
  typedef itk::Statistics::RicianMixtureModelComponent< SampleType > ComponentType2;

  // Create the Gausian components
  std::vector< ComponentType::Pointer > components;
  
    components.push_back( ComponentType::New() );
    (components[0])->SetSample( sample );
    (components[0])->SetParameters( initialParameters[0] );


  // Create the Rician components
  std::vector< ComponentType2::Pointer > components2;

     components2.push_back( ComponentType2::New() );
    (components2[1])->SetSample( sample );
    (components2[1])->SetParameters( initialParameters[1] );


 /////////////////////////////////////////////////////////////////////////////

  typedef itk::Statistics::ExpectationMaximizationMixtureModelEstimator<
    SampleType > EstimatorType;
  EstimatorType::Pointer estimator = EstimatorType::New();
 
  estimator->SetSample( sample );
  estimator->SetMaximumIteration( 20 );
 
  itk::Array< double > initialProportions(numberOfClasses);
  initialProportions[0] = 0.8;
 initialProportions[1] = 0.2;
 
  estimator->SetInitialProportions( initialProportions );
  estimator->AddComponent( (ComponentType::Superclass*) (components[0]).GetPointer()) ;
   estimator->AddComponent( (ComponentType2::Superclass*) (components2[1]).GetPointer()) ;  
  estimator->Update();
  // Output the results

    std::cout << "Cluster[" << 0 << "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components[0])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[0] << std::endl;
    
    std::cout << "Cluster[" << 1 << "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components2[1])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[1] << std::endl;
    
 
}
