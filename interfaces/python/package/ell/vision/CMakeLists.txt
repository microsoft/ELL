if(${PYTHON_ENABLED})

    set(module_name "pythonpackage_ell_vision")

    file(GLOB PYFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${PYFILES})
    
    copy_newer_files(${module_name} PYFILES)

    # Put some useful files from the utilities directory into our package
    set(other_files ${PROJECT_SOURCE_DIR}/tools/importers/CNTK/cntk_to_ell.py
                    ${PROJECT_SOURCE_DIR}/tools/importers/darknet/darknet_to_ell.py
                    ${PROJECT_SOURCE_DIR}/tools/utilities/pythonlibs/vision/demoHelper.py)
    
    copy_newer_files(${module_name} other_files)

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python/package/ell/vision")

endif()  # PYTHON_ENABLED