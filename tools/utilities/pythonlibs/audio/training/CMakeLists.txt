#
# cmake file for python audio training utilities
#

if(${PYTHON_ENABLED})

    set(module_name "pythonlibs-audio-training")
    
    file(GLOB lib_src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)

    add_custom_target(${module_name} ALL DEPENDS SOURCES ${lib_src})
    add_dependencies(${module_name} pythonlibs)

    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities/audio/training")

    # copy files
    copy_newer_files(${module_name} lib_src)

endif()  # PYTHON_ENABLED
