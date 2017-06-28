#
# SWIG example projects
#

add_subdirectory(data)
add_subdirectory(python)

if(SWIG_FOUND)
    add_subdirectory(emitted_interfaces EXCLUDE_FROM_ALL)
endif()