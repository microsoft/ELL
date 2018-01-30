////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRAssemblyWriter.h (emitters)
//  Authors:  Chuck Jacobs, Piali Choudhury, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Windows-only:
#if (MSVC)
#pragma warning(disable : 4146 4996)
#endif

#include "IRAssemblyWriter.h"
#include "EmitterException.h"
#include "IRBlockRegion.h"
#include "IRDiagnosticHandler.h"
#include "IRModuleEmitter.h"

// llvm
#include <llvm/ADT/Triple.h>
#include <llvm/Analysis/TargetLibraryInfo.h>

#include <llvm/CodeGen/MachineModuleInfo.h>
#include <llvm/CodeGen/TargetPassConfig.h>

#include <llvm/IR/Attributes.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DiagnosticInfo.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>

#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

#include <llvm/Pass.h>

#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_os_ostream.h>

#include <llvm/Target/TargetMachine.h>

// stl
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ell
{
namespace emitters
{
    namespace
    {
        // Append attributes to a function
        void AddAttributes(llvm::Function& function, const llvm::AttributeSet& attributes)
        {
            auto& context = function.getContext();
            llvm::AttributeSet currentAttributes = function.getAttributes();
            function.setAttributes(currentAttributes.addAttributes(context, llvm::AttributeSet::FunctionIndex, attributes));
        }

        void SetFunctionAttributes(const std::string& cpu, const std::string& features, llvm::Module& module)
        {
            // Loop over the functions in the module, settings the cpu and features attributes
            for (auto& function : module)
            {
                auto& context = function.getContext(); // Is it really possible the functions don't all have the same context?

                // Attributes are non-mutable, so we have to replace newAttributes with the modified attributes
                llvm::AttributeSet newAttributes;
                if (!cpu.empty())
                {
                    newAttributes = newAttributes.addAttribute(context, llvm::AttributeSet::FunctionIndex, "target-cpu", cpu);
                }

                if (!features.empty())
                {
                    newAttributes = newAttributes.addAttribute(context, llvm::AttributeSet::FunctionIndex, "target-features", features);
                }

                AddAttributes(function, newAttributes);
            }
        }

        // A factory function for creating TargetOptions structs, in case we want to change the default TargetOptions values
        llvm::TargetOptions MakeTargetOptions()
        {
            llvm::TargetOptions options;
            options.MCOptions.PreserveAsmComments = true; // Note: not the default
            return options;
        }
    }

    //
    // Exported functions
    //
    bool IsBinaryOutputType(const OutputFileType& filetype)
    {
        return filetype == OutputFileType::CGFT_ObjectFile;
    }

    //
    // GenerateMachineCode
    //

    // GenerateMachineCode may modify the Module object passed in. Should we clone it first?
    void GenerateMachineCode(llvm::raw_ostream& os, IRModuleEmitter& moduleEmitter, OutputFileType fileType, const MachineCodeOutputOptions& ellOptions)
    {
        llvm::Module& module = *(moduleEmitter.GetLLVMModule());

        llvm::LLVMContext context;
        context.setDiscardValueNames(false); // Don't throw away names of non-global values

        // Verify module if requested
        if (ellOptions.verifyModule && llvm::verifyModule(module))
        {
            throw EmitterException(EmitterError::unexpected, "Module verification failed");
        }

        // Set the triple for the module, and retrieve it as a Triple object
        auto targetTripleStr = ellOptions.targetDevice.triple.empty() ? llvm::sys::getDefaultTargetTriple() : ellOptions.targetDevice.triple;
        module.setTargetTriple(llvm::Triple::normalize(targetTripleStr));

        // Get the target-specific parser.
        std::string error;
        const llvm::Target* target = llvm::TargetRegistry::lookupTarget(module.getTargetTriple(), error);
        if (!target)
        {
            throw EmitterException(EmitterError::unexpected, std::string("Couldn't create target ") + error);
        }

        llvm::TargetOptions targetOptions = MakeTargetOptions();
        targetOptions.MCOptions.AsmVerbose = ellOptions.verboseOutput;
        targetOptions.FloatABIType = ellOptions.floatABI;

        llvm::Reloc::Model relocModel = llvm::Reloc::Static;
        llvm::CodeModel::Model codeModel = llvm::CodeModel::Default;

        std::unique_ptr<llvm::TargetMachine> targetMachine(target->createTargetMachine(module.getTargetTriple(),
                                                                                       ellOptions.targetDevice.cpu,
                                                                                       ellOptions.targetDevice.features,
                                                                                       targetOptions,
                                                                                       relocModel,
                                                                                       codeModel,
                                                                                       ellOptions.optimizationLevel));

        if (!targetMachine)
        {
            throw EmitterException(EmitterError::unexpected, "Unable to allocate target machine");
        }

        // Build up all of the passes that we want to apply to the module
        llvm::legacy::PassManager passManager;

        // Get a targetLibraryInfo describing the library functions available for this triple,
        // and any special processing we might want to do. For instance, if we want to
        // disable all builtin library functions, do this: `targetLibraryInfo.disableAllFunctions();`
        llvm::TargetLibraryInfoImpl targetLibraryInfo(llvm::Triple(module.getTargetTriple()));

        // ...and add it to the pass manager, so various optimizations can be done
        passManager.add(new llvm::TargetLibraryInfoWrapperPass(targetLibraryInfo));

        // Set the data layout of the module to match the target machine
        module.setDataLayout(targetMachine->createDataLayout());

        // Override function attributes based on cpu and features
        if (ellOptions.targetDevice.cpu != "")
        {
            SetFunctionAttributes(ellOptions.targetDevice.cpu, ellOptions.targetDevice.features, module);
        }

        // Set up passes to emit code to a memory stream
        llvm::SmallVector<char, 0> buffer;
        llvm::raw_svector_ostream bufferedStream(buffer);
        if (targetMachine->addPassesToEmitFile(passManager, bufferedStream, fileType, ellOptions.verifyModule, nullptr, nullptr, nullptr, nullptr))
        {
            throw EmitterException(EmitterError::unexpected, "target does not support generation of this file type!");
        }

        // Finally, run the passes to emit code to the straem
        passManager.run(module); // run() returns a bool indicating if the module was modified (true if it was)

        if (moduleEmitter.GetDiagnosticHandler().HadError())
        {
            throw EmitterException(EmitterError::unexpected, "Error compiling module");
        }

        // Write memory buffer to our output stream
        os << buffer;
    }
}
}
