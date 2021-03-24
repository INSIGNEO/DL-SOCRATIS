itk_module_test()
set(ITKRacianMixtureModelTests
itkRicianMembershipFunctionTest.cxx
itkRicianMixtureModelComponentTest.cxx
)
CreateTestDriver(ITKRacianMixtureModel  "${ITKRacianMixtureModel-Test_LIBRARIES}" "${ITKRacianMixtureModelTests}")
add_library(ITKRacianMixtureModelTests
itkRicianMembershipFunctionTest.cxx
itkRicianMixtureModelComponentTest.cxx
)
target_link_libraries(ITKRacianMixtureModelTests LINK_PUBLIC ${ITKStatistics_LIBRARIES} ${ITKRicianMM_LIBRARIES})

set(TEMP ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME itkRicianMembershipFunctionTest
      COMMAND ITKStatisticsTestDriver itkRicianMembershipFunctionTest)
itk_add_test(NAME itkRicianMixtureModelComponentTest
      COMMAND ITKStatisticsTestDriver itkRicianMixtureModelComponentTest)
        DATA{${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat})


