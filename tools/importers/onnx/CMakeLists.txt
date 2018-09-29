#
# cmake file
#

if(${PYTHON_ENABLED} AND ${ONNX})

    set(module_name "onnx_importer")

    set(importer_src onnx_to_ell.py onnx_import.py)

    set(importer_lib lib/onnx_converters.py)
        
    source_group("lib" FILES ${importer_lib})
    add_custom_target(${module_name} DEPENDS SOURCES ${importer_src} ${importer_lib})
    add_dependencies(${module_name} pythonlibs _ELL_python)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")
    
    #copy files
    copy_newer_files(${module_name} importer_src)

    #copy libs
    copy_newer_files(${module_name} importer_lib "${CMAKE_CURRENT_BINARY_DIR}/lib/")

endif()  # PYTHON_ENABLED
