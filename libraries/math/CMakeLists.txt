#
# cmake file
#

set(library_name math)

if(MSVC)
  add_compile_options(-D_SCL_SECURE_NO_WARNINGS)
endif()

include (OpenBLASSetup)

set(src src/BlasWrapper.cpp
         src/Tensor.cpp
)

set(include include/BlasWrapper.h
             include/Common.h
             include/MathConstants.h
             include/Matrix.h
             include/MatrixOperations.h
             include/Tensor.h
             include/TensorOperations.h
             include/Vector.h
             include/VectorOperations.h
)

set(tcc tcc/Matrix.tcc
         tcc/MatrixOperations.tcc
         tcc/Tensor.tcc
         tcc/TensorOperations.tcc
         tcc/Vector.tcc
         tcc/VectorOperations.tcc
)

set(doc doc/README.md)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("doc" FILES ${doc})

add_library(${library_name} ${src} ${include} ${tcc} ${doc})
target_include_directories(${library_name} PUBLIC include)
target_include_directories(${library_name} SYSTEM PUBLIC ${BLAS_INCLUDE_DIRS})
target_link_libraries(${library_name} utilities ${BLAS_LIBS})

if(BLAS_FOUND)
  target_compile_definitions(${library_name} PUBLIC USE_BLAS=1)
endif()

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# math test
#

set(test_name ${library_name}_test)

set(test_src test/src/main.cpp)

set(test_include test/include/math_profile.h
                  test/include/Matrix_test.h
                  test/include/Tensor_test.h
                  test/include/Vector_test.h)

set(test_tcc test/tcc/math_profile.tcc
              test/tcc/Matrix_test.tcc
              test/tcc/Tensor_test.tcc
              test/tcc/Vector_test.tcc)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})
source_group("tcc" FILES ${test_tcc})

add_executable(${test_name} ${test_src} ${test_include} ${test_tcc} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} math testing)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

#
# math profile
#

set(profile_name ${library_name}_profile)

set(profile_src test/src/math_profile_main.cpp)

set(profile_include test/include/math_profile.h)

set(profile_tcc test/tcc/math_profile.tcc)

source_group("src" FILES ${profile_src})
source_group("include" FILES ${profile_include})
source_group("tcc" FILES ${profile_tcc})

add_executable(${profile_name} ${profile_src} ${profile_include} ${profile_tcc} ${include})
target_include_directories(${profile_name} PRIVATE test/include)
target_link_libraries(${profile_name} math testing)
copy_shared_libraries(${profile_name})

set_property(TARGET ${profile_name} PROPERTY FOLDER "tests")

if (PROFILING)
add_test(NAME ${profile_name} COMMAND ${profile_name} CONFIGURATIONS Release)
set_test_library_path(${profile_name})
endif()