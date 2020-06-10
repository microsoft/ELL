if(${PYTHON_ENABLED})

    set(module_name "pythonpackage_ell")

    set(src __init__.py rpi_magic.py platform.py)
    add_custom_target(${module_name} ALL DEPENDS SOURCES ${src})

    set(module_components
        data
        math
        model
        neural
        nodes
        trainers
        util
        vision
    )

    foreach(component ${module_components})
        add_subdirectory(${component})
        add_dependencies(${module_name} ${module_name}_${component})
    endforeach(component ${module_components})

    copy_newer_files(${module_name} src)

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python/package/ell")

endif()  # PYTHON_ENABLED
