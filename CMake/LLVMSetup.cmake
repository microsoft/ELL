# Centralized macros to define LLVM variables that we can leverage in components with dependencies on the emittersLib
if(MSVC)
    set (PACKAGE_SOURCE "\\\\cjacobs-z840w10\\packages")
    set (PACKAGE_ROOT ${CMAKE_SOURCE_DIR}/packages)
    set (LLVM_PACKAGE_NAME LLVMNativeLibraries)
    set (LLVM_PACKAGE_VERSION 3.9.0)
    set (LLVM_PACKAGE_DIR ${PACKAGE_ROOT}/${LLVM_PACKAGE_NAME}.${LLVM_PACKAGE_VERSION})

    # Get LLVM libraries via NuGet if we're on Windows
    find_program(NUGET nuget)
    if(NUGET)
        message(STATUS "Installing LLVM NuGet package")
        execute_process(COMMAND ${NUGET} install ${LLVM_PACKAGE_NAME} -source ${PACKAGE_SOURCE} -outputdirectory ${CMAKE_SOURCE_DIR}/packages)
    endif()
        
    set(LLVM_INCLUDEROOT ${LLVM_PACKAGE_DIR}/build/native/include)
    set(LLVM_LIBROOT_DEBUG ${LLVM_PACKAGE_DIR}/build/native/lib/Debug)
    set(LLVM_LIBROOT_RELEASE ${LLVM_PACKAGE_DIR}/build/native/lib/Release)

    # Mirror variables used by LLVM's cmake find module
    set(LLVM_LIBRARY_DIRS ${LLVM_LIBROOT_RELEASE})
    set(LLVM_INCLUDE_DIRS ${LLVM_INCLUDEROOT})

    if(EXISTS ${LLVM_INCLUDEROOT})
        set(LLVM_FOUND TRUE)
    else()
        set(LLVM_FOUND FALSE)
    endif()
endif()

if(LLVM_FOUND)

    if(MSVC)
        # Required to compile against LLVM libraries. See LLVM documentation
        if(${CMAKE_GENERATOR_TOOLSET} MATCHES "clang")
            message(STATUS "Using clang toolset")
        else()
            add_compile_options(-w14062 /Zc:inline /Zc:rvalueCast /Zc:sizedDealloc-)
        endif()

        # Warnings that must be disabled. See LLVM documentation. 
        add_compile_options(/D_SCL_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_WARNINGS)
        add_compile_options( /wd4141 /wd4146 /wd4180 /wd4244 /wd4258 /wd4267 /wd4291 /wd4345 /wd4351 /wd4355 /wd4456 /wd4457 /wd4458 /wd4459 /wd4503 /wd4624 /wd4722 /wd4800 /wd4100 /wd4127 /wd4512 /wd4505 /wd4610 /wd4510 /wd4702 /wd4245 /wd4706 /wd4310 /wd4701 /wd4703 /wd4389 /wd4611 /wd4805 /wd4204 /wd4577 /wd4091 /wd4592 /wd4319 /wd4324 /wd4996)
        add_compile_options( /wd4996)
    endif()

    # LLVM Include files are here
    include_directories(${LLVM_INCLUDEROOT})
    include_directories(${LLVM_INCLUDEROOT}/llvm)
    include_directories(${LLVM_INCLUDEROOT}/Support)
    include_directories(${LLVM_INCLUDEROOT}/machine)
    include_directories(${LLVM_INCLUDEROOT}/IR)

    #=====================
    # LLVM static Libraries
    #=====================

    link_directories(${LLVM_LIBROOT_DEBUG})

    add_library(LLVMAnalysis STATIC IMPORTED)
    set_property(TARGET LLVMAnalysis PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMAnalysis.lib)
    set_property(TARGET LLVMAnalysis PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMAnalysis.lib)

    add_library(LLVMAsmParser STATIC IMPORTED)
    set_property(TARGET LLVMAsmParser PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMAsmParser.lib)
    set_property(TARGET LLVMAsmParser PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMAsmParser.lib)

    add_library(LLVMCore STATIC IMPORTED)
    set_property(TARGET LLVMCore PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMCore.lib)
    set_property(TARGET LLVMCore PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMCore.lib)

    add_library(LLVMSupport STATIC IMPORTED)
    set_property(TARGET LLVMSupport PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMSupport.lib)
    set_property(TARGET LLVMSupport PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMSupport.lib)

    add_library(LLVMBitWriter STATIC IMPORTED)
    set_property(TARGET LLVMBitWriter PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMBitWriter.lib)
    set_property(TARGET LLVMBitWriter PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMBitWriter.lib)

    #=====================
    # Optimizer Libs
    #=====================
    add_library(LLVMInstCombine STATIC IMPORTED)
    set_property(TARGET LLVMInstCombine PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMInstCombine.lib)
    set_property(TARGET LLVMInstCombine PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMInstCombine.lib)

    add_library(LLVMTransformUtils STATIC IMPORTED)
    set_property(TARGET LLVMTransformUtils PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMTransformUtils.lib)
    set_property(TARGET LLVMTransformUtils PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMTransformUtils.lib)

    add_library(LLVMScalarOpts STATIC IMPORTED)
    set_property(TARGET LLVMScalarOpts PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMScalarOpts.lib)
    set_property(TARGET LLVMScalarOpts PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMScalarOpts.lib)

    #=====================
    # Jitter Libs
    #=====================

    add_library(LLVMExecutionEngine STATIC IMPORTED)
    set_property(TARGET LLVMExecutionEngine PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMExecutionEngine.lib)
    set_property(TARGET LLVMExecutionEngine PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMExecutionEngine.lib)

    add_library(LLVMRuntimeDyld STATIC IMPORTED)
    set_property(TARGET LLVMRuntimeDyld PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMRuntimeDyld.lib)
    set_property(TARGET LLVMRuntimeDyld PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMRuntimeDyld.lib)

    add_library(LLVMObject STATIC IMPORTED)
    set_property(TARGET LLVMObject PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMObject.lib)
    set_property(TARGET LLVMObject PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMObject.lib)

    add_library(LLVMMC STATIC IMPORTED)
    set_property(TARGET LLVMMC PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMMC.lib)
    set_property(TARGET LLVMMC PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMMC.lib)

    add_library(LLVMTarget STATIC IMPORTED)
    set_property(TARGET LLVMTarget PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMTarget.lib)
    set_property(TARGET LLVMTarget PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMTarget.lib)

    add_library(LLVMMCParser STATIC IMPORTED)
    set_property(TARGET LLVMMCParser PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMMCParser.lib)
    set_property(TARGET LLVMMCParser PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMMCParser.lib)

    add_library(LLVMBitReader STATIC IMPORTED)
    set_property(TARGET LLVMBitReader PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMBitReader.lib)
    set_property(TARGET LLVMBitReader PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMBitReader.lib)

    add_library(LLVMCodeGen STATIC IMPORTED)
    set_property(TARGET LLVMCodeGen PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMCodeGen.lib)
    set_property(TARGET LLVMCodeGen PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMCodeGen.lib)

    add_library(LLVMSelectionDAG STATIC IMPORTED)
    set_property(TARGET LLVMSelectionDAG PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMSelectionDAG.lib)
    set_property(TARGET LLVMSelectionDAG PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMSelectionDAG.lib)

    add_library(LLVMAsmPrinter STATIC IMPORTED)
    set_property(TARGET LLVMAsmPrinter PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMAsmPrinter.lib)
    set_property(TARGET LLVMAsmPrinter PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMAsmPrinter.lib)

    add_library(LLVMX86CodeGen STATIC IMPORTED)
    set_property(TARGET LLVMX86CodeGen PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMX86CodeGen.lib)
    set_property(TARGET LLVMX86CodeGen PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMX86CodeGen.lib)

    add_library(LLVMX86Info STATIC IMPORTED)
    set_property(TARGET LLVMX86Info PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMX86Info.lib)
    set_property(TARGET LLVMX86Info PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMX86Info.lib)

    add_library(LLVMX86Desc STATIC IMPORTED)
    set_property(TARGET LLVMX86Desc PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMX86Desc.lib)
    set_property(TARGET LLVMX86Desc PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMX86Desc.lib)

    add_library(LLVMX86Utils STATIC IMPORTED)
    set_property(TARGET LLVMX86Utils PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMX86Utils.lib)
    set_property(TARGET LLVMX86Utils PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMX86Utils.lib)

    add_library(LLVMX86AsmPrinter STATIC IMPORTED)
    set_property(TARGET LLVMX86AsmPrinter PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMX86AsmPrinter.lib)
    set_property(TARGET LLVMX86AsmPrinter PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMX86AsmPrinter.lib)

    add_library(LLVMMCDisassembler STATIC IMPORTED)
    set_property(TARGET LLVMMCDisassembler PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMMCDisassembler.lib)
    set_property(TARGET LLVMMCDisassembler PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMMCDisassembler.lib)

    add_library(LLVMDebugInfoCodeView STATIC IMPORTED)
    set_property(TARGET LLVMDebugInfoCodeView PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMDebugInfoCodeView.lib)
    set_property(TARGET LLVMDebugInfoCodeView PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMDebugInfoCodeView.lib)

    add_library(LLVMMCJIT STATIC IMPORTED)
    set_property(TARGET LLVMMCJIT PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/LLVMMCJIT.lib)
    set_property(TARGET LLVMMCJIT PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/LLVMMCJIT.lib)

    set(LLVM_LIBS LLVMCore LLVMAnalysis LLVMAsmParser LLVMSupport LLVMBitWriter LLVMInstCombine LLVMTransformUtils LLVMScalarOpts LLVMExecutionEngine LLVMRuntimeDyld LLVMObject LLVMMC LLVMTarget LLVMMCParser LLVMBitReader LLVMCodeGen LLVMSelectionDAG LLVMAsmPrinter LLVMX86CodeGen LLVMX86Info LLVMX86Desc LLVMX86Utils LLVMX86AsmPrinter LLVMMCDisassembler LLVMDebugInfoCodeView LLVMMCJIT)
endif()
