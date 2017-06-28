#
# cmake file
#

set (tool_name scripts)

set (src recipeUtil.py)

add_custom_target(${tool_name} DEPENDS ${src} SOURCES ${src})
add_dependencies(${tool_name} _ELL_python)
set_property(TARGET ${tool_name} PROPERTY FOLDER "examples")

# copy contents of scripts directory to build/scripts
foreach(script_file ${src})
    configure_file(${script_file} ${script_file} COPYONLY)
endforeach()                   