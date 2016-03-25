#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME linear)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/Base.cpp
         src/DoubleMatrix.cpp
         src/DoubleSubvector.cpp
         src/DoubleVector.cpp
         src/IMatrix.cpp
         src/IVector.cpp)

set (INCLUDE include/Base.h
             include/BiMatrix.h
             include/DoubleMatrix.h
             include/DoubleSubvector.h
             include/DoubleVector.h
             include/IMatrix.h
             include/IndexValue.h
             include/IVector.h
             include/RowMatrix.h)

set (TCC tcc/BiMatrix.tcc
         tcc/DoubleVector.tcc
         tcc/RowMatrix.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../utilities/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

set (TEST_SRC test/src/main.cpp)
set (TEST_INCLUDE )

include_directories(../testing/include)
include_directories(test/include)

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})

target_link_libraries(${TEST_NAME} linear testing)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
