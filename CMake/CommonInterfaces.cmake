#
# CMake macro to create swig-generated language wrapper for Embedded Machine Learning Library
#

macro(generate_interface LANGUAGE_NAME LANGUAGE_DIR LANGUAGE_LIBRARIES EXTRA_INTERFACE)

cmake_minimum_required(VERSION 2.8.11)
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# set compiler SWIG generated cxx compiler flags
set(CMAKE_CXX_FLAGS ${SWIG_CXX_COMPILER_FLAGS})

# unset any release or distribution flags
# we don't want them when compiling SWIG generated source
#set(CMAKE_CXX_FLAGS_RELEASE "")
#set(CMAKE_CXX_FLAGS_DISTRIBUTION "")
#set(CMAKE_CXX_FLAGS_DEBUG "")

set (module_name EMLL_${LANGUAGE_NAME})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common/include)

set (INTERFACE_SRC ../common/src/DataLoadersInterface.cpp
                   ../common/src/LoadModelInterface.cpp
                   ../common/src/MapInterface.cpp
                   ../common/src/ModelInterface.cpp
                   ../common/src/RowDatasetInterface.cpp)

set (INTERFACE_INCLUDE ../common/include/DataLoadersInterface.h
                       ../common/include/LoadModelInterface.h
                       ../common/include/MapInterface.h
                       ../common/include/ModelInterface.h
                       ../common/include/RowDatasetInterface.h)

set (INTERFACE_MAIN ../common/EMLL.i)

set (INTERFACE_FILES ../common/common.i
                     ../common/dataset.i
                     ../common/layers.i
                     ../common/linear.i
                     ../common/lossFunctions.i
                     ../common/noncopyable.i
                     ../common/trainers.i
                     ../common/predictors.i
                     ../common/unique_ptr.i
                     ../common/utilities.i)     

source_group("src" FILES ${INTERFACE_SRC})
source_group("include" FILES ${INTERFACE_INCLUDE})
source_group("tcc" FILES ${INTERFACE_TCC})
source_group("interface" FILES ${INTERFACE_MAIN} ${INTERFACE_FILES})

if(${LANGUAGE_NAME} STREQUAL "common")
    find_file(THIS_FILE_PATH CommonInterfaces.cmake PATHS ${CMAKE_MODULE_PATH})
    add_custom_target(${module_name} ALL DEPENDS ${INTERFACE_SRC} ${INTERFACE_INCLUDE} ${INTERFACE_MAIN} ${INTERFACE_FILES} SOURCES ${INTERFACE_SRC} ${INTERFACE_INCLUDE} ${INTERFACE_MAIN} ${INTERFACE_FILES} ${THIS_FILE_PATH})

else()

# Add EMLL library include directories
include_directories(../../libraries/common/include)
include_directories(../../libraries/dataset/include)
include_directories(../../libraries/layers/include)
include_directories(../../libraries/linear/include)
include_directories(../../libraries/lossFunctions/include)
include_directories(../../libraries/trainers/include)
include_directories(../../libraries/predictors/include)
include_directories(../../libraries/utilities/include)

# FOREACH(file ${INTERFACE_FILES} ${INTERFACE_MAIN})
# 	get_filename_component(fname ${file} NAME)
# 	add_custon_command(OUTPUT ${fname}
# 		DEPENDS ${file}
# 		COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${file} ${fname}
# 		COMMENT ""
# 	)
# ENDFOREACH()

foreach(file ${INTERFACE_FILES})
    configure_file(${file} ${file} COPYONLY)
endforeach()

# -debug-classes -debug-typedef 
set(CMAKE_SWIG_FLAGS -c++ -Fmicrosoft -debug-template)
set(SWIG_MODULE_${module_name}_EXTRA_DEPS ${INTERFACE_FILES} ${EXTRA_INTERFACE})

# set_source_files_properties(${INTERFACE_MAIN} PROPERTIES OBJECT_DEPENDS ${INTERFACE_FILES}) # Doesn't seem to work
# set_source_files_properties(${INTERFACE_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties(${INTERFACE_MAIN} ${INTERFACE_FILES} PROPERTIES CPLUSPLUS ON)
# set_source_files_properties(${INTERFACE_FILES} PROPERTIES SWIG_FLAGS "-includeall") # Don't want this, I think

message(STATUS "Creating wrappers for ${LANGUAGE_NAME}")

# create target here
if(${LANGUAGE_NAME} STREQUAL "python")
    SET(PREPEND_TARGET "_")
endif()

swig_add_module(${module_name} ${LANGUAGE_NAME} ${INTERFACE_MAIN} ${INTERFACE_SRC} ${INTERFACE_INCLUDE} ${EXTRA_INTERFACE})
swig_link_libraries(${module_name} ${LANGUAGE_LIBRARIES} common dataset layers lossFunctions trainers predictors utilities)
set_target_properties(${SWIG_MODULE_${module_name}_REAL_NAME} PROPERTIES OUTPUT_NAME ${PREPEND_TARGET}EMLL)
endif()

set_property(TARGET ${PREPEND_TARGET}${module_name} PROPERTY FOLDER "interfaces") 

endmacro()
