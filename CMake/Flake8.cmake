#
# cmake macro for invoking flake8 on all python source files in entire subtree of ${CMAKE_CURRENT_SOURCE_DIR} 
#
if(${PYTHON_ENABLED})
    find_program(flake8_BIN NAMES flake8)
    if(NOT flake8_BIN)
        message(WARNING "### could not find 'flake8', please run 'pip install flake8'")
    else()    
        message(STATUS "using program '${flake8_BIN}' with ${FLAKE8_CONFIG}")
    endif()

    # macro to check all python files in CMAKE_CURRENT_SOURCE_DIR
    macro(flake8 target_name)
        if(flake8_BIN)
            add_custom_command(TARGET ${target_name}
                PRE_BUILD 
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMAND ${flake8_BIN} --config ${FLAKE8_CONFIG} .
                COMMENT "Running flake8 on ${CMAKE_CURRENT_SOURCE_DIR} ...")
        endif()
    endmacro()
else()
    macro(flake8 target_name)
    endmacro()
endif()  # PYTHON_ENABLED