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

if(${APPLE})
  set (BLASDirectory /System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/Current/Headers/)
  set (BLASLibrary cblas)
elseif(${WIN32})
  set (BLASDirectory ${CMAKE_SOURCE_DIR}/private/dependencies/OpenBLAS/win64/v0.2.19/haswell/include)
  set (BLASLibrary ${CMAKE_SOURCE_DIR}/private/dependencies/OpenBLAS/win64/v0.2.19/haswell/lib/libopenblas.dll.a)
endif()

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include ${BLASDirectory})
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${PROJ_NAME} PRIVATE -fPIC)
endif()

target_link_libraries(${PROJ_NAME} utilities ${BLASLibrary})

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

