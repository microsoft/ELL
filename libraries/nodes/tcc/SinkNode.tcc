////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SinkNode.tcc (nodes)
//  Authors:  Lisa Ong
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
    SinkNode<ValueType>::SinkNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, SinkFunction<ValueType> sink, const std::string& sinkFunctionName)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _sink(std::move(sink)), _sinkFunctionName(sinkFunctionName)
    {
    }

    template <typename ValueType>
    void SinkNode<ValueType>::Compute() const
    {
        DEBUG_THROW(_sink == nullptr, utilities::InputException(utilities::InputExceptionErrors::nullReference, "Sink function is not set"));

        auto result = EvaluateInput();
        if (result && _sink != nullptr)
        {
            _sink(_input.GetValue());
        }
        _output.SetOutput({ result });
    }

    template <typename ValueType>
    void SinkNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        if (IsScalar(input))
        {
            // Callback signature: void SinkFunction(ValueType t)
            const emitters::ValueTypeList parameters = { emitters::GetVariableType<ValueType>() };
            function.GetModule().DeclareFunction(_sinkFunctionName, emitters::VariableType::Void, parameters);

            llvm::Function* pSinkFunction = function.GetModule().GetFunction(_sinkFunctionName);
            // TODO: tag metadata on pSinkFunction

            function.Call(pSinkFunction, { pInput });
        }
        else
        {
            // Callback signature: void SinkFunction(ValueType* array)
            const emitters::ValueTypeList parameters = { emitters::GetPointerType(emitters::GetVariableType<ValueType>()) };
            function.GetModule().DeclareFunction(_sinkFunctionName, emitters::VariableType::Void, parameters);

            llvm::Function* pSinkFunction = function.GetModule().GetFunction(_sinkFunctionName);
            // TODO: tag metadata on pSinkFunction

            function.Call(pSinkFunction, { function.PointerOffset(pInput, function.Literal(0)) });
        }

        // EvaluateInput defaults to 'pass through' in base implementation
        function.SetValueAt(pResult, function.Literal(0), function.Literal(true));
    }

    template <typename ValueType>
    void SinkNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SinkNode<ValueType>>(newPortElements, _sink, _sinkFunctionName);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void SinkNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver["sinkFunctionName"] << _sinkFunctionName;
    }

    template <typename ValueType>
    void SinkNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver["sinkFunctionName"] >> _sinkFunctionName;
        // _sink needs to be set separately
    }

    template <typename ValueType>
    bool SinkNode<ValueType>::EvaluateInput() const
    {
        // Default pass through (derived classes will override).
        return true;
    }
}
}