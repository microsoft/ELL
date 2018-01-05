if(${PYTHON_ENABLED})

    set(module_name "pythonpackage")
    
    set(src bld.bat build.sh MANIFEST.in meta.yaml setup.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${src})

    foreach(FILE ${src})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python/package")

endif()  # PYTHON_ENABLED

add_subdirectory(ell)
