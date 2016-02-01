#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME common)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/CoordinateListTools.cpp
         src/DataLoadArguments.cpp
         src/DataSaveArguments.cpp
         src/Loaders.cpp
         src/MapLoadArguments.cpp
         src/MapSaveArguments.cpp)

set (INCLUDE include/CoordinateListTools.h
             include/DataLoadArguments.h
             include/DataSaveArguments.h
             include/Loaders.h
             include/MapLoadArguments.h
             include/MapSaveArguments.h)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../dataset/include)
include_directories(../layers/include)
include_directories(../linear/include)
include_directories(../types/include)
include_directories(../utilities/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_link_libraries(${PROJ_NAME} dataset layers utilities)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")
