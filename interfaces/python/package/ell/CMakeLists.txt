if(${PYTHON_ENABLED})

    set(module_name "pythonpackage_ell")

    set(src __init__.py rpi_magic.py platform.py)

    add_subdirectory(data)
    add_subdirectory(math)
    add_subdirectory(model)
    add_subdirectory(neural)
    add_subdirectory(nodes)
    add_subdirectory(trainers)
    add_subdirectory(util)
    add_subdirectory(vision)

    add_custom_target(${module_name} DEPENDS SOURCES ${src})

    foreach(FILE ${src})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python/package/ell")

endif()  # PYTHON_ENABLED
