#
# CompilerCache
#

if(NOT DEFINED ENV{CMAKE_COMPILER_CACHE})
  set(CMAKE_COMPILER_CACHE ON)
else()
  set(CMAKE_COMPILER_CACHE $ENV{CMAKE_COMPILER_CACHE})
endif()
option(USE_COMPILER_CACHE "Use a compiler cache (ccache) to speed up build times" ${CMAKE_COMPILER_CACHE})

if(USE_COMPILER_CACHE)
  find_program(CCACHE_EXECUTABLE ccache)
  if(CCACHE_EXECUTABLE)
    message(STATUS "Found cccache at ${CCACHE_EXECUTABLE}")

    # Support Unix Makefiles and Ninja
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_EXECUTABLE}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK  "${CCACHE_EXECUTABLE}")
  endif()
else()
  message(STATUS "Not using ccache")
endif()
