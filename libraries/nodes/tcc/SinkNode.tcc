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
        : SinkNode({}, {}, model::MemoryShape{ 0 }, "", nullptr)
    {
    }

    // Following the pattern of OutputNode, we provide a constructor override that infers the shape from the input
    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, const model::PortElements<bool>& trigger, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : SinkNode(input, trigger, model::MemoryShape{ static_cast<int>(input.Size()) }, sinkFunctionName, sink)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, const model::PortElements<bool>& trigger, size_t outputVectorSize, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : SinkNode(input, trigger, model::MemoryShape{ static_cast<int>(outputVectorSize) }, sinkFunctionName, sink)
    {
    }

    template <typename ValueType>
    SinkNode<ValueType>::SinkNode(const model::PortElements<ValueType>& input, const model::PortElements<bool>& trigger, const model::MemoryShape& shape, const std::string& sinkFunctionName, SinkFunction<ValueType> sink)
        : model::SinkNodeBase(_input, _trigger, _output, shape, sinkFunctionName),
        _input(this, input, defaultInputPortName),
        _trigger(this, trigger, triggerPortName),
        _output(this, defaultOutputPortName, shape),
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

        function.If(emitters::TypedComparison::equals, pTrigger, function.Literal(true), [prefixedName, pInput, &module, &compiler, this](emitters::IRFunctionEmitter& function) {
            // look up our global context object
            auto context = module.GlobalPointer(compiler.GetNamespacePrefix() + "_context", emitters::VariableType::Byte);
            auto globalContext = function.Load(context);

            if (IsScalar(input))
            {
                // Callback signature: void SinkFunction(void* context, ValueType t)
                const emitters::NamedVariableTypeList parameters = { { "context", emitters::VariableType::BytePointer }, 
                                                                     { "output", emitters::GetVariableType<ValueType>() } };
                module.DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);
                
                llvm::Function* pSinkFunction = module.GetFunction(prefixedName);
                function.Call(pSinkFunction, { globalContext, compiler.LoadPortElementVariable(input.GetInputElement(0)) });
            }
            else
            {
                // Callback signature: void SinkFunction(void* context, ValueType* array)
                const emitters::NamedVariableTypeList parameters = { { "context", emitters::VariableType::BytePointer }, 
                                                                     { "output", emitters::GetPointerType(emitters::GetVariableType<ValueType>()) } };
                module.DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);

                llvm::Function* pSinkFunction = module.GetFunction(prefixedName);
                function.Call(pSinkFunction, { globalContext, function.PointerOffset(pInput, function.Literal(0)) });
            }
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
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        assert(input.Size() == output.Size());

        // Concatenate the input ports in a similar way as OutputNodes,
        // because SinkNodes are just callback-enabled OutputNodes.
        auto inputElements = input.GetInputElements();
        int rangeStart = 0;
        for (auto range : inputElements.GetRanges())
        {
            llvm::Value* pInput = compiler.EnsurePortEmitted(*range.ReferencedPort());
            auto rangeSize = range.Size();
            function.For(rangeSize, [range, rangeStart, pInput, pOutput](emitters::IRFunctionEmitter& function, llvm::Value* i) {
                auto inputIndex = function.Operator(emitters::TypedOperator::add, i, function.Literal<int>(range.GetStartIndex()));
                auto outputIndex = function.Operator(emitters::TypedOperator::add, i, function.Literal(rangeStart));
                llvm::Value* value = function.ValueAt(pInput, inputIndex);
                function.SetValueAt(pOutput, outputIndex, value);
            });
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