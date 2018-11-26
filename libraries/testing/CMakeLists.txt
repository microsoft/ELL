#
# cmake file
#

set(library_name testing)

set(src
    src/testing.cpp
)
set(include
    include/testing.h
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

add_library(${library_name} ${src} ${include})
target_include_directories(${library_name} PRIVATE include ${ELL_LIBRARIES_DIR})
target_link_libraries(${library_name} utilities)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")
