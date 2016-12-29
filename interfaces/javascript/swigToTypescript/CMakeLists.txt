#
# swigToTypescript project
#

set (PROJ_NAME ell_module_d_ts)
PROJECT(${PROJ_NAME})

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (MODULE_FILES 
    generate-d-ts.xslt
    index.js
    package.json
)

find_program(NPM_EXECUTABLE npm)
find_program(NODE_EXECUTABLE node)
if(NPM_EXECUTABLE AND NODE_EXECUTABLE)

    add_custom_target(${PROJ_NAME}_files ALL
        DEPENDS ${MODULE_FILES}
        COMMENT "Copying module files"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(${PROJ_NAME}_install ALL
                    COMMAND ${NPM_EXECUTABLE} install
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Installing prerequisites"
                    VERBATIM)

    add_custom_target(${PROJ_NAME} ALL
                    COMMAND ${NPM_EXECUTABLE} run run generate-d-ts.xslt ${CMAKE_CURRENT_BINARY_DIR}/../../xml/ELLXML_wrap.xml ${CMAKE_CURRENT_BINARY_DIR}/../ell_module/ell.d.ts
                    DEPENDS ELL_xml ${PROJ_NAME}_install
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                    COMMENT "Creating .d.ts files"
                    VERBATIM)

    add_dependencies(${PROJ_NAME} ${PROJ_NAME}_files ${PROJ_NAME}_install)

    foreach(MODULE_FILE ${MODULE_FILES})
        configure_file(${MODULE_FILE} ${MODULE_FILE} @ONLY)
        set_property(TARGET ${PROJ_NAME}_files APPEND PROPERTY OBJECT_DEPENDS ${MODULE_FILE})
    endforeach()

endif()