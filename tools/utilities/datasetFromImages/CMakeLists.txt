#
# CMake file for datasetFromImages utility script
#

if(${PYTHON_ENABLED})

    set(tool_name "datasetFromImages")

    set(src datasetFromImages.py)

    add_custom_target(${tool_name} ALL DEPENDS SOURCES ${src})
    add_dependencies(${tool_name} pythonlibs)
    
    # copy files
    copy_newer_files(${tool_name} src)

    # put this project in the tools/utilities folder in the IDE 
    set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")

endif()
