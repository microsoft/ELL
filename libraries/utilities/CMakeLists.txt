#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME utilities)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-std=c++1y)
endif()

set (SRC src/Archiver.cpp
         src/CommandLineParser.cpp
         src/CompressedIntegerList.cpp
         src/ConformingVector.cpp
         src/Files.cpp
         src/Format.cpp
         src/IArchivable.cpp
         src/IntegerList.cpp
         src/IntegerStack.cpp
         src/JsonArchiver.cpp
         src/ObjectArchive.cpp
         src/ObjectArchiver.cpp
         src/OutputStreamImpostor.cpp
         src/RandomEngines.cpp
         src/Tokenizer.cpp
         src/TypeName.cpp
         src/UniqueId.cpp
         src/Variant.cpp
         src/XmlArchiver.cpp)

set (INCLUDE include/AbstractInvoker.h
             include/AnyIterator.h
             include/Archiver.h
             include/CommandLineParser.h
             include/CompressedIntegerList.h
             include/ConformingVector.h
             include/DynamicArray.h
             include/Exception.h
             include/Files.h
             include/Format.h
             include/FunctionUtils.h
             include/IArchivable.h
             include/IIterator.h
             include/IntegerList.h
             include/IntegerStack.h
             include/JsonArchiver.h
             include/ObjectArchive.h
             include/ObjectArchiver.h
             include/OutputStreamImpostor.h
             include/ParallelTransformIterator.h
             include/Parser.h
             include/RandomEngines.h
             include/StlReferenceIterator.h
             include/Tokenizer.h
             include/TransformIterator.h
             include/TupleWrapper.h
             include/TypeFactory.h
             include/TypeName.h
             include/TypeTraits.h
             include/UniqueId.h
             include/Variant.h
             include/XmlArchiver.h)

set (TCC tcc/AbstractInvoker.tcc
         tcc/AnyIterator.tcc
         tcc/Archiver.tcc
         tcc/CommandLineParser.tcc
         tcc/DynamicArray.tcc
         tcc/Exception.tcc
         tcc/Format.tcc
         tcc/FunctionUtils.tcc
         tcc/IArchivable.tcc
         tcc/JsonArchiver.tcc
         tcc/ObjectArchive.tcc
         tcc/ObjectArchiver.tcc
         tcc/OutputStreamImpostor.tcc
         tcc/ParallelTransformIterator.tcc
         tcc/Parser.tcc
         tcc/StlReferenceIterator.tcc
         tcc/TransformIterator.tcc
         tcc/TypeFactory.tcc
         tcc/TypeName.tcc
         tcc/TypeTraits.tcc
         tcc/Variant.tcc
         tcc/XmlArchiver.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${PROJ_NAME} PRIVATE -fPIC)
endif()

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
              test/src/IArchivable_test.cpp
              test/src/Iterator_test.cpp
              test/src/ObjectArchive_test.cpp
              test/src/TypeFactory_test.cpp
              test/src/Variant_test.cpp)

set (TEST_INCLUDE test/include/IArchivable_test.h
                  test/include/Iterator_test.h
                  test/include/ObjectArchive_test.h
                  test/src/TypeFactory_test.cpp
                  test/src/Variant_test.cpp)
                  
source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})
target_include_directories(${TEST_NAME} PRIVATE test/include)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(utilities_test utilities data model nodes testing -pthread)
else()
target_link_libraries(utilities_test utilities data model nodes testing)
endif()
set_property(TARGET utilities_test PROPERTY FOLDER "tests")
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

