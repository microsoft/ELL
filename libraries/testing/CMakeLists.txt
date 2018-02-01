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
set(tcc )

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} utilities)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")
