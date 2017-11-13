#
# cmake file for retarget project
#

# define project
set(tool_name retarget)

set(src src/RetargetArguments.cpp
    src/main.cpp)

set(include include/RetargetArguments.h)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${src} ${include})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} common data functions model predictors trainers evaluators utilities)
copy_shared_libraries(${tool_name})

# put this project in the utilities folder in the IDE 
set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")
