#
# cmake file for print project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(print)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/HorizontalLayout.cpp
               src/PrintableCoordinatewise.cpp
               src/PrintableEmpty.cpp
               src/PrintableInput.cpp
               src/PrintableMap.cpp
               src/PrintableSum.cpp
               src/main.cpp
               src/svgHelpers.cpp)

set (INCLUDE include/CommandLineArguments.h
                   include/HorizontalLayout.h
                   include/IPrintable.h
                   include/PrintableCoordinatewise.h
                   include/PrintableEmpty.h
                   include/PrintableInput.h
                   include/PrintableMap.h
                   include/PrintableSum.h
                   include/svgHelpers.h)
	     
source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

include_directories(include)
include_directories(../libraries/layers/include)
include_directories(../libraries/types/include)
include_directories(../libraries/utilities/include)

add_executable(print ${SRC} ${INCLUDE})
target_link_libraries(print utilities layers)

