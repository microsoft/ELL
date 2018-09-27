#
# cmake file
#

if(${PYTHON_ENABLED})

    set (test_name ${module_name}_test)

    set (test_src common_importer_test.py)

    add_custom_target(${test_name} DEPENDS ${test_src} SOURCES ${test_src})
    add_dependencies(${test_name} _ELL_python)

    set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

    # copy files
    foreach(PY_FILE ${test_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

endif()  # PYTHON_ENABLED
