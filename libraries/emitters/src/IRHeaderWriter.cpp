////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRHeaderWriter.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRHeaderWriter.h"

// emitters
#include "EmitterException.h"
#include "IRMetadata.h"

// llvm
#include "llvm/IR/Attributes.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_os_ostream.h"

// stl
#include <sstream>
#include <string>
#include <vector>

namespace ell
{
namespace emitters
{
    namespace
    {
        void WriteStructType(std::ostream& os, llvm::StructType* t)
        {
            if (t->hasName()) // && !t->isLiteral() ?
            {
                std::string typeName = t->getName();
                os << "struct " << typeName;
            }
        }

        void WriteArrayType(std::ostream& os, llvm::ArrayType* t)
        {
            auto size = t->getNumElements();
            auto elemType = t->getTypeAtIndex(0u);
            WriteLLVMType(os, elemType);
            os << "[" << size << "]";
        }

        void WritePointerType(std::ostream& os, llvm::PointerType* t)
        {
            auto elemType = t->getTypeAtIndex(0u);
            WriteLLVMType(os, elemType);
            os << "*";
        }

        void WriteIntegerType(std::ostream& os, llvm::IntegerType* t)
        {
            auto size = t->getBitWidth();
            os << "int" << size << "_t";
        }

        void WriteFunctionType(std::ostream& os, llvm::FunctionType* t)
        {
            auto returnType = t->getReturnType();
            WriteLLVMType(os, returnType);
            os << " (";
            bool first = true;
            for (auto pt : t->params())
            {
                if (!first)
                {
                    os << ", ";
                }
                first = false;
                WriteLLVMType(os, pt);
            }
            os << ");";
        }

        void WriteLLVMVariableDeclaration(std::ostream& os, llvm::Type* t, std::string name)
        {
            if (t->isArrayTy())
            {
                auto arrType = llvm::cast<llvm::ArrayType>(t);
                auto size = arrType->getNumElements();
                auto elemType = arrType->getTypeAtIndex(0u);
                WriteLLVMType(os, elemType);
                os << " " << name << "[" << size << "]";
            }
            else
            {
                WriteLLVMType(os, t);
                os << " " << name;
            }
        }

        void WriteStructDefinition(std::ostream& os, llvm::StructType* t)
        {
            if (t->hasName()) // && !t->isLiteral() ?
            {
                std::string typeName = t->getName();
                os << "struct " << typeName << "\n";
                os << "{\n";
                auto index = 0;
                for (auto& fieldType : t->elements())
                {
                    os << "    ";
                    std::string fieldName = std::string("param") + std::to_string(index);
                    WriteLLVMVariableDeclaration(os, fieldType, fieldName);
                    os << ";\n";
                    ++index;
                }
                os << "};";
            }
        }
    }

    void WriteLLVMType(std::ostream& os, llvm::Type* t)
    {
        if (t->isStructTy())
        {
            WriteStructType(os, llvm::cast<llvm::StructType>(t));
        }
        else if (t->isArrayTy())
        {
            WriteArrayType(os, llvm::cast<llvm::ArrayType>(t));
        }
        else if (t->isPointerTy())
        {
            WritePointerType(os, llvm::cast<llvm::PointerType>(t));
        }
        else if (t->isIntegerTy())
        {
            WriteIntegerType(os, llvm::cast<llvm::IntegerType>(t));
        }
        else if (t->isFloatTy())
        {
            os << "float";
        }
        else if (t->isDoubleTy())
        {
            os << "double";
        }
        else if (t->isVoidTy())
        {
            os << "void";
        }
        else if (t->isFunctionTy())
        {
            WriteFunctionType(os, llvm::cast<llvm::FunctionType>(t));
        }
        else
        {
            os << "[[UNKNOWN]]";
            // look up in table
            // ???
        }
    }

    void WriteFunctionDeclaration(std::ostream& os, IRModuleEmitter& moduleEmitter, llvm::Function& function)
    {
        auto hasName = function.hasName();
        if (hasName)
        {
            std::string name = function.getName();

            // Check if we've added comments for this function
            if (moduleEmitter.HasFunctionComments(name))
            {
                auto comments = moduleEmitter.GetFunctionComments(name);
                for (auto comment : comments)
                {
                    os << "// " << comment << "\n";
                }
            }

            // Now write the function signature
            auto returnType = function.getReturnType();
            WriteLLVMType(os, returnType);
            os << " " << name << "(";
            bool first = true;
            for (const auto& arg : function.args())
            {
                if (!first)
                {
                    os << ", ";
                }
                first = false;
                WriteLLVMType(os, arg.getType());

                bool hasParamName = false;
                if (hasParamName)
                {
                    auto paramName = "param";
                    os << " " << paramName;
                }
            }

            os << ");";
        }
    }

    void WriteModuleHeader(std::ostream& os, IRModuleEmitter& moduleEmitter)
    {
        auto pModule = moduleEmitter.GetLLVMModule();

        // Header comment
        std::string moduleName = pModule->getName();
        os << "//\n// ELL header for module " << moduleName << "\n//\n\n";

        os << "#include <stdint.h>\n\n";

        {
            DeclareExternC externC(os);

            // preprocessor definitions
            auto defines = moduleEmitter.GetPreprocessorDefinitions();
            if (defines.size() > 0)
            {
                for (const auto& def : defines)
                {
                    os << "#define " << def.first << " " << def.second << "\n";
                }
                os << "\n";
            }

            // First write out type definitions
            os << "//\n// Types\n//\n\n";

            // Look for the module-level "declare in header" tag
            if (moduleEmitter.HasMetadata("", c_declareInHeaderTagName))
            {
                auto typeNames = GetModuleTagValues(moduleEmitter, c_declareInHeaderTagName);
                auto structTypes = pModule->getIdentifiedStructTypes();
                for (const auto& t : structTypes)
                {
                    if (t->hasName() && (typeNames.cend() != typeNames.find(t->getName())))
                    {
                        WriteStructDefinition(os, t);
                        os << "\n\n";
                    }
                }
            }

            os << "\n";
            os << "//\n// Functions\n//\n\n";
            // Now write out function signatures
            auto tagValues = GetFunctionsWithTag(moduleEmitter, c_declareInHeaderTagName);
            for (auto& tv : tagValues)
            {
                WriteFunctionDeclaration(os, moduleEmitter, *(tv.function));
                os << "\n\n";
            }
        }
    }

    //
    // DeclareExternC
    //
    DeclareExternC::DeclareExternC(std::ostream& os)
        : _os(&os)
    {
        os << "#ifdef __cplusplus\n";
        os << "extern \"C\"\n{\n";
        os << "#endif\n";
    }

    DeclareExternC::~DeclareExternC()
    {
        *_os << "#ifdef __cplusplus\n";
        *_os << "} // extern \"C\"\n";
        *_os << "#endif\n\n";
    }
}
}
