#
# python tutorial projects
#
set (TUTORIAL_NAME GettingStarted)
set (PYTHON_TUTORIALS gettingStarted.py README.md cntkVgg16ImageNetLabels.txt darknetImageNetLabels.txt)
add_custom_target(${TUTORIAL_NAME} ALL DEPENDS ${PYTHON_TUTORIALS} SOURCES ${PYTHON_TUTORIALS})

set_property(TARGET ${TUTORIAL_NAME} PROPERTY FOLDER "tutorials//vision")

# copy tutorial code
foreach(PY_FILE ${PYTHON_TUTORIALS})
    configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
endforeach()