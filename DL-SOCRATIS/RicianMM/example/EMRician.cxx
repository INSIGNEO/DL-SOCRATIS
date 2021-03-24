////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.1

//////////////////////////////////////////////////////////////////////////////////
#include "EMRician.h"






void EMRician::Image_EMRician( )
{
  // number of classes two Rician-Gaussian
  unsigned int numberOfClasses = 2;
  typedef itk::Vector< double, 1 > MeasurementVectorType;
  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;
  SampleType::Pointer sample = SampleType::New();

  MeasurementVectorType mv;

// rician random values from matlab randn

double rician[100]={1.32595168, 0.79523587, 2.59137529, 0.87268470 ,1.00340144, 1.33291187,
1.63894369, 0.06776289, 1.82813895, 1.06842387, 1.95264240, 1.05034602, 1.24223891, 0.94968164, 1.48597489, 1.54346281, 0.29663535, 1.02269985, 1.05483959, 1.25625629, 0.99195153, 0.81130926, 0.10702484, 1.58678210, 0.49107290, 1.22695604, 1.74448249, 0.88059886, 0.55515030, 3.52463611, 1.33216412, 1.16967531, 1.81486649, 1.39203280, 0.67988854, 1.43971325, 1.13601666, 0.44968029, 0.90349463, 0.96094478, 0.52436055, 1.15263954, 1.70127752, 2.11401188, 1.32400322, 1.32399818, 0.60253188, 1.21673114, 1.67021605, 1.24561488, 1.80396108, 0.87253365, 1.75820043, 1.57296936, 1.44347780, 2.05198482, 1.16374947, 0.72310030, 2.01443191, 2.10528012, 2.55270383, 0.87903088, 0.49487460, 1.32542813, 0.47653167, 1.84750282,
0.51578235, 2.46453670, 1.15939450, 1.05306040, 1.88142746, 0.95734694, 0.44939267, 0.40580312, 0.79243354, 0.29250500, 1.24200882, 2.06759681, 2.07204079, 1.09612074, 1.51597589, 1.61274952, 1.40238873, 1.76125624, 1.58578425, 1.48518271, 1.69024593, 0.62644497, 2.39996074, 2.06074265, 1.23625016, 0.68873761, 0.61727106, 1.57962348, 0.79268484, 1.07916952,
0.97534468, 1.22397498, 0.30139368, 1.57409580};

  double mean2 = 200;
  double standardDeviation2 = 35;
  double plot_rician[100];
  for ( unsigned int i = 0 ; i < 100 ; ++i )
    {
    mv[0] = ( rician[i]* standardDeviation2 ) + mean2;
    plot_rician[i]=( rician[i]*standardDeviation2 ) + mean2;
    sample->PushBack( mv );
    }

  double mean = 100;
  double standardDeviation = 30;
  double plot_rician2[100];
  for ( unsigned int i = 0 ; i < 100 ; ++i )
    {
    mv[0] = ( rician[i]* standardDeviation ) + mean;
    plot_rician2[i]=( rician[i]*standardDeviation ) + mean;
    sample->PushBack( mv );
    std::cout<<mv<<endl;


    }
 
  typedef itk::Array< double > ParametersType;
  ParametersType params( 4 );
 
  // Create the first set of initial Gaussian parameters
  std::vector< ParametersType > initialParameters( numberOfClasses );
  params[0] = 210.0; // mean of dimension 1
  params[1] = 850.0; // covariance(0,0)
  initialParameters[0] = params;
 
  // Create the second set of initial Rician parameters
  params[0] = 110.0; // mean of dimension 1
  params[1] = 800.0; // covariance(0,0)
  initialParameters[1] = params;
 


 ///////////////////////////////////////////////////////////////////////////////////
//  typedef itk::Statistics::GaussianMixtureModelComponent<SampleType > ComponentType;
  typedef itk::Statistics::RicianMixtureModelComponent< SampleType >
    ComponentType;
  // Create the Gausian components
  std::vector< ComponentType::Pointer > components;
  for ( unsigned int i = 0 ; i < numberOfClasses ; i++ )
    {
    components.push_back( ComponentType::New() );
    (components[i])->SetSample( sample );
    (components[i])->SetParameters( initialParameters[i] );
    }

  // Create the Rician components


 /////////////////////////////////////////////////////////////////////////////

  typedef itk::Statistics::ExpectationMaximizationMixtureModelEstimator<
    SampleType > EstimatorType;
  EstimatorType::Pointer estimator = EstimatorType::New();
 
  estimator->SetSample( sample );
  estimator->SetMaximumIteration( 200 );
 
  itk::Array< double > initialProportions(numberOfClasses);
  initialProportions[0] = 0.8;
  initialProportions[1] = 0.2;
 
  estimator->SetInitialProportions( initialProportions );
 
  for ( unsigned int i = 0 ; i < numberOfClasses ; i++)
    {
    estimator->AddComponent( (ComponentType::Superclass*)
                             (components[i]).GetPointer() );
  
    }
 
  estimator->Update();
 
  // Output the results
  for ( unsigned int i = 0 ; i < numberOfClasses ; i++ )
    {
    std::cout << "Cluster[" << i << "]" << std::endl;
    std::cout << "    Parameters:" << std::endl;
    std::cout << "         " << (components[i])->GetFullParameters()
              << std::endl;
    std::cout << "    Proportion: ";
    std::cout << "         " << estimator->GetProportions()[i] << std::endl;
    }
 
 
}
