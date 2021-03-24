////////////////////////////////////////////////////////////////////////////////////

//Copyright (C) 2018 Michail Mamalakis

//Author: Michail Mamalakis <mmamalakis1@sheffield.ac.uk>
//Maintainer: Michail Mamalakis <mixalis.mamalakis@gmail.com>
//Keywords: Segmentations scars pipeline
//Version: 0.2

//////////////////////////////////////////////////////////////////////////////////

#ifndef RGhistogram_test_h
#define RGhistogram_test_h

#include "HistogramGaussianRician.h"

#include <iostream>

class RGhistogram_test
{
public:

RGhistogram_test(const char * inputFileName, const char * bins){
//Histogramm RG Mixture Model
std::shared_ptr<HistogramGaussianRician > HGR =std::make_shared<HistogramGaussianRician>(inputFileName,bins );
HGR->print=true;
HGR->Update();




//Histogramm RRG Mixture Model
std::shared_ptr<HistogramGaussianRician > HGR1 =std::make_shared<HistogramGaussianRician>(inputFileName,bins );

HGR1->RRG=true;
HGR1->print=true;
HGR1->Update();



//Histogramm RGG Mixture Model
std::shared_ptr<HistogramGaussianRician > HGR2 =std::make_shared<HistogramGaussianRician>(inputFileName,bins );

HGR2->RGG=true;
HGR2->print=true;
HGR2->Update();


//Histogramm RRGG Mixture Model
std::shared_ptr<HistogramGaussianRician > HGR3 =std::make_shared<HistogramGaussianRician>(inputFileName,bins );
HGR3->Reset();
HGR3->RRGG=true;
HGR3->print=true;
HGR3->Update();



///////////////////////////////////no maximun initial conditions////////////////////////////
std::shared_ptr<HistogramGaussianRician > HGR4 =std::make_shared<HistogramGaussianRician>(inputFileName,bins );
HGR4->Reset();
HGR4->RRG=true;
HGR4->detect_max=false;
HGR4->print=true;
HGR4->Update();



//Histogramm RRG Mixture Model
std::shared_ptr<HistogramGaussianRician > HGR5 =std::make_shared<HistogramGaussianRician>(inputFileName,bins );
HGR5->Reset();
HGR5->RRG=true;
HGR5->detect_max=false;
HGR5->print=true;
HGR5->Update();



//Histogramm RGG Mixture Model
std::shared_ptr<HistogramGaussianRician > HGR6 =std::make_shared<HistogramGaussianRician>(inputFileName,bins );
HGR6->Reset();
HGR6->RGG=true;
HGR6->detect_max=false;
HGR6->print=true;
HGR6->Update();



//Histogramm RRGG Mixture Model
std::shared_ptr<HistogramGaussianRician > HGR7 =std::make_shared<HistogramGaussianRician>(inputFileName,bins );
HGR7->Reset();
HGR7->RRGG=true;
HGR7->detect_max=false;
HGR7->print=true;
HGR7->Update();

};

~RGhistogram_test(){};
};
#endif
