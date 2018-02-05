#
# cmake file for pythonPlugin project
#

# define project
set (library_name pythonPlugins)

set (src  src/InvokePython.cpp src/imageConverter.py src/audioConverter.py )

set (include include/InvokePython.h)
  

source_group("src" FILES ${src})
source_group("include" FILES ${include})

if(${PYTHON_ENABLED})
    include_directories(${PYTHON_INCLUDE_PATH})
endif()  # PYTHON_ENABLED

include_directories(../../../libraries/utilities/include)

# create executable in build\bin
add_library(${library_name} ${src} ${include})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} Threads::Threads)

set_property(TARGET ${library_name} PROPERTY FOLDER "tools/utilities")

if(${PYTHON_ENABLED})
    target_link_libraries(${library_name} ${PYTHON_LIBRARIES})
    target_compile_options(${library_name} PRIVATE -DPYTHON_FOUND)
else()
    message(WARNING "Python build is disabled, so the pythonPlugin will not function, it will just be a stub")
endif()  # PYTHON_ENABLED
