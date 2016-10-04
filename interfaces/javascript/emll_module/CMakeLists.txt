#
# Build node module
# 

set (PROJ_NAME emll_module)
PROJECT(${PROJ_NAME})

find_program(NPM_EXECUTABLE npm)

if(NPM_EXECUTABLE)
    message(STATUS "Found npm at ${NPM_EXECUTABLE}")

    if(WIN32)
        set (MODULE_BUILD_COMMAND buildwin)
        set (MODULE_COPY_COMMAND copyWindowsLib)
    elseif(APPLE)
        set (MODULE_BUILD_COMMAND buildmac)
        set (MODULE_COPY_COMMAND copyMacLib)
    elseif(UNIX)
        set (MODULE_BUILD_COMMAND buildlinux)
        set (MODULE_COPY_COMMAND copyLinuxLib)
    endif()

    add_custom_target(${PROJ_NAME} ALL
        COMMAND ${NPM_EXECUTABLE} run ${MODULE_BUILD_COMMAND}
        DEPENDS EMLL_javascript
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_custom_command(TARGET ${PROJ_NAME} PRE_BUILD
                    COMMAND ${NPM_EXECUTABLE} install
                    DEPENDS EMLL_javascript common dataset evaluators linear lossFunctions math model nodes predictors trainers utilities
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                    COMMENT "Installing prerequisites"
                    VERBATIM)

    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
                    COMMAND ${MODULE_COPY_COMMAND}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                    COMMENT "Copying module"
                    VERBATIM)
endif()

#
# add tests
#
find_program(NODE_EXECUTABLE node)
if(NPM_EXECUTABLE AND NODE_EXECUTABLE)

    # set up environment for test
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
                    COMMAND ${NPM_EXECUTABLE} install && ${NPM_EXECUTABLE} install emll
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js)

    # add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
    #                 COMMAND ${NPM_EXECUTABLE} install emll
    #                 WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js)

    # add test
    set (TEST_NAME ${PROJ_NAME}_test)
    add_test(NAME ${TEST_NAME}
            COMMAND ${NODE_EXECUTABLE} TrainersAsync.js
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js )

endif()