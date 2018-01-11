#
# cmake file for makeExamples project
#

# define project
set(tool_name makeExamples)

set(src
    src/main.cpp
    src/ModelGenerateArguments.cpp
    src/GenerateModels.cpp
)

set(include
    include/GenerateModels.h
    include/ModelGenerateArguments.h
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

# create executable in build\bin
set(GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR})
add_executable(${tool_name} ${src} ${include})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} utilities model nodes common)
copy_shared_libraries(${tool_name})

set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")

#
# Add build command to generate the examples
#

add_custom_command(TARGET ${tool_name}
  POST_BUILD
  COMMAND ${tool_name} --outputPath ${CMAKE_BINARY_DIR}/examples/models
  COMMENT "Generating example models"
)
