if(${PYTHON_ENABLED})

    set(module_name "pythonpackage_ell_util")

    file(GLOB PYFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${PYFILES})
    
    copy_newer_files(${module_name} PYFILES)

    set (other_files ${PROJECT_SOURCE_DIR}/tools/utilities/pythonlibs/download_helper.py
                     ${PROJECT_SOURCE_DIR}/tools/utilities/pythonlibs/logger.py
                     ${PROJECT_SOURCE_DIR}/tools/utilities/pythonlibs/find_ell.py)

                     
    copy_newer_files(${module_name} other_files)

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python/package/ell/util")

endif()  # PYTHON_ENABLED