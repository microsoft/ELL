#
# cmake file
#

set (library_name data)

set (src src/Dataset.cpp
         src/DataVectorOperators.cpp
         src/SequentialLineIterator.cpp
         src/SparseEntryParser.cpp
         src/Example.cpp)

set (include include/AutoDataVector.h
             include/DenseDataVector.h
             include/Example.h
             include/ExampleIterator.h
             include/DataVector.h
             include/DataVectorOperators.h
             include/ParsingExampleIterator.h
             include/Dataset.h
             include/IndexValue.h
             include/SequentialLineIterator.h
             include/SparseBinaryDataVector.h
             include/SparseDataVector.h
             include/SparseEntryParser.h
             include/StlIndexValueIterator.h
             include/SupervisedExampleBuilder.h
             include/TransformingIndexValueIterator.h)

set (tcc tcc/AutoDataVector.tcc
         tcc/DataVector.tcc
         tcc/DenseDataVector.tcc
         tcc/Example.tcc
         tcc/ExampleIterator.tcc
         tcc/ParsingExampleIterator.tcc
         tcc/Dataset.tcc
         tcc/SparseBinaryDataVector.tcc
         tcc/SparseDataVector.tcc
         tcc/StlIndexValueIterator.tcc
         tcc/SupervisedExampleBuilder.tcc
         tcc/TransformingIndexValueIterator.tcc)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()
target_link_libraries(${library_name} math utilities)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src test/src/main.cpp
              test/src/Dataset_test.cpp
              test/src/DataVector_test.cpp
              test/src/Example_test.cpp)

set (test_include test/include/Dataset_test.h
                  test/include/DataVector_test.h
                  test/include/Example_test.h)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} data testing utilities)
copy_shared_libraries(${test_name} $<TARGET_FILE_DIR:${test_name}>)

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
