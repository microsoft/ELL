#
# CMake macros to create swig-generated language wrappers for Embedded Learning Library
#

# NOTE: Interfaces are not part of the ALL target, they must be built explicitly.
# On Windows, this can be done by right-clicking on the specific language wrapper
# project and choosing *Build*. e.g. _ELL_python project.
# On Linux and Mac, this can be done by call *make* on the specific language wrapper e.g.
# make _ELL_python

cmake_minimum_required(VERSION 2.8.11)

if (WIN32 AND NOT SWIG_FOUND) 
  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../external/swigwintools.3.0.12/tools/swigwin-3.0.12/swig.exe")
    set(SWIG_DIR "${CMAKE_CURRENT_LIST_DIR}/../external/swigwintools.3.0.12/tools/swigwin-3.0.12")
    set(SWIG_EXECUTABLE "${SWIG_DIR}/swig.exe")
    set(SWIG_VERSION "3.0.12")
    set(SWIG_FOUND TRUE)
  endif()
endif()
    
find_package(SWIG 3.0.12)

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

  set (module_name ${MODULE_NAME})

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

    foreach(file ${INTERFACE_SRC})
        configure_file(${file} ${file} COPYONLY)
    endforeach()

    foreach(file ${INTERFACE_INCLUDE})
        configure_file(${file} ${file} COPYONLY)
    endforeach()

    foreach(file ${INTERFACE_TCC})
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
        swig_link_libraries(${module_name} ${LANGUAGE_LIBRARIES} ${INTERFACE_LIBRARIES} common evaluators functions model nodes predictors trainers utilities emitters)

      else()
        add_custom_target(${PREPEND_TARGET}${module_name} 
          DEPENDS ${generated_sources})
      endif()
      set_target_properties(${SWIG_MODULE_${module_name}_REAL_NAME} PROPERTIES OUTPUT_NAME ${PREPEND_TARGET}${TARGET_NAME})
      set_target_properties(${SWIG_MODULE_${module_name}_REAL_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
      add_dependencies(${SWIG_MODULE_${module_name}_REAL_NAME} ELL_common)
    endif()
  endif()

  set_property(TARGET ${PREPEND_TARGET}${module_name} PROPERTY FOLDER "interfaces") 
  set(SWIG_MODULE_TARGET ${SWIG_MODULE_${module_name}_REAL_NAME})

endmacro()

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

macro(generate_interface LANGUAGE_NAME LANGUAGE_DIR LANGUAGE_LIBRARIES EXTRA_INTERFACE)
  generate_interface_module("ELL_${LANGUAGE_NAME}" "ELL" "${LANGUAGE_NAME}" "${LANGUAGE_DIR}" "${LANGUAGE_LIBRARIES}" "${EXTRA_INTERFACE}")
endmacro()

#
# Macro to create swig-generated language wrappers for emitted models
#
# Variables assumed to have been set in parent scope:
# INTERFACE_SRC
# INTERFACE_INCLUDE
# INTERFACE_TCC
# INTERFACE_MAIN  (the main .i file)
# INTERFACE_FILES (the other .i files)
# INTERFACE_DEPENDENCIES
# INTERFACE_LIBRARIES (language-independent libraries)

macro(generate_emitted_interface_module MODEL_NAME MODEL_LIBRARIES COMMON_PATH LANGUAGE_NAME SUCCESS)
  set(SUCCESS FALSE)

  # Set variables expected by generate_interface_module
  set (INTERFACE_MAIN ${MODEL_NAME}.i)

  list(APPEND INTERFACE_FILES ${COMMON_PATH}/callback.i
                       ${COMMON_PATH}/callback_javascript_post.i
                       ${COMMON_PATH}/callback_javascript_pre.i
                       ${COMMON_PATH}/callback_python_post.i
                       ${COMMON_PATH}/callback_python_pre.i
                       ${COMMON_PATH}/vector.i)

  list(APPEND INTERFACE_INCLUDE ${COMMON_PATH}/include/CallbackInterface.h)

  list(APPEND INTERFACE_TCC ${COMMON_PATH}/tcc/CallbackInterface.tcc)

  set(INTERFACE_LIBRARIES ${MODEL_LIBRARIES})

  string(TOLOWER "${LANGUAGE_NAME}" language)

  if(${language} STREQUAL "python")

    find_package(PythonInterp)
    find_package(PythonLibs)
    if(${PYTHONLIBS_FOUND})

      message(STATUS "Using python found at: ${PYTHON_EXECUTABLE}")
      message(STATUS "Using python libraries found at: ${PYTHON_LIBRARY}")

      include_directories(${COMMON_PATH} ${PYTHON_INCLUDE_PATH})

      generate_interface_module(${MODEL_NAME} ${MODEL_NAME} ${language} ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_LIBRARIES} "")

      set (SUCCESS TRUE)
    endif()
  endif()

endmacro()

#
# Macro to generate a target for compiling emitted models
#

macro(generate_compile_model_commands MODEL_NAME SUCCESS)
  set(SUCCESS FALSE)
  set(COMPILED_MODEL_TARGET compiled_${MODEL_NAME})
  set(COMPILED_MODEL_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MODEL_NAME}.o)

  add_custom_command(
    OUTPUT ${COMPILED_MODEL_OUTPUT}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_NAME}.ll
    COMMAND ${LLVM_TOOLS_BINARY_DIR}/llc ${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_NAME}.ll -o ${MODEL_NAME}.o -filetype=obj -relocation-model=pic
    COMMENT "Compiling ${MODEL_NAME}.ll to ${COMPILED_MODEL_OUTPUT}")

  add_custom_target(${COMPILED_MODEL_TARGET} DEPENDS ${COMPILED_MODEL_OUTPUT})
  add_dependencies(${COMPILED_MODEL_TARGET} _ELL_python)
  set_source_files_properties(${COMPILED_MODEL_OUTPUT} PROPERTIES GENERATED TRUE)

  set (SUCCESS TRUE)
endmacro()
