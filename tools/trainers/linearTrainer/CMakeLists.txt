#
# cmake file for linearTrainer project
#

# define project
set (tool_name linearTrainer)

set (src src/LinearTrainerArguments.cpp
         src/main.cpp)

set (include include/LinearTrainerArguments.h)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${src} ${include})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} common data functions model predictors trainers evaluators utilities)
copy_shared_libraries(${tool_name})

# put this project in the trainers folder in the IDE 
set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/trainers")

# copy executable to build/bin directory
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# tests
set (test_name ${tool_name}_test_0)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/tinyTestData.txt -dd 3 -r 0.01 --outputModelFilename linearTrainer_model_1.model -v -ne 20 --lossFunction log)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_1)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt --inputModelFilename ${CMAKE_BINARY_DIR}/examples/models/model_3.model --modelInputs 1047 --modelOutputs 1054.output -dd 3 -r 0.01 --outputModelFilename linearTrainer_model_3.model -v -ne 20 --lossFunction log)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_2)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd 3 -lf log -v -ne 30 -r 0.001 -a SGD)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_3)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd 3 -lf hinge -v -ne 30 -r 0.001 -a SGD)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_4)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd 3 -lf squared -v -ne 30 -r 1 -a SGD)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_5)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd 3 -lf log -v -ne 30 -r 0.001 -a SparseDataSGD)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_6)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd 3 -lf hinge -v -ne 30 -r 0.001 -a SparseDataSGD)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_7)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd 3 -lf squared -v -ne 30 -r 1 -a SparseDataSGD)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_8)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd 3 -lf log -v -ne 30 -r 0.01 -a SparseDataCenteredSGD)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_9)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd 3 -lf hinge -v -ne 30 -r 0.01 -a SparseDataCenteredSGD)
set_test_library_path(${test_name})

set (test_name ${tool_name}_test_10)
add_test(NAME ${test_name}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${tool_name} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd 3 -lf squared -v -ne 30 -r 1 -a SparseDataCenteredSGD)
set_test_library_path(${test_name})
