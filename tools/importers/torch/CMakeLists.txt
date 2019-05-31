#
# cmake file
#

if(${PYTHON_ENABLED} AND ${ONNX})

    set(module_name "torch_importer")

    set(importer_src torch_verifier.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${importer_src})
    add_dependencies(${module_name} onnx_importer pythonlibs)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")

    #copy files
    copy_newer_files(${module_name} importer_src)

    add_subdirectory(test)

    # run flake8
    flake8(${module_name})
endif()  # PYTHON_ENABLED
