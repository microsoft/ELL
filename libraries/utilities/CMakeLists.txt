#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME utilities)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-std=c++1y)
endif()

set (SRC src/BinaryClassificationEvaluator.cpp
                   src/CommandLineParser.cpp
                   src/files.cpp
                   src/IntegerStack.cpp
                   src/JsonSerializer.cpp
                   src/randomEngines.cpp
                   ../amalgamated_jsoncpp/jsoncpp.cpp)

set (INCLUDE include/BinaryClassificationEvaluator.h
                       include/CommandLineParser.h
                       include/IIterator.h
                       include/IntegerStack.h
                       include/JsonSerializer.h
                       include/ParallelTransformIterator.h
                       include/Parser.h
                       include/TransformIterator.h
                       include/files.h
                       include/Format.h
                       include/StlIteratorAdapter.h
                       include/randomEngines.h)

set (TCC tcc/BinaryClassificationEvaluator.tcc
                   tcc/CommandLineParser.tcc
                   tcc/JsonSerializer.tcc
                   tcc/ParallelTransformIterator.tcc
                   tcc/Parser.tcc
                   tcc/TransformIterator.tcc
                   tcc/Format.tcc
                   tcc/StlIteratorAdapter.tcc)
                   
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../types/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(${PROJ_NAME} -pthread)
else()
target_link_libraries(${PROJ_NAME})
endif()

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

include_directories(../testing/include)

add_executable(${TEST_NAME} test/test.cpp ${INCLUDE})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(${TEST_NAME} dataset linear testing -pthread)
else()
target_link_libraries(${TEST_NAME} dataset linear testing)
endif()

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
