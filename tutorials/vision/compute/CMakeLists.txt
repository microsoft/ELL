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
    ../../../tools/utilities/pythonlibs/demo.py
    ../../../tools/utilities/pythonlibs/demoHelper.py
    ../../../tools/utilities/pythonlibs/find_ell.py
    README.md
    zebra.png)

add_custom_target(${TUTORIAL_NAME} DEPENDS ${PYTHON_TUTORIALS} SOURCES ${PYTHON_TUTORIALS})
add_dependencies(${TUTORIAL_NAME} _ELL_python ell_python_utilities)
set_property(TARGET ${TUTORIAL_NAME} PROPERTY FOLDER "tutorials/vision")

# copy tutorial files
configure_file(../../../tools/utilities/pythonlibs/demo.py demo.py COPYONLY)
configure_file(../../../tools/utilities/pythonlibs/demoHelper.py demoHelper.py COPYONLY)
configure_file(../../../tools/utilities/pythonlibs/find_ell.py find_ell.py COPYONLY)
configure_file(README.md README.md COPYONLY)
configure_file(zebra.png zebra.png COPYONLY)
