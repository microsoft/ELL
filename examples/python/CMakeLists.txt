#
# python example projects
#
set (PYTHON_EXAMPLES sgd.py run.bat run.sh)
add_custom_target(python_examples ALL DEPENDS ${PYTHON_EXAMPLES} SOURCES ${PYTHON_EXAMPLES})
add_dependencies(python_examples _ELL_python)
set_property(TARGET python_examples PROPERTY FOLDER "examples")

# copy python example code
copy_newer_files(python_examples PYTHON_EXAMPLES)