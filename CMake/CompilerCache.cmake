#
# CompilerCache 
#

function(use_compiler_cache)

  find_program(ccache_executable ccache)
  if(ccache_executable)
    message(STATUS "Found cccache at ${ccache_executable}")

    # Support Unix Makefiles and Ninja
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${ccache_executable}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK  "${ccache_executable}")
  else()
    message(STATUS "Could not find ccache")
  endif()

endfunction()
