#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME common)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/DataLoadArguments.cpp
         src/DataSaveArguments.cpp
         src/DataLoaders.cpp
         src/EvaluatorArguments.cpp
         src/IsNodeCompilable.cpp
         src/LoadModel.cpp
         src/MakeTrainer.cpp
         src/MapLoadArguments.cpp
         src/MapSaveArguments.cpp
         src/ModelLoadArguments.cpp
         src/ModelSaveArguments.cpp
         src/MultiEpochIncrementalTrainerArguments.cpp
         src/SGDIncrementalTrainerArguments.cpp
         src/ForestTrainerArguments.cpp
         src/TrainerArguments.cpp
)

set (INCLUDE include/DataLoadArguments.h
             include/DataSaveArguments.h
             include/DataLoaders.h
             include/EvaluatorArguments.h
             include/IsNodeCompilable.h
             include/LoadModel.h
             include/MakeEvaluator.h
             include/MakeTrainer.h
             include/MapLoadArguments.h
             include/MapSaveArguments.h
             include/ModelLoadArguments.h
             include/ModelSaveArguments.h
             include/MultiEpochIncrementalTrainerArguments.h
             include/ParametersEnumerator.h
             include/SGDIncrementalTrainerArguments.h
             include/ForestTrainerArguments.h
             include/TrainerArguments.h
)

set (TCC tcc/IsNodeCompilable.tcc
         tcc/DataLoaders.tcc
         tcc/MakeEvaluator.tcc
         tcc/ParametersEnumerator.tcc
)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${PROJ_NAME} PRIVATE -fPIC)
endif()
target_link_libraries(${PROJ_NAME} data linear utilities lossFunctions model nodes predictors evaluators trainers)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

set (TEST_SRC test/src/main.cpp
              test/src/LoadModel_test.cpp)
set (TEST_INCLUDE test/include/LoadModel_test.h)

source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})


add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})
target_include_directories(${TEST_NAME} PRIVATE test/include)
target_link_libraries(${TEST_NAME} common data linear testing utilities lossFunctions predictors trainers model nodes)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
