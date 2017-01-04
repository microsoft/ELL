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
        add_compile_options(/wd4141 /wd4146 /wd4180 /wd4244 /wd4258 /wd4267 /wd4291 /wd4345 /wd4351 /wd4355 /wd4456 /wd4457 /wd4458 /wd4459 /wd4503 /wd4624 /wd4722 /wd4800 /wd4100 /wd4127 /wd4512 /wd4505 /wd4610 /wd4510 /wd4702 /wd4245 /wd4706 /wd4310 /wd4701 /wd4703 /wd4389 /wd4611 /wd4805 /wd4204 /wd4577 /wd4091 /wd4592 /wd4319 /wd4324 /wd4996)
        add_compile_options(/wd4996)
    endif()

    # LLVM Include files are here
    include_directories(${LLVM_INCLUDEROOT})
    include_directories(${LLVM_INCLUDEROOT}/llvm)
    include_directories(${LLVM_INCLUDEROOT}/Support)
    include_directories(${LLVM_INCLUDEROOT}/machine) ## TODO: This doesn't exist, remove?
    include_directories(${LLVM_INCLUDEROOT}/IR)

    link_directories(${LLVM_LIBROOT_DEBUG}) ## TODO: Why only debug?

   set (LLVM_LIBS

        # Core libs
        LLVMAnalysis
        LLVMAsmParser
        LLVMBitWriter
        LLVMCore
        LLVMSupport

        # Optimizer libs
        LLVMInstCombine
        LLVMScalarOpts
        LLVMTransformUtils

        # Jitter libs
        LLVMAsmPrinter
        LLVMBitReader
        LLVMCodeGen
        LLVMDebugInfoCodeView
        LLVMExecutionEngine
        LLVMMC
        LLVMMCDisassembler
        LLVMMCJIT
        LLVMMCParser
        LLVMObject
        LLVMRuntimeDyld
        LLVMSelectionDAG
        LLVMTarget
        LLVMX86AsmPrinter
        LLVMX86CodeGen
        LLVMX86Desc
        LLVMX86Info
        LLVMX86Utils
   )

    foreach(LIBRARY ${LLVM_LIBS})
        add_library(${LIBRARY} STATIC IMPORTED)
        set_property(TARGET ${LIBRARY} PROPERTY IMPORTED_LOCATION_DEBUG ${LLVM_LIBROOT_DEBUG}/${LIBRARY}.lib)
        set_property(TARGET ${LIBRARY} PROPERTY IMPORTED_LOCATION_RELEASE ${LLVM_LIBROOT_RELEASE}/${LIBRARY}.lib)
    endforeach()

endif()
