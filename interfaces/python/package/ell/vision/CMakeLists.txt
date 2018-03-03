if(${PYTHON_ENABLED})

    set(module_name "pythonpackage_ell_vision")

    file(GLOB PYFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${PYFILES})

    foreach(FILE ${PYFILES})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()

    # Put some useful files from the utilities directory into our package
    configure_file(${PROJECT_SOURCE_DIR}/tools/importers/CNTK/cntk_to_ell.py cntk_to_ell.py COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/tools/importers/darknet/darknet_to_ell.py cntk_to_ell.py COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/tools/utilities/pythonlibs/vision/demoHelper.py demoHelper.py COPYONLY)

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python/package/ell/vision")

endif()  # PYTHON_ENABLED