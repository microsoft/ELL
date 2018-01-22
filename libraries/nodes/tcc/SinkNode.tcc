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
        : SinkNode({}, math::TensorShape{ 0, 0, 0 }, "", nullptr)
    {
    }

    // Following the pattern of OutputNode, we provide a constructor override that infers the shape from the input
    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : SinkNode(input, math::TensorShape{ input.Size(), 1, 1 }, sinkFunctionName, sink)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, size_t outputVectorSize, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : SinkNode(input, math::TensorShape{ outputVectorSize, 1, 1 }, sinkFunctionName, sink)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, const math::TensorShape& shape, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : model::SinkNodeBase(_input, _output, shape, sinkFunctionName),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, shape.Size()),
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
        std::string prefixedName(compiler.GetNamespacePrefix() + "_" + GetCallbackName());

        // EvaluateInput defaults to 'pass through' in base implementation, which means
        // we always call the sink function
        if (IsScalar(input))
        {
            // Callback signature: void SinkFunction(ValueType t)
            const emitters::VariableTypeList parameters = { emitters::GetVariableType<ValueType>() };
            function.GetModule().DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);

            llvm::Function* pSinkFunction = function.GetModule().GetFunction(prefixedName);
            function.Call(pSinkFunction, { compiler.LoadPortElementVariable(input.GetInputElement(0)) });
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
        auto newNode = transformer.AddNode<SinkNode<ValueType>>(newPortElements, GetShape(), GetCallbackName(), _sink);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    utilities::ArchiveVersion SinkNode<ValueType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion sinkNodeShapeArchiveVersion = { utilities::ArchiveVersionNumbers::v4_source_sink_shapes };

        return sinkNodeShapeArchiveVersion;
    }

    template <typename ValueType>
    bool SinkNode<ValueType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        constexpr utilities::ArchiveVersion sinkNodeNoShapeArchiveVersion = { utilities::ArchiveVersionNumbers::v0_initial };
        constexpr utilities::ArchiveVersion sinkNodeShapeArchiveVersion = { utilities::ArchiveVersionNumbers::v4_source_sink_shapes };

        return version >= sinkNodeNoShapeArchiveVersion && version <= sinkNodeShapeArchiveVersion;
    }

    template <typename ValueType>
    void SinkNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["sinkFunctionName"] << GetCallbackName();
        archiver["shape"] << static_cast<std::vector<size_t>>(GetShape());
    }

    template <typename ValueType>
    void SinkNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;

        std::string sinkFunctionName;
        archiver["sinkFunctionName"] >> sinkFunctionName;
        SetCallbackName(sinkFunctionName);

        std::vector<size_t> shapeVector;
        archiver["shape"] >> shapeVector;
        SetShape(math::TensorShape{ shapeVector });

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
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        assert(input.Size() == output.Size());

        // Concatenate the input ports in a similar way as OutputNodes,
        // because SinkNodes are just callback-enabled OutputNodes.
        auto inputElements = input.GetInputElements();
        int rangeStart = 0;
        for (auto range : inputElements.GetRanges())
        {
            llvm::Value* pInput = compiler.EnsurePortEmitted(*range.ReferencedPort());
            auto forLoop = function.ForLoop();
            auto rangeSize = range.Size();
            forLoop.Begin(rangeSize);
            {
                auto i = forLoop.LoadIterationVariable();
                auto inputIndex = function.Operator(emitters::TypedOperator::add, i, function.Literal<int>(range.GetStartIndex()));
                auto outputIndex = function.Operator(emitters::TypedOperator::add, i, function.Literal(rangeStart));
                llvm::Value* value = function.ValueAt(pInput, inputIndex);
                function.SetValueAt(pOutput, outputIndex, value);
            }
            forLoop.End();
            rangeStart += rangeSize;
        }
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
            // Concatenate the input ports
            llvm::Value* value = compiler.LoadPortElementVariable(input.GetInputElement(i));
            function.SetValueAt(pOutput, function.Literal(static_cast<int>(i)), value);
        }
    }
}
}