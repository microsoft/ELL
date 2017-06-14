#
# cmake file for Embedded Learning Library subprojects
#

if(BUILD_INTERFACES)
    add_subdirectory(CNTK)
    add_subdirectory(darknet)
endif()
