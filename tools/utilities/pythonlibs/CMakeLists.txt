#
# cmake file
#

find_package(PythonInterp)
find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})

    set(module_name "pythonlibs")

    set(importer_src demo.py demoHelper.py find_ell.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${importer_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities")
    #copy files
    foreach(PY_FILE ${importer_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

endif()  # PYTHONLIBS_FOUND
