#
# SWIG cmake directives for generating interfaces to an emitted model
#

# macros
include (CommonInterfaces)

# model
set(model_name ELL_step10)
set(common_path ../../../interfaces/common)

include_directories(./
  ../../../interfaces/common/include
  ../../../libraries/emitters/include)

# compile the model (sets COMPILED_MODEL_OUTPUT and COMPILED_MODEL_TARGET)
generate_compile_model_commands(${model_name} COMPILED_MODEL_OUTPUT COMPILED_MODEL_TARGET)

# emit SWIG interfaces (sets SWIG_MODULE_TARGET)
set(INTERFACE_FILES)
set(INTERFACE_INCLUDE ${model_name}.i.h
  ../../../libraries/emitters/include/ClockInterface.h)
set(INTERFACE_SRC)
set(INTERFACE_TCC)
generate_emitted_interface_module(${model_name} ${COMPILED_MODEL_OUTPUT} ${common_path} python SUCCESS SWIG_MODULE_TARGET)

if(SUCCESS)
  # connect dependencies between the two targets
  add_dependencies(${SWIG_MODULE_TARGET} ${COMPILED_MODEL_TARGET})

  # folder for VS projects
  set_property(TARGET ${COMPILED_MODEL_TARGET} PROPERTY FOLDER "examples")
  set_property(TARGET ${SWIG_MODULE_TARGET} PROPERTY FOLDER "examples")

  # setup tests
  configure_file(../../../interfaces/python/test/testing.py python/testing.py COPYONLY)  
  configure_file(python/step_test.py python/step_test.py COPYONLY)

  find_package(PythonInterp 3.5 QUIET)
  if(${PYTHONINTERP_FOUND})  
    add_test(NAME ${model_name}_python_test
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python
      COMMAND ${PYTHON_EXECUTABLE} step_test.py)
  endif()
else()
  message(WARNING "Couldn't generate python interfaces for ${model_name}.")
endif()
