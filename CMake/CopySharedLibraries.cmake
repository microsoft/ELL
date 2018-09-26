#
# CopySharedLibraries.cmake
#

# Copies necessary DLLs to global binary directory
macro(copy_shared_libraries target_name)
    if(WIN32)
        set(target_location "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/")
        if(EXISTS ${BLAS_DLL_DIR})
            set(command_target_name copy_dlls_to_${target_name})
            foreach(blas_dll ${BLAS_DLLS})
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${target_location}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BLAS_DLL_DIR}/${blas_dll} ${target_location}
                )
            endforeach()
        endif()
    endif()
endmacro()

macro(set_test_library_path test_name)
    if(WIN32)
        set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
        set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "PATH=${GLOBAL_BIN_DIR}/$<CONFIG>/;%PATH%")
    endif()
endmacro()

macro(copy_newer_files target_name file_list)
    set(out_dir ${CMAKE_CURRENT_BINARY_DIR})

    # see if optional 3rd argument specified which provides alternate output dir
    set(extra_macro_args ${ARGN})
    list(LENGTH extra_macro_args num_extra_args)
    if (${num_extra_args} GREATER 0)
        list(GET extra_macro_args 0 out_dir)
        add_custom_target(${target_name}_make_directory ALL 
            COMMAND ${CMAKE_COMMAND} -E make_directory ${out_dir})
    endif()

    foreach(file_name ${${file_list}})
        # note: this works better than configure_file.  configure_file causes cmake regeneration
        # any time you touch any file used by configure_file, which makes incremental build of 
        # such projects slow and painful.
        add_custom_command(TARGET ${target_name} 
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file_name} ${out_dir})
    endforeach()
endmacro()
