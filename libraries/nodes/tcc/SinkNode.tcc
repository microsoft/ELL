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
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size()), _sink(std::move(sink)), _sinkFunctionName(sinkFunctionName)
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
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void SinkNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);

        // EvaluateInput defaults to 'pass through' in base implementation, which means
        // we always call the sink function
        if (IsScalar(input))
        {
            // Callback signature: void SinkFunction(ValueType t)
            const emitters::ValueTypeList parameters = { emitters::GetVariableType<ValueType>() };
            function.GetModule().DeclareFunction(_sinkFunctionName, emitters::VariableType::Void, parameters);

            llvm::Function* pSinkFunction = function.GetModule().GetFunction(_sinkFunctionName);
            DEBUG_EMIT_PRINTF(function, _sinkFunctionName + "\n");

            function.Call(pSinkFunction, { pInput });
        }
        else
        {
            // Callback signature: void SinkFunction(ValueType* array)
            const emitters::ValueTypeList parameters = { emitters::GetPointerType(emitters::GetVariableType<ValueType>()) };
            function.GetModule().DeclareFunction(_sinkFunctionName, emitters::VariableType::Void, parameters);

            llvm::Function* pSinkFunction = function.GetModule().GetFunction(_sinkFunctionName);
            DEBUG_EMIT_PRINTF(function, _sinkFunctionName + "\n");

            function.Call(pSinkFunction, { function.PointerOffset(pInput, function.Literal(0)) });
        }

        // Tag the sink function as a callback that is emitted in headers
        function.GetModule().IncludeInHeader(_sinkFunctionName);
        function.GetModule().IncludeInCallbackInterface(_sinkFunctionName, "SinkNode");

        // Set output values as well, useful when user code is in a non-event-driven mode
        if (!IsScalar(input) && !compiler.GetCompilerParameters().unrollLoops)
        {
            SetOutputValuesLoop(compiler, function);
        }
        else
        {
            SetOutputValuesExpanded(compiler, function);
        }
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

    template <typename ValueType>
    void SinkNode<ValueType>::SetOutputValuesLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        auto numInputs = input.Size();
        assert(numInputs == output.Size());

        auto forLoop = function.ForLoop();
        forLoop.Begin(0, numInputs, 1);
        {
            auto i = forLoop.LoadIterationVariable();
            auto value = function.ValueAt(pInput, i);
            function.SetValueAt(pOutput, i, value);
        }
        forLoop.End();
    }

    template <typename ValueType>
    void SinkNode<ValueType>::SetOutputValuesExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        auto numInputs = input.Size();
        assert(numInputs == output.Size());

        for (size_t i = 0; i < numInputs; ++i)
        {
            llvm::Value* value = compiler.LoadPortElementVariable(input.GetInputElement(i));
            function.SetValueAt(pOutput, function.Literal(static_cast<int>(i)), value);
        }
    }
}
}