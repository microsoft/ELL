#
# cmake file
#

set (library_name predictors)

set (src src/ConstantPredictor.cpp
         src/LinearPredictor.cpp
         src/NeuralNetworkPredictor.cpp
         src/SingleElementThresholdPredictor.cpp)

set (include include/ConstantPredictor.h
             include/ForestPredictor.h
             include/IPredictor.h
             include/LinearPredictor.h
             include/NeuralNetworkPredictor.h
             include/SignPredictor.h
             include/SingleElementThresholdPredictor.h)

set (tcc tcc/SignPredictor.tcc
         tcc/ForestPredictor.tcc)

set (nn_include neural/include/ActivationLayer.h
                neural/include/BatchNormalizationLayer.h
                neural/include/BiasLayer.h
                neural/include/ConvolutionalLayer.h
                neural/include/FullyConnectedLayer.h
                neural/include/ILayer.h
                neural/include/InputLayer.h
                neural/include/LeakyReLUActivation.h
                neural/include/MaxPoolingFunction.h
                neural/include/MeanPoolingFunction.h
                neural/include/PoolingLayer.h
                neural/include/ReLUActivation.h
                neural/include/ScaledInputLayer.h
                neural/include/ScalingLayer.h
                neural/include/SigmoidActivation.h
                neural/include/SoftMaxActivation.h)

set (nn_src neural/src/BatchNormalizationLayer.cpp
            neural/src/BiasLayer.cpp
            neural/src/ConvolutionalLayer.cpp
            neural/src/FullyConnectedLayer.cpp
            neural/src/InputLayer.cpp
            neural/src/LeakyReLUActivation.cpp
            neural/src/MaxPoolingFunction.cpp
            neural/src/MeanPoolingFunction.cpp
            neural/src/ReLUActivation.cpp
            neural/src/ScaledInputLayer.cpp
            neural/src/ScalingLayer.cpp
            neural/src/SigmoidActivation.cpp
            neural/src/SoftMaxActivation.cpp)

set (nn_tcc neural/tcc/ActivationLayer.tcc
            neural/tcc/PoolingLayer.tcc)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("neural\\include" FILES ${nn_include})
source_group("neural\\src" FILES ${nn_src})
source_group("neural\\tcc" FILES ${nn_tcc})

add_library(${library_name} ${src} ${include} ${tcc} ${nn_src} ${nn_include} ${nn_tcc})
target_include_directories(${library_name} PUBLIC include neural/include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()
target_link_libraries(${library_name} data utilities)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src test/src/main.cpp)
set (test_include )

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include} ${tcc} ${nn_include} ${nn_tcc})
target_include_directories(${test_name} PRIVATE test/include)

target_link_libraries(${test_name} data predictors testing)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})

