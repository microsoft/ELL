#
# cmake file
#

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PYTHON_EXECUTABLE python)
endif()

if(${PYTHON_ENABLED})
    # test project

    set (test_name ${module_name}_test)

    set (test_src darknet_to_ell_importer_test.py)

    set (test_data unittest.cfg unittest.weights)

    add_custom_target(${test_name} DEPENDS ${test_src} SOURCES ${test_src})
    add_dependencies(${test_name} _ELL_python)

    set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

    # copy files
    foreach(PY_FILE ${test_src})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

    # copy data files
    foreach(PY_FILE ${test_data})
        configure_file(${PY_FILE} ${PY_FILE} COPYONLY)
    endforeach()

    add_test(NAME ${test_name} COMMAND ${PYTHON_EXECUTABLE} -m unittest ${test_src})

endif()  # PYTHON_ENABLED