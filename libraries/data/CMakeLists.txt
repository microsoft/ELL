#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME data)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/Dataset.cpp
         src/DataVectorOperators.cpp
         src/SequentialLineIterator.cpp
         src/SparseEntryParser.cpp
         src/Example.cpp)

set (INCLUDE include/AutoDataVector.h
             include/DenseDataVector.h
             include/Example.h
             include/ExampleIterator.h
             include/DataVector.h
             include/DataVectorOperators.h
             include/ParsingExampleIterator.h
             include/Dataset.h
             include/IndexValue.h
             include/SequentialLineIterator.h
             include/SparseBinaryDataVector.h
             include/SparseDataVector.h
             include/SparseEntryParser.h
             include/StlIndexValueIterator.h
             include/SupervisedExampleBuilder.h
             include/TransformingIndexValueIterator.h)

set (TCC tcc/AutoDataVector.tcc
         tcc/DataVector.tcc
         tcc/DenseDataVector.tcc
         tcc/Example.tcc
         tcc/ExampleIterator.tcc
         tcc/ParsingExampleIterator.tcc
         tcc/Dataset.tcc
         tcc/SparseBinaryDataVector.tcc
         tcc/SparseDataVector.tcc
         tcc/StlIndexValueIterator.tcc
         tcc/SupervisedExampleBuilder.tcc
         tcc/TransformingIndexValueIterator.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${PROJ_NAME} PRIVATE -fPIC)
endif()
target_link_libraries(${PROJ_NAME} math utilities)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

set (TEST_SRC test/src/main.cpp
              test/src/Dataset_test.cpp
              test/src/DataVector_test.cpp
              test/src/Example_test.cpp)

set (TEST_INCLUDE test/include/Dataset_test.h
                  test/include/DataVector_test.h
                  test/include/Example_test.h)

source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})


add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})
target_include_directories(${TEST_NAME} PRIVATE test/include)
target_link_libraries(${TEST_NAME} data testing utilities)
copy_shared_libraries(${TEST_NAME} $<TARGET_FILE_DIR:${TEST_NAME}>)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
