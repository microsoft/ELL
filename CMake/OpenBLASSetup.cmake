# Centralized place to define OpenBLAS variables 
# Sets the following variables:
#
# General information:
# BLAS_FOUND
#
# Settings for compiling against OpenBLAS libraries:
# BLAS_INCLUDE_DIRS
# BLAS_LIBS
# BLAS_DLL_DIR
#
# Using FindBLAS module:
# find_package(BLAS)
# if(BLAS_FOUND)
#     message(STATUS "Blas libraries: ${BLAS_LIBRARIES}")
#     message(STATUS "Blas linker flags: ${BLAS_LINKER_FLAGS}")
#     message(STATUS "Blas vendor: ${BLA_VENDOR}")
#
# Variables defined by FindBLAS module that we don't set:
#     BLAS_LIBRARIES
#     BLAS_LINKER_FLAGS
#     BLA_VENDOR

# Include guard so we don't try to find or download BLAS more than once
if(BLASSetup_included)
    return()
endif()
set(BLASSetup_included true)

find_package(BLAS)
if(BLAS_FOUND)
    message(STATUS "Blas libraries: ${BLAS_LIBRARIES}")
    message(STATUS "Blas linker flags: ${BLAS_LINKER_FLAGS}")
    message(STATUS "Blas include directories: ${BLAS_INCLUDE_DIRS}")
    set(BLAS_LIBS ${BLAS_LIBRARIES})
else()

    set(BLAS_LIB_NAMES cblas openblas libopenblas.dll.a)

    set(BLAS_INCLUDE_SEARCH_PATHS
        /System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/Current/Headers/
        /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Accelerate.framework/Frameworks/vecLib.framework/Headers/
        /usr/include
        /usr/local/include
    )

    if(APPLE)
    # To install commandline tools (which seems to also install headers in /System/Library/Frameworks/...), do this:
    # xcode-select --install
    endif()

    set(BLAS_FOUND "NO")
    if(WIN32)
        # Determine CPU type
        unset(processor_id CACHE)
        get_filename_component(processor_id "[HKEY_LOCAL_MACHINE\\Hardware\\Description\\System\\CentralProcessor\\0;Identifier]" ABSOLUTE CACHE)
        string(REGEX REPLACE ".* Family ([0-9]+) .*" "\\1" processor_family "${processor_id}")
        string(REGEX REPLACE ".* Model ([0-9]+) .*" "\\1" processor_model "${processor_id}")
        message(STATUS "Processor family: ${processor_family}, model: ${processor_model}")

        set(BLAS_PACKAGE_NAME OpenBLASLibs)
        set(BLAS_PACKAGE_VERSION 0.2.19.2)
        set(PROCESSOR_GENERATION sandybridge)
        if(processor_family EQUAL 6)
            if(processor_model EQUAL 60 OR processor_model EQUAL 63 OR processor_model EQUAL 69 OR processor_model EQUAL 70)
                set(PROCESSOR_GENERATION haswell)
                message(STATUS "Using OpenBLAS compiled for ${PROCESSOR_GENERATION}")
            # elseif(processor_model EQUAL 42 OR processor_model EQUAL 45)
            #     set(PROCESSOR_GENERATION sandybridge)
            #     message(STATUS "Using OpenBLAS compiled for ${PROCESSOR_GENERATION}")
            else()
                # message(WARNING "Unknown processor, assuming ${PROCESSOR_GENERATION}")
                message(WARNING "Unknown processor, diabling OpenBLAS")
                return()
            endif()
        else()
            # message(WARNING "Unknown processor, assuming ${PROCESSOR_GENERATION}")
            message(WARNING "Unknown processor, diabling OpenBLAS")
            return()
        endif()

        set(BLAS_PACKAGE_DIR ${PACKAGE_ROOT}/${BLAS_PACKAGE_NAME}.${BLAS_PACKAGE_VERSION}/build/native/x64/${PROCESSOR_GENERATION})
        set(BLAS_DLLS libopenblas.dll libgcc_s_seh-1.dll libgfortran-3.dll libquadmath-0.dll)
        set(BLAS_DLL_DIR ${BLAS_PACKAGE_DIR}/bin)
        list(APPEND BLAS_INCLUDE_SEARCH_PATHS
            ${BLAS_PACKAGE_DIR}/include/
        )
    endif()

    ## Note: libopenblas installs on ubuntu in /usr/lib and /usr/include
    find_path(BLAS_INCLUDE_DIRS cblas.h
        PATHS ${BLAS_INCLUDE_SEARCH_PATHS}
            NO_DEFAULT_PATH
        )

    set(BLAS_LIB_SEARCH_PATHS
        /usr/lib64/atlas-sse3 /usr/lib64/atlas /usr/lib64 /usr/local/lib64/atlas /usr/local/lib64 /usr/lib/atlas-sse3 /usr/lib/atlas-sse2 /usr/lib/atlas-sse /usr/lib/atlas-3dnow /usr/lib/atlas /usr/lib /usr/local/lib/atlas /usr/local/lib
    )

    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} ".dll.a" ".a")
        list(APPEND BLAS_LIB_SEARCH_PATHS ${BLAS_PACKAGE_DIR}/lib/)    
    endif()

    find_library(BLAS_LIBS
    NAMES ${BLAS_LIB_NAMES}
    PATHS ${BLAS_LIB_SEARCH_PATHS}
    NO_DEFAULT_PATH
    )
endif()

if(BLAS_LIBS AND BLAS_INCLUDE_DIRS)
    message(STATUS "Using BLAS include path: ${BLAS_INCLUDE_DIRS}")
    message(STATUS "Using BLAS library: ${BLAS_LIBS}")
    message(STATUS "Using BLAS DLLs: ${BLAS_DLLS}")
    set(BLAS_FOUND "YES")
else()
    message(STATUS "BLAS library not found")
    set(BLAS_INCLUDE_DIRS "")
    set(BLAS_LIBS "")
    set(BLAS_FOUND "NO")
endif()
