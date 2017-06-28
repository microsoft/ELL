#
# python example projects
#
set (PYTHON_EXAMPLES sgd.py run.bat run.sh)
add_custom_target(python_examples DEPENDS ${PYTHON_EXAMPLES} SOURCES ${PYTHON_EXAMPLES})
add_dependencies(python_examples _ELL_python)
set_property(TARGET python_examples PROPERTY FOLDER "examples")

# copy python example code
foreach(PY_FILE ${PYTHON_EXAMPLES})
    configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
endforeach()
