#
# cmake file for python utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs")

    set(lib_src
        add_input_metadata.py
        buildtools.py
        cpuinfo.py
        dependency_installer.py
        download_helper.py
        find_ell.py
        logger.py
        modelHelpers.py
        picluster.py
        procmon.py
        remoterunner.py
        ziptools.py)

    add_subdirectory(vision)
    add_subdirectory(audio)

    add_custom_target(${module_name} DEPENDS SOURCES ${lib_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities")

    # copy files
    copy_newer_files(${module_name} lib_src)

    # run flake8
    flake8(${module_name})

endif()