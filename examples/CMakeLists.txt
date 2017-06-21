#
# SWIG example projects
#

add_subdirectory(data)
add_subdirectory(python)

if(BUILD_INTERFACES AND SWIG_FOUND)
    add_subdirectory(emitted_interfaces)
endif()