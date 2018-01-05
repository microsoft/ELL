#
# cmake file
#

if(${PYTHON_ENABLED})

    set(module_name "darknet_importer")

    set(importer_src darknet_to_ell.py
        darknet_import.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${importer_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")
    #copy files
    foreach(PY_FILE ${importer_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

    add_subdirectory(test)
endif()  # PYTHON_ENABLED
