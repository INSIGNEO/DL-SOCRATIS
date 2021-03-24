 set(DOCUMENTATION "This module contains the central classes of the ITK
  toolkit.  They include, basic data structures \(such as Points, Vectors,
  Images, Regions\) the core of the process objects \(such as base
  classes for image filters\) the pipeline infrastructure classes, the support
  for multi-threading, and a collection of classes that isolate ITK from
  platform specific features. It is anticipated that most other ITK modules will
  depend on this one.")
  
 
itk_module( ITKRicianMM
  ENABLE_SHARED
  DEPENDS
    ITKCommon
    ITKNetlib
  DESCRIPTION
    "${DOCUMENTATION}"
)
   

