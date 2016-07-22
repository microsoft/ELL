#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME dataset)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/DenseDataVector.cpp
         src/OnesDataVector.cpp
         src/SequentialLineIterator.cpp
         src/SparseBinaryDataVector.cpp
         src/SparseDataVector.cpp
         src/SparseEntryParser.cpp
         src/Example.cpp
         src/ZeroDataVector.cpp)

set (INCLUDE include/DataVectorBuilder.h
             include/DenseDataVector.h
             include/Example.h
             include/IDataVector.h
             include/MappedParser.h
             include/OnesDataVector.h
             include/ParsingIterator.h
             include/RowDataset.h
             include/SequentialLineIterator.h
             include/SparseBinaryDataVector.h
             include/SparseDataVector.h
             include/SparseEntryParser.h
             include/SupervisedExample.h
             include/SupervisedExampleBuilder.h
             include/ZeroDataVector.h)

set (TCC tcc/DataVectorBuilder.tcc
         tcc/DenseDataVector.tcc
         tcc/Example.tcc
         tcc/MappedParser.tcc
         tcc/ParsingIterator.tcc
         tcc/RowDataset.tcc
         tcc/SparseBinaryDataVector.tcc
         tcc/SparseDataVector.tcc
         tcc/SupervisedExample.tcc
         tcc/SupervisedExampleBuilder.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
target_link_libraries(${PROJ_NAME} linear utilities)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

set (TEST_SRC test/src/main.cpp)
set (TEST_INCLUDE )

source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})


add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})
target_include_directories(${TEST_NAME} PRIVATE test/include)
target_link_libraries(${TEST_NAME} dataset linear testing utilities)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
