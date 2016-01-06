#
# cmake file for dataset library
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(dataset)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")


set (SRC src/CompressedIntegerList.cpp
                 src/DenseDatavector.cpp
                 src/IntegerList.cpp
                 src/OnesDatavector.cpp
                 src/SequentialLineIterator.cpp
                 src/SparseBinaryDatavector.cpp
                 src/SparseDatavector.cpp
                 src/SparseEntryParser.cpp
                 src/SupervisedExample.cpp
                 src/ZeroDatavector.cpp)

set (INCLUDE include/CompressedIntegerList.h
                     include/DatavectorBuilder.h
                     include/DenseDatavector.h
                     include/IDataVector.h
                     include/IntegerList.h
                     include/MappedParser.h
                     include/OnesDatavector.h
                     include/ParsingIterator.h
                     include/SequentialLineIterator.h
                     include/SparseBinaryDatavector.h
                     include/SparseDatavector.h
                     include/SparseEntryParser.h
                     include/SupervisedExample.h
                     include/SupervisedExampleBuilder.h
                     include/ZeroDatavector.h)

set (TCC tcc/DataVectorBuilder.tcc
                 tcc/DenseDataVector.tcc
                 tcc/MappedParser.tcc
                 tcc/ParsingIterator.tcc
                 tcc/SparseBinaryDataVector.tcc
                 tcc/SparseDataVector.tcc
                 tcc/SupervisedExampleBuilder.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../linear/include)
include_directories(../layers/include)
include_directories(../types/include)
include_directories(../utilities/include)

add_library(dataset ${SRC} ${INCLUDE} ${TCC})
