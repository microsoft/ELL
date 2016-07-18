#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME utilities)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-std=c++1y)
endif()

set (SRC src/CompressedIntegerList.cpp
         src/CommandLineParser.cpp
         src/CompressedIntegerList.cpp
         src/Files.cpp
         src/Format.cpp
         src/IntegerList.cpp
         src/IntegerStack.cpp
         src/ISerializable.cpp
         src/OutputStreamImpostor.cpp
         src/RandomEngines.cpp
         src/UniqueId.cpp
         src/Variant.cpp
         src/XMLSerialization.cpp)

set (INCLUDE include/CompressedIntegerList.h
             include/CommandLineParser.h
             include/Exception.h
             include/Files.h
             include/Format.h
             include/IIterator.h
             include/IntegerList.h
             include/IntegerStack.h
             include/ISerializable.h
             include/OutputStreamImpostor.h
             include/ParallelTransformIterator.h
             include/Parser.h
             include/RandomEngines.h
             include/StlIndexValueIterator.h
             include/StlIterator.h
             include/TransformIterator.h
             include/TypeFactory.h
             include/TypeName.h
             include/UniqueId.h
             include/Variant.h
             include/XMLSerialization.h)

set (TCC tcc/CommandLineParser.tcc
         tcc/Exception.tcc
         tcc/Format.tcc
         tcc/ParallelTransformIterator.tcc
         tcc/Parser.tcc
         tcc/StlIndexValueIterator.tcc
         tcc/StlIterator.tcc
         tcc/TransformIterator.tcc
         tcc/TypeFactory.tcc
         tcc/TypeName.tcc
         tcc/Variant.tcc
         tcc/XMLSerialization.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(${PROJ_NAME} -pthread)
else()
target_link_libraries(${PROJ_NAME})
endif()

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test projects
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

set (TEST_SRC test/src/main.cpp 
              test/src/ISerializable_test.cpp
              test/src/Variant_test.cpp
              test/src/XMLSerialization_test.cpp)

set (TEST_INCLUDE test/include/ISerializable_test.h
                  test/include/Variant_test.h
                  test/include/XMLSerialization_test.h)
                  
source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})
target_include_directories(${TEST_NAME} PRIVATE test/include)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(utilities_test utilities dataset linear layers testing -pthread)
else()
target_link_libraries(utilities_test utilities dataset linear layers testing)
endif()
set_property(TARGET utilities_test PROPERTY FOLDER "tests")
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

