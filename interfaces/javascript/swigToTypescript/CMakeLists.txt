#
# swigToTypescript project
#

set (PROJ_NAME ell_module_d_ts)
PROJECT(${PROJ_NAME})

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

find_program(NPM_EXECUTABLE npm)
find_program(NODE_EXECUTABLE node)
if(NPM_EXECUTABLE AND NODE_EXECUTABLE)
    add_custom_target(${PROJ_NAME} ALL
                    COMMAND ${NPM_EXECUTABLE} run run generate-d-ts.xslt ${CMAKE_CURRENT_BINARY_DIR}/../../xml/ELLXML_wrap.xml ${CMAKE_CURRENT_BINARY_DIR}/../ell_module/ell.d.ts
                    DEPENDS ELL_xml
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                    COMMENT "Creating .d.ts files"
                    VERBATIM)
endif()