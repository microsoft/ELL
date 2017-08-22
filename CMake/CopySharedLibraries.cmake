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
