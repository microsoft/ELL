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
target_link_libraries(${tool_name} common model passes utilities)
copy_shared_libraries(${tool_name})

set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")

#
# tests
#
set (test_name ${tool_name}_test1)
add_test(NAME ${test_name}
        WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
        COMMAND ${tool_name} -imf ${CMAKE_BINARY_DIR}/examples/models/identity.model --ir)

set (test_name ${tool_name}_test2)
add_test(NAME ${test_name}
        WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
        COMMAND ${tool_name} -imf ${CMAKE_BINARY_DIR}/examples/models/is_equal.model --ir)
set_test_library_path(${test_name})

