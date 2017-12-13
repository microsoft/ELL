#
# swigToTypescript project
#

set(tool_name ell_module_d_ts)

set(module_files
    templates/generate-d-ts.xslt
    templates/index.js
    templates/package.json
)

find_program(NPM_EXECUTABLE npm)
find_program(NODE_EXECUTABLE node)
if(NPM_EXECUTABLE AND NODE_EXECUTABLE)

    add_custom_target(${tool_name}_files
        DEPENDS ${module_files}
        COMMENT "Copying module files"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_property(TARGET ${tool_name}_files PROPERTY FOLDER "interfaces/javascript")

    add_custom_target(${tool_name}_install
                    COMMAND ${NPM_EXECUTABLE} install
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Installing prerequisites"
                    VERBATIM)
    set_property(TARGET ${tool_name}_install PROPERTY FOLDER "interfaces/javascript")

    add_custom_target(${tool_name}
                    COMMAND ${NPM_EXECUTABLE} run run generate-d-ts.xslt ${CMAKE_CURRENT_BINARY_DIR}/../../xml/ELLXML_wrap.xml ${CMAKE_CURRENT_BINARY_DIR}/../ell_module/ell.d.ts
                    DEPENDS ELL_xml ${tool_name}_install
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Creating .d.ts files"
                    VERBATIM)
    set_property(TARGET ${tool_name} PROPERTY FOLDER "interfaces/javascript")

    add_dependencies(${tool_name} ${tool_name}_files ${tool_name}_install)

    foreach(module_file_template ${module_files})
        get_filename_component(module_file ${module_file_template} NAME)
        configure_file(${module_file_template} ${module_file} @ONLY)
        set_property(TARGET ${tool_name}_files APPEND PROPERTY OBJECT_DEPENDS ${module_file})
    endforeach()

endif()
