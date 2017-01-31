#
# cmake file
#

set (tool_name scripts)

set (src recipeUtil.py)

add_custom_target(${tool_name} ALL DEPENDS ${src} SOURCES ${src})

# copy contents of scripts directory to build/scripts
foreach(script_file ${src})
    configure_file(${script_file} ${script_file} COPYONLY)
endforeach()                   