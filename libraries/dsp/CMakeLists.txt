#
# cmake file
#

set(library_name dsp)

if(MSVC)
  add_compile_options(-D_SCL_SECURE_NO_WARNINGS)
endif()

set(src
  src/FilterBank.cpp
)

set(include
  include/FFT.h
  include/FilterBank.h
  include/IIRFilter.h
  include/WindowFunctions.h
)

set(tcc
  tcc/FFT.tcc
  tcc/IIRFilter.tcc
  tcc/WindowFunctions.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} math utilities)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# dsp test
#

set(test_name ${library_name}_test)

set(test_src 
  test/src/main.cpp
  test/src/CepstrumTestData.cpp
  test/src/DCTTest.cpp
  test/src/DSPTestData.cpp
  test/src/FFTTest.cpp
  test/src/FilterTest.cpp
  test/src/MelTest.cpp
  test/src/WindowTest.cpp
)
  
set(test_include
  test/include/CepstrumTestData.h
  test/include/DCTTest.h
  test/include/DSPTestData.h
  test/include/FFTTest.h
  test/include/FilterTest.h
  test/include/MelTest.h
  test/include/WindowTest.h
)

set(test_tcc
)

set(test_py
  test/py/generate_test_data.py
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})
source_group("tcc" FILES ${test_tcc})
source_group("py" FILES ${test_py})

add_executable(${test_name} ${test_src} ${test_include} ${test_tcc} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} dsp testing)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})
