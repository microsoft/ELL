#
# cmake file
#

set (library_name trainers)

set (src src/ForestTrainer.cpp
         src/KMeansTrainer.cpp
         src/LogitBooster.cpp
         src/MeanCalculator.cpp
         src/SGDTrainer.cpp
         src/ThresholdFinder.cpp
)

set (include include/EvaluatingTrainer.h
             include/ForestTrainer.h
             include/HistogramForestTrainer.h
             include/ITrainer.h
             include/KMeansTrainer.h
             include/LogitBooster.h
             include/MeanCalculator.h
             include/SortingForestTrainer.h
             include/SweepingTrainer.h
             include/SDCATrainer.h
             include/SGDTrainer.h
             include/ThresholdFinder.h
)

set (tcc tcc/EvaluatingTrainer.tcc
         tcc/ForestTrainer.tcc
         tcc/HistogramForestTrainer.tcc
         tcc/MeanCalculator.tcc
         tcc/SortingForestTrainer.tcc
         tcc/SweepingTrainer.tcc
         tcc/SDCATrainer.tcc
         tcc/SGDTrainer.tcc
         tcc/ThresholdFinder.tcc)

set (pr_src protonn/src/ProtoNNTrainer.cpp
            protonn/src/ProtoNNInit.cpp)

set (pr_include protonn/include/ProtoNNTrainerUtils.h
                protonn/include/ProtoNNTrainer.h
                protonn/include/ProtoNNModel.h
                protonn/include/ProtoNNInit.h)

set (pr_tcc protonn/tcc/ProtoNNTrainerUtils.tcc)

set (doc doc/README.md)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("doc" FILES ${doc})
source_group("protonn\\include" FILES ${pr_include})
source_group("protonn\\src" FILES ${pr_src})
source_group("protonn\\tcc" FILES ${pr_tcc})

add_library(${library_name} ${src} ${include} ${tcc} ${pr_src} ${pr_include} ${pr_tcc} ${doc})
target_include_directories(${library_name} PUBLIC include protonn/include)

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

source_group("src" FILES ${test_src})

add_executable(${test_name} ${test_src} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} common testing trainers)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
