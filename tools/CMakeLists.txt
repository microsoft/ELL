#
# cmake file for Embedded Learning Library subprojects
#

set(ELL_LIBRARIES_DIR ${CMAKE_CURRENT_LIST_DIR}/../libraries)

add_subdirectory(importers)
add_subdirectory(trainers)
add_subdirectory(utilities)
add_subdirectory(wrap)
