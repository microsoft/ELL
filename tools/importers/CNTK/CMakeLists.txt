#
# cmake file
#

if(${PYTHON_ENABLED} AND ${CNTK})

    set(module_name "cntk_importer")

    set(importer_src cntk_to_ell.py
        cntk_import.py
        custom_functions.py)
        
    set(importer_lib lib/cntk_converters.py
        lib/cntk_layers.py
        lib/cntk_utilities.py)
        
    source_group("lib" FILES ${importer_lib})

    add_custom_target(${module_name} DEPENDS SOURCES ${importer_src} ${importer_lib})
    add_dependencies(${module_name} pythonlibs)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")

    # copy files
    copy_newer_files(${module_name} importer_src)
    copy_newer_files(${module_name} importer_lib "${CMAKE_CURRENT_BINARY_DIR}/lib/")

    add_subdirectory(test)
endif()  # PYTHON_ENABLED
