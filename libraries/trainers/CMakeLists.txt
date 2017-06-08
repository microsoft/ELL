#
# cmake file
#

set (library_name trainers)

set (src src/AbsoluteMeanTrainer.cpp
         src/ForestTrainer.cpp
         src/LogitBooster.cpp
         src/SGDLinearTrainer.cpp
         src/ThresholdFinder.cpp)

set (include include/AbsoluteMeanTrainer.h
             include/EvaluatingIncrementalTrainer.h
             include/ForestTrainer.h
             include/HistogramForestTrainer.h
             include/ITrainer.h
             include/LogitBooster.h
             include/MeanTrainer.h
             include/MultiEpochIncrementalTrainer.h
             include/SortingForestTrainer.h
             include/SweepingIncrementalTrainer.h
             include/SGDLinearTrainer.h
             include/ThresholdFinder.h)

set (tcc tcc/EvaluatingIncrementalTrainer.tcc
         tcc/ForestTrainer.tcc
         tcc/HistogramForestTrainer.tcc
         tcc/MeanTrainer.tcc
         tcc/MultiEpochIncrementalTrainer.tcc
         tcc/SortingForestTrainer.tcc
         tcc/SweepingIncrementalTrainer.tcc
         tcc/SGDLinearTrainer.tcc
         tcc/ThresholdFinder.tcc)

set (doc doc/README.md)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("doc" FILES ${doc})

add_library(${library_name} ${src} ${include} ${tcc} ${doc})
target_include_directories(${library_name} PUBLIC include)

if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()
target_link_libraries(${library_name} evaluators predictors)

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
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} testing trainers)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
