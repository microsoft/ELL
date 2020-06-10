#
# cmake file
#

set(library_name emittable_functions)

set(src
    src/Convolutions.cpp
    src/IIRFilter.cpp
    src/LogisticFunctions.cpp
    src/VoiceActivityDetector.cpp
)

set(include
    include/Convolutions.h
    include/IIRFilter.h
    include/LogisticFunctions.h
    include/VoiceActivityDetector.h
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

add_library(${library_name} ${src} ${include})
target_include_directories(${library_name} PRIVATE include ${ELL_LIBRARIES_DIR})
target_link_libraries(${library_name} PUBLIC value)
target_compile_options(${library_name} PUBLIC ${LLVM_COMPILE_OPTIONS})

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set(test_name ${library_name}_test)

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
endif()

set(test_src
    test/src/main.cpp
    test/src/Convolutions_test.cpp
    test/src/Emittable_functions_util.cpp
    test/src/IIRFilter_test.cpp
    test/src/LogisticFunctionsTest.cpp
    test/src/VoiceActivityDetectorTest.cpp
)

set(test_include
    test/include/Convolutions_test.h
    test/include/Emittable_functions_util.h
    test/include/IIRFilter_test.h
    test/include/LogisticFunctionsTest.h
    test/include/VoiceActivityDetectorTest.h
)

set(test_data
    test/src/VadData.txt
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})
source_group("data" FILES ${test_data})

add_executable(${test_name} ${test_src} ${test_include} ${test_data})
target_include_directories(${test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${test_name} testing ${library_name} data math utilities common)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")
add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

copy_newer_files(${test_name} test_data)
