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
            : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
        {
        }

        template <typename ValueType>
        DebugSinkNode<ValueType>::DebugSinkNode(const model::PortElements<ValueType>& input, DebugSinkFunction<ValueType> sink, const std::string& label, const std::string& sinkFunctionName)
            : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size()), _sink(std::move(sink)), _label(label), _sinkFunctionName(sinkFunctionName)
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
                _sink(_label, _input.GetValue());
            }
            _output.SetOutput(_input.GetValue());
        }

        template <typename ValueType>
        void DebugSinkNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
        {
            llvm::Value* pInput = compiler.EnsurePortEmitted(input);

            // EvaluateInput defaults to 'pass through' in base implementation, which means
            // we always call the sink function
            if (IsScalar(input))
            {
                const emitters::NamedVariableTypeList parameters = { { "label", emitters::GetVariableType<char*>() },
                                                                     { "output", emitters::GetVariableType<ValueType>() } };

                // Callback signature: void DebugSinkNode(ValueType t)
                function.GetModule().DeclareFunction(_sinkFunctionName, emitters::VariableType::Void, parameters);

                llvm::Function* pSinkFunction = function.GetModule().GetFunction(_sinkFunctionName);
                DEBUG_EMIT_PRINTF(function, _sinkFunctionName + "\n");

                function.Call(pSinkFunction, { function.Literal(_label), pInput });
            }
            else
            {
                const emitters::NamedVariableTypeList parameters = { { "label", emitters::GetVariableType<char*>() },
                                                                     { "output", emitters::GetPointerType(emitters::GetVariableType<ValueType>()) } };

                // Callback signature: void DebugSinkNode(ValueType* array)
                function.GetModule().DeclareFunction(_sinkFunctionName, emitters::VariableType::Void, parameters);

                llvm::Function* pSinkFunction = function.GetModule().GetFunction(_sinkFunctionName);
                DEBUG_EMIT_PRINTF(function, _sinkFunctionName + "\n");

                function.Call(pSinkFunction, { function.Literal(_label), function.PointerOffset(pInput, function.Literal(0)) });
            }

            // Tag the sink function as a callback that is emitted in headers
            function.GetModule().IncludeInHeader(_sinkFunctionName);
        }

        template <typename ValueType>
        void DebugSinkNode<ValueType>::Copy(model::ModelTransformer& transformer) const
        {
            auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
            auto newNode = transformer.AddNode<DebugSinkNode<ValueType>>(newPortElements, _sink, _label, _sinkFunctionName);
            transformer.MapNodeOutput(output, newNode->output);
        }

        template <typename ValueType>
        void DebugSinkNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            Node::WriteToArchive(archiver);
            archiver[inputPortName] << _input;
            archiver["sinkFunctionName"] << _sinkFunctionName;
        }

        template <typename ValueType>
        void DebugSinkNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            Node::ReadFromArchive(archiver);
            archiver[inputPortName] >> _input;
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