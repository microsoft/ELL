#
# cmake file
#

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PYTHON_EXECUTABLE python)
endif()


if(${PYTHON_ENABLED} AND NOT APPLE)
    # test project

    set (test_name ${module_name}_test)

    set (test_src cntk_to_ell_importer_test.py)
    set (src ${test_src} cntk_full_model_test.py)

    set (test_data xorModel1.dnn)

    add_custom_target(${test_name} ALL DEPENDS ${src} SOURCES ${src})
    add_dependencies(${test_name} ${module_name})

    set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

    # copy files
    copy_newer_files(${test_name} src)
    copy_newer_files(${test_name} test_data)
    
    add_test(NAME ${test_name} COMMAND ${PYTHON_EXECUTABLE} -m unittest ${test_src})

endif()  # PYTHON_ENABLED