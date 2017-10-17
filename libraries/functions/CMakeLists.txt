#
# cmake file
#

set (library_name functions)

set (src src/ElasticNetRegularizer.cpp
         src/HingeLoss.cpp
         src/L2Regularizer.cpp
         src/LogLoss.cpp
         src/SmoothHingeLoss.cpp
         src/SquaredLoss.cpp)

set (include include/ElasticNetRegularizer.h
             include/HingeLoss.h
             include/L2Regularizer.h
             include/LogLoss.h
             include/SmoothHingeLoss.h
             include/SquaredLoss.h)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

add_library(${library_name} ${src} ${include})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} utilities math)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src test/src/main.cpp)
set (test_include )

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
copy_shared_libraries(${test_name})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} functions testing utilities math)

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})
