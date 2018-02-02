#
# CMake file for datasetFromImages utility script
#

if(${PYTHON_ENABLED})

    set(tool_name "datasetFromImages")

    set(src datasetFromImages.py)

    add_custom_target(${tool_name} DEPENDS SOURCES ${src})
    add_dependencies(${tool_name} pythonlibs _ELL_python)
    
    # copy files
    foreach(FILE ${src})
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()

    # put this project in the tools/utilities folder in the IDE 
    set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")

endif()
