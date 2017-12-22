#
# CMake macros to create swig-generated language wrappers for Embedded Learning Library
#

# NOTE: Interfaces are not part of the ALL target, they must be built explicitly.
# On Windows, this can be done by right-clicking on the specific language wrapper
# project and choosing *Build*. e.g. _ELL_python project.
# On Linux and Mac, this can be done by call *make* on the specific language wrapper e.g.
# make _ELL_python

cmake_minimum_required(VERSION 2.8.11)

set(GLOBAL_BIN_DIR "${CMAKE_BINARY_DIR}/bin")
if(WIN32)
set(GLOBAL_BIN_DIR "${CMAKE_BINARY_DIR}/bin/release")
endif()

#
# Common macro to create swig-generated language wrappers
#
# Variables assumed to have been set in parent scope:
# INTERFACE_SRC
# INTERFACE_INCLUDE
# INTERFACE_TCC
# INTERFACE_MAIN  (the main .i file)
# INTERFACE_FILES (the other .i files)
# INTERFACE_DEPENDENCIES
# INTERFACE_LIBRARIES (language-independent libraries)

# Also, the include paths are assumed to have been set via include_directories

macro(generate_interface_module MODULE_NAME TARGET_NAME LANGUAGE_NAME LANGUAGE_DIR LANGUAGE_LIBRARIES EXTRA_INTERFACE)

  string(TOLOWER "${LANGUAGE_NAME}" language)

  if(SWIG_FOUND)
    include(${SWIG_USE_FILE})
  else()
    # Patch up values that SWIG would normally generate
    if (${language} STREQUAL "python")
      set(SWIG_MODULE_${MODULE_NAME}_REAL_NAME _${MODULE_NAME})
    else()
      set(SWIG_MODULE_${MODULE_NAME}_REAL_NAME ${MODULE_NAME})
    endif()
  endif()

  # set compiler SWIG generated cxx compiler flags
  set(CMAKE_CXX_FLAGS ${SWIG_CXX_COMPILER_FLAGS})

  # unset any release or distribution flags
  # we don't want them when compiling SWIG generated source
  #set(CMAKE_CXX_FLAGS_RELEASE "")
  #set(CMAKE_CXX_FLAGS_DISTRIBUTION "")
  #set(CMAKE_CXX_FLAGS_DEBUG "")

  set(module_name ${MODULE_NAME})

  source_group("src" FILES ${INTERFACE_SRC})
  source_group("include" FILES ${INTERFACE_INCLUDE})
  source_group("tcc" FILES ${INTERFACE_TCC})
  source_group("interface" FILES ${INTERFACE_MAIN} ${INTERFACE_FILES})

  if(${language} STREQUAL "common")
    find_file(THIS_FILE_PATH CommonInterfaces.cmake PATHS ${CMAKE_MODULE_PATH})
    add_custom_target(${module_name} DEPENDS ${INTERFACE_SRC} ${INTERFACE_INCLUDE} ${INTERFACE_TCC} ${INTERFACE_MAIN} ${INTERFACE_FILES} SOURCES ${INTERFACE_SRC} ${INTERFACE_INCLUDE} ${INTERFACE_TCC} ${INTERFACE_MAIN} ${INTERFACE_FILES} ${THIS_FILE_PATH})

    # Make interface code be dependent on all libraries
    add_dependencies(${module_name} ${INTERFACE_DEPENDENCIES})

  else()

    include_directories(${EXTRA_INCLUDE_PATHS})

    foreach(file ${INTERFACE_FILES} ${INTERFACE_SRC} ${INTERFACE_INCLUDE} ${INTERFACE_TCC})
        configure_file(${file} ${file} COPYONLY)
    endforeach()

    set(CMAKE_SWIG_FLAGS -c++ -Fmicrosoft) # for debugging type-related problems, try adding these flags: -debug-classes -debug-typedef  -debug-template)
    if(${language} STREQUAL "javascript")
      # Note: if compiling against older version of node, we may have to specify the
      # V8 version explicitly. For instance, when building against electron 0.36.7,
      # add this flag to the CMAKE_SWIG_FLAGS: -DV8_VERSION=0x032530
      set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -node)
    endif()

    if(${language} STREQUAL "python")
      set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -py3)

      # Fix link problems when building in Debug mode:
      add_definitions(-DSWIG_PYTHON_INTERPRETER_NO_DEBUG)
    endif()

    set(SWIG_MODULE_${module_name}_EXTRA_DEPS ${INTERFACE_FILES} ${INTERFACE_SRC} ${INTERFACE_INCLUDE} ${INTERFACE_TCC} ${EXTRA_INTERFACE})

    foreach(file ${INTERFACE_INCLUDE} ${INTERFACE_SRC})
      set_source_files_properties(${INTERFACE_MAIN} PROPERTIES OBJECT_DEPENDS ${file})
    endforeach()

    foreach(file ${INTERFACE_INCLUDE} ${INTERFACE_TCC})
      set_source_files_properties(${INTERFACE_MAIN} PROPERTIES OBJECT_DEPENDS ${file})
    endforeach()

    set_source_files_properties(${INTERFACE_MAIN} ${INTERFACE_FILES} PROPERTIES CPLUSPLUS ON)

    message(STATUS "Creating wrappers for ${LANGUAGE_NAME}")
    
    # create target here
    if(${language} STREQUAL "python")
      # Python needs an underscore on the module name because when you run "import ell" the Python loader looks for "_ell.pyd" on Windows and "_ell.so" on Unix.
      SET(PREPEND_TARGET "_")
    endif()

    if( ((${language} STREQUAL "xml") OR (${language} STREQUAL "javascript")))

      if(SWIG_FOUND)
        SWIG_MODULE_INITIALIZE(${module_name} ${language})
        set(generated_sources)
        foreach(i_file ${INTERFACE_MAIN})
            SWIG_ADD_SOURCE_TO_MODULE(${module_name} generated_source ${i_file})
            list(APPEND generated_sources "${generated_source}")
        endforeach()
      endif()
      add_custom_target(${module_name}
        DEPENDS ${generated_sources})
    else()
      if(SWIG_FOUND)
        swig_add_module(${module_name} ${LANGUAGE_NAME} ${INTERFACE_MAIN} ${INTERFACE_SRC} ${INTERFACE_INCLUDE}) # ${EXTRA_INTERFACE})
        swig_link_libraries(${module_name} ${LANGUAGE_LIBRARIES} ${INTERFACE_LIBRARIES} common evaluators functions model nodes predictors trainers utilities emitters math)

      else()
        add_custom_target(${PREPEND_TARGET}${module_name}
          DEPENDS ${generated_sources})
      endif()
      set_target_properties(${SWIG_MODULE_${module_name}_REAL_NAME} PROPERTIES OUTPUT_NAME ${PREPEND_TARGET}${TARGET_NAME})
      set_target_properties(${SWIG_MODULE_${module_name}_REAL_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
      add_dependencies(${SWIG_MODULE_${module_name}_REAL_NAME} ELL_common)
      copy_shared_libraries(${PREPEND_TARGET}${module_name})
    endif()
  endif()

  set_property(TARGET ${PREPEND_TARGET}${module_name} PROPERTY FOLDER "interfaces/${language}")
  set(SWIG_MODULE_TARGET ${SWIG_MODULE_${module_name}_REAL_NAME})

endmacro() # generate_interface_module

#
# Macro to create swig-generated language wrappers for host models
#
# Variables assumed to have been set in parent scope:
# INTERFACE_SRC
# INTERFACE_INCLUDE
# INTERFACE_TCC
# INTERFACE_MAIN  (the main .i file)
# INTERFACE_FILES (the other .i files)
# INTERFACE_DEPENDENCIES
# INTERFACE_LIBRARIES (language-independent libraries)

macro(generate_interface LANGUAGE_NAME MODULE_NAME LANGUAGE_DIR LANGUAGE_LIBRARIES EXTRA_INTERFACE)
  generate_interface_module("ELL_${LANGUAGE_NAME}" "${MODULE_NAME}" "${LANGUAGE_NAME}" "${LANGUAGE_DIR}" "${LANGUAGE_LIBRARIES}" "${EXTRA_INTERFACE}")
endmacro()

#

