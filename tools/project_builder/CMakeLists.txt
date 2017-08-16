#
# cmake file for builder project
#
include (CommonInterfaces)

find_package(PythonInterp)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(OBJECT_EXTENSION obj)
    set(LLC_EXECUTABLE ${CMAKE_SOURCE_DIR}/external/LLVMNativeWindowsLibs.x64.3.9.1/build/native/tools/llc.exe)
    set(ADDITIONAL_DISTUTILS_FLAGS "--plat-name=win-amd64 install")
else()
    set(OBJECT_EXTENSION o)
    set(LLC_EXECUTABLE llc)
    set(ADDITIONAL_DISTUTILS_FLAGS install)
endif()


set (module_name builder)
set (builder_src
    ../utilities/pythonlibs/find_ell.py
    make_python_project.py
    README.md)

add_custom_target(${module_name} DEPENDS ${builder_src} SOURCES ${builder_src})
set_property(TARGET ${module_name} PROPERTY FOLDER "tools/project_builder")

# copy tutorial files
foreach(PY_FILE ${builder_src})
    configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
endforeach()

# this one is special
configure_file(../utilities/pythonlibs/find_ell.py find_ell.py COPYONLY)

find_program(LLC_EXECUTABLE llc HINTS ${LLVM_TOOLS_BINARY_DIR})
if(NOT LLC_EXECUTABLE)
    message(ERROR "LLVM not found, please check that LLVM is installed.")
    return()
endif()

set (OUTPUT_DIR ${CMAKE_BINARY_DIR}/tools/project_builder)

set(JSON "{ \"llc\": \"${LLC_EXECUTABLE}\", \"swig\": \"${SWIG_EXECUTABLE}\", \"compile\": \"${GLOBAL_BIN_DIR}/compile.exe\" }")
file(WRITE "${OUTPUT_DIR}/tools.json" ${JSON})
