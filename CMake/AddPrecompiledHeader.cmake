macro(add_precompiled_header precompiled_header precompiled_source sources_list)
  if(MSVC)
    get_filename_component(precompiled_basename ${precompiled_header} NAME_WE)
    set(precompiled_binary "$(IntDir)/${precompiled_basename}.pch")
    set(sources ${${sources_list}})
    message(STATUS "Old src: ${sources}")

    set_source_files_properties(${precompiled_source}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${precompiled_header}\" /Fp\"${precompiled_binary}\""
                                           OBJECT_OUTPUTS "${precompiled_binary}")
    set_source_files_properties(${sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${precompiled_header}\" /FI\"${precompiled_header}\" /Fp\"${precompiled_binary}\""
                                           OBJECT_DEPENDS "${precompiled_binary}")  
    # Add precompiled header to SourcesVar
    list(APPEND ${sources_list} ${precompiled_source})

    message(STATUS "New src: ${${sources_list}}")
  endif()
endmacro()
