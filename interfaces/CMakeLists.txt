#
# SWIG interfaces
#
include (CommonInterfaces)

add_subdirectory(common)

find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})
  add_subdirectory(python)
else()
  message(WARNING "Couldn't find Python libraries, python interfaces skipped.")
endif()

# add_subdirectory(R) # currently doesn't compile because std_shared_ptr.i doesn't exist for R
add_subdirectory(csharp)
# add_subdirectory(xml) # currently doesn't compile because CMake wants to compile SWIG's ouptut code file, which doesn't work when it's XML

