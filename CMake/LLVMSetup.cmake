# Centralized macros to define LLVM variables that we can leverage in components with dependencies on the emittersLib

# LLVM CMake variables:
# LLVM_CMAKE_DIR
# The path to the LLVM CMake directory (i.e. the directory containing LLVMConfig.cmake).
#
# +LLVM_DEFINITIONS
# A list of preprocessor defines that should be used when building against LLVM.
#
# LLVM_ENABLE_ASSERTIONS
# This is set to ON if LLVM was built with assertions, otherwise OFF.
#
# LLVM_ENABLE_EH
# This is set to ON if LLVM was built with exception handling (EH) enabled, otherwise OFF.
#
# LLVM_ENABLE_RTTI
# This is set to ON if LLVM was built with run time type information (RTTI), otherwise OFF.
#
# +LLVM_INCLUDE_DIRS
# A list of include paths to directories containing LLVM header files.
#
# LLVM_PACKAGE_VERSION
# The LLVM version. This string can be used with CMake conditionals. E.g. if (${LLVM_PACKAGE_VERSION} VERSION_LESS "3.5").
#
# + LLVM_TOOLS_BINARY_DIR
# The path to the directory containing the LLVM tools (e.g. llvm-as).

# First try to use LLVM's CMake target  (see http://llvm.org/releases/3.7.0/docs/CMake.html for documentation)
find_package(LLVM QUIET CONFIG PATHS /usr/local/opt/llvm /usr/local/opt/llvm/lib/cmake/llvm )
if(LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")

    # Find the libraries that correspond to the LLVM components that we wish to use
    llvm_map_components_to_libnames(LLVM_LIBS all) #support core irreader)
    message(STATUS "Using LLVM libraries: ${LLVM_LIBS}")
elseif(MSVC) # Didn't find LLVM via find_package. If we're on Windows, try installing via NuGet
    set (PACKAGE_SOURCE "\\\\cjacobs-z840w10\\packages")
    set (PACKAGE_ROOT ${CMAKE_SOURCE_DIR}/packages)
    set (LLVM_PACKAGE_NAME LLVMNativeLibraries)
    set (LLVM_PACKAGE_VERSION 3.9.0)
    set (LLVM_PACKAGE_DIR ${PACKAGE_ROOT}/${LLVM_PACKAGE_NAME}.${LLVM_PACKAGE_VERSION})

    # Get LLVM libraries via NuGet if we're on Windows
    find_program(NUGET nuget PATHS ${CMAKE_SOURCE_DIR}/private/binaries/nuget)
    if(NUGET)
        message(STATUS "Installing LLVM NuGet package")
        execute_process(COMMAND ${NUGET} install ${LLVM_PACKAGE_NAME} -Version ${LLVM_PACKAGE_VERSION} -source ${PACKAGE_SOURCE} -outputdirectory ${CMAKE_SOURCE_DIR}/packages -Verbosity quiet)
    endif()
        
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

if(LLVM_FOUND)
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    add_compile_options(${LLVM_COMPILE_OPTIONS})
endif()
