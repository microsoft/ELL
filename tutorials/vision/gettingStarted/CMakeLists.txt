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

set (TUTORIAL_NAME GettingStarted)
set (PYTHON_TUTORIALS
    coffeemug.jpg
    compiledCntkDemo.py
    compiledDarknetDemo.py
    cntk.md
    cntkDemo.py
    cntkVgg16ImageNetLabels.txt
    compiling.md
    darknet.md
    darknetDemo.py
    darknetImageNetLabels.txt
    findEll.py
    modelHelper.py
    opencv.md
    PretrainedModels.md
    README.md
    Screenshot.png)

add_custom_target(${TUTORIAL_NAME} DEPENDS ${PYTHON_TUTORIALS} SOURCES ${PYTHON_TUTORIALS})
add_dependencies(${TUTORIAL_NAME} _ELL_python ell_python_utilities)

set_property(TARGET ${TUTORIAL_NAME} PROPERTY FOLDER "tutorials//vision")

# copy tutorial code
foreach(PY_FILE ${PYTHON_TUTORIALS})
    configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
endforeach()

# Create a compile script and setup.py files for the CNTK vgg16ImageNet model
# and for the darknetReference model
set (SAMPLE_MODELS vgg16ImageNet darknetReference)
foreach(MODEL_IN ${SAMPLE_MODELS})
    set(ELL_model ${MODEL_IN})
    configure_file(setup.py.in ${MODEL_IN}_setup.py)
    configure_file(compileModel.cmd.in compile_${MODEL_IN}.cmd)
    configure_file(compileModel.sh.in compile_${MODEL_IN}.sh)
endforeach()

