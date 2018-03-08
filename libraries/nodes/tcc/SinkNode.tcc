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
        : SinkNode({}, {}, math::TensorShape{ 0, 0, 0 }, "", nullptr)
    {
    }

    // Following the pattern of OutputNode, we provide a constructor override that infers the shape from the input
    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, const model::PortElements<bool>& trigger, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : SinkNode(input, trigger, math::TensorShape{ input.Size(), 1, 1 }, sinkFunctionName, sink)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, const model::PortElements<bool>& trigger, size_t outputVectorSize, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : SinkNode(input, trigger, math::TensorShape{ outputVectorSize, 1, 1 }, sinkFunctionName, sink)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, const model::PortElements<bool>& trigger, const math::TensorShape& shape, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : model::SinkNodeBase(_input, _trigger, _output, shape, sinkFunctionName),
        _input(this, input, defaultInputPortName),
        _trigger(this, trigger, triggerPortName),
        _output(this, defaultOutputPortName, shape.Size()),
        _sink(sink == nullptr ? [](const auto&){} : sink)
    {
    }

    template <typename ValueType>
    void SinkNode<ValueType>::Compute() const
    {
        DEBUG_THROW(_sink == nullptr, utilities::InputException(utilities::InputExceptionErrors::nullReference, "Sink function is not set"));

        if (_sink != nullptr && _trigger.GetValue(0))
        {
            _sink(_input.GetValue());
        }
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void SinkNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pTrigger = compiler.EnsurePortEmitted(trigger);
        std::string prefixedName(compiler.GetNamespacePrefix() + "_" + GetCallbackName());
        auto& module = function.GetModule();

        auto if1 = function.If(emitters::TypedComparison::equals, pTrigger, function.Literal(true));
        {
            if (IsScalar(input))
            {
                // Callback signature: void SinkFunction(ValueType t)
                const emitters::NamedVariableTypeList parameters = { { "output", emitters::GetVariableType<ValueType>() } };
                module.DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);

                llvm::Function* pSinkFunction = module.GetFunction(prefixedName);
                function.Call(pSinkFunction, { compiler.LoadPortElementVariable(input.GetInputElement(0)) });
            }
            else
            {
                // Callback signature: void SinkFunction(ValueType* array)
                const emitters::NamedVariableTypeList parameters = { { "output", emitters::GetPointerType(emitters::GetVariableType<ValueType>()) } };
                module.DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);

                llvm::Function* pSinkFunction = module.GetFunction(prefixedName);
                function.Call(pSinkFunction, { function.PointerOffset(pInput, function.Literal(0)) });
            }
        }
        if1.End();

        // Tag the sink function as a callback that is emitted in headers
        module.IncludeInCallbackInterface(prefixedName, "SinkNode");

        // Set output values as well, useful when user code is in a non-event-driven mode
        if (!IsScalar(input) && !compiler.GetCompilerOptions().unrollLoops)
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
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newTrigger = transformer.TransformPortElements(_trigger.GetPortElements());
        auto newNode = transformer.AddNode<SinkNode<ValueType>>(newInput, newTrigger, GetShape(), GetCallbackName(), _sink);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    utilities::ArchiveVersion SinkNode<ValueType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion sinkNodeShapeArchiveVersion = { utilities::ArchiveVersionNumbers::v6_sink_triggers };

        return sinkNodeShapeArchiveVersion;
    }

    template <typename ValueType>
    bool SinkNode<ValueType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        constexpr utilities::ArchiveVersion sinkNodeNoShapeArchiveVersion = { utilities::ArchiveVersionNumbers::v0_initial };
        constexpr utilities::ArchiveVersion sinkNodeShapeArchiveVersion = { utilities::ArchiveVersionNumbers::v6_sink_triggers };

        return version >= sinkNodeNoShapeArchiveVersion && version <= sinkNodeShapeArchiveVersion;
    }

    template <typename ValueType>
    void SinkNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[triggerPortName] << _trigger;
        archiver["sinkFunctionName"] << GetCallbackName();
        archiver["shape"] << static_cast<std::vector<size_t>>(GetShape());
    }

    template <typename ValueType>
    void SinkNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[triggerPortName] >> _trigger;

        std::string sinkFunctionName;
        archiver["sinkFunctionName"] >> sinkFunctionName;
        SetCallbackName(sinkFunctionName);

        std::vector<size_t> shapeVector;
        archiver["shape"] >> shapeVector;
        SetShape(math::TensorShape{ shapeVector });

        // _sink needs to be set separately
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