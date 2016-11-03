#
# Build node module
# 

set (PROJ_NAME emll_module)
PROJECT(${PROJ_NAME})

set (MODULE_FILES 
    binding.gyp
    buildme
    buildme.cmd
    copyMacLib.sh
    copyLinuxLib.sh
    copyWindowsLib.cmd
    emll.d.ts
    emll_manual.d.ts
    index.d.ts
    index.js
    package.json
    README.md)

set (NODE_ABI_VERSION 50)
set (ELECTRON_VERSION 1.4.4)

find_program(NPM_EXECUTABLE npm)

if(NPM_EXECUTABLE)
    message(STATUS "Found npm at ${NPM_EXECUTABLE}")

    if(WIN32)
        set (MODULE_BUILD_COMMAND build:win)
    elseif(APPLE)
        set (MODULE_BUILD_COMMAND build:mac)
    elseif(UNIX)
        set (MODULE_BUILD_COMMAND build:linux)
    endif()

    add_custom_target(${PROJ_NAME}_files ALL
        DEPENDS ${MODULE_FILES} 
        COMMENT "Copying module files"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(${PROJ_NAME}_install ALL
                    COMMAND ${NPM_EXECUTABLE} install
                    DEPENDS EMLL_javascript common data evaluators lossFunctions math model nodes predictors trainers utilities compiler emitters
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Installing prerequisites"
                    VERBATIM)

    add_custom_target(${PROJ_NAME} ALL
        COMMAND ${NPM_EXECUTABLE} run ${MODULE_BUILD_COMMAND}
        COMMENT "Building module"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_dependencies(${PROJ_NAME} ${PROJ_NAME}_files ${PROJ_NAME}_install EMLL_xml)

    foreach(MODULE_FILE ${MODULE_FILES})
        add_custom_command(OUTPUT ${MODULE_FILE} 
                        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_FILE} ${CMAKE_CURRENT_BINARY_DIR}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        COMMENT "Copying ${MODULE_FILE}"
                        VERBATIM)
        set_property(TARGET ${PROJ_NAME}_files APPEND PROPERTY OBJECT_DEPENDS ${MODULE_FILE})
    endforeach()

endif()

#
# add tests
#
find_program(NODE_EXECUTABLE node)
if(NPM_EXECUTABLE AND NODE_EXECUTABLE)
    # set up environment for test
    add_custom_command(TARGET ${PROJ_NAME} 
                    POST_BUILD
                    COMMAND ${NPM_EXECUTABLE} install 
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js
                    VERBATIM)

    add_custom_command(TARGET ${PROJ_NAME} 
                    POST_BUILD
                    COMMAND ${NPM_EXECUTABLE} install emll
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js
                    VERBATIM)

    # add test
    set (TEST_NAME ${PROJ_NAME}_test)
    add_test(NAME ${TEST_NAME}
            COMMAND ${NODE_EXECUTABLE} TrainersAsync.js
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js )

endif()