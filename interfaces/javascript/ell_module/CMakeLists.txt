#
# Build node module
#

# Exports the following global variables
#
# MODULE_INTERFACE_SRC
# MODULE_BLAS_LIBS
# MODULE_LLVM_LIBS
# MODULE_BUILD_COMMAND
#

set(module_name ell_module)

set(module_files
    templates/binding.gyp
    templates/buildme
    templates/buildme.cmd
    templates/copyMacLib.sh
    templates/copyLinuxLib.sh
    templates/copyWindowsLib.cmd
    templates/ell_manual.d.ts
    templates/index.d.ts
    templates/index.js
    templates/package.json
    templates/README.md
)

set (NODE_ABI_VERSION 53)
set (ELECTRON_VERSION 1.7.9)

find_program(NPM_EXECUTABLE npm)
if(NPM_EXECUTABLE)
    message(STATUS "Found npm at ${NPM_EXECUTABLE}")

    if(WIN32)
        set(MODULE_BUILD_COMMAND build:win)
    elseif(APPLE)
        set(MODULE_BUILD_COMMAND build:mac)
    elseif(UNIX)
        set(MODULE_BUILD_COMMAND build:linux)
    endif()

    add_custom_target(${module_name}_files
        DEPENDS ${module_files} ell_module_d_ts
        COMMENT "Copying module files"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_property(TARGET ${module_name}_files PROPERTY FOLDER "interfaces/javascript")

    add_custom_target(${module_name}_install
                    COMMAND ${NPM_EXECUTABLE} install
                    DEPENDS ELL_javascript common data evaluators functions math model nodes predictors trainers utilities emitters
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Installing prerequisites"
                    VERBATIM)
    set_property(TARGET ${module_name}_install PROPERTY FOLDER "interfaces/javascript")

    add_custom_target(${module_name}
        COMMAND ${NPM_EXECUTABLE} run ${MODULE_BUILD_COMMAND}
        COMMENT "Building module"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_property(TARGET ${module_name} PROPERTY FOLDER "interfaces/javascript")

    add_dependencies(${module_name} ${module_name}_files ${module_name}_install ELL_xml)

    # Generate list of external libraries to link against in a format binding.gyp will like
    foreach(lib ${LLVM_LIBS})
        list(APPEND MODULE_LLVM_LIBS "'-l${lib}',\n")
    endforeach()
    string(CONCAT MODULE_LLVM_LIBS ${MODULE_LLVM_LIBS})

    if(UNIX)
        # On Linux, ld expects -lblas instead of the path to libblas.so
        list(APPEND MODULE_BLAS_LIBS "'-lblas',\n")
    else()
        foreach(lib ${BLAS_LIBS})
            list(APPEND MODULE_BLAS_LIBS "'-l${lib}',\n")
        endforeach()
    endif()
    string(CONCAT MODULE_BLAS_LIBS ${MODULE_BLAS_LIBS})

    foreach(src ${INTERFACE_SRC})
        list(APPEND MODULE_INTERFACE_SRC "'${src}',\n")
    endforeach()
    string(CONCAT MODULE_INTERFACE_SRC ${MODULE_INTERFACE_SRC})

    foreach(module_file_template ${module_files})
        get_filename_component(module_file ${module_file_template} NAME)
        configure_file(${module_file_template} ${module_file} @ONLY)
        set_property(TARGET ${module_name}_files APPEND PROPERTY OBJECT_DEPENDS ${module_file})
    endforeach()

endif()

#
# add tests
#
find_program(NODE_EXECUTABLE node)
if(NPM_EXECUTABLE AND NODE_EXECUTABLE)
    set(TEST_DEPLOY_DIR ${CMAKE_BINARY_DIR}/interfaces/javascript)
    
    file(COPY ${CMAKE_CURRENT_LIST_DIR}/test
        DESTINATION ${TEST_DEPLOY_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
        GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

    # set up environment for test
    add_custom_command(TARGET ${module_name}
                    POST_BUILD
                    COMMAND ${NPM_EXECUTABLE} install
                    WORKING_DIRECTORY ${TEST_DEPLOY_DIR}/test/js
                    VERBATIM)

    add_custom_command(TARGET ${module_name}
                    POST_BUILD
                    COMMAND ${NPM_EXECUTABLE} install ell
                    WORKING_DIRECTORY ${TEST_DEPLOY_DIR}/test/js
                    VERBATIM)

    # add test
    set(test_name ${module_name}_test)
    add_test(NAME ${test_name}
            COMMAND ${NODE_EXECUTABLE} TestCompiler.js
            WORKING_DIRECTORY ${TEST_DEPLOY_DIR}/test/js )
endif()
