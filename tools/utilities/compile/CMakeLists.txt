#
# cmake file for compile project
#

# define project
set (tool_name compile)

set (src src/main.cpp)

set (include )

set (tcc )

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${src} ${include} ${tcc})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} utilities model common)
copy_shared_libraries(${tool_name} $<TARGET_FILE_DIR:${tool_name}>)

set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")
