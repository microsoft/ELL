#
# cmake file for profile project
#

# define project
set (tool_name profile)

set (src 
  src/ProfileArguments.cpp
  src/main.cpp
  )

set (include 
  include/ProfileArguments.h
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${src} ${include} ${tcc})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} utilities model nodes common emitters imageConverter)
copy_shared_libraries(${tool_name})
set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")

#
# A script that generates compiled profilers
#
find_program(LLC_EXECUTABLE llc HINTS ${LLVM_TOOLS_BINARY_DIR})
find_program(OPT_EXECUTABLE opt HINTS ${LLVM_TOOLS_BINARY_DIR})
if(WIN32)
  set(COMPILE_EXECUTABLE ${GLOBAL_BIN_DIR}/Release/compile)
else()
  set(COMPILE_EXECUTABLE ${GLOBAL_BIN_DIR}/compile)
endif()

if(NOT LLC_EXECUTABLE)
    message(STATUS "LLVM not found, please check that LLVM is installed.")
endif()

if(WIN32)
  set(comment_str "REM ")
else()
  set(comment_str "#")
endif()

if(NOT LLC_EXECUTABLE)
  set(LLC_EXECUTABLE ${comment_str})
endif()

if(NOT OPT_EXECUTABLE)
  set(OPT_EXECUTABLE ${comment_str})
endif()

configure_file(CMakeLists-compiled.txt.in CMakeLists-compiled.txt.in @ONLY)
configure_file(CMakeLists-compiled-parallel.txt.in CMakeLists-compiled-parallel.txt.in @ONLY)
configure_file(src/compiled_profile_main.cpp compiled_profile_main.cpp COPYONLY)
configure_file(src/compiled_exercise_model_main.cpp compiled_exercise_model_main.cpp COPYONLY)
configure_file(make_profiler.sh.in make_profiler.sh @ONLY)
configure_file(make_profiler.cmd.in make_profiler.cmd @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMake/OpenBLASSetup.cmake OpenBLASSetup.cmake COPYONLY)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/make_profiler.sh
  DESTINATION ${GLOBAL_BIN_DIR}
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
  GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

if(WIN32)
  file(COPY ${CMAKE_CURRENT_BINARY_DIR}/make_profiler.cmd
    DESTINATION ${GLOBAL_BIN_DIR}
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
    GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

add_custom_target(make_profiler DEPENDS compile CMakeLists-compiled.txt.in CMakeLists-compiled-parallel.txt.in src/compiled_profile_main.cpp src/compiled_exercise_model_main.cpp make_profiler.sh.in make_profiler.cmd.in)
