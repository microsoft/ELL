#
# cmake file for python vision utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs-vision")

    set(lib_src
        add_image_preprocessing_metadata.py
        demo.py
        demoHelper.py
        tiled_image.py)

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${lib_src})
    add_dependencies(${module_name} pythonlibs)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities/vision")

    # copy files
    copy_newer_files(${module_name} lib_src)

endif()  # PYTHON_ENABLED
