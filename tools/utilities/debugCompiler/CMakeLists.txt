#
# cmake file for debugCompiler project
#

# define project
set (tool_name debugCompiler)

set (src 
  src/CompareArguments.cpp
  src/main.cpp
  src/ModelComparison.cpp
  src/visualizeReport.py
)

set (include
  include/CompareArguments.h
  include/ModelComparison.h
  include/VectorStatistics.h
)
    
set (tcc 
  tcc/VectorStatistics.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${src} ${include} ${tcc})
target_include_directories(${tool_name} PRIVATE include pythonPlugins utilities)
target_link_libraries(${tool_name} common math model nodes passes utilities pythonPlugins)
copy_shared_libraries(${tool_name})

set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")
