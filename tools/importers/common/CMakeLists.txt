#
# cmake file
#

if(${PYTHON_ENABLED})

    set(module_name "importers_common")

    set(common_src converters.py 
        importer.py
        memory_shapes.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${common_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")
    #copy files
    foreach(PY_FILE ${common_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

endif()  # PYTHON_ENABLED
