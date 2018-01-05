if(${PYTHON_ENABLED})

    set(module_name "pythonpackage_ell_trainers")

    file(GLOB PYFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${PYFILES})

    foreach(FILE ${PYFILES})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python/package/ell/trainers")

endif()  # PYTHON_ENABLED