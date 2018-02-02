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

set(tcc
    tcc/ForestPredictor.tcc
    tcc/LinearPredictor.tcc
    tcc/NeuralNetworkPredictor.tcc
    tcc/Normalizer.tcc
    tcc/SignPredictor.tcc
)

set(neural_include
    neural/include/ActivationLayer.h
    neural/include/BatchNormalizationLayer.h
    neural/include/BiasLayer.h
    neural/include/BinaryConvolutionalLayer.h
    neural/include/ConvolutionalLayer.h
    neural/include/FullyConnectedLayer.h
    neural/include/GRULayer.h
    neural/include/HardSigmoidActivation.h
    neural/include/InputLayer.h
    neural/include/LSTMLayer.h
    neural/include/Layer.h
    neural/include/LeakyReLUActivation.h
    neural/include/MaxPoolingFunction.h
    neural/include/MeanPoolingFunction.h
    neural/include/ParametricReLUActivation.h
    neural/include/PoolingLayer.h
    neural/include/ReLUActivation.h
    neural/include/RecurrentLayer.h
    neural/include/RegionDetectionLayer.h
    neural/include/ScalingLayer.h
    neural/include/SigmoidActivation.h
    neural/include/SoftmaxLayer.h
)

set(neural_src)

set(neural_tcc
    neural/tcc/ActivationLayer.tcc
    neural/tcc/BatchNormalizationLayer.tcc
    neural/tcc/BiasLayer.tcc
    neural/tcc/BinaryConvolutionalLayer.tcc
    neural/tcc/ConvolutionalLayer.tcc
    neural/tcc/FullyConnectedLayer.tcc
    neural/tcc/GRULayer.tcc
    neural/tcc/HardSigmoidActivation.tcc
    neural/tcc/InputLayer.tcc
    neural/tcc/LSTMLayer.tcc
    neural/tcc/Layer.tcc
    neural/tcc/LeakyReLUActivation.tcc
    neural/tcc/MaxPoolingFunction.tcc
    neural/tcc/MeanPoolingFunction.tcc
    neural/tcc/ParametricReLUActivation.tcc
    neural/tcc/PoolingLayer.tcc
    neural/tcc/ReLUActivation.tcc
    neural/tcc/RecurrentLayer.tcc
    neural/tcc/RegionDetectionLayer.tcc
    neural/tcc/ScalingLayer.tcc
    neural/tcc/SigmoidActivation.tcc
    neural/tcc/SoftmaxLayer.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("neural\\include" FILES ${neural_include})
source_group("neural\\src" FILES ${neural_src})
source_group("neural\\tcc" FILES ${neural_tcc})

add_library(${library_name} ${src} ${include} ${tcc} ${neural_src} ${neural_include} ${neural_tcc})
target_include_directories(${library_name} PUBLIC include neural/include)
target_link_libraries(${library_name} data utilities)

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

set(test_tcc
    test/tcc/LinearPredictorTests.tcc
    test/tcc/NeuralNetworkPredictorTests.tcc
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})
source_group("tcc" FILES ${test_tcc})

add_executable(${test_name} ${test_src} ${test_include} ${test_tcc} ${include} ${tcc} ${neural_include} ${neural_tcc})
target_include_directories(${test_name} PRIVATE test/include)

target_link_libraries(${test_name} data predictors testing)
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

