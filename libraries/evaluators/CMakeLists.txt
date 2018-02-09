#
# cmake file
#

set (library_name evaluators)

set (src src/AUCAggregator.cpp
         src/BinaryErrorAggregator.cpp)

set (include include/AUCAggregator.h
             include/BinaryErrorAggregator.h
             include/Evaluator.h
             include/IncrementalEvaluator.h
             include/LossAggregator.h)

set (tcc tcc/Evaluator.tcc
         tcc/IncrementalEvaluator.tcc
         tcc/LossAggregator.tcc)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} data)

# MSVC emits warnings incorrectly when mixing inheritance, templates,
# and member function definitions outside of class definitions
if(MSVC)
    target_compile_options(${library_name} PRIVATE /wd4505)
endif()

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src test/src/main.cpp test/src/Evaluators_test.cpp)
set (test_include test/include/Evaluators_test.h)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} data evaluators functions predictors testing)
copy_shared_libraries(${test_name})

# MSVC emits warnings incorrectly when mixing inheritance, templates,
# and member function definitions outside of class definitions
if(MSVC)
    target_compile_options(${test_name} PRIVATE /wd4505)
endif()

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} 
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${test_name})
set_test_library_path(${test_name})
