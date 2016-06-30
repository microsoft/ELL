#
# cmake file for Embedded Machine Learning Library sub projects
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

add_subdirectory(apply)
add_subdirectory(compile)
if (LLVM_ENABLED)
	add_subdirectory(compileNew)
endif()
add_subdirectory(print)
