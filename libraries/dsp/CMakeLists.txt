#
# cmake file
#

set(library_name dsp)

if(MSVC)
  add_compile_options(-D_SCL_SECURE_NO_WARNINGS)
endif()

set(src
  src/Convolution.cpp
  src/FilterBank.cpp
  src/SimpleConvolution.cpp
  src/UnrolledConvolution.cpp
  src/VoiceActivityDetector.cpp
  src/WinogradConvolution.cpp
)

set(include
  include/Convolution.h
  include/FFT.h
  include/FilterBank.h
  include/IIRFilter.h
  include/SimpleConvolution.h
  include/VoiceActivityDetector.h
  include/UnrolledConvolution.h
  include/WindowFunctions.h
  include/WinogradConvolution.h
)

set(py
  py/symbolic.py
  py/winograd.py
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("py" FILES ${py})

add_library(${library_name} ${src} ${include})
target_include_directories(${library_name} PRIVATE include ${ELL_LIBRARIES_DIR})
target_link_libraries(${library_name} math utilities)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# dsp test
#

set(test_name ${library_name}_test)

set(test_src
  test/src/main.cpp
  test/src/CepstrumTestData.cpp
  test/src/ConvolutionTest.cpp
  test/src/ConvolutionTestData.cpp
  test/src/DCTTest.cpp
  test/src/DSPTestData.cpp
  test/src/DSPTestUtilities.cpp
  test/src/FFTTest.cpp
  test/src/FilterTest.cpp
  test/src/HammingWindowCoefficients.cpp
  test/src/MelTest.cpp
  test/src/VoiceActivityDetectorTest.cpp
  test/src/WindowTest.cpp
)

set(test_include
  test/include/CepstrumTestData.h
  test/include/ConvolutionTest.h
  test/include/ConvolutionTestData.h
  test/include/DCTTest.h
  test/include/DSPTestData.h
  test/include/DSPTestUtilities.h
  test/include/FFTTest.h
  test/include/FilterTest.h
  test/include/HammingWindowCoefficients.h
  test/include/MelTest.h
  test/include/VoiceActivityDetectorTest.h
  test/include/WindowTest.h
)

set(test_py
  test/py/generate_test_data.py
)

set(test_data
  test/src/VadData.txt
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})
source_group("py" FILES ${test_py})
source_group("data" FILES ${test_data})

add_executable(${test_name} ${test_src} ${test_include} ${test_data})
target_include_directories(${test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${test_name} dsp testing utilities data common)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

add_custom_target(dspDataFiles ALL DEPENDS ${test_data} SOURCES ${test_data})
add_dependencies(${test_name} dspDataFiles)
set_property(TARGET dspDataFiles PROPERTY FOLDER "examples")
add_custom_command(TARGET dspDataFiles
                POST_BUILD
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMAND ${CMAKE_COMMAND} -E copy ${test_data} ${CMAKE_CURRENT_BINARY_DIR})

#
# dsp timing
#

set(timing_name ${library_name}_timing)

set(timing_src
  test/src/timing_main.cpp
  test/src/ConvolutionTiming.cpp
  test/src/DSPTestUtilities.cpp
)

set(timing_include
  test/include/ConvolutionTiming.h
  test/include/DSPTestUtilities.h
)

set(timing_py
)

source_group("src" FILES ${timing_src})
source_group("include" FILES ${timing_include})
source_group("py" FILES ${timing_py})

add_executable(${timing_name} ${timing_src} ${timing_include} ${include})
target_include_directories(${timing_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${timing_name} dsp testing utilities)
copy_shared_libraries(${timing_name})

set_property(TARGET ${timing_name} PROPERTY FOLDER "tests")

if (PROFILING)
add_test(NAME ${timing_name} COMMAND ${timing_name})
set_test_library_path(${timing_name})
endif()
