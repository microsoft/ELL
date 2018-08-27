#
# cmake file
#

if(${PYTHON_ENABLED})

    set(module_name "onnx_importer")

    set(importer_src onnx_to_ell.py
        onnx_import.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${importer_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")
    #copy files
    foreach(PY_FILE ${importer_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

endif()  # PYTHON_ENABLED
