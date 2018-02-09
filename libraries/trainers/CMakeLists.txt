#
# cmake file
#

set (library_name trainers)

set (src src/ForestTrainer.cpp
         src/KMeansTrainer.cpp
         src/LogitBooster.cpp
         src/MeanCalculator.cpp
         src/ProtoNNInit.cpp
         src/ProtoNNTrainer.cpp
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
             include/ProtoNNInit.h
             include/ProtoNNModel.h
             include/ProtoNNTrainer.h
             include/ProtoNNTrainerUtils.h
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
         tcc/ProtoNNTrainerUtils.tcc
         tcc/SortingForestTrainer.tcc
         tcc/SweepingTrainer.tcc
         tcc/SDCATrainer.tcc
         tcc/SGDTrainer.tcc
         tcc/ThresholdFinder.tcc)


set (doc doc/README.md)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("doc" FILES ${doc})

add_library(${library_name} ${src} ${include} ${tcc} ${doc})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} utilities evaluators predictors)

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

set (test_src test/src/main.cpp)

source_group("src" FILES ${test_src})

add_executable(${test_name} ${test_src} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} functions testing trainers)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})
