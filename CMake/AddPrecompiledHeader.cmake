#
# AddPrecompiledHeader 
#

function(add_precompiled_header target precompiled_header precompiled_source)
  if(MSVC)
    get_filename_component(precompiled_basename ${precompiled_header} NAME_WE)
    set(precompiled_binary "$(IntDir)/${precompiled_basename}.pch")
    get_target_property(sources ${target} SOURCES)
    message(STATUS "Sources before adding precomp header: ${sources}")

    set_source_files_properties(precompiled_source
                                PROPERTIES COMPILE_FLAGS "/Yc\"${precompiled_header}\" /Fp\"${precompiled_binary}\""
                                           OBJECT_OUTPUTS "${precompiled_binary}")
    set_source_files_properties(sources
                                PROPERTIES COMPILE_FLAGS "/Yu\"${precompiled_header}\" /FI\"${precompiled_header}\" /Fp\"${precompiled_binary}\""
                                           OBJECT_DEPENDS "${precompiled_binary}")  
    # Add precompiled header to SourcesVar
    # list(APPEND ${sources_list} ${precompiled_source})
    # target_sources(${target} ${precompiled_header})
    target_sources(${target} PRIVATE ${precompiled_source})

    get_target_property(sources2 ${target} SOURCES)
    message(STATUS "Sources after adding precomp header: ${sources2}")
  endif()
endfunction()
