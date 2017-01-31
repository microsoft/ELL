#
# cmake file
#

set (library_name predictors)

set (src src/ConstantPredictor.cpp
         src/LinearPredictor.cpp
         src/SingleElementThresholdPredictor.cpp)

set (include include/ConstantPredictor.h
             include/IPredictor.h
             include/LinearPredictor.h
             include/SignPredictor.h
             include/SingleElementThresholdPredictor.h
             include/ForestPredictor.h)

set (tcc tcc/SignPredictor.tcc
         tcc/ForestPredictor.tcc)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()
target_link_libraries(${library_name} data utilities)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src test/src/main.cpp)
set (test_include )

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include} ${tcc})
target_include_directories(${test_name} PRIVATE test/include)

target_link_libraries(${test_name} data predictors testing)
copy_shared_libraries(${test_name} $<TARGET_FILE_DIR:${test_name}>)

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})

