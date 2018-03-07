#
# cmake file
#

if(${PYTHON_ENABLED})

    set(module_name "cntk_importer")

    set(importer_src cntk_to_ell.py
        cntk_import.py
        lib/cntk_converters.py
        lib/cntk_layers.py
        lib/cntk_utilities.py
        lib/shared_converters.py
        lib/shared_importer.py
        custom_functions.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${importer_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")

    # copy files
    foreach(PY_FILE ${importer_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

    add_subdirectory(test)
endif()  # PYTHON_ENABLED
