#
# cmake file for compile project
#

# define project
set (tool_name compile)

include (LLVMSetup)
if(NOT LLVM_FOUND)
  message("LLVM unavailable. Compiler disabled.")
  return()
endif()

# compile project
set (src
  src/main.cpp
  src/CompileArguments.cpp)

set (include
  include/CompileArguments.h)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 

add_executable(${tool_name} ${src} ${include} ${tcc})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} utilities model common)
copy_shared_libraries(${tool_name})

set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")

#
# tests
#
set (test_name ${tool_name}_test1)
add_test(NAME ${test_name}
        WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
        COMMAND ${tool_name} -imf ${CMAKE_SOURCE_DIR}/examples/data/identity.model --ir)

set (test_name ${tool_name}_test2)
add_test(NAME ${test_name}
        WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
        COMMAND ${tool_name} -imf ${CMAKE_SOURCE_DIR}/examples/data/is_equal.model --ir)
