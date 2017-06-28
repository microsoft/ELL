#
# cmake file for protoNNTrainer project
#

# define project
set (tool_name protoNNTrainer)

# enable C++11 in GCC, etc
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (src src/main.cpp)
source_group("src" FILES ${src})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR})
add_executable(${tool_name} ${src})
target_link_libraries(${tool_name} common data functions model nodes predictors trainers evaluators utilities)
copy_shared_libraries(${tool_name})

# put this project in the trainers folder in the IDE
set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/trainers")

# copy executable to build/bin directory
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
