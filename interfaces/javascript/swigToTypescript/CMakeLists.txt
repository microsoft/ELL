#
# swigToTypescript project
#

set (tool_name ell_module_d_ts)


set (MODULE_FILES 
    generate-d-ts.xslt
    index.js
    package.json
)

find_program(NPM_EXECUTABLE npm)
find_program(NODE_EXECUTABLE node)
if(NPM_EXECUTABLE AND NODE_EXECUTABLE)

    add_custom_target(${tool_name}_files ALL
        DEPENDS ${MODULE_FILES}
        COMMENT "Copying module files"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(${tool_name}_install ALL
                    COMMAND ${NPM_EXECUTABLE} install
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Installing prerequisites"
                    VERBATIM)

    add_custom_target(${tool_name} ALL
                    COMMAND ${NPM_EXECUTABLE} run run generate-d-ts.xslt ${CMAKE_CURRENT_BINARY_DIR}/../../xml/ELLXML_wrap.xml ${CMAKE_CURRENT_BINARY_DIR}/../ell_module/ell.d.ts
                    DEPENDS ELL_xml ${tool_name}_install
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Creating .d.ts files"
                    VERBATIM)

    add_dependencies(${tool_name} ${tool_name}_files ${tool_name}_install)

    foreach(MODULE_FILE ${MODULE_FILES})
        configure_file(${MODULE_FILE} ${MODULE_FILE} @ONLY)
        set_property(TARGET ${tool_name}_files APPEND PROPERTY OBJECT_DEPENDS ${MODULE_FILE})
    endforeach()

endif()