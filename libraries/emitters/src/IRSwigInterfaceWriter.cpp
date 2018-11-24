////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRSwigInterfaceWriter.cpp (emitters)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRSwigInterfaceWriter.h"
#include "EmitterException.h"
#include "IRHeaderWriter.h"
#include "IRMetadata.h"

// utilities
#include "StringUtil.h"

// stl
#include <regex>
#include <sstream>
#include <string>

namespace ell
{
namespace emitters
{
    namespace
    {
        // Utilities
        std::string AsVectorType(const std::string& type)
        {
            return type == "float" ? "FloatVector" : "DoubleVector";
        }

        // Writes SWIG C++ interfaces for predict that take std::vectors for input and output (with optional SinkNode callbacks)
        class PredictFunctionWriter
        {
        public:
            PredictFunctionWriter(IRModuleEmitter& moduleEmitter, llvm::Function& predictFunction) :
                _function(&predictFunction)
            {
                _moduleName = moduleEmitter.GetModuleName();
                InitPredictFunctionInfo(moduleEmitter);
            }

            void WriteSwigCode(std::ostream& os) const
            {
                DeclareIfDefGuard guard(os, "SWIGPYTHON", DeclareIfDefGuard::Type::Positive);

                // clang-format off
                std::string predictPythonCode(
                    #include "SwigPredictPython.in"
                );
                // clang-format on

                auto resetFunctionName = _moduleName + "_Reset";
                std::string predictMethodName = TrimPrefix(_functionName, _moduleName + "_");
                predictMethodName[0] = ::toupper(predictMethodName[0]); // pascal case

                std::string className = _moduleName + "Wrapper";
                className[0] = ::toupper(className[0]); // pascal case

                std::string inputVectorType = AsVectorType(_inputType);

                ReplaceDelimiter(predictPythonCode, "WRAPPER_CLASS", className);
                ReplaceDelimiter(predictPythonCode, "PREDICT_METHOD", predictMethodName);
                ReplaceDelimiter(predictPythonCode, "INPUT_VECTOR_TYPE", inputVectorType);
                ReplaceDelimiter(predictPythonCode, "RESET_FUNCTION", resetFunctionName);

                os << "%pythoncode %{\n"
                   << predictPythonCode
                   << "\n%}\n";
            }

        private:
            void InitPredictFunctionInfo(IRModuleEmitter& moduleEmitter)
            {
                auto callbacks = GetFunctionsWithTag(moduleEmitter, c_callbackFunctionTagName);

                ModuleCallbackDefinitions moduleCallbacks(callbacks);

                _functionName = _function->getName();

                if (moduleCallbacks.sources.empty())
                {
                    // Three arguments context, input, output (input may be a scalar or pointer)
                    auto it = _function->args().begin();
                    ++it; // skip context argument

                    {
                        std::ostringstream os;
                        auto& argument = *it;
                        auto type = argument.getType();
                        if (type->isPointerTy())
                        {
                            _inputIsScalar = false;
                            WriteLLVMType(os, argument.getType()->getPointerElementType());
                        }
                        else
                        {
                            // todo: IRHeaderWriter doesn't really support this case, the ModelWrapper simply wraps everything is a std::vector...
                            _inputIsScalar = true;
                            WriteLLVMType(os, argument.getType());
                        }
                        _inputType = os.str();
                    }
                }
                else
                {
                    // The predict function is different if we have source nodes.  In this case we have:
                    //   Predict(context, double timestamp, ...) and the real input type for model then is
                    // defined in the SourceNode callback
                    _inputType = moduleCallbacks.sources[0].inputType;
                }
            }

            std::string _moduleName;
            std::string _functionName;
            std::string _inputType;
            bool _inputIsScalar;
            LLVMFunction _function;
        };

        void WriteShapeWrappers(std::ostream& os, IRModuleEmitter& moduleEmitter)
        {
            auto pModule = moduleEmitter.GetLLVMModule();
            std::string moduleName = pModule->getName();

            /*
            * Wrap the module_GetInputShape "C" function so we return the TensorShape on the stack
            * as a conveniently wrapped Python type.

            %inline %{
                ell::api::math::TensorShape get_default_input_shape() {
                    TensorShape  s;
                    module_GetInputShape(index, &s);
                    return ell::api::math::TensorShape{s.rows, s.columns, s.channels};
                }
            %} */

            // clang-format off
            std::string shapeWrappers(
                #include "SwigShapeWrappers.in"
            );
            // clang-format on

            ReplaceDelimiter(shapeWrappers, "MODULE", moduleName);

            os << shapeWrappers << "\n";
        }

        void WriteModuleSwigCode(std::ostream& os, IRModuleEmitter& moduleEmitter, const std::string& headerName)
        {
            auto pModule = moduleEmitter.GetLLVMModule();
            std::string moduleName = pModule->getName();
            std::string className = moduleName + "Wrapper";
            className[0] = ::toupper(className[0]); // pascal case

            //
            // Module
            //
            // clang-format off
            std::string moduleCode(
                #include "SwigModule.in"
            );
            // clang-format on

            ReplaceDelimiter(moduleCode, "MODULE", moduleName);
            ReplaceDelimiter(moduleCode, "HEADER", headerName);
            ReplaceDelimiter(moduleCode, "CLASSNAME", className);
            os << moduleCode << "\n";

            //
            // Predictor interfaces and callback macros
            //
            auto predicts = GetFunctionsWithTag(moduleEmitter, c_predictFunctionTagName);
            std::string predictFunctionName = (predicts[0].function)->getName();

            PredictFunctionWriter writer(moduleEmitter, *(predicts[0].function));
            writer.WriteSwigCode(os);

            os << "%include \"" << headerName << "\"\n";

            //
            // Shape wrappers
            //
            WriteShapeWrappers(os, moduleEmitter);
        }
    } // namespace

    void WriteModuleSwigInterface(std::ostream& os, IRModuleEmitter& moduleEmitter, const std::string& headerName)
    {
        auto pModule = moduleEmitter.GetLLVMModule();
        std::string moduleName = pModule->getName();

        os << "//\n// ELL SWIG interface for module " << moduleName << "\n//\n\n";

        WriteModuleSwigCode(os, moduleEmitter, headerName);
    }
} // namespace emitters
} // namespace ell
