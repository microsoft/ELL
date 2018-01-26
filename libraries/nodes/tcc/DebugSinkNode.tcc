////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DebugSinkNode.tcc (nodes)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
    namespace nodes
    {
        template <typename ValueType>
        DebugSinkNode<ValueType>::DebugSinkNode()
            : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0), _userData(nullptr)
        {
        }

        template <typename ValueType>
        DebugSinkNode<ValueType>::DebugSinkNode(const model::PortElements<ValueType>& input, DebugSinkFunction<ValueType> sink, const std::string& label, void* userData, const std::string& sinkFunctionName)
            : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, _input.Size()), _label(label), _userData(userData), _sinkFunctionName(sinkFunctionName), _sink(std::move(sink))
        {
        }

        template <typename ValueType>
        bool DebugSinkNode<ValueType>::ShouldCompileInline() const
        {
            return true;
        }

        template <typename ValueType>
        void DebugSinkNode<ValueType>::Compute() const
        {
            DEBUG_THROW(_sink == nullptr, utilities::InputException(utilities::InputExceptionErrors::nullReference, "Sink function is not set"));

            auto result = EvaluateInput();
            if (result && _sink != nullptr)
            {
                _sink(_label, _input.GetValue(), _userData);
            }
            _output.SetOutput(_input.GetValue());
        }

        template <typename ValueType>
        void DebugSinkNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
        {
            llvm::Value* pInput = compiler.EnsurePortEmitted(input);
            auto userData = function.Pointer((char*)_userData);

            // EvaluateInput defaults to 'pass through' in base implementation, which means
            // we always call the sink function
            if (IsScalar(input))
            {
                const emitters::NamedVariableTypeList parameters = { { "label", emitters::GetVariableType<char*>() },
                                                                     { "output", emitters::GetVariableType<ValueType>() },
                                                                     { "userData", emitters::GetVariableType<char*>() } };

                // Callback signature: void DebugSinkNode(char* label, ValueType t, char* userData)
                function.GetModule().DeclareFunction(_sinkFunctionName, emitters::VariableType::Void, parameters);
                llvm::Function* pSinkFunction = function.GetModule().GetFunction(_sinkFunctionName);
                function.Call(pSinkFunction, { function.Literal(_label), pInput, userData });
            }
            else
            {
                const emitters::NamedVariableTypeList parameters = { { "label", emitters::GetVariableType<char*>() },
                                                                     { "output", emitters::GetPointerType(emitters::GetVariableType<ValueType>()) },
                                                                     { "userData", emitters::GetVariableType<char*>() } };

                // Callback signature: void DebugSinkNode(char* label, ValueType* array, char* userData)
                function.GetModule().DeclareFunction(_sinkFunctionName, emitters::VariableType::Void, parameters);
                llvm::Function* pSinkFunction = function.GetModule().GetFunction(_sinkFunctionName);
                function.Call(pSinkFunction, { function.Literal(_label), function.PointerOffset(pInput, function.Literal(0)), userData });
            }

            // Tag the sink function as a callback that is emitted in headers
            function.IncludeInHeader();
        }

        template <typename ValueType>
        void DebugSinkNode<ValueType>::Copy(model::ModelTransformer& transformer) const
        {
            auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
            auto newNode = transformer.AddNode<DebugSinkNode<ValueType>>(newPortElements, _sink, _label, _userData, _sinkFunctionName);
            transformer.MapNodeOutput(output, newNode->output);
        }

        template <typename ValueType>
        void DebugSinkNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            Node::WriteToArchive(archiver);
            archiver[defaultInputPortName] << _input;
            archiver["sinkFunctionName"] << _sinkFunctionName;
        }

        template <typename ValueType>
        void DebugSinkNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Node::ReadFromArchive(archiver);
            archiver[defaultInputPortName] >> _input;
            archiver["sinkFunctionName"] >> _sinkFunctionName;
            // _sink needs to be set separately
        }

        template <typename ValueType>
        bool DebugSinkNode<ValueType>::EvaluateInput() const
        {
            // Default pass through (derived classes will override).
            return true;
        }
    };
}