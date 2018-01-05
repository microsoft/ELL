#
# cmake file for builder project
#
include(CommonInterfaces)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(OBJECT_EXTENSION obj)
    set(ADDITIONAL_DISTUTILS_FLAGS "--plat-name=win-amd64 install")
else()
    set(OBJECT_EXTENSION o)
    set(ADDITIONAL_DISTUTILS_FLAGS install)
endif()

set (module_name wrap)
set (builder_src
    templates/CMakeLists.cpp.txt.in
    templates/CMakeLists.python.txt.in
    wrap.py
    README.md)

add_custom_target(${module_name} DEPENDS ${builder_src} SOURCES ${builder_src})
set_property(TARGET ${module_name} PROPERTY FOLDER "tools")

# copy tutorial files
foreach(PY_FILE ${builder_src})
    configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
endforeach()

find_program(LLC_EXECUTABLE llc HINTS ${LLVM_TOOLS_BINARY_DIR})
if(NOT LLC_EXECUTABLE)
    message(ERROR "LLVM not found, please check that LLVM is installed.")
    return()
endif()

find_program(OPT_EXECUTABLE opt HINTS ${LLVM_TOOLS_BINARY_DIR})
if(NOT OPT_EXECUTABLE)
    message(ERROR "LLVM not found, please check that LLVM is installed.")
    return()
endif()

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/tools)

set(JSON "{ \"llc\": \"${LLC_EXECUTABLE}\", \"swig\": \"${SWIG_EXECUTABLE}\", \"compile\": \"${GLOBAL_BIN_DIR}/compile\", \"blas\": \"${BLAS_LIBS}\", \"opt\": \"${OPT_EXECUTABLE}\" }")
file(WRITE "${OUTPUT_DIR}/tools.json" ${JSON})
