#
# cmake file for mappings library
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(utilities)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/BinaryClassificationEvaluator.cpp
                   src/CommandLineParser.cpp
                   src/JsonSerializer.cpp
                   src/files.cpp
                   src/randomEngines.cpp
                   ../amalgamated_jsoncpp/jsoncpp.cpp)

set (INCLUDE include/BinaryClassificationEvaluator.h
                       include/CommandLineParser.h
                       include/JsonSerializer.h
                       include/Parser.h
                       include/SharedArguments.h
                       include/files.h
                       include/randomEngines.h)

set (TCC tcc/BinaryClassificationEvaluator.tcc
                   tcc/CommandLineParser.tcc
                   tcc/JsonSerializer.tcc
                   tcc/Parser.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../common/include)

add_library(utilities ${SRC} ${INCLUDE} ${TCC})