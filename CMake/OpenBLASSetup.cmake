# Centralized place to define OpenBLAS variables 
# Sets the following variables:
#
# General information:
# BLAS_FOUND
#
# Settings for compiling against OpenBLAS libraries:
# BLAS_INCLUDE_DIRS
# BLAS_LIBS
# BLAS_DLL_DIRECTORY

# Using FindBLAS module:
#find_package(BLAS)
#if(BLAS_FOUND)
#  message(WARNING "BLAS Found!")
#  message(WARNING "Blas libraries: ${BLAS_LIBRARIES}")
#  message(WARNING "Blas linker flags: ${BLAS_LINKER_FLAGS}")
#  message(WARNING "Blas vendor: ${BLA_VENDOR}")
#else()
#  message(WARNING "BLAS NOT Found!")
#endif()

# Include guard so we don't try to find or download BLAS more than once
if(BLASSetup_included)
    return()
endif()
set(BLASSetup_included true)

set (BLAS_LIB_NAMES cblas openblas libopenblas.dll.a)

set (BLAS_INCLUDE_SEARCH_PATHS
    /System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/Current/Headers/
    /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Accelerate.framework/Frameworks/vecLib.framework/Headers/
    /usr/include
    /usr/local/include
)

if(APPLE)
  # To install commandline tools (which seems to also install headers in /System/Library/Frameworks/...), do this:
  # xcode-select --install
endif()

if(WIN32)
    set (PACKAGE_SOURCE_LOCAL "\\\\cjacobs-z840w10\\packages")
    set (PACKAGE_SOURCE_URL "https://intelligentdevices.pkgs.visualstudio.com/_packaging/ELLNugetPackages/nuget/v3/index.json")
    set (PACKAGE_SOURCE_NAME "ELLNugetPackages")
    set (PACKAGE_READ_TOKEN "7xn3h6i6f5zes3nfnk2cqm3r6jt5l5n4c7nausukx5mbskywewjq")
    set (PACKAGE_ROOT ${CMAKE_SOURCE_DIR}/packages)

    set (BLAS_PACKAGE_NAME OpenBLASWin64)
    set (BLAS_PACKAGE_VERSION 0.2.19.1)
    set (PROCESSOR_GENERATION haswell)
    set (BLAS_PACKAGE_DIR ${PACKAGE_ROOT}/${BLAS_PACKAGE_NAME}.${BLAS_PACKAGE_VERSION}/build/native/${PROCESSOR_GENERATION})
    set (BLAS_DLL_DIR ${BLAS_PACKAGE_DIR}/bin)

    list (APPEND BLAS_INCLUDE_SEARCH_PATHS
        ${BLAS_PACKAGE_DIR}/include/
     )

    # Install via nuget
    find_program(NUGET nuget HINTS ${CMAKE_SOURCE_DIR}/private/binaries/nuget)
    if(NUGET)
        message(STATUS "Installing OpenBLAS NuGet package")
        set(NUGET_CONFIG_FILE "${CMAKE_BINARY_DIR}/NuGet.config")
        # Write an empty NuGet.config file and use it so we don't mess up the user's global NuGet configuration
        file(WRITE ${NUGET_CONFIG_FILE} "<?xml version=\"1.0\" encoding=\"utf-8\"?><configuration></configuration>")
        execute_process(COMMAND ${NUGET} sources add -Name ${PACKAGE_SOURCE_NAME} -Source ${PACKAGE_SOURCE_URL} -UserName USER -Password ${PACKAGE_READ_TOKEN} -ConfigFile ${NUGET_CONFIG_FILE} -StorePasswordInClearText -Verbosity quiet)
        execute_process(COMMAND ${NUGET} install ${BLAS_PACKAGE_NAME} -Version ${BLAS_PACKAGE_VERSION} -Source ${PACKAGE_SOURCE_NAME} -Outputdirectory ${CMAKE_SOURCE_DIR}/packages -PackageSaveMode nuspec -Verbosity quiet)
    endif()
endif()

## Note: libopenblas install on ubuntu in /usr/lib and /usr/include
find_path(BLAS_INCLUDE_DIRS cblas.h
    PATHS ${BLAS_INCLUDE_SEARCH_PATHS}
        NO_DEFAULT_PATH
    )
message(STATUS "Using BLAS include path: ${BLAS_INCLUDE_DIRS}")

set (BLAS_LIB_SEARCH_PATHS
    /usr/lib64/atlas-sse3 /usr/lib64/atlas /usr/lib64 /usr/local/lib64/atlas /usr/local/lib64 /usr/lib/atlas-sse3 /usr/lib/atlas-sse2 /usr/lib/atlas-sse /usr/lib/atlas-3dnow /usr/lib/atlas /usr/lib /usr/local/lib/atlas /usr/local/lib
)

if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} ".dll.a" ".a")
    list (APPEND BLAS_LIB_SEARCH_PATHS 
        ${BLAS_PACKAGE_DIR}/lib/
    )    
endif()

find_library(BLAS_LIBS
  NAMES ${BLAS_LIB_NAMES}
  PATHS ${BLAS_LIB_SEARCH_PATHS}
  NO_DEFAULT_PATH
  )
message(STATUS "Using BLAS library: ${BLAS_LIBS}")


if (BLAS_LIBS AND BLAS_INCLUDE_DIRS)
    set(BLAS_FOUND "YES")
else ()
    set(BLAS_FOUND "NO")
endif ()
