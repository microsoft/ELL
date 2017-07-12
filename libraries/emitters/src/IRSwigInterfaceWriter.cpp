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

// utilities
#include "Debug.h"

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

        struct CallbackSignature
        {
            CallbackSignature(llvm::Function& f)
            {
                functionName = f.getName();

                // Callbacks have a pointer parameter and a return (which can be void)
                {
                    std::ostringstream os;
                    auto& argument = *(f.args().begin());
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

                {
                    std::ostringstream os;
                    WriteLLVMType(os, f.getReturnType());
                    returnType = os.str();
                }

                className = inputType + "CallbackBase";
                className[0] = toupper(className[0]); // pascal case
            }

            std::string functionName;
            std::string className;
            std::string inputType;
            std::string returnType;
            bool inputIsScalar;
        };

        struct TimeFunctionSignature
        {
            TimeFunctionSignature(llvm::Function& f, const std::string& apiName)
                : apiName(apiName), functionName(f.getName())
            {
                // Time functions have no parameters and a non-void return
                std::ostringstream os;
                WriteLLVMType(os, f.getReturnType());
                returnType = os.str();
            }

            std::string apiName;
            std::string functionName;
            std::string returnType;
        };

        // Writes SWIG interfaces for predictors with step support
        class SteppablePredictorInterfaceWriter
        {
        public:
            SteppablePredictorInterfaceWriter(IRModuleEmitter& moduleEmitter, const std::vector<FunctionTagValues>& callbacks)
                : _moduleEmitter(&moduleEmitter)
            {
                InitStepFunctionInfo();
                InitTimeFunctionsInfo();

                std::string moduleName = moduleEmitter.GetLLVMModule()->getName();
                _className = moduleName + "Predictor";

                for (const auto& c : callbacks)
                {
                    if (c.values.size())
                    {
                        if (c.values[0] == "SourceNode")
                        {
                            _inputCallbacks.push_back(CallbackSignature(*c.function));
                        }
                        else if (c.values[0] == "SinkNode")
                        {
                            _outputCallbacks.push_back(CallbackSignature(*c.function));
                        }
                    }
                }

                // Eventually we'd support multiple sources and sinks.
                // For now, assert that we're only going to look at the first ones of each.
                DEBUG_THROW(_inputCallbacks.size() > 1, EmitterException(EmitterError::badFunctionDefinition, "At most one input callback function will be generated"));
                DEBUG_THROW(_outputCallbacks.size() > 1, EmitterException(EmitterError::badFunctionDefinition, "At most one output callback function will be generated"));
            }

            virtual ~SteppablePredictorInterfaceWriter() = default;

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
                        for (const auto& cs : _inputCallbacks)
                        {
                            WriteCallbackFunction(os, cs, false);
                        }

                        for (const auto& cs : _outputCallbacks)
                        {
                            WriteCallbackFunction(os, cs, true);
                        }
                    }

                    // Wrapper class definition
                    WriteForwarderClassBody(os);
                }
            }

        private:
            //
            // Callback writers
            //
            void WriteCallbackFunction(std::ostream& os, const CallbackSignature& cs, bool isOutputCallback) const
            {
                std::string inputType = cs.inputType;
                std::string predictorFunction = (isOutputCallback) ? "InvokeOutput" : "InvokeInput";

                if (!cs.inputIsScalar)
                {
                    // Emitted function takes a pointer input, fix the parameter signature accordingly
                    inputType.append("*");
                }
                else
                {
                    // Detect internal logic errors (i.e. only output callbacks can have scalar inputs)
                    assert(isOutputCallback);
                }

                os << cs.returnType << " " << cs.functionName << " (" << inputType << " input)\n{\n";
                if (cs.returnType == "void")
                {
                    os << "    _predictor." << predictorFunction << "(input);\n";
                }
                else
                {
                    os << "    return static_cast<" << cs.returnType << ">("
                       << "_predictor." << predictorFunction << "(input));\n";
                }
                os << "}\n";
            }

            void WriteCallbackSwigCode(std::ostream& os) const
            {
                std::vector<std::string> parameters;

                // Note: assumes at most 1 input and output callback (see debug throw in the constructor)
                if (_inputCallbacks.size() > 0)
                {
                    parameters.push_back(_inputCallbacks[0].className);
                    parameters.push_back(_inputCallbacks[0].inputType);
                }

                if (_outputCallbacks.size() > 0)
                {
                    parameters.push_back(_outputCallbacks[0].className);
                    parameters.push_back(_outputCallbacks[0].inputType);
                }

                if (parameters.size() > 0)
                {
                    os << "WRAP_CALLABLES_AS_CALLBACKS(" << _className << ", ";
                    WriteCommaSeparatedList(os, parameters);
                    os << ")\n\n";
                }
            }

            void WriteCallbackParameterList(std::ostream& os) const
            {
                std::vector<std::string> parameters;

                // Note: assumes at most 1 input and output callback (see debug throw in the constructor)
                if (_inputCallbacks.size() > 0)
                {
                    parameters.push_back("ell::api::CallbackBase<" + _inputCallbacks[0].inputType + ">& inputCallback");
                    parameters.push_back("std::vector<" + _inputCallbacks[0].inputType + ">& inputBuffer");
                }

                if (_outputCallbacks.size() > 0)
                {
                    parameters.push_back("ell::api::CallbackBase<" + _outputCallbacks[0].inputType + ">& outputCallback");
                }

                WriteCommaSeparatedList(os, parameters);
            }

            void WriteCallbackVariableList(std::ostream& os) const
            {
                std::vector<std::string> list;

                // Note: assumes at most 1 input and output callback (see debug throw in the constructor)
                if (_inputCallbacks.size() > 0)
                {
                    list.push_back("inputCallback");
                    list.push_back("inputBuffer");
                }

                if (_outputCallbacks.size() > 0)
                {
                    list.push_back("outputCallback");
                    list.push_back("_outputSize");
                }

                WriteCommaSeparatedList(os, list);
            }

            void WriteCallbackValueTypeList(std::ostream& os) const
            {
                std::vector<std::string> list;

                // Note: assumes at most 1 input and output callback (see debug throw in the constructor)
                if (_inputCallbacks.size() > 0)
                {
                    list.push_back(_inputCallbacks[0].inputType);
                }

                if (_outputCallbacks.size() > 0)
                {
                    list.push_back(_outputCallbacks[0].inputType);
                }

                WriteCommaSeparatedList(os, list);
            }

            //
            // Time functions
            //
            void WriteTimeFunctionsHeader(std::ostream& os) const
            {
                for (const auto& tf : _timeFunctions)
                {
                    os << "    " << tf.returnType << " " << tf.apiName << "();\n";
                }
            }

            void WriteTimeFunctionsCode(std::ostream& os) const
            {
                for (const auto& tf : _timeFunctions)
                {
                    os << tf.returnType << " " << _className << "::" << tf.apiName << "()\n";
                    os << "{\n";
                    os << "    return " << tf.functionName << "();\n";
                    os << "}\n\n";
                }
            }

            //
            // Forwarder class writers
            //
            void WriteForwarderClassHeader(std::ostream& os) const
            {
                os << "class " << _className << " : public ell::api::CallbackForwarder<";
                WriteCallbackValueTypeList(os);
                os << ">\n";
                os << "{\npublic:\n";

                // Constructor
                os << "    " << _className << "() = default;\n";

                // Destructor
                os << "    virtual ~" << _className << "() = default;\n";

                // Step
                os << "    std::vector<" << _stepOutputType << "> Step();\n";

                // Time functions
                WriteTimeFunctionsHeader(os);

                // GetInstance
                os << "    static " << _className << "& GetInstance(";
                WriteCallbackParameterList(os);
                os << ");\n\n";

                os << "private:\n";
                os << "    static constexpr size_t _inputSize = " << _stepInputSize << ";\n";
                os << "    static constexpr size_t _outputSize = " << _stepOutputSize << ";\n";
                os << "    " << _stepInputType << " _input[_inputSize];\n";
                os << "    " << _stepOutputType << " _output[_outputSize];\n";
                os << "};\n\n";
            }

            void WriteForwarderClassBody(std::ostream& os) const
            {
                // Step
                os << "std::vector<" << _stepOutputType << "> " << _className << "::Step()\n";
                os << "{\n";
                os << "    " << _stepFunctionName << "(_input, _output);\n";
                os << "    return std::vector<" << _stepOutputType << ">(_output, _output + _outputSize);\n";
                os << "}\n\n";

                // Time functions
                WriteTimeFunctionsCode(os);

                // GetInstance
                os << _className << "& " << _className << "::GetInstance(";
                WriteCallbackParameterList(os);
                os << ")\n{\n";
                os << "    _predictor.InitializeOnce(";
                WriteCallbackVariableList(os);
                os << ");\n";
                os << "    return _predictor;\n";
                os << "}\n\n";
            }

            void WriteForwarderClassInstance(std::ostream& os) const
            {
                os << "static " << _className << " _predictor;\n";
            }

            //
            // Initializers
            //
            void InitStepFunctionInfo()
            {
                auto functions = GetFunctionsWithTag(*_moduleEmitter, c_stepFunctionTagName);
                DEBUG_THROW(functions.size() > 1, EmitterException(EmitterError::badFunctionDefinition, "At most one Step function may be emitted"));

                if (functions.size() > 0)
                {
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
            }

            void InitTimeFunctionsInfo()
            {
                auto functions = GetFunctionsWithTag(*_moduleEmitter, c_stepTimeFunctionTagName);

                for (const auto& f : functions)
                {
                    if (f.values.size() > 0)
                    {
                        _timeFunctions.push_back(TimeFunctionSignature(*f.function, f.values[0]));
                    }
                }
            }

            //
            // Utilities
            //
            void WriteCommaSeparatedList(std::ostream& os, const std::vector<std::string>& list) const
            {
                bool first = true;
                for (const auto& item : list)
                {
                    if (!first)
                    {
                        os << ", ";
                    }
                    os << item;
                    first = false;
                }
            }

            // Step
            std::string _stepFunctionName;
            std::string _stepInputType;
            std::string _stepOutputType;
            const size_t _stepInputSize = 2; // time signal
            size_t _stepOutputSize;

            // Callbacks
            std::vector<CallbackSignature> _outputCallbacks;
            std::vector<CallbackSignature> _inputCallbacks;

            // Time functions
            std::vector<TimeFunctionSignature> _timeFunctions;

            // The name of the class we are emitting
            std::string _className;
            IRModuleEmitter* _moduleEmitter;
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
            if (callbacks.size() > 0)
            {
                SteppablePredictorInterfaceWriter writer(moduleEmitter, callbacks);
                writer.WriteSwigCode(os);
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
        auto profilers = GetFunctionsWithTag(moduleEmitter, c_profilingFunctionTagName);

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
        if (callbacks.size() > 0)
        {
            SteppablePredictorInterfaceWriter writer(moduleEmitter, callbacks);
            writer.WriteHeaderCode(os);
        }

        // Profiling functions
        for (const auto& p : profilers)
        {
            WriteFunctionDeclaration(os, moduleEmitter, *(p.function));
            os << "\n\n";
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
