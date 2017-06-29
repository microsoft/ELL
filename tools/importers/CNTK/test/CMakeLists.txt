#
# cmake file
#

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PYTHON_EXECUTABLE python)
endif()

find_package(PythonInterp 3.5 QUIET)
if(${PYTHONINTERP_FOUND})
    # test project

    set (test_name ${module_name}_test)

    set (test_src cntk_to_ell_importer_test.py)

    set (test_data xorModel1.dnn)

    add_custom_target(${test_name} DEPENDS ${test_src} SOURCES ${test_src})
    add_dependencies(${test_name} _ELL_python ell_python_utilities)

    set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

    # copy files
    foreach(PYTHON_FILE ${test_src})
        configure_file(${PYTHON_FILE} ${PYTHON_FILE} COPYONLY)
    endforeach()

    foreach(PYTHON_FILE ${test_data})
        configure_file(${PYTHON_FILE} ${PYTHON_FILE} COPYONLY)
    endforeach()

    add_test(NAME ${test_name} COMMAND ${PYTHON_EXECUTABLE} -m unittest ${test_src})
endif()
