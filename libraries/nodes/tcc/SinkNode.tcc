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
    SinkNode<ValueType>::SinkNode() :
        SinkNode({}, {}, model::MemoryShape{ 0 }, "", nullptr)
    {
    }

    // Following the pattern of OutputNode, we provide a constructor override that infers the shape from the input
    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::OutputPort<ValueType>& input, const model::OutputPort<bool>& trigger, const std::string& sinkFunctionName, SinkFunction<ValueType> sink) :
        SinkNode(input, trigger, model::MemoryShape{ static_cast<int>(input.Size()) }, sinkFunctionName, sink)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::OutputPort<ValueType>& input, const model::OutputPort<bool>& trigger, size_t outputVectorSize, const std::string& sinkFunctionName, SinkFunction<ValueType> sink) :
        SinkNode(input, trigger, model::MemoryShape{ static_cast<int>(outputVectorSize) }, sinkFunctionName, sink)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::OutputPort<ValueType>& input, const model::OutputPort<bool>& trigger, const model::MemoryShape& shape, const std::string& sinkFunctionName, SinkFunction<ValueType> sink) :
        model::SinkNodeBase(_input, _trigger, _output, shape, sinkFunctionName),
        _input(this, input, defaultInputPortName),
        _trigger(this, trigger, triggerPortName),
        _output(this, defaultOutputPortName, shape),
        _sink(sink == nullptr ? [](const auto&) {} : sink)
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
        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(input);
        emitters::LLVMValue pTrigger = compiler.EnsurePortEmitted(trigger);
        std::string prefixedName(compiler.GetNamespacePrefix() + "_" + GetCallbackName());
        auto& module = function.GetModule();
        auto triggerValue = function.ValueAt(pTrigger, 0);

        function.If(emitters::TypedComparison::equals, triggerValue, function.Literal(true), [prefixedName, pInput, &module, &compiler](emitters::IRFunctionEmitter& function) {
            // look up our global context object
            auto context = module.GlobalPointer(compiler.GetNamespacePrefix() + "_context", emitters::VariableType::Byte);
            auto globalContext = function.Load(context);

            // Callback signature: void SinkFunction(void* context, ValueType* array)
            const emitters::NamedVariableTypeList parameters = { { "context", emitters::VariableType::BytePointer },
                                                                 { "output", emitters::GetPointerType(emitters::GetVariableType<ValueType>()) } };
            module.DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);

            emitters::LLVMFunction pSinkFunction = module.GetFunction(prefixedName);
            function.Call(pSinkFunction, { globalContext, function.PointerOffset(pInput, function.Literal(0)) });
        });

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
        const auto& newInput = transformer.GetCorrespondingInputs(_input);
        const auto& newTrigger = transformer.GetCorrespondingInputs(_trigger);
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
        archiver["shape"] << GetShape().ToVector();
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

        std::vector<int> shapeVector;
        archiver["shape"] >> shapeVector;
        SetShape({ shapeVector });

        // _sink needs to be set separately
    }

    template <typename ValueType>
    void SinkNode<ValueType>::SetOutputValuesLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        assert(input.Size() == output.Size());

        // Concatenate the input ports in a similar way as OutputNodes,
        // because SinkNodes are just callback-enabled OutputNodes.
        auto input = function.LocalArray(compiler.EnsurePortEmitted(_input));
        auto output = function.LocalArray(compiler.EnsurePortEmitted(_output));
        // check if the output variable is null.
        function.If(ell::emitters::TypedComparison::notEquals, output, function.NullPointer(output.value->getType()->getPointerElementType()->getPointerTo()), [input, output, this](emitters::IRFunctionEmitter& function) {
            auto size = _input.Size();
            function.For(size, [input, output](emitters::IRFunctionEmitter& function, auto i) {
                output[i] = input[i];
            });
        });
    }

    template <typename ValueType>
    void SinkNode<ValueType>::SetOutputValuesExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        compiler.EnsurePortEmitted(input);
        emitters::LLVMValue pOutput = compiler.EnsurePortEmitted(output);

        auto numInputs = input.Size();
        assert(numInputs == output.Size());

        for (size_t i = 0; i < numInputs; ++i)
        {
            // Concatenate the input ports
            emitters::LLVMValue value = compiler.LoadPortElementVariable(input.GetInputElement(i));
            function.SetValueAt(pOutput, function.Literal(static_cast<int>(i)), value);
        }
    }
} // namespace nodes
} // namespace ell