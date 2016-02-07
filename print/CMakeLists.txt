#
# cmake file for print project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME print)
PROJECT(${PROJ_NAME})

#enable C++11 in GCC, etc
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/LayerLayout.cpp
         src/PrintableInput.cpp
         src/PrintableLayer.cpp
         src/PrintableMap.cpp
         src/PrintableCoordinatewise.cpp
         src/PrintableSum.cpp
         src/main.cpp
         src/SvgHelpers.cpp)

set (INCLUDE include/PrintArguments.h
             include/LayerLayout.h
             include/PrintableLayer.h
             include/PrintableInput.h
             include/PrintableMap.h
             include/PrintableCoordinatewise.h
             include/PrintableSum.h
             include/SvgHelpers.h)
         
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

include_directories(include)
include_directories(../libraries/layers/include)
include_directories(../libraries/types/include)
include_directories(../libraries/utilities/include)
include_directories(../libraries/common/include)

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC} ${INCLUDE})
target_link_libraries(${PROJ_NAME} utilities layers common)

