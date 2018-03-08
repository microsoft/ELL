#
# cmake file for python utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs")

    set(lib_src 
        buildtools.py 
        dependency_installer.py
        download_helper.py
        find_ell.py 
        logger.py
        picluster.py
        procmon.py 
        remoterunner.py
        ziptools.py)

    add_subdirectory(vision)
    add_subdirectory(audio)

    add_custom_target(${module_name} DEPENDS SOURCES ${lib_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities")
    
    # copy files
    foreach(PY_FILE ${lib_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

endif()  # PYTHON_ENABLED
