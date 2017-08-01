#
# cmake file
#

find_package(PythonInterp)
find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})
    find_package(PythonInterp)

    set(module_name "cntk_importer")

    set(importer_src cntk_to_ell.py
        lib/cntk_converters.py
        lib/cntk_layers.py
        lib/cntk_utilities.py
        custom_functions.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${importer_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")

    # copy files
    foreach(PY_FILE ${importer_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

    add_subdirectory(test)
endif()  # PYTHONLIBS_FOUND
