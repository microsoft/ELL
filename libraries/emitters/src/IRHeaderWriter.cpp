////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRHeaderWriter.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRHeaderWriter.h"
#include "EmitterException.h"
#include "IRMetadata.h"
#include "IRModuleEmitter.h"

// llvm
#include <llvm/IR/Attributes.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_os_ostream.h>

// utilities
#include "Debug.h"
#include "StringUtil.h"

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
                os << typeName;
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

        void WriteStructDefinition(std::ostream& os, llvm::StructType* t, const std::vector<std::string>& fieldNames)
        {
            if (t->hasName()) // && !t->isLiteral() ?
            {
                std::string typeName = t->getName();
                DeclareIfDefDefine guard(os, "ELL_" + typeName);
                os << "typedef struct " << typeName << "\n";
                os << "{\n";
                size_t index = 0;
                for (auto& fieldType : t->elements())
                {
                    os << "    ";
                    std::string fieldName = (index >= fieldNames.size()) ? std::string("param") + std::to_string(index) : fieldNames[index];
                    WriteLLVMVariableDeclaration(os, fieldType, fieldName);
                    os << ";\n";
                    ++index;
                }
                os << "} " << typeName << ";\n\n";
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

                std::string argName = arg.getName();
                if (!argName.empty())
                {
                    os << " " << argName;
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
            for (const auto& def : defines)
            {
                DeclareIfDefDefine define(os, def.first, def.second);
            }

            // First write out type definitions
            os << "//\n// Types\n//\n\n";

            // Look for the module-level "declare in header" tag
            if (moduleEmitter.HasMetadata(c_declareTypeInHeaderTagName))
            {
                auto typeNames = GetSingletonModuleTagValues(moduleEmitter, c_declareTypeInHeaderTagName);

                auto structTypes = pModule->getIdentifiedStructTypes();
                for (const auto& t : structTypes)
                {
                    if (t->hasName() && (typeNames.cend() != typeNames.find(t->getName())))
                    {
                        // Get struct field names
                        auto tagName = GetStructFieldsTagName(t);
                        std::vector<std::string> fieldNames;
                        if (moduleEmitter.HasMetadata(tagName))
                        {
                            auto fieldNameMetadata = moduleEmitter.GetMetadata(tagName);
                            if (!fieldNameMetadata.empty())
                            {
                                fieldNames = fieldNameMetadata[0];
                            }
                        }
                        WriteStructDefinition(os, t, fieldNames);
                    }
                }
            }

            os << "//\n// Functions\n//\n\n";
            // Now write out function signatures
            auto tagValues = GetFunctionsWithTag(moduleEmitter, c_declareFunctionInHeaderTagName);
            for (auto& tv : tagValues)
            {
                WriteFunctionDeclaration(os, moduleEmitter, *(tv.function));
                os << "\n\n";
            }
        }
    }

    void WriteModuleCppWrapper(std::ostream& os, IRModuleEmitter& moduleEmitter)
    {
        auto callbacks = GetFunctionsWithTag(moduleEmitter, c_callbackFunctionTagName);
        if (!callbacks.empty())
        {
            // If callbacks are part of the module, write the predict wrapper

            // (Note: newlines are part of the syntax for #include)
            // clang-format off
            std::string predictWrapperCode(
                #include "CppPredictWrapper.in"
            );
            // clang-format on

            ReplaceDelimiter(predictWrapperCode, "MODULE", moduleEmitter.GetLLVMModule()->getName());

            auto predictFunctions = GetFunctionsWithTag(moduleEmitter, c_predictFunctionTagName);
            ReplaceDelimiter(predictWrapperCode, "FUNCTION", predictFunctions[0].function->getName());

            ModuleCallbackDefinitions moduleCallbacks(callbacks);
            if (moduleCallbacks.sources.size() > 0) 
            {
                ReplaceDelimiter(predictWrapperCode, "SOURCE_TYPE", moduleCallbacks.sources[0].inputType);
                ReplaceDelimiter(predictWrapperCode, "SOURCE_CALLBACK", moduleCallbacks.sources[0].functionName);
            }
            else
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                    "SourceNode callback is missing");
            }

            if (moduleCallbacks.sinks.size() > 0)
            {
                ReplaceDelimiter(predictWrapperCode, "SINK_TYPE", moduleCallbacks.sinks[0].inputType);
                ReplaceDelimiter(predictWrapperCode, "SINK_CALLBACK", moduleCallbacks.sinks[0].functionName);
            }
            else
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                    "SinkNode callback is missing");
            }
            os << predictWrapperCode << "\n";
        }
    }

    void ReplaceDelimiter(std::string& text, const std::string& delimiter, const std::string& replacement)
    {
        utilities::ReplaceAll(text, "@@" + delimiter + "@@", replacement);
    }

    //
    // DeclareExternC
    //
    DeclareExternC::DeclareExternC(std::ostream& os)
        : _os(&os)
    {
        DeclareIfDefGuard guard(os, "__cplusplus", DeclareIfDefGuard::Type::Positive);
        *_os << "extern \"C\"\n{\n";
    }

    DeclareExternC::~DeclareExternC()
    {
        DeclareIfDefGuard guard(*_os, "__cplusplus", DeclareIfDefGuard::Type::Positive);
        *_os << "} // extern \"C\"\n";
    }

    DeclareIfDefGuard::DeclareIfDefGuard(std::ostream& os, std::string symbol, Type type)
        : _os(os), _symbol(std::move(symbol)), _type(type)
    {
        _os << "#if " << (_type == Type::Negative ? "!" : "") << "defined(" << _symbol << ")\n";
    }

    DeclareIfDefGuard::~DeclareIfDefGuard()
    {
        _os << "#endif // " << (_type == Type::Negative ? "!" : "") << "defined(" << _symbol << ")\n\n";
    }

    DeclareIfDefDefine::DeclareIfDefDefine(std::ostream& os, std::string symbol, const std::string& value /* = "" */)
        : DeclareIfDefGuard(os, std::move(symbol), Type::Negative)
    {
        _os << "#define " << _symbol << (value.empty() ? "" : " ") << value << "\n\n";
    }

    //
    // ModuleCallbackDefinitions
    //
    ModuleCallbackDefinitions::ModuleCallbackDefinitions(const std::vector<FunctionTagValues>& callbacks)
    {
        for (const auto& c : callbacks)
        {
            if (!c.values.empty())
            {
                auto nodeType = c.values[0];
                if (nodeType == "SourceNode")
                {
                    sources.push_back(CallbackSignature(*c.function));
                }
                else if (nodeType == "SinkNode")
                {
                    sinks.push_back(CallbackSignature(*c.function));
                }
                else if (nodeType == "ClockNode")
                {
                    lagNotifications.push_back(CallbackSignature(*c.function));
                }
            }
        }

        // Eventually we'd support multiple sources and sinks.
        // For now, assert that we're only going to look at the first ones of each.
        DEBUG_THROW(sources.size() != 1, EmitterException(EmitterError::badFunctionDefinition, "Only one source callback function will be generated"));
        DEBUG_THROW(sinks.size() != 1, EmitterException(EmitterError::badFunctionDefinition, "Only one sink callback function will be generated"));
        DEBUG_THROW(lagNotifications.size() != 1, EmitterException(EmitterError::badFunctionDefinition, "Only one lag callback function will be generated"));
    }

    ModuleCallbackDefinitions::CallbackSignature::CallbackSignature(llvm::Function& function) :
        functionName(function.getName())
    {
        // Parameters
        // Callbacks have one input parameter and a return (which can be void)
        {
            std::ostringstream os;
            auto& argument = *(function.args().begin());
            auto type = argument.getType();
            if (type->isPointerTy())
            {
                inputIsScalar = false;
                WriteLLVMType(os, argument.getType()->getPointerElementType());
            }
            else
            {
                inputIsScalar = true;
                WriteLLVMType(os, argument.getType());
            }
            inputType = os.str();
        }

        // Return type
        {
            std::ostringstream os;
            WriteLLVMType(os, function.getReturnType());
            returnType = os.str();
        }

        // Wrapper class name
        className = inputType + "CallbackBase";
        className[0] = ::toupper(className[0]); // pascal case
    }
}
}
