#
# CMake macro to create swig-generated language wrapper for Embedded Machine Learning Library
#

# Variables assumed to have been set in parent scope:
# INTERFACE_SRC
# INTERFACE_INCLUDE
# INTERFACE_MAIN  (the main .i file)
# INTERFACE_FILES (the other .i files)
# INTERFACE_DEPENDENCIES

# Also, the include paths are assumed to have been set via include_directories

macro(generate_interface LANGUAGE_NAME LANGUAGE_DIR LANGUAGE_LIBRARIES EXTRA_INTERFACE)

string(TOLOWER "${LANGUAGE_NAME}" language)
  
cmake_minimum_required(VERSION 2.8.11)
find_package(SWIG 3.0 REQUIRED)
include(${SWIG_USE_FILE})

# set compiler SWIG generated cxx compiler flags
set(CMAKE_CXX_FLAGS ${SWIG_CXX_COMPILER_FLAGS})

# unset any release or distribution flags
# we don't want them when compiling SWIG generated source
#set(CMAKE_CXX_FLAGS_RELEASE "")
#set(CMAKE_CXX_FLAGS_DISTRIBUTION "")
#set(CMAKE_CXX_FLAGS_DEBUG "")

set (module_name EMLL_${LANGUAGE_NAME})

source_group("src" FILES ${INTERFACE_SRC})
source_group("include" FILES ${INTERFACE_INCLUDE})
source_group("tcc" FILES ${INTERFACE_TCC})
source_group("interface" FILES ${INTERFACE_MAIN} ${INTERFACE_FILES})

if(${language} STREQUAL "common")
    find_file(THIS_FILE_PATH CommonInterfaces.cmake PATHS ${CMAKE_MODULE_PATH})
    add_custom_target(${module_name} ALL DEPENDS ${INTERFACE_SRC} ${INTERFACE_INCLUDE} ${INTERFACE_MAIN} ${INTERFACE_FILES} SOURCES ${INTERFACE_SRC} ${INTERFACE_INCLUDE} ${INTERFACE_MAIN} ${INTERFACE_FILES} ${THIS_FILE_PATH})

    # Make interface code be dependent on all libraries
    add_dependencies(${module_name} ${INTERFACE_DEPENDENCIES})

else()

include_directories(${EXTRA_INCLUDE_PATHS})

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
set(CMAKE_SWIG_FLAGS -c++ -Fmicrosoft) # for debugging type-related problems, try adding these flags: -debug-typedef  -debug-template)
if(${language} STREQUAL "javascript")
    set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -node -DV8_VERSION=0x032530)
endif()

set(SWIG_MODULE_${module_name}_EXTRA_DEPS ${INTERFACE_FILES} ${EXTRA_INTERFACE})

foreach(file ${INTERFACE_INCLUDE} ${INTERFACE_SRC})
    set_source_files_properties(${INTERFACE_MAIN} PROPERTIES OBJECT_DEPENDS ${file})
endforeach()

# set_source_files_properties(${INTERFACE_MAIN} PROPERTIES OBJECT_DEPENDS ${INTERFACE_INCLUDE}) # Doesn't seem to work
# set_source_files_properties(${INTERFACE_MAIN} PROPERTIES OBJECT_DEPENDS ${INTERFACE_FILES}) # Doesn't seem to work

# set_source_files_properties(${INTERFACE_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties(${INTERFACE_MAIN} ${INTERFACE_FILES} PROPERTIES CPLUSPLUS ON)
# set_source_files_properties(${INTERFACE_FILES} PROPERTIES SWIG_FLAGS "-includeall") # Don't want this, I think

message(STATUS "Creating wrappers for ${LANGUAGE_NAME}")

# create target here
if(${language} STREQUAL "python")
    SET(PREPEND_TARGET "_")
endif()

if( ((${language} STREQUAL "xml") OR (${language} STREQUAL "javascript")))
    SWIG_MODULE_INITIALIZE(${module_name} ${language})
    set(generated_sources)
    foreach(i_file ${INTERFACE_MAIN})
        SWIG_ADD_SOURCE_TO_MODULE(${module_name} generated_source ${i_file})
        list(APPEND generated_sources "${generated_source}")
    endforeach()
    add_custom_target(${module_name} ALL 
        DEPENDS ${generated_sources})
else()
    swig_add_module(${module_name} ${LANGUAGE_NAME} ${INTERFACE_MAIN} ${INTERFACE_SRC}) # ${INTERFACE_INCLUDE} ${EXTRA_INTERFACE})

    swig_link_libraries(${module_name} ${LANGUAGE_LIBRARIES} common dataset evaluators linear lossFunctions model nodes predictors trainers utilities)
    set_target_properties(${SWIG_MODULE_${module_name}_REAL_NAME} PROPERTIES OUTPUT_NAME ${PREPEND_TARGET}EMLL)
    add_dependencies(${SWIG_MODULE_${module_name}_REAL_NAME} EMLL_common)
endif()

endif()

set_property(TARGET ${PREPEND_TARGET}${module_name} PROPERTY FOLDER "interfaces") 

endmacro()
