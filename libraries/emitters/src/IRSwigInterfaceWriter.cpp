////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRSwigInterfaceWriter.cpp (emitters)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRSwigInterfaceWriter.h"

// emitters
#include "EmitterException.h"
#include "IRHeaderWriter.h"
#include "IRMetadata.h"

// stl
#include <sstream>
#include <string>

namespace ell
{
namespace emitters
{
    namespace
    {
        // Writes a scoped #ifdef SWIG declaration
        class DeclareIfndefSwig
        {
        public:
            DeclareIfndefSwig(std::ostream& os)
                : _os(&os)
            {
                os << "#ifndef SWIG\n";
            }

            ~DeclareIfndefSwig()
            {
                *_os << "#endif // SWIG\n\n";
            }

        private:
            std::ostream* _os;
        };

        // Writes SWIG interfaces for predict
        class PredictInterfaceWriter
        {
        public:
            PredictInterfaceWriter(IRModuleEmitter& moduleEmitter, llvm::Function& predictFunction)
                : _moduleEmitter(&moduleEmitter), _function(&predictFunction)
            {
                InitPredictFunctionInfo();
            }

            void WriteHeaderCode(std::ostream& os)
            {
                // Write header for SWIG to generate a wrapper
                std::ostringstream osHeader;
                osHeader << "void " << _functionName << "(const std::vector<" << _inputType << ">& input, std::vector<" << _outputType << ">& output)";
                os << osHeader.str() << ";\n\n";

                {
                    DeclareIfndefSwig ifndefSwig(os);

                    // Write implementation
                    os << osHeader.str() << "\n{\n";
                    os << "    " << _functionName << "(const_cast<" << _inputType << "*>(&input[0]), &output[0]);\n";
                    os << "}\n";
                }
            }

        private:
            void InitPredictFunctionInfo()
            {
                _functionName = _function->getName();

                // Two pointer arguments
                auto it = _function->args().begin();
                {
                    std::ostringstream os;
                    WriteLLVMType(os, (*it).getType()->getPointerElementType());
                    _inputType = os.str();
                }

                {
                    std::ostringstream os;
                    WriteLLVMType(os, (*(++it)).getType()->getPointerElementType());
                    _outputType = os.str();
                }
            }

            std::string _functionName;
            std::string _inputType;
            std::string _outputType;

            IRModuleEmitter* _moduleEmitter;
            llvm::Function* _function;
        };

        // Base class for writing callback SWIG interfaces
        class CallbackWriterBase
        {
        public:
            CallbackWriterBase(IRModuleEmitter& moduleEmitter, llvm::Function& callbackFunction)
                : _moduleEmitter(&moduleEmitter), _function(&callbackFunction)
            {
                _functionName = _function->getName();

                // Callbacks have a pointer parameter and a return (which can be void)
                {
                    std::ostringstream os;
                    auto& argument = *(callbackFunction.args().begin());
                    WriteLLVMType(os, argument.getType()->getPointerElementType());
                    _valueType = os.str();
                }

                {
                    std::ostringstream os;
                    WriteLLVMType(os, callbackFunction.getReturnType());
                    _returnType = os.str();
                }

                _className = _valueType + "CallbackBase";
                _className[0] = toupper(_className[0]); // pascal case
            }

            virtual ~CallbackWriterBase() = default;

        protected:
            //
            // Callback writer methods
            //
            virtual void WriteCallbackFunction(std::ostream& os) const = 0;

            void WriteCallbackSwigCode(std::ostream& os) const
            {
                os << "WRAP_CALLABLES_AS_CALLBACKS(" << _forwarderClassName << ", "
                   << _className << ", " << _valueType << ")\n\n";
            }

            //
            // Forwarder writer methods
            //
            virtual void WriteForwarderClassHeader(std::ostream& os) const = 0;
            virtual void WriteForwarderClassBody(std::ostream& os) const = 0;
            virtual void WriteForwarderClassInstance(std::ostream& os) const = 0;

            std::string _valueType;
            std::string _returnType;
            std::string _functionName;

            std::string _className;
            std::string _forwarderClassName;

            llvm::Function* _function;
            IRModuleEmitter* _moduleEmitter;
        };

        // Writes SWIG interfaces for SourceNode
        class SourceNodeInterfaceWriter : public CallbackWriterBase
        {
        public:
            SourceNodeInterfaceWriter(IRModuleEmitter& moduleEmitter, llvm::Function& callbackFunction)
                : CallbackWriterBase(moduleEmitter, callbackFunction)
            {
                InitStepFunctionInfo();
                InitWaitTimeFunctionInfo();

                std::string moduleName = moduleEmitter.GetLLVMModule()->getName();
                _forwarderClassName = moduleName + "Predictor";
            }

            virtual ~SourceNodeInterfaceWriter() = default;

        public:
            void WriteSwigCode(std::ostream& os)
            {
                WriteCallbackSwigCode(os);
            }

            void WriteHeaderCode(std::ostream& os)
            {
                // Wrapper class declaration
                WriteForwarderClassHeader(os);

                {
                    DeclareIfndefSwig ifndefSwig(os);
                    WriteForwarderClassInstance(os);

                    // Callback C definition
                    {
                        DeclareExternC externC(os);
                        WriteCallbackFunction(os);
                    }

                    // Wrapper class definition
                    WriteForwarderClassBody(os);
                }
            }

        protected:
            //
            // Callback writer methods
            //
            virtual void WriteCallbackFunction(std::ostream& os) const override
            {
                os << _returnType << " " << _functionName << " (" << _valueType << "* buffer)\n{\n";
                os << "    return static_cast<" << _returnType << ">(_predictor.Invoke(buffer));\n";
                os << "}\n";
            }

            //
            // Forwarder writer methods
            //
            virtual void WriteForwarderClassHeader(std::ostream& os) const override
            {
                os << "class " << _forwarderClassName << " : public ell::api::common::CallbackForwarder<" << _valueType << ">\n";
                os << "{\npublic:\n";
                // Constructor
                os << "    " << _forwarderClassName << "() = default;\n";
                // Destructor
                os << "    virtual ~" << _forwarderClassName << "() = default;\n";
                // Step
                os << "    std::vector<" << _stepOutputType << "> Step();\n";
                // WaitTimeForNextStep
                os << "    " << _waitTimeReturnType << " WaitTimeForNextStep();\n";
                // GetInstance
                os << "    static " << _forwarderClassName << "& GetInstance(ell::api::common::CallbackBase<"
                   << _valueType << ">& callback, std::vector<" << _valueType << ">& buffer);\n\n";
                os << "private:\n";
                os << "    static constexpr size_t inputSize = " << _stepInputSize << ";\n";
                os << "    static constexpr size_t outputSize = " << _stepOutputSize << ";\n";
                os << "    " << _stepInputType << " _input[inputSize];\n";
                os << "    " << _stepOutputType << " _output[outputSize];\n";
                os << "};\n\n";
            }

            virtual void WriteForwarderClassBody(std::ostream& os) const override
            {
                // Step
                os << "std::vector<" << _stepOutputType << "> " << _forwarderClassName << "::Step()\n";
                os << "{\n";
                os << "    " << _stepFunctionName << "(_input, _output);\n";
                os << "    return std::vector<" << _stepOutputType << ">(_output, _output + outputSize);\n";
                os << "}\n\n";

                // WaitTimeForNextStep
                os << _waitTimeReturnType << " " << _forwarderClassName << "::WaitTimeForNextStep()\n";
                os << "{\n";
                os << "    return " << _waitTimeFunctionName << "();\n";
                os << "}\n\n";

                // GetInstance
                os << _forwarderClassName << "& " << _forwarderClassName << "::GetInstance(ell::api::common::CallbackBase<"
                   << _stepInputType << ">& callback, std::vector<" << _stepInputType << ">& buffer)\n";
                os << "{\n";
                os << "    _predictor.InitializeOnce(callback, buffer);\n";
                os << "    return _predictor;\n";
                os << "}\n\n";
            }

            virtual void WriteForwarderClassInstance(std::ostream& os) const override
            {
                os << "static " << _forwarderClassName << " _predictor;\n";
            }

        private:
            void InitStepFunctionInfo()
            {
                auto functions = GetFunctionsWithTag(*_moduleEmitter, c_stepFunctionTagName);
                if (functions.size() != 1)
                {
                    throw EmitterException(EmitterError::badFunctionDefinition, "One and only one Step function must be emitted");
                }

                auto stepFunction = functions[0];
                llvm::Function* f = stepFunction.function;
                _stepFunctionName = f->getName();
                _stepOutputSize = std::stoul(stepFunction.values[0]);

                // Step functions have two pointer arguments
                auto it = f->args().begin();
                {
                    std::ostringstream os;
                    WriteLLVMType(os, (*it).getType()->getPointerElementType());
                    _stepInputType = os.str();
                }

                {
                    std::ostringstream os;
                    WriteLLVMType(os, (*(++it)).getType()->getPointerElementType());
                    _stepOutputType = os.str();
                }
            }

            void InitWaitTimeFunctionInfo()
            {
                auto functions = GetFunctionsWithTag(*_moduleEmitter, c_stepWaitTimeFunctionTagName);
                if (functions.size() != 1)
                {
                    throw EmitterException(EmitterError::badFunctionDefinition, "One and only one WaitTime function must be emitted");
                }

                llvm::Function* f = functions[0].function;
                _waitTimeFunctionName = f->getName();

                // WaitTime functions only return
                std::ostringstream os;
                WriteLLVMType(os, f->getReturnType());
                _waitTimeReturnType = os.str();
            }

            // Step
            std::string _stepFunctionName;
            std::string _stepInputType;
            std::string _stepOutputType;
            const size_t _stepInputSize = 2; // time signal
            size_t _stepOutputSize;

            // GetWaitTime
            std::string _waitTimeFunctionName;
            std::string _waitTimeReturnType;
        };

        void WriteCommonSwigCode(std::ostream& os, IRModuleEmitter& moduleEmitter)
        {
            auto pModule = moduleEmitter.GetLLVMModule();
            std::string moduleName = pModule->getName();

            // Declare the module and enable the directors feature
            os << "%module(directors=\"1\") " << moduleName << "\n";

            // Generate docstrings from types and method signatures
            os << "%feature(\"autodoc\", \"3\");\n\n";

            // Common SWIG code
            os << "%include \"callback.i\"\n";
            os << "%include \"macros.i\"\n\n";

            // Language-specific SWIG code
            os << "#ifdef SWIGPYTHON\n";
            os << "%rename(\"%(undercase)s\", %$isfunction) \"\";\n";
            os << "#endif\n\n";
        }

        void WriteModuleSwigCode(std::ostream& os, IRModuleEmitter& moduleEmitter, const std::string& headerName)
        {
            // Module C++ #include
            os << "%{\n";
            os << "#include \"" << headerName << "\"\n";
            os << "%}\n\n";

            // Module callback macros
            auto callbacks = GetFunctionsWithTag(moduleEmitter, c_callbackFunctionTagName);
            for (const auto& c : callbacks)
            {
                if (c.values.size() > 0 && c.values[0] == "SourceNode")
                {
                    SourceNodeInterfaceWriter writer(moduleEmitter, *(c.function));
                    writer.WriteSwigCode(os);
                }
            }

            os << "%include \"" << headerName << "\"\n";
        }
    }

    void WriteModuleSwigHeader(std::ostream& os, IRModuleEmitter& moduleEmitter)
    {
        std::string moduleName = moduleEmitter.GetLLVMModule()->getName();

        os << "//\n// ELL SWIG header for module " << moduleName << "\n//\n\n";
        os << "#pragma once\n\n";

        auto predicts = GetFunctionsWithTag(moduleEmitter, c_predictFunctionTagName);
        auto callbacks = GetFunctionsWithTag(moduleEmitter, c_callbackFunctionTagName);

        // Dependencies
        {
            DeclareIfndefSwig ifndefSwig(os);

            // Callbacks, if present
            if (callbacks.size() > 0)
            {
                os << "#include \"CallbackInterface.h\"\n";
                os << "#include \"ClockInterface.h\"\n";
            }

            // Module definitions (a.k.a. the C/C++ header)
            WriteModuleHeader(os, moduleEmitter);
        }

        // Predict functions
        for (const auto& p : predicts)
        {
            PredictInterfaceWriter writer(moduleEmitter, *(p.function));
            writer.WriteHeaderCode(os);
        }

        // Callbacks
        for (const auto& c : callbacks)
        {
            if (c.values.size() > 0 && c.values[0] == "SourceNode")
            {
                SourceNodeInterfaceWriter writer(moduleEmitter, *(c.function));
                writer.WriteHeaderCode(os);
            }
        }
    }

    void WriteModuleSwigInterface(std::ostream& os, IRModuleEmitter& moduleEmitter, const std::string& headerName)
    {
        auto pModule = moduleEmitter.GetLLVMModule();
        std::string moduleName = pModule->getName();

        os << "//\n// ELL SWIG interface for module " << moduleName << "\n//\n\n";

        WriteCommonSwigCode(os, moduleEmitter);
        WriteModuleSwigCode(os, moduleEmitter, headerName);
    }
}
}
