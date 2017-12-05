find_package(PythonInterp)
find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})

    set(module_name "pythonpackage_ell_util")

    file(GLOB PYFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${PYFILES})

    foreach(FILE ${PYFILES})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()

    configure_file(
        ${PROJECT_SOURCE_DIR}/tools/utilities/pythonlibs/download_helper.py
        download_helper.py
        COPYONLY
    )

    configure_file(
        ${PROJECT_SOURCE_DIR}/interfaces/python/utilities/ell_utilities.py
        ell_utilities.py
        COPYONLY
    )

    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/python/packages/ell")
endif()
