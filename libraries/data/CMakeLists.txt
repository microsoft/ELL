#
# cmake file
#

set (library_name data)

set (src src/Dataset.cpp
         src/DataVector.cpp
         src/DataVectorOperations.cpp
         src/GeneralizedSparseParsingIterator.cpp
         src/SequentialLineIterator.cpp
         src/TextLine.cpp
         src/WeightLabel.cpp)

set (include include/AutoDataVector.h
             include/Dataset.h
             include/DataVector.h
             include/DataVectorOperations.h
             include/DenseDataVector.h
             include/Example.h
             include/ExampleIterator.h
             include/GeneralizedSparseParsingIterator.h
             include/IndexValue.h
             include/SingleLineParsingExampleIterator.h
             include/SequentialLineIterator.h
             include/SparseBinaryDataVector.h
             include/SparseDataVector.h
             include/StlIndexValueIterator.h
             include/TransformedDataVector.h
             include/TransformingIndexValueIterator.h
             include/TextLine.h
             include/WeightLabel.h
             )

set (tcc tcc/AutoDataVector.tcc
         tcc/DataVector.tcc
         tcc/DataVectorOperations.tcc
         tcc/DenseDataVector.tcc
         tcc/Example.tcc
         tcc/ExampleIterator.tcc
         tcc/Dataset.tcc
         tcc/SingleLineParsingExampleIterator.tcc
         tcc/SparseBinaryDataVector.tcc
         tcc/SparseDataVector.tcc
         tcc/StlIndexValueIterator.tcc
         tcc/TextLine.tcc
         tcc/TransformedDataVector.tcc
         tcc/TransformingIndexValueIterator.tcc)

set (doc doc/GeneralizedSparseFormat.md
         doc/README.md)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("doc" FILES ${doc})

add_library(${library_name} ${src} ${include} ${tcc} ${doc})
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
              test/src/Example_test.cpp
              test/src/Parser_test.cpp)

set (test_include test/include/Dataset_test.h
                  test/include/DataVector_test.h
                  test/include/Example_test.h
                  test/include/Parser_test.h)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} data testing utilities)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
