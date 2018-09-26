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

    add_custom_target(${test_name} ALL DEPENDS ${test_src} SOURCES ${test_src})
    add_dependencies(${test_name} ${module_name})

    set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

    # copy files
    copy_newer_files(${test_name} test_src)

    # copy data files
    copy_newer_files(${test_name} test_data)

    add_test(NAME ${test_name} COMMAND ${PYTHON_EXECUTABLE} -m unittest ${test_src})

endif()  # PYTHON_ENABLED