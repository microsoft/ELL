#
# python tutorial projects
#
set (TUTORIAL_NAME GettingStarted)
set (PYTHON_TUTORIALS
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