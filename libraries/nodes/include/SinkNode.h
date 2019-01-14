////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SinkNode.h (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConstantNode.h"

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/OutputNodeBase.h>

#include <emitters/include/IRMetadata.h>

#include <utilities/include/TypeTraits.h>

#include <functional>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A function that the SinkNode calls to deliver data to user code. </summary>
    ///
    /// In device-side compiled code, the function signature should be:
    /// ```
    /// void SinkFunction(ValueType* data)
    /// ```
    template <typename ValueType>
    using SinkFunction = std::function<void(const std::vector<ValueType>&)>;

    template <typename ValueType>
    class SinkNode : public model::SinkNodeBase
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* triggerPortName = "inputTrigger";

        const model::InputPort<ValueType>& input = _input;
        const model::InputPort<bool>& trigger = _trigger;
        const model::OutputPort<ValueType>& output = _output; // maybe we don't need this because results are reported via callback
        /// @}

        SinkNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> Port elements for input values. </param>
        /// <param name="trigger"> Port elements for the trigger. </param>
        /// <param name="sinkFunctionName"> The sink function name to be emitted. </param>
        /// <param name="sink"> The optional sink function that will receive output values. </param>
        SinkNode(const model::OutputPort<ValueType>& input, const model::OutputPort<bool>& trigger, const std::string& sinkFunctionName, SinkFunction<ValueType> sink = nullptr);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> Port elements for input values. </param>
        /// <param name="trigger"> Port elements for the trigger. </param>
        /// <param name="shape"> The output shape. </param>
        /// <param name="sinkFunctionName"> The sink function name to be emitted. </param>
        /// <param name="sink"> The optional sink function that will receive output values. </param>
        SinkNode(const model::OutputPort<ValueType>& input, const model::OutputPort<bool>& trigger, const model::MemoryShape& shape, const std::string& sinkFunctionName, SinkFunction<ValueType> sink = nullptr);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> Port elements for input values. </param>
        /// <param name="trigger"> Port elements for the trigger. </param>
        /// <param name="outputVectorSize"> The output vector size. </param>
        /// <param name="sinkFunctionName"> The sink function name to be emitted. </param>
        /// <param name="sink"> The optional sink function that will receive output values. </param>
        SinkNode(const model::OutputPort<ValueType>& input, const model::OutputPort<bool>& trigger, size_t outputVectorSize, const std::string& sinkFunctionName, SinkFunction<ValueType> sink = nullptr);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SinkNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Sets the sink function for this node for use in Compute(). </summary>
        ///
        /// <param name="function"> The sink function to set. </param>
        void SetSinkFunction(SinkFunction<ValueType> function) { _sink = function; }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: callback function name, shape

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void SetOutputValuesLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void SetOutputValuesExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        model::InputPort<ValueType> _input;
        model::InputPort<bool> _trigger;
        model::OutputPort<ValueType> _output;

        SinkFunction<ValueType> _sink;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="model"> The Model or ModelTransformer to add the node to. </param>
    /// <param name="onto"> The output port to use as the input for the new node. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ModelLikeType, typename ValueType>
    const model::OutputPort<ValueType>& AppendSink(ModelLikeType& model, const model::OutputPort<ValueType>& onto);
} // namespace nodes
} // namespace ell

#pragma region implementation

#include <utilities/include/Debug.h>
#include <utilities/include/Exception.h>

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

    template <typename ModelLikeType, typename ValueType>
    const model::OutputPort<ValueType>& AppendSink(ModelLikeType& model, const model::OutputPort<ValueType>& output)
    {
        static_assert(utilities::IsOneOf<ModelLikeType, model::Model, model::ModelTransformer>, "'model' parameter must be a model::Model or model::ModelTransformer");
        auto sinkNode = model.template AddNode<SinkNode<ValueType>>(output, AppendConstant(model, true), "OutputCallback");
        return sinkNode->output;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
