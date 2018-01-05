#
# CMake file for Raspberry Pi test tools
#

if(${PYTHON_ENABLED})

    set(module_name "remoterun")

    set(src remoterun.py)

    add_custom_target(${module_name} DEPENDS SOURCES ${src})
    add_dependencies(${module_name} pythonlibs)
    set_property(TARGET ${module_name} PROPERTY FOLDER "tools/utilities")
    
    # copy files
    foreach(FILE ${src})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()
  
endif()  # PYTHON_ENABLED
