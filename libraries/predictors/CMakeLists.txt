#
# cmake file
#

set(library_name predictors)

set(src
    src/ConstantPredictor.cpp
    src/SingleElementThresholdPredictor.cpp
    src/ProtoNNPredictor.cpp
)

set(include
    include/ConstantPredictor.h
    include/ForestPredictor.h
    include/IPredictor.h
    include/LinearPredictor.h
    include/NeuralNetworkPredictor.h
    include/Normalizer.h
    include/ProtoNNPredictor.h
    include/SignPredictor.h
    include/SingleElementThresholdPredictor.h
)

set(neural_include
    neural/include/Activation.h
    neural/include/ActivationLayer.h
    neural/include/BatchNormalizationLayer.h
    neural/include/BiasLayer.h
    neural/include/BinaryConvolutionalLayer.h
    neural/include/ConvolutionalLayer.h
    neural/include/FullyConnectedLayer.h
    neural/include/HardSigmoidActivation.h
    neural/include/HardTanhActivation.h
    neural/include/InputLayer.h
    neural/include/Layer.h
    neural/include/LeakyReLUActivation.h
    neural/include/MaxPoolingFunction.h
    neural/include/MeanPoolingFunction.h
    neural/include/ParametricReLUActivation.h
    neural/include/PoolingLayer.h
    neural/include/ReLUActivation.h
    neural/include/RegionDetectionLayer.h
    neural/include/ScalingLayer.h
    neural/include/SigmoidActivation.h
    neural/include/SoftmaxLayer.h
    neural/include/TanhActivation.h
)

set(neural_src
    neural/src/Activation.cpp)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("neural\\include" FILES ${neural_include})
source_group("neural\\src" FILES ${neural_src})

add_library(${library_name} ${src} ${include} ${neural_src} ${neural_include})
target_include_directories(${library_name} PRIVATE include neural/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${library_name} data dsp math utilities value emittable_functions)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set(test_name ${library_name}_test)

set(test_src
    test/src/ForestPredictorTests.cpp
    test/src/ProtoNNPredictorTests.cpp
    test/src/main.cpp
)

set(test_include
    test/include/ForestPredictorTests.h
    test/include/LinearPredictorTests.h
    test/include/NeuralNetworkPredictorTests.h
    test/include/ProtoNNPredictorTests.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include})
target_include_directories(${test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})

target_link_libraries(${test_name} common data predictors testing value emittable_functions)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

# MSVC emits warnings incorrectly when mixing inheritance, templates,
# and member function definitions outside of class definitions
if(MSVC)
    target_compile_options(${library_name} PRIVATE /wd4505)
    target_compile_options(${test_name} PRIVATE /wd4505)
endif()
