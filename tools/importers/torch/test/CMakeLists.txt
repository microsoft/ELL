#
# cmake file
#

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PYTHON_EXECUTABLE python)
endif()

if(${PYTHON_ENABLED})
    # test project

    set (test_name ${module_name}_test)

    set (test_src torch_importer_test.py custom_layers.py eight.jpg)

    add_custom_target(${test_name} ALL DEPENDS ${test_src} SOURCES ${test_src})
    add_dependencies(${test_name} ${module_name})

    set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

    # copy files
    copy_newer_files(${test_name} test_src)

    # disabled until we merge with master
    # add_test(NAME ${test_name} COMMAND ${PYTHON_EXECUTABLE} -m unittest torch_importer_test.py)

endif()  # PYTHON_ENABLED
