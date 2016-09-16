#
# cmake file for apply project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME apply)
PROJECT(${PROJ_NAME})

#enable C++11 in GCC, etc
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp)

set (INCLUDE )

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC} ${INCLUDE})
target_include_directories(${PROJ_NAME} PRIVATE include)
target_link_libraries(${PROJ_NAME} utilities dataset model nodes common)

# put this project in the tools/utilities folder in the IDE 
set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tools/utilities")

# tests
set (TEST_NAME ${PROJ_NAME}_test)
add_test(NAME ${TEST_NAME}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${PROJ_NAME} -idf ${CMAKE_SOURCE_DIR}/examples/data/tinyTestData.txt -dd auto -omf null -v)
