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
         src/Files.cpp
         src/Format.cpp
         src/IntegerStack.cpp
         src/OutputStream.cpp
         src/RandomEngines.cpp
         src/XMLSerialization.cpp)

set (INCLUDE include/BinaryClassificationEvaluator.h
             include/CommandLineParser.h
             include/Files.h
             include/Format.h
             include/IIterator.h
             include/IntegerStack.h
             include/OutputStream.h
             include/ParallelTransformIterator.h
             include/Parser.h
             include/RandomEngines.h
             include/StlIterator.h
             include/TransformIterator.h
             include/TypeFactory.h
             include/TypeName.h
             include/XMLSerialization.h)

set (TCC tcc/BinaryClassificationEvaluator.tcc
         tcc/CommandLineParser.tcc
         tcc/Format.tcc
         tcc/ParallelTransformIterator.tcc
         tcc/Parser.tcc
         tcc/StlIterator.tcc
         tcc/TransformIterator.tcc
         tcc/TypeFactory.tcc
         tcc/TypeName.tcc
         tcc/XMLSerialization.tcc)

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
# test projects
#

include_directories(../testing/include)

add_executable(utilities_test test/test.cpp ${INCLUDE})
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(utilities_test utilities dataset linear testing -pthread)
else()
target_link_libraries(utilities_test utilities dataset linear testing)
endif()
set_property(TARGET utilities_test PROPERTY FOLDER "tests")
add_test(NAME utilities_test COMMAND utilities_test)

add_executable(XMLSerialization_test test/XMLSerialization_test.cpp ${INCLUDE})
target_link_libraries(XMLSerialization_test utilities testing)
set_property(TARGET XMLSerialization_test PROPERTY FOLDER "tests")
add_test(NAME XMLSerialization_test COMMAND XMLSerialization_test)
