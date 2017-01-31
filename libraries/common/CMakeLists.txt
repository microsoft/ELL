#
# cmake file
#

set (library_name common)

set (src 
  src/DataLoadArguments.cpp
  src/DataSaveArguments.cpp
  src/DataLoaders.cpp
  src/EvaluatorArguments.cpp
  src/LoadModel.cpp
  src/MakeTrainer.cpp
  src/MapLoadArguments.cpp
  src/MapSaveArguments.cpp
  src/ModelLoadArguments.cpp
  src/ModelSaveArguments.cpp
  src/MultiEpochIncrementalTrainerArguments.cpp
  src/ForestTrainerArguments.cpp
  src/TrainerArguments.cpp
)

set (include 
  include/AppendNodeToModel.h
  include/DataLoadArguments.h
  include/DataSaveArguments.h
  include/DataLoaders.h
  include/EvaluatorArguments.h
  include/LoadModel.h
  include/MakeEvaluator.h
  include/MakeTrainer.h
  include/MapLoadArguments.h
  include/MapSaveArguments.h
  include/ModelLoadArguments.h
  include/ModelSaveArguments.h
  include/MultiEpochIncrementalTrainerArguments.h
  include/ParametersEnumerator.h
  include/ForestTrainerArguments.h
  include/TrainerArguments.h
)

set (tcc 
  tcc/AppendNodeToModel.tcc
  tcc/DataLoaders.tcc
  tcc/MakeEvaluator.tcc
  tcc/ParametersEnumerator.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()
target_link_libraries(${library_name} data utilities lossFunctions model nodes predictors evaluators trainers)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src 
  test/src/main.cpp
  test/src/LoadDataset_test.cpp
  test/src/LoadMap_test.cpp
  test/src/LoadModel_test.cpp
)

set (test_include 
  test/include/LoadDataset_test.h
  test/include/LoadMap_test.h
  test/include/LoadModel_test.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} common data testing utilities lossFunctions predictors trainers model nodes)
copy_shared_libraries(${test_name} $<TARGET_FILE_DIR:${test_name}>)

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
