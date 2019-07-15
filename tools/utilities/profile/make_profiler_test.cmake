
# First create models to test on
message(STATUS "Generating test models")
execute_process(COMMAND ${MAKE_MODELS_EXE} RESULT_VARIABLE COMMAND_RESULT WORKING_DIRECTORY ${BUILD_DIR})
if(COMMAND_RESULT)
  message(FATAL_ERROR "Error generating profile models: " ${COMMAND_RESULT})
endif()

# Run make_profiler script
message(STATUS "Generating profiler project using ${MAKE_PROFILE_SCRIPT}")
execute_process(COMMAND ${MAKE_PROFILE_SCRIPT} ${MODEL_FILE} ${PROFILE_DIRECTORY} --target host RESULT_VARIABLE COMMAND_RESULT WORKING_DIRECTORY ${BUILD_DIR})
if(COMMAND_RESULT)
  message(FATAL_ERROR "Error running make_profiler: " ${COMMAND_RESULT})
endif()

# Build resulting project
message(STATUS "Building profiler project")
if(WIN32)
  execute_process(COMMAND ${PROFILE_DIRECTORY}/build_and_run.cmd RESULT_VARIABLE COMMAND_RESULT WORKING_DIRECTORY ${PROFILE_DIRECTORY})
else()
  execute_process(COMMAND chmod +x ./build_and_run.sh RESULT_VARIABLE COMMAND_RESULT WORKING_DIRECTORY ${PROFILE_DIRECTORY})
  execute_process(COMMAND ./build_and_run.sh RESULT_VARIABLE COMMAND_RESULT WORKING_DIRECTORY ${PROFILE_DIRECTORY})
endif()
if(COMMAND_RESULT)
  message(FATAL_ERROR "Error compiling profile projects: " ${COMMAND_RESULT})
endif()
