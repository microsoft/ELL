#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME layers)
PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/BinaryOpLayer.cpp
         src/ConstantLayer.cpp
         src/Coordinate.cpp
         src/CoordinateList.cpp
         src/CoordinateListTools.cpp
         src/Coordinatewise.cpp
         src/DecisionTreePath.cpp
         src/Layer.cpp
         src/Map.cpp
         src/Model.cpp
         src/ShiftRegisterLayer.cpp
         src/Sum.cpp
         src/UnaryOpLayer.cpp)

set (INCLUDE include/BinaryOpLayer.h
             include/ConstantLayer.h
             include/Coordinate.h
             include/CoordinateList.h
             include/CoordinateListTools.h
             include/Coordinatewise.h
             include/DecisionTreePath.h
             include/Layer.h
             include/Map.h
             include/Model.h
             include/ShiftRegisterLayer.h
             include/Sum.h
             include/UnaryOpLayer.h)

set (TCC tcc/Map.tcc
         tcc/Model.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../layers/include)
include_directories(../linear/include)
include_directories(../utilities/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

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

include_directories(../testing/include)
include_directories(test/include)

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})

target_link_libraries(${TEST_NAME} layers testing utilities)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
