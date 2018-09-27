#
# cmake file
#

if(${PYTHON_ENABLED})

    set(module_name "importers_common")

    set(common_src converters.py 
        importer.py
        memory_shapes.py)

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${common_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/importers")

    #copy files
    copy_newer_files(${module_name} common_src)

    add_subdirectory(test)

endif()  # PYTHON_ENABLED
