#
# cmake file for python audio utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs-audio-test")
    
    file(GLOB test_src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${test_src})
    add_dependencies(${module_name} pythonlibs)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tests")

    # copy files
    copy_newer_files(${module_name} test_src)
    
    # unit test
    if(${ONNX})
        add_test(NAME ${module_name} COMMAND ${PYTHON_EXECUTABLE} -m unittest ${test_src})
    endif() # ONNX

endif()  # PYTHON_ENABLED
