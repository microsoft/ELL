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
        set (MODULE_BUILD_COMMAND buildwin)
        set (MODULE_COPY_COMMAND copyWindowsLib)
    elseif(APPLE)
        set (MODULE_BUILD_COMMAND buildmac)
        set (MODULE_COPY_COMMAND copyMacLib)
    elseif(UNIX)
        set (MODULE_BUILD_COMMAND buildlinux)
        set (MODULE_COPY_COMMAND copyLinuxLib)
    endif()

    if(WIN32)
        add_custom_target(${PROJ_NAME} ALL
            COMMAND ${NPM_EXECUTABLE} run ${MODULE_BUILD_COMMAND}
            DEPENDS EMLL_javascript ${PROJ_NAME}_copy
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    else()
        add_custom_target(${PROJ_NAME} ALL
            COMMAND ${NPM_EXECUTABLE} run ${MODULE_BUILD_COMMAND}
            DEPENDS EMLL_javascript
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
    endif()

    foreach(MODULE_FILE ${MODULE_FILES})
        add_custom_command(TARGET ${PROJ_NAME} 
                        PRE_BUILD
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                        COMMAND ${CMAKE_COMMAND} -E copy ${MODULE_FILE} ${CMAKE_CURRENT_BINARY_DIR})
    endforeach()

    add_custom_command(TARGET ${PROJ_NAME} PRE_BUILD
                    COMMAND ${NPM_EXECUTABLE} install
                    DEPENDS EMLL_javascript common dataset evaluators linear lossFunctions math model nodes predictors trainers utilities
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                    COMMENT "Installing prerequisites"
                    VERBATIM)
endif()


#
# add tests
#
find_program(NODE_EXECUTABLE node)
if(NPM_EXECUTABLE AND NODE_EXECUTABLE)
    # set up environment for test
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
                    COMMAND ${NPM_EXECUTABLE} install 
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js
                    VERBATIM)

    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
                    COMMAND ${NPM_EXECUTABLE} install emll
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js
                    VERBATIM)

    # add test
    set (TEST_NAME ${PROJ_NAME}_test)
    add_test(NAME ${TEST_NAME}
            COMMAND ${NODE_EXECUTABLE} TrainersAsync.js
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js )

endif()