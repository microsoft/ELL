# Centralized place to define LLVM variables that we can leverage in components with dependencies on the emittersLib
# Sets the following variables:
#
# General information:
# LLVM_FOUND
# LLVM_PACKAGE_VERSION
#
# Settings for compiling against LLVM libraries:
# LLVM_DEFINITIONS
# LLVM_COMPILE_OPTIONS
# LLVM_INCLUDE_DIRS
# LLVM_LIBRARY_DIRS
# LLVM_LIBS
#
# Info about how LLVM was built:
# LLVM_ENABLE_ASSERTIONS
# LLVM_ENABLE_EH
# LLVM_ENABLE_RTTI
#
# Location of the executable tools:
# LLVM_TOOLS_BINARY_DIR

# Include guard so we don't try to find or download LLVM more than once
if(LLVMSetup_included)
    return()
endif()
set(LLVMSetup_included true)

# First try to use LLVM's CMake target (see http://llvm.org/releases/3.7.0/docs/CMake.html for documentation)
find_package(LLVM QUIET CONFIG PATHS /usr/local/opt/llvm /usr/local/opt/llvm/lib/cmake/llvm )
if(LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    # Find the libraries that correspond to the LLVM components that we wish to use
    llvm_map_components_to_libnames(LLVM_LIBS all) 
elseif(MSVC) # Didn't find LLVM via find_package. If we're on Windows, try installing via NuGet
    set(PACKAGE_SOURCE_URL "https://intelligentdevices.pkgs.visualstudio.com/_packaging/ELLNugetPackages/nuget/v3/index.json")
    set(PACKAGE_SOURCE_NAME "ELLNugetPackages")
    set(PACKAGE_READ_TOKEN "7xn3h6i6f5zes3nfnk2cqm3r6jt5l5n4c7nausukx5mbskywewjq")
    set(PACKAGE_ROOT ${CMAKE_SOURCE_DIR}/packages)
    set(LLVM_PACKAGE_NAME LLVMNativeLibraries)
    set(LLVM_PACKAGE_VERSION 3.9.0)
    set(LLVM_PACKAGE_DIR ${PACKAGE_ROOT}/${LLVM_PACKAGE_NAME}.${LLVM_PACKAGE_VERSION})

    # Get LLVM libraries via NuGet if we're on Windows
    find_program(NUGET nuget HINTS ${CMAKE_SOURCE_DIR}/private/binaries/nuget)
    if(NUGET)
        message(STATUS "Installing LLVM NuGet package")
        set(NUGET_CONFIG_FILE "${CMAKE_BINARY_DIR}/NuGet.config")
        # Write an empty NuGet.config file and use it so we don't mess up the user's global NuGet configuration
        file(WRITE ${NUGET_CONFIG_FILE} "<?xml version=\"1.0\" encoding=\"utf-8\"?><configuration></configuration>")
        execute_process(COMMAND ${NUGET} sources add -Name ${PACKAGE_SOURCE_NAME} -Source ${PACKAGE_SOURCE_URL} -UserName USER -Password ${PACKAGE_READ_TOKEN} -ConfigFile ${NUGET_CONFIG_FILE} -StorePasswordInClearText -Verbosity quiet)
        execute_process(COMMAND ${NUGET} install ${LLVM_PACKAGE_NAME} -Version ${LLVM_PACKAGE_VERSION} -Source ${PACKAGE_SOURCE_NAME} -Outputdirectory ${CMAKE_SOURCE_DIR}/packages -PackageSaveMode nuspec -Verbosity quiet)
    endif()
    set(LLVM_ENABLE_ASSERTIONS OFF) # But ON for debug build
    set(LLVM_ENABLE_EH OFF)
    set(LLVM_ENABLE_RTTI OFF)
    set(LLVM_LIBROOT_DEBUG ${LLVM_PACKAGE_DIR}/build/native/lib/Debug)
    set(LLVM_LIBROOT_RELEASE ${LLVM_PACKAGE_DIR}/build/native/lib/Release)
    set(LLVM_INCLUDEROOT ${LLVM_PACKAGE_DIR}/build/native/include)
    if(NOT EXISTS ${LLVM_INCLUDEROOT})
        set(LLVM_FOUND FALSE)
        return() 
    endif()

    set(LLVM_FOUND TRUE)

    # Mirror variables used by LLVM's cmake find module
    set(LLVM_INCLUDE_DIRS ${LLVM_INCLUDEROOT})
    set(LLVM_LIBRARY_DIRS ${LLVM_LIBROOT_RELEASE})
    set(LLVM_TOOLS_BINARY_DIR ${LLVM_PACKAGE_DIR}/build/native/tools)
        
    # Warnings that must be disabled. See LLVM documentation. 
    set(LLVM_DEFINITIONS /D_SCL_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_WARNINGS)
    set(LLVM_COMPILE_OPTIONS /wd4141 /wd4146 /wd4180 /wd4244 /wd4258 /wd4267 /wd4291 /wd4345 /wd4351 /wd4355 /wd4456 /wd4457 /wd4458 /wd4459 /wd4503 /wd4624 /wd4722 /wd4800 /wd4100 /wd4127 /wd4512 /wd4505 /wd4610 /wd4510 /wd4702 /wd4245 /wd4706 /wd4310 /wd4701 /wd4703 /wd4389 /wd4611 /wd4805 /wd4204 /wd4577 /wd4091 /wd4592 /wd4319 /wd4324 /wd4996)

    # Required to compile against LLVM libraries. See LLVM documentation
    if(${CMAKE_GENERATOR_TOOLSET} MATCHES "clang")
        message(STATUS "Using clang toolset")
    else()
        list(APPEND LLVM_COMPILE_OPTIONS -w14062 /Zc:inline /Zc:rvalueCast /Zc:sizedDealloc-)
    endif()

    set(LLVM_LIBS
        LLVMAArch64AsmParser
        LLVMAArch64AsmPrinter
        LLVMAArch64CodeGen
        LLVMAArch64Desc
        LLVMAArch64Disassembler
        LLVMAArch64Info
        LLVMAArch64Utils
        LLVMAMDGPUAsmParser
        LLVMAMDGPUAsmPrinter
        LLVMAMDGPUCodeGen
        LLVMAMDGPUDesc
        LLVMAMDGPUDisassembler
        LLVMAMDGPUInfo
        LLVMAMDGPUUtils
        LLVMAnalysis
        LLVMARMAsmParser
        LLVMARMAsmPrinter
        LLVMARMCodeGen
        LLVMARMDesc
        LLVMARMDisassembler
        LLVMARMInfo
        LLVMAsmParser
        LLVMAsmPrinter
        LLVMBitReader
        LLVMBitWriter
        LLVMBPFAsmPrinter
        LLVMBPFCodeGen
        LLVMBPFDesc
        LLVMBPFInfo
        LLVMCodeGen
        LLVMCore
        LLVMCoverage
        LLVMDebugInfoCodeView
        LLVMDebugInfoDWARF
        LLVMDebugInfoPDB
        LLVMExecutionEngine
        LLVMGlobalISel
        LLVMHexagonAsmParser
        LLVMHexagonCodeGen
        LLVMHexagonDesc
        LLVMHexagonDisassembler
        LLVMHexagonInfo
        LLVMInstCombine
        LLVMInstrumentation
        LLVMInterpreter
        LLVMipo
        LLVMIRReader
        LLVMLibDriver
        LLVMLineEditor
        LLVMLinker
        LLVMLTO
        LLVMMC
        LLVMMCDisassembler
        LLVMMCJIT
        LLVMMCParser
        LLVMMipsAsmParser
        LLVMMipsAsmPrinter
        LLVMMipsCodeGen
        LLVMMipsDesc
        LLVMMipsDisassembler
        LLVMMipsInfo
        LLVMMIRParser
        LLVMMSP430AsmPrinter
        LLVMMSP430CodeGen
        LLVMMSP430Desc
        LLVMMSP430Info
        LLVMNVPTXAsmPrinter
        LLVMNVPTXCodeGen
        LLVMNVPTXDesc
        LLVMNVPTXInfo
        LLVMObjCARCOpts
        LLVMObject
        LLVMObjectYAML
        LLVMOption
        LLVMOrcJIT
        LLVMPasses
        LLVMPowerPCAsmParser
        LLVMPowerPCAsmPrinter
        LLVMPowerPCCodeGen
        LLVMPowerPCDesc
        LLVMPowerPCDisassembler
        LLVMPowerPCInfo
        LLVMProfileData
        LLVMRuntimeDyld
        LLVMScalarOpts
        LLVMSelectionDAG
        LLVMSparcAsmParser
        LLVMSparcAsmPrinter
        LLVMSparcCodeGen
        LLVMSparcDesc
        LLVMSparcDisassembler
        LLVMSparcInfo
        LLVMSupport
        LLVMSymbolize
        LLVMSystemZAsmParser
        LLVMSystemZAsmPrinter
        LLVMSystemZCodeGen
        LLVMSystemZDesc
        LLVMSystemZDisassembler
        LLVMSystemZInfo
        LLVMTableGen
        LLVMTarget
        LLVMTransformUtils
        LLVMVectorize
        LLVMX86AsmParser
        LLVMX86AsmPrinter
        LLVMX86CodeGen
        LLVMX86Desc
        LLVMX86Disassembler
        LLVMX86Info
        LLVMX86Utils
        LLVMXCoreAsmPrinter
        LLVMXCoreCodeGen
        LLVMXCoreDesc
        LLVMXCoreDisassembler
        LLVMXCoreInfo
    )

    foreach(LIBRARY ${LLVM_LIBS})
        add_library(${LIBRARY} STATIC IMPORTED)
        set_property(TARGET ${LIBRARY} PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/${LIBRARY}.lib)
        set_property(TARGET ${LIBRARY} PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/${LIBRARY}.lib)
    endforeach()
endif()
