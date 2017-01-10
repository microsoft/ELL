#
# Build node module
# 

set (PROJ_NAME ell_module)
PROJECT(${PROJ_NAME})

set (MODULE_FILES 
    binding.gyp
    buildme
    buildme.cmd
    copyMacLib.sh
    copyLinuxLib.sh
    copyWindowsLib.cmd
    ell_manual.d.ts
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
        DEPENDS ${MODULE_FILES} ell_module_d_ts
        COMMENT "Copying module files"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(${PROJ_NAME}_install ALL
                    COMMAND ${NPM_EXECUTABLE} install
                    DEPENDS ELL_javascript common data evaluators lossFunctions math model nodes predictors trainers utilities compiler emitters
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Installing prerequisites"
                    VERBATIM)

    add_custom_target(${PROJ_NAME} ALL
        COMMAND ${NPM_EXECUTABLE} run ${MODULE_BUILD_COMMAND}
        COMMENT "Building module"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_dependencies(${PROJ_NAME} ${PROJ_NAME}_files ${PROJ_NAME}_install ELL_xml)

    # Generate list of LLVM libraries to link against in a format binding.gyp will like
    foreach(LIB ${LLVM_LIBS})
        list(APPEND MODULE_LLVM_LIBS "'-l${LIB}',\n")
    endforeach()
    string(CONCAT MODULE_LLVM_LIBS ${MODULE_LLVM_LIBS})
    foreach(MODULE_FILE ${MODULE_FILES})
        configure_file(${MODULE_FILE} ${MODULE_FILE} @ONLY)
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
                    COMMAND ${NPM_EXECUTABLE} install ell
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js
                    VERBATIM)

    # add test
    set (TEST_NAME ${PROJ_NAME}_test)
    add_test(NAME ${TEST_NAME}
            COMMAND ${NODE_EXECUTABLE} TestCompiler.js
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/test/js )

endif()