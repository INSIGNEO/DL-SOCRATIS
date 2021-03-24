//Copyright (c) 2009-2013 ConsortiumOfANTS
//All rights reserved.

#include "evaluate.h"
#include <iostream>



void evaluate::evaluate_results(const char * m_predictFileName, const char * m_labelFileName, std::string metric_store_path){
int m_CSV=1;
 //resize the m_predictFileName image to 512x512
  std::shared_ptr<Resample> Resample1 =std::make_shared<Resample>();
  Resample1->resample(m_predictFileName,512,0);

 //resize the input image to 512x512
  std::shared_ptr<Resample> Resample2 =std::make_shared<Resample>();
  Resample2->resample(m_labelFileName,512,0);

std::cout<<" manual label  path : "<<m_labelFileName <<std::endl;
std::cout<<" predict label  path : "<<m_predictFileName <<std::endl;
//respace the images to be equal 
 std::shared_ptr<Change_information> CI =std::make_shared<Change_information>();
CI ->Change_information_run(m_labelFileName,  m_predictFileName);

CI ->Change_information_2D_similarity_transform(m_predictFileName, center ,  scale,  m_angle );

  bool outputCSVFormat = false;
  if( m_CSV==1  ){outputCSVFormat = true;}

  typedef unsigned int                          PixelType;
  typedef itk::Image<PixelType, 2> ImageType;

  typename ImageType::Pointer sourceImage = ImageType::New();
  ReadImage<ImageType>( sourceImage, m_labelFileName );
  typename ImageType::Pointer targetImage = ImageType::New();
  ReadImage<ImageType>( targetImage, m_predictFileName );

  typedef itk::LabelOverlapMeasuresImageFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetSourceImage( sourceImage );
  filter->SetTargetImage( targetImage );
  filter->Update();

  typename FilterType::MapType labelMap = filter->GetLabelSetMeasures();

  std::vector<int> allLabels;
  allLabels.clear();
  for( typename FilterType::MapType::const_iterator it = labelMap.begin(); it != labelMap.end(); ++it )
    {
    if( (*it).first == 0 )
      {
      continue;
      }

    const int label = (*it).first;
    allLabels.push_back( label );
    }
  std::sort( allLabels.begin(), allLabels.end() );

  if( outputCSVFormat )
    {
    std::vector<std::string>   columnHeaders;

    columnHeaders.emplace_back( "Label" );
    columnHeaders.emplace_back( "Total/Target" );
    columnHeaders.emplace_back( "Jaccard" );
    columnHeaders.emplace_back( "Dice" );
    columnHeaders.emplace_back( "VolumeSimilarity" );
    columnHeaders.emplace_back( "FalseNegative" );
    columnHeaders.emplace_back( "FalsePositive" );

    std::vector<std::string>   rowHeaders;
    rowHeaders.emplace_back( "All" );

    std::vector<int>::const_iterator itL = allLabels.begin();
    while( itL != allLabels.end() )
      {
      std::ostringstream convert; // stream used for the conversion
      convert << *itL;   // insert the textual representation of 'Number' in the characters in the stream
      rowHeaders.push_back( convert.str() );
      ++itL;
      }

    vnl_matrix<double> measures( allLabels.size() + 1, 6 );

    measures( 0, 0 ) = filter->GetTotalOverlap();
    measures( 0, 1 ) = filter->GetUnionOverlap();
    measures( 0, 2 ) = filter->GetMeanOverlap();
    measures( 0, 3 ) = filter->GetVolumeSimilarity();
    measures( 0, 4 ) = filter->GetFalseNegativeError();
    measures( 0, 5 ) = filter->GetFalsePositiveError();

    unsigned int rowIndex = 1;
    for( itL = allLabels.begin(); itL != allLabels.end(); ++itL )
      {
      measures( rowIndex, 0 ) = filter->GetTargetOverlap( *itL );
      measures( rowIndex, 1 ) = filter->GetUnionOverlap( *itL );
      measures( rowIndex, 2 ) = filter->GetMeanOverlap( *itL );
      measures( rowIndex, 3 ) = filter->GetVolumeSimilarity( *itL );
      measures( rowIndex, 4 ) = filter->GetFalseNegativeError( *itL );
      measures( rowIndex, 5 ) = filter->GetFalsePositiveError( *itL );
      rowIndex++;
      }

    typedef itk::CSVNumericObjectFileWriter<double, 1, 1> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( (metric_store_path+"measurement_results.csv").c_str());
    writer->SetColumnHeaders( columnHeaders );
    writer->SetRowHeaders( rowHeaders );
    writer->SetInput( &measures );
    try
      {
      writer->Write();
      }
    catch( itk::ExceptionObject& exp )
      {
      std::cerr << "Exception caught!" << std::endl;
      std::cerr << exp << std::endl;
     
      }
    }
  else
    {
    std::cout << "                                          "
              << "************ All Labels *************" << std::endl;
    std::cout << std::setw( 10 ) << "   "
              << std::setw( 17 ) << "Total"
              << std::setw( 17 ) << "Union (jaccard)"
              << std::setw( 17 ) << "Mean (dice)"
              << std::setw( 17 ) << "Volume sim."
              << std::setw( 17 ) << "False negative"
              << std::setw( 17 ) << "False positive" << std::endl;
    std::cout << std::setw( 10 ) << "   ";
    std::cout << std::setw( 17 ) << filter->GetTotalOverlap();
    std::cout << std::setw( 17 ) << filter->GetUnionOverlap();
    std::cout << std::setw( 17 ) << filter->GetMeanOverlap();
    std::cout << std::setw( 17 ) << filter->GetVolumeSimilarity();
    std::cout << std::setw( 17 ) << filter->GetFalseNegativeError();
    std::cout << std::setw( 17 ) << filter->GetFalsePositiveError();
    std::cout << std::endl;

    std::cout << "                                       "
              << "************ Individual Labels *************" << std::endl;
    std::cout << std::setw( 10 ) << "Label"
              << std::setw( 17 ) << "Target"
              << std::setw( 17 ) << "Union (jaccard)"
              << std::setw( 17 ) << "Mean (dice)"
              << std::setw( 17 ) << "Volume sim."
              << std::setw( 17 ) << "False negative"
              << std::setw( 17 ) << "False positive"
              << std::endl;
    for(int label : allLabels)
      {
      std::cout << std::setw( 10 ) << label;
      std::cout << std::setw( 17 ) << filter->GetTargetOverlap( label );
      std::cout << std::setw( 17 ) << filter->GetUnionOverlap( label );
      std::cout << std::setw( 17 ) << filter->GetMeanOverlap( label );
      std::cout << std::setw( 17 ) << filter->GetVolumeSimilarity( label );
      std::cout << std::setw( 17 ) << filter->GetFalseNegativeError( label );
      std::cout << std::setw( 17 ) << filter->GetFalsePositiveError( label );
      std::cout << std::endl;
      }
    }
dice=filter->GetMeanOverlap();
total=filter->GetVolumeSimilarity();
jaccard=filter->GetUnionOverlap();

}






 //vector of chars!!! feature extend!!!!!!


// entry point for the library; parameter 'args' is equivalent to 'argv' in (argc,argv) of commandline parameters to
// 'main()'
//int LabelOverlapMeasures( std::vector<std::string> args, std::ostream * /*out_stream = nullptr */ )
//{
  // put the arguments coming in as 'args' into standard (argc,argv) format;
  // 'args' doesn't have the command name as first, argument, so add it manually;
  // 'args' may have adjacent arguments concatenated into one argument,
  // which the parser should handle
//  args.insert( args.begin(), "LabelOverlapMeasures" );

 // int     argc = args.size();
 // char* * argv = new char *[args.size() + 1];
 // for( unsigned int i = 0; i < args.size(); ++i )
 //   {
    // allocate space for the string plus a null character
 //   argv[i] = new char[args[i].length() + 1];
 //   std::strncpy( argv[i], args[i].c_str(), args[i].length() );
    // place the null character in the end
 //   argv[i][args[i].length()] = '\0';
  //  }
  //argv[argc] = nullptr;
  // class to automatically cleanup argv upon destruction
 
 // Cleanup_argv cleanup_argv( argv, argc + 1 );

  // antscout->set_stream( out_stream );

 // if( argc < 4 )
 //   {
 //   std::cout << "Usage: " << argv[0] << " imageDimension sourceImage "
  //            << "targetImage [outputCSVFile]" << std::endl;
 //   if( argc >= 2 &&
   //     ( std::string( argv[1] ) == std::string("--help") || std::string( argv[1] ) == std::string("-h") ) )
   //   {
   //   return EXIT_SUCCESS;
   //   }
  //  return EXIT_FAILURE;
  //  }

 // switch( std::stoi( argv[1] ) )
 //   {
 //   case 2:
 //     {
 //     return LabelOverlapMeasures<2>( argc, argv );
 //     }
 //     break;
 //   case 3:
 //     {
 //     return LabelOverlapMeasures<3>( argc, argv );
 //     }
  //    break;
  //  default:
  //    std::cout << "Unsupported dimension" << std::endl;
   //   return EXIT_FAILURE;
  //  }
 // return EXIT_SUCCESS;
//}



