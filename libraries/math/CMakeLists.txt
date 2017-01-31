#
# cmake file
#

set (library_name math)

include (OpenBLASSetup)

if(MSVC)
  add_compile_options(-D_SCL_SECURE_NO_WARNINGS)
endif()

set (src src/BlasWrapper.cpp)

set (include include/BlasWrapper.h
             include/Matrix.h
             include/Vector.h
             include/Operations.h
             include/Print.h)

set (tcc tcc/Matrix.tcc
         tcc/Vector.tcc
         tcc/Operations.tcc
         tcc/Print.tcc)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include ${BLAS_INCLUDE_DIRS})
target_link_libraries(${library_name} utilities ${BLAS_LIBS})

if(BLAS_FOUND)
  target_compile_definitions(${library_name} PUBLIC USE_BLAS=1)

  if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${library_name} PRIVATE -fPIC)
  endif()
endif()

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
target_link_libraries(${test_name} math testing)
copy_shared_libraries(${test_name} $<TARGET_FILE_DIR:${test_name}>)

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})

