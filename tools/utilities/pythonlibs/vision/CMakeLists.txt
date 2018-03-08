#
# cmake file for python vision utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs-vision")

    set(lib_src 
        demo.py
        demoHelper.py
        tiled_image.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${lib_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities/vision")
    
    # copy files
    foreach(PY_FILE ${lib_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

endif()  # PYTHON_ENABLED
