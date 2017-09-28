find_package(PythonInterp)
find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})

    set(module_name "pythonpackage_ell")

    set(src __init__.py rpi_magic.py platform.py pretrained_model.py)

    add_subdirectory(util)
    add_subdirectory(vision)

    add_custom_target(${module_name} DEPENDS SOURCES ${src})

    foreach(FILE ${src})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/packages/ell")
endif()

