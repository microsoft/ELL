#
# cmake file
#

set(library_name common)

set(src
    src/DataLoadArguments.cpp
    src/DataSaveArguments.cpp
    src/DataLoaders.cpp
    src/EvaluatorArguments.cpp
    src/LoadModel.cpp
    src/MakeTrainer.cpp
    src/MapCompilerArguments.cpp
    src/MapLoadArguments.cpp
    src/MapSaveArguments.cpp
    src/ModelLoadArguments.cpp
    src/ModelSaveArguments.cpp
    src/ForestTrainerArguments.cpp
    src/RegisterNodeCreators.cpp
    src/TrainerArguments.cpp
    src/ProtoNNTrainerArguments.cpp
)

set(include
    include/AppendNodeToModel.h
    include/DataLoadArguments.h
    include/DataSaveArguments.h
    include/DataLoaders.h
    include/EvaluatorArguments.h
    include/LoadModel.h
    include/MakeEvaluator.h
    include/MakeTrainer.h
    include/MapCompilerArguments.h
    include/MapLoadArguments.h
    include/MapSaveArguments.h
    include/ModelLoadArguments.h
    include/ModelSaveArguments.h
    include/ParametersEnumerator.h
    include/RegisterNodeCreators.h
    include/ForestTrainerArguments.h
    include/TrainerArguments.h
    include/ProtoNNTrainerArguments.h
)

set(tcc
    tcc/AppendNodeToModel.tcc
    tcc/DataLoaders.tcc
    tcc/LoadModel.tcc
    tcc/MakeEvaluator.tcc
    tcc/ParametersEnumerator.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} data utilities functions model nodes predictors evaluators trainers)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

get_target_property(library_sources ${library_name} SOURCES)

#
# test project
#

set(test_name ${library_name}_test)

set(test_src
    test/src/main.cpp
    test/src/LoadDataset_test.cpp
    test/src/LoadMap_test.cpp
    test/src/LoadModel_test.cpp
    test/src/LoadTestModels.cpp
)

set(test_include
    test/include/LoadDataset_test.h
    test/include/LoadMap_test.h
    test/include/LoadModel_test.h
    test/include/LoadTestModels.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include})
add_dependencies(${test_name} makeExamples)
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} common data testing utilities functions predictors trainers model nodes)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name} ${CMAKE_BINARY_DIR}/examples)
set_test_library_path(${test_name})

