#
# python tutorial projects
#

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


set (TUTORIAL_NAME compute)
set (PYTHON_TUTORIALS
    README.md
    zebra.png)

add_custom_target(${TUTORIAL_NAME} DEPENDS ${PYTHON_TUTORIALS} SOURCES ${PYTHON_TUTORIALS})
add_dependencies(${TUTORIAL_NAME} _ELL_python ell_python_utilities)
set_property(TARGET ${TUTORIAL_NAME} PROPERTY FOLDER "tutorials/vision")

# copy tutorial files
foreach(PY_FILE ${PYTHON_TUTORIALS})
    configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
endforeach()
