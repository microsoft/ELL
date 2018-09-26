#
# cmake file for builder project
#
include(CommonInterfaces)

add_subdirectory(test)

if(WIN32)
    set(OBJECT_EXTENSION "o")
    set(ADDITIONAL_DISTUTILS_FLAGS "--plat-name=win-amd64 install")
else()
    set(OBJECT_EXTENSION "o")
    set(ADDITIONAL_DISTUTILS_FLAGS install)
endif()

set (module_name wrap)
set (builder_src
    wrap.py
    README.md)

set (template_src 
    templates/__init__.py.in
    templates/CMakeLists.cpp.txt.in
    templates/CMakeLists.python.txt.in)

source_group("templates" FILES ${template_src})
add_custom_target(${module_name} DEPENDS ${builder_src} SOURCES ${builder_src} ${template_src})
add_dependencies(${module_name} pythonlibs)
set_property(TARGET ${module_name} PROPERTY FOLDER "tools")

# copy files
copy_newer_files(${module_name} builder_src)
copy_newer_files(${module_name} template_src "${CMAKE_CURRENT_BINARY_DIR}/templates/")

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

set(OUTPUT_DIR ${CMAKE_BINARY_DIR})

set(JSON "{ \"llc\": \"${LLC_EXECUTABLE}\", \"swig\": \"${SWIG_EXECUTABLE}\", \"compile\": \"${GLOBAL_BIN_DIR}/compile\", \"blas\": \"${BLAS_LIBS}\", \"opt\": \"${OPT_EXECUTABLE}\" }")
file(WRITE "${OUTPUT_DIR}/ell_build_tools.json" "${JSON}")

file(TO_NATIVE_PATH "${BLAS_DLL_DIR}" OPENBLAS_DLL_DIR)
file(WRITE "${OUTPUT_DIR}/openblas.cmd" "set PATH=%PATH%;${OPENBLAS_DLL_DIR}")