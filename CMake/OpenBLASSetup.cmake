#
# OpenBLASSetup
#

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

# Set policy saying to use newish IN_LIST operator
cmake_policy(SET CMP0057 NEW)

# Map of processor name -> OpenBLAS version to use
macro(set_processor_mapping _processor_generation _openblas_version)
  set("processor_map_${_processor_generation}" "${_openblas_version}")
endmacro()

macro(get_processor_mapping _result _processor_generation)
    if(DEFINED processor_map_${_processor_generation})
        set(${_result} ${processor_map_${_processor_generation}})
    else()
        set(${_result} ${_processor_generation})
    endif()
endmacro()

set(BLAS_INCLUDE_SEARCH_PATHS )
set(BLAS_LIB_SEARCH_PATHS )
set(BLAS_LIB_NAMES libopenblas.dll.a libopenblas.a openblas cblas libopenblas.lib openblas.lib)

if(NOT WIN32)
    ## Note: libopenblas installs on ubuntu in /usr/lib and /usr/include or /opt/OpenBLAS if it's manually built
    ## Note: libopenblas installs on openSUSE in /usr/lib64 and /usr/include/openblas
    set(BLAS_INCLUDE_SEARCH_PATHS
        /opt/OpenBLAS/include
        /System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/Current/Headers/
        /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Accelerate.framework/Frameworks/vecLib.framework/Headers/
        /usr/local/Cellar/openblas/0.2.20_1/include
        /usr/include
        /usr/local/include
        /usr/include/openblas
        ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}/${CMAKE_LIBRARY_ARCHITECTURE} # Ubuntu 18.04
    )

    set(BLAS_LIB_SEARCH_PATHS
        /opt/OpenBLAS/lib
        /usr/local/Cellar/openblas/0.2.20_1/lib
        /usr/lib64/atlas-sse3
        /usr/lib64/atlas
        /usr/lib64
        /usr/local/lib64/atlas
        /usr/local/lib64
        /usr/lib/atlas-sse3
        /usr/lib/atlas-sse2
        /usr/lib/atlas-sse
        /usr/lib/atlas-3dnow
        /usr/lib/atlas
        /usr/lib
        /usr/local/lib/atlas
        /usr/local/lib
    )
endif()

# Unless told otherwise, we're going to only look for OpenBLAS since
# others may not be compatible
if(NOT BLA_VENDOR)
    set(BLA_VENDOR "OpenBLAS")
endif()
find_package(BLAS QUIET)
if(BLAS_FOUND)
    if(WIN32)
        get_filename_component(BLAS_LIB_PATH ${BLAS_LIBRARIES} DIRECTORY)
        set(BLAS_INCLUDE_SEARCH_PATHS
            ${BLAS_LIB_PATH}/../include/
            ${BLAS_LIB_PATH}/../include/openblas
        )
        set(BLAS_LIB_SEARCH_PATHS ${BLAS_LIB_PATH})
    endif()

    message(STATUS "Blas libraries: ${BLAS_LIBRARIES}")
    message(STATUS "Blas linker flags: ${BLAS_LINKER_FLAGS}")
    message(STATUS "Blas include directories: ${BLAS_INCLUDE_DIRS}")
    message(STATUS "Blas Vendor: ${BLA_VENDOR}")
    set(BLAS_LIBS ${BLAS_LIBRARIES})
else()
    if(WIN32)

        # This is the default location for a manually installed OpenBlas build.
        if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
            set(BLAS_PACKAGE_DIR $ENV{ProgramW6432}/OpenBlas)
        else()
            set(BLAS_PACKAGE_DIR $ENV{ProgramFiles}/OpenBlas)
        endif()

        if(EXISTS ${BLAS_PACKAGE_DIR}/lib/libopenblas.lib)
            message(STATUS "Found OpenBlas in ${BLAS_PACKAGE_DIR}")
            set(BLAS_DLLS libopenblas.dll)
            set(BLAS_DLL_DIR ${BLAS_PACKAGE_DIR}/bin)
            set(BLAS_INCLUDE_SEARCH_PATHS ${BLAS_PACKAGE_DIR}/include/)
            set(BLAS_LIB_SEARCH_PATHS ${BLAS_PACKAGE_DIR}/lib/)
            set(BLAS_FOUND TRUE)
        else()
            # Known registry ID (family, model) settings for various Intel CPU types
            #
            # Haswell: Family 6, model 60, 63, 69
            # Broadwell: Family 6, Model 70, 79 (compatible with Haswell)
            # Kaby Lake: Family 6, Model 78, 142, 158 (compatible with Haswell)
            # Sandybridge: Family 6, model 42, 45
            # Ivybridge: Family 6, model 58 (compatible with Sandybridge)
            # Skylake: Family 6, model 85
            #
            # Known registry ID (family, model) settings for various AMD CPU types
            # Epyc: Family 23, model 1 (compatible with Haswell)

            # We can set up a mapping from a detected processor generation to the version of
            # the OpenBLAS libraries to use with the set_processor_mapping macro. For instance,
            # if we want to use the haswell libraries on skylake processors, add the following:
            #
            # set_processor_mapping("skylake" "haswell")

            # Determine CPU type
            list(APPEND supported_processors "haswell" "sandybridge") # The list of processor-specific versions of OpenBLAS available in the package

            get_filename_component(processor_id "[HKEY_LOCAL_MACHINE\\Hardware\\Description\\System\\CentralProcessor\\0;Identifier]" ABSOLUTE)
            string(REGEX REPLACE ".* Family ([0-9]+) .*" "\\1" processor_family "${processor_id}")
            string(REGEX REPLACE ".* Model ([0-9]+) .*" "\\1" processor_model "${processor_id}")
            message(STATUS "Processor family: ${processor_family}, model: ${processor_model}")

            set(PROCESSOR_HINT auto CACHE STRING "Processor detection hint (haswell | auto)")
            set(processor_generation "unknown")
            if(${PROCESSOR_HINT} STREQUAL "auto")
                if(processor_family EQUAL 6)
                    if(processor_model EQUAL 60 OR processor_model EQUAL 63 OR processor_model EQUAL 69 OR processor_model EQUAL 70)
                        set(processor_generation "haswell")
                    elseif(processor_model EQUAL 79)
                        set(processor_generation "haswell")  # technically this is broadwell, but it is compatible with haswell.
                    elseif (processor_model EQUAL 78 OR processor_model EQUAL 142 OR processor_model EQUAL 158)
                        set(processor_generation "haswell") # technically this is Kaby Lake, but it is compatible with haswell.
                    elseif(processor_model EQUAL 42 OR processor_model EQUAL 45)
                        set(processor_generation "sandybridge")
                    elseif(processor_model EQUAL 58)
                        set(processor_generation "sandybridge") # actually ivybridge, but it is compatible with sandybridge
                    elseif(processor_model EQUAL 85)
                        set(processor_generation "sandybridge") # actually skylake, but it is compatible with sandybridge
                    endif()
                elseif(processor_family EQUAL 23)
                    if(processor_model EQUAL 1)
                        set(processor_generation "haswell")
                    endif()
                endif()
            endif()

            set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} ".dll.a" ".a")
            set(processor_to_use "")
            get_processor_mapping(processor_to_use ${processor_generation})
            if("${processor_to_use}" IN_LIST supported_processors)
                message(STATUS "Using OpenBLAS compiled for ${processor_generation}")
                set(BLAS_PACKAGE_NAME "OpenBLASLibs")
                set(BLAS_PACKAGE_VERSION 0.2.19.3)
                set(BLAS_PACKAGE_DIR ${PACKAGE_ROOT}/${BLAS_PACKAGE_NAME}.${BLAS_PACKAGE_VERSION}/build/native/x64/${processor_generation})
                if(NOT IS_DIRECTORY "${BLAS_PACKAGE_DIR}")
                    message(STATUS "Searching ELL_ROOT=$ENV{ELL_ROOT} for OpenBlas")
                    set(BLAS_PACKAGE_DIR $ENV{ELL_ROOT}/external/${BLAS_PACKAGE_NAME}.${BLAS_PACKAGE_VERSION}/build/native/x64/${processor_generation})
                endif()
                set(BLAS_DLLS libopenblas.dll libgcc_s_seh-1.dll libgfortran-3.dll libquadmath-0.dll)
                set(BLAS_DLL_DIR ${BLAS_PACKAGE_DIR}/bin)
                set(BLAS_INCLUDE_SEARCH_PATHS ${BLAS_PACKAGE_DIR}/include/)
                set(BLAS_LIB_SEARCH_PATHS ${BLAS_PACKAGE_DIR}/lib/)
            else()
                message(STATUS "Unknown processor, disabling OpenBLAS")
            endif()
        endif()
    endif()
endif()


find_path(BLAS_INCLUDE_DIRS cblas.h
    PATHS ${BLAS_INCLUDE_SEARCH_PATHS} ${BLAS_INCLUDE_DIRS}
    NO_DEFAULT_PATH
)

find_library(BLAS_LIBS
    NAMES ${BLAS_LIB_NAMES}
    PATHS ${BLAS_LIB_SEARCH_PATHS}
    NO_DEFAULT_PATH
)

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
