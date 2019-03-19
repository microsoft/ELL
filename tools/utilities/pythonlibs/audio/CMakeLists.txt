#
# cmake file for python audio utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs-audio")
    
    file(GLOB lib_src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${lib_src})
    add_dependencies(${module_name} pythonlibs)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities/audio")

    # copy files
    copy_newer_files(${module_name} lib_src)
    
    add_subdirectory(training)
    add_subdirectory(test)

endif()  # PYTHON_ENABLED
