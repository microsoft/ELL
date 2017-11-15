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
        : SinkNode({}, "", nullptr)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : CompilableNode({ &_input }, { &_output }),
        _input(this, input, inputPortName),
        _output(this, outputPortName, _input.Size()),
        _sinkFunctionName(sinkFunctionName),
        _sink(sink == nullptr ? [](const auto&){} : sink)
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
        std::string prefixedName(function.GetModule().GetModuleName() + "_" + _sinkFunctionName);
        DEBUG_EMIT_PRINTF(function, prefixedName + "\n");

        // EvaluateInput defaults to 'pass through' in base implementation, which means
        // we always call the sink function
        if (IsScalar(input))
        {
            // Callback signature: void SinkFunction(ValueType t)
            const emitters::VariableTypeList parameters = { emitters::GetVariableType<ValueType>() };
            function.GetModule().DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);

            llvm::Function* pSinkFunction = function.GetModule().GetFunction(prefixedName);
            function.Call(pSinkFunction, { pInput });
        }
        else
        {
            // Callback signature: void SinkFunction(ValueType* array)
            const emitters::VariableTypeList parameters = { emitters::GetPointerType(emitters::GetVariableType<ValueType>()) };
            function.GetModule().DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);

            llvm::Function* pSinkFunction = function.GetModule().GetFunction(prefixedName);
            function.Call(pSinkFunction, { function.PointerOffset(pInput, function.Literal(0)) });
        }

        // Tag the sink function as a callback that is emitted in headers
        function.GetModule().IncludeInHeader(prefixedName);
        function.GetModule().IncludeInCallbackInterface(prefixedName, "SinkNode");

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
        auto newNode = transformer.AddNode<SinkNode<ValueType>>(newPortElements, _sinkFunctionName);
        newNode->_sink = _sink;
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
        compiler.EnsurePortEmitted(input);
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