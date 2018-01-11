#
# cmake file for apply project
#

# define project
set (tool_name apply)

set (src src/ApplyArguments.cpp 
         src/main.cpp)

set (include include/ApplyArguments.h)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${src} ${include})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} utilities data model nodes common)
copy_shared_libraries(${tool_name})

# put this project in the tools/utilities folder in the IDE 
set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")

# tests
set (test_name ${tool_name}_test)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} -idf ${CMAKE_BINARY_DIR}/examples/data/testData.txt -imf ${CMAKE_BINARY_DIR}/examples/models/times_two.model -odf null)
set_test_library_path(${test_name})
