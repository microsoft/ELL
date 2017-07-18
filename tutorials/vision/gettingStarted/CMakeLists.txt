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
    cntk.md
    cntkBinarizedDemo.py
    cntkDemo.py
    cntkVgg16ImageNetLabels.txt
    coffeemug.jpg
    compiledCntkDemo.py
    compiledDarknetDemo.py
    compiling-Linux.md
    compiling-Windows.md
    compiling-Pi3.md
    compiling.md
    compilingAdvanced.md
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
set_property(TARGET ${TUTORIAL_NAME} PROPERTY FOLDER "tutorials/vision")

# copy tutorial files
foreach(PY_FILE ${PYTHON_TUTORIALS})
    configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
endforeach()

# creates compile targets for the sample models
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
include(GenerateModelTargets)

set(sample_models
    "vgg16ImageNet compiledCntkDemo.py cntkVgg16ImageNetLabels.txt"
    "darknetReference compiledDarknetDemo.py darknetImageNetLabels.txt")

foreach(sample_model ${sample_models})
    separate_arguments(sample_model)
    list(GET sample_model 0 model_in)
    list(GET sample_model 1 demo_in)
    list(GET sample_model 2 labels_in)

    set(demo_files ${demo_in} ${labels_in})

    # generate the demo model targets
    # host
    set(target_name compiled_${model_in}_host)
    generate_ell_model_compile_target(${model_in} host ${target_name} "${demo_files}")

    # host with profiling
    set(target_name compiled_${model_in}_host_profile)
    generate_ell_model_compile_target(${model_in} host ${target_name} "${demo_files}" --profile)

    # Raspberry Pi 3
    set(target_name compiled_${model_in}_pi3)
    generate_ell_model_compile_target(${model_in} pi3 ${target_name} "${demo_files}")

    # Raspberry Pi 3 with profiling
    set(target_name compiled_${model_in}_pi3_profile)
    generate_ell_model_compile_target(${model_in} pi3 ${target_name} "${demo_files}" --profile)

    # Raspberry Pi Zero
    set(target_name compiled_${model_in}_pi0)
    generate_ell_model_compile_target(${model_in} pi0 ${target_name} "${demo_files}")

    # arm64 Linux
    set(target_name compiled_${model_in}_aarch64)
    generate_ell_model_compile_target(${model_in} aarch64 ${target_name} "${demo_files}")
endforeach()
