#
# cmake file
#

set(library_name model_testing)

set(src 
    src/ModelTestUtilities.cpp 
)
set(include 
    include/ModelTestUtilities.h 
)
set(tcc 
    tcc/ModelTestUtilities.tcc )

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} utilities model nodes emitters testing)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")
