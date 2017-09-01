#
# CMake file for Raspberry Pi test tools
#

find_package(PythonInterp)
if(${PYTHONINTERP_FOUND})

    set(module_name "pitest")

    set(test_src drivetest.py runtest.sh.in coffeemug.jpg buildtask.cmd modelTester.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${test_src})

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities")
    # copy files
    foreach(FILE ${test_src})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()

endif()  # PYTHONINTERP_FOUND
