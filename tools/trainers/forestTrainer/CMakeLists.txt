#
# cmake file for forestTrainer project
#

# define project
set (tool_name forestTrainer)

set (src src/main.cpp)

source_group("src" FILES ${src})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${src} ${include})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} common data functions model nodes predictors trainers utilities)
copy_shared_libraries(${tool_name})

# put this project in the trainers folder in the IDE 
set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/trainers")

# copy executable to build/bin directory
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# tests
set (test_name ${tool_name}_test)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} -idf ${CMAKE_SOURCE_DIR}/examples/data/tinyTestData.txt -dd auto -omf null -v)
set_test_library_path(${test_name})
