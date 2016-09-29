#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME math)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
else()
  add_compile_options(-D_SCL_SECURE_NO_WARNINGS)
endif()

set (SRC src/BlasWrapper.cpp)

set (INCLUDE include/BlasWrapper.h
             include/Matrix.h
             include/Vector.h
             include/Operations.h
             include/Print.h)

set (TCC tcc/Matrix.tcc
         tcc/Vector.tcc
         tcc/Operations.tcc
         tcc/Print.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

# Using FindBLAS module:
#find_package(BLAS)
#if(BLAS_FOUND)
#  message(WARNING "BLAS Found!")
#  message(WARNING "Blas libraries: ${BLAS_LIBRARIES}")
#  message(WARNING "Blas linker flags: ${BLAS_LINKER_FLAGS}")
#  message(WARNING "Blas vendor: ${BLA_VENDOR}")
#else()
#  message(WARNING "BLAS NOT Found!")
#endif()

set (BLAS_LIB_NAMES ${BLAS_LIB_NAMES} cblas openblas)

## libopenblas install on ubuntu in /usr/lib and /usr/include
## TODO: Add private/dependencies/OpenBlas/linux/...

find_library(BLAS_LIBRARY
  NAMES ${BLAS_LIB_NAMES}
  PATHS /usr/lib64/atlas-sse3 /usr/lib64/atlas /usr/lib64 /usr/local/lib64/atlas /usr/local/lib64 /usr/lib/atlas-sse3 /usr/lib/atlas-sse2 /usr/lib/atlas-sse /usr/lib/atlas-3dnow /usr/lib/atlas /usr/lib /usr/local/lib/atlas /usr/local/lib
      ${CMAKE_SOURCE_DIR}/private/dependencies/OpenBLAS/win64/v0.2.19/haswell/lib/
      ${CMAKE_SOURCE_DIR}/private/dependencies/OpenBLAS/win64/v0.2.19/sandybridge/lib/
  )

find_path(BLAS_INCLUDE_DIR cblas.h
  /System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/Current/Headers/
  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Accelerate.framework/Frameworks/vecLib.framework/Headers/
  /usr/include
  /usr/local/include
  ${CMAKE_SOURCE_DIR}/private/dependencies/OpenBLAS/win64/v0.2.19/haswell/include/
  ${CMAKE_SOURCE_DIR}/private/dependencies/OpenBLAS/win64/v0.2.19/sandybridge/include/)

if(${APPLE})
  # To install commandline tools (which seems to also install headers in /System/Library/Frameworks/...), do this:
  # xcode-select --install
elseif(${WIN32})

endif()

if (BLAS_LIBRARY AND BLAS_INCLUDE_DIR)
    set(CBLAS_FOUND "YES")
else ()
    set(CBLAS_FOUND "NO")
endif ()

message(WARNING "BLAS library: ${BLAS_LIBRARY}")
message(WARNING "BLAS includes: ${BLAS_INCLUDE_DIR}")

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include ${BLAS_INCLUDE_DIR})
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${PROJ_NAME} PRIVATE -fPIC)
endif()

target_link_libraries(${PROJ_NAME} utilities ${BLAS_LIBRARY})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

set (TEST_SRC test/src/main.cpp)
set (TEST_INCLUDE )

source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})
target_include_directories(${TEST_NAME} PRIVATE test/include)
target_link_libraries(${TEST_NAME} math testing)

if(${WIN32})
  set (BLASDLLDirectory ${CMAKE_SOURCE_DIR}/private/dependencies/OpenBLAS/win64/v0.2.19/haswell/bin)
  add_custom_command(TARGET math_test POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BLASDLLDirectory}/libopenblas.dll $<TARGET_FILE_DIR:math_test>)
  add_custom_command(TARGET math_test POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BLASDLLDirectory}/libgcc_s_seh-1.dll $<TARGET_FILE_DIR:math_test>)
  add_custom_command(TARGET math_test POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BLASDLLDirectory}/libgfortran-3.dll $<TARGET_FILE_DIR:math_test>)
  add_custom_command(TARGET math_test POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BLASDLLDirectory}/libquadmath-0.dll $<TARGET_FILE_DIR:math_test>)
endif()

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

