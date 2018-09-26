#
# cmake file
#

if(${PYTHON_ENABLED})

    set(module_name "darknet_importer")

    set(importer_src darknet_to_ell.py
        darknet_import.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${importer_src})
    add_dependencies(${module_name} pythonlibs _ELL_python)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")

    #copy files
    copy_newer_files(${module_name} importer_src)
    
    add_subdirectory(test)
endif()  # PYTHON_ENABLED
