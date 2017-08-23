#
# cmake file for compare project
#

# define project
set (tool_name compare)

set (src 
  src/CompareArguments.cpp
  src/CompareUtils.cpp
  src/main.cpp
  src/ModelComparison.cpp
  src/visualizeReport.py
)

set (include
  include/CompareArguments.h
  include/CompareUtils.h
  include/ModelComparison.h
  include/VectorStats.h
)
    
set (tcc 
  tcc/CompareUtils.tcc
  tcc/VectorStats.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${src} ${include} ${tcc})
target_include_directories(${tool_name} PRIVATE include imageConverter utilities)
target_link_libraries(${tool_name} utilities model nodes common imageConverter)
copy_shared_libraries(${tool_name})

set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")
