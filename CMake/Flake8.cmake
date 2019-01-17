#
# cmake file for invoking flake8 on all python source files
#

find_program(flake8_BIN NAMES flake8)
if(NOT flake8_BIN)
    message(WARNING "### could not find 'flake8', please run 'pip install flake8'")
else()    
    message(STATUS "using program '${flake8_BIN}'")
endif()

# macro to check all python files in CMAKE_CURRENT_SOURCE_DIR
macro(flake8 target_name config_file)
    if(flake8_BIN)
        add_custom_command(TARGET ${target_name}
            PRE_BUILD 
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${flake8_BIN} --config ${config_file} .
            COMMENT "Running flake8 on ${CMAKE_CURRENT_SOURCE_DIR} ...")
    endif()
endmacro()