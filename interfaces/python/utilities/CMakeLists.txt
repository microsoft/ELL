#
# cmake file
#

find_package(PythonInterp)
find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})
    find_package(PythonInterp)

    set(module_name "ell_python_utilities")

    set(src ell_utilities.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${src})
    add_dependencies(${module_name} _ELL_python)

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces")

    foreach(PY_FILE ${src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

endif()  # PYTHONLIBS_FOUND
