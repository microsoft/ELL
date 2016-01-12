#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

set (PROJ_NAME utilities)

PROJECT(${PROJ_NAME})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/BinaryClassificationEvaluator.cpp
                   src/CommandLineParser.cpp
                   src/DataLoadArguments.cpp
                   src/DataSaveArguments.cpp
                   src/JsonSerializer.cpp
                   src/MapLoadArguments.cpp
                   src/MapSaveArguments.cpp
                   src/files.cpp
                   src/randomEngines.cpp
                   ../amalgamated_jsoncpp/jsoncpp.cpp)

set (INCLUDE include/BinaryClassificationEvaluator.h
                       include/CommandLineParser.h
                       include/DataLoadArguments.h
                       include/DataSaveArguments.h
                       include/JsonSerializer.h
                       include/MapLoadArguments.h
                       include/MapSaveArguments.h
                       include/Parser.h
                       include/files.h
                       include/randomEngines.h)

set (TCC tcc/BinaryClassificationEvaluator.tcc
                   tcc/CommandLineParser.tcc
                   tcc/JsonSerializer.tcc
                   tcc/Parser.tcc
                   tcc/VectorIterator.tcc)
                   
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../types/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")
