#
# cmake file for print project
#

# define project
set(tool_name print)

set(src
    src/PrintArguments.cpp
    src/PrintGraph.cpp
    src/PrintModel.cpp
    src/main.cpp
)

set(include
    include/LayerInspector.h
    include/PrintArguments.h
    include/PrintGraph.h
    include/PrintModel.h
)

set(tcc
    tcc/LayerInspector.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

# create executable in build\bin
set(GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR})
add_executable(${tool_name} ${src} ${include} ${tcc})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} utilities model nodes common utilities)
copy_shared_libraries(${tool_name})

set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")
