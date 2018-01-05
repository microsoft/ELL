#
# cmake file
#

if(${PYTHON_ENABLED})
    set(module_name "ell_python_utilities")

    set(src ell_utilities.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${src})
    add_dependencies(${module_name} _ELL_python)

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python")

    foreach(PY_FILE ${src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

endif()  # PYTHON_ENABLED
