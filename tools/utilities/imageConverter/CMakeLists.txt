#
# cmake file for compare project
#
find_package(PythonInterp 3.5 )
find_package(PythonLibs 3.5 )

# define project
set (library_name imageConverter)

set (src src/LoadImage.cpp
		 src/InvokePython.cpp)

set (include include/LoadImage.h
				include/InvokePython.h)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

if(${PYTHONLIBS_FOUND})
include_directories(${PYTHON_INCLUDE_PATH})
endif()

include_directories(../../../libraries/utilities/include)

# create executable in build\bin
add_library(${library_name} ${src} ${include})
target_include_directories(${library_name} PUBLIC include)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(${library_name} -pthread)
endif()

set_property(TARGET ${library_name} PROPERTY FOLDER "tools/utilities")

if(${PYTHONLIBS_FOUND})
target_link_libraries(${library_name} ${PYTHON_LIBRARIES})
target_compile_options(${library_name} PRIVATE -DPYTHON_FOUND)
else()
message(WARNING "Since PYTHONLIBS_FOUND is false, the imageConverter will not function, it will just be a stub")
endif()  # PYTHONLIBS_FOUND
