#
# cmake file
#

if(${PYTHON_ENABLED} AND ${ONNX})

    set(module_name "onnx_importer")

    set(importer_src onnx_to_ell.py
        onnx_import.py)

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${importer_src})
    add_dependencies(${module_name} pythonlibs _ELL_python)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")
    
    #copy files
    copy_newer_files(${module_name} importer_src)

endif()  # PYTHON_ENABLED
