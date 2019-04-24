////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SourceNode.h (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ClockNode.h" // for TimeTickType

#include <emitters/include/IRMetadata.h>
#include <emitters/include/IRModuleEmitter.h>
#include <emitters/include/LLVMUtilities.h>

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputNodeBase.h>
#include <model/include/ModelTransformer.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A function that the SourceNode calls to receive data from user code. </summary>
    ///
    /// In device-side compiled code, the function signature should be:
    /// ```
    /// bool SourceFunction(ValueType* data)
    /// ```
    /// where the bool return value is used to indicate if a new sample is available from the source
    template <typename ValueType>
    using SourceFunction = std::function<bool(std::vector<ValueType>&)>;

    /// <summary> A node that provides a source of data through a sampling function callback. </summary>
    template <typename ValueType>
    class SourceNode : public model::SourceNodeBase
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<TimeTickType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        SourceNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> Port elements for input values (sample time, current time). </param>
        /// <param name="inputVectorSize"> The input vector size. </param>
        /// <param name="sourceFunctionName"> The source function name to be emitted. </param>
        /// <param name="source"> The optional source function that will provide input values. </param>
        SourceNode(const model::OutputPort<nodes::TimeTickType>& input, size_t inputVectorSize, const std::string& sourceFunctionName, SourceFunction<ValueType> source = nullptr);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> Port elements for input values (sample time, current time). </param>
        /// <param name="shape"> The input shape. </param>
        /// <param name="sourceFunctionName"> The source function name to be emitted. </param>
        /// <param name="source"> The optional source function that will provide input values. </param>
        SourceNode(const model::OutputPort<nodes::TimeTickType>& input, const model::MemoryShape& shape, const std::string& sourceFunctionName, SourceFunction<ValueType> source = nullptr);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> Port elements for input values (sample time, current time). </param>
        /// <param name="layout"> The input memory layout. </param>
        /// <param name="sourceFunctionName"> The source function name to be emitted. </param>
        /// <param name="source"> The optional source function that will provide input values. </param>
        SourceNode(const model::OutputPort<nodes::TimeTickType>& input, const model::PortMemoryLayout& layout, const std::string& sourceFunctionName, SourceFunction<ValueType> source = nullptr);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SourceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Interpolates the buffered sample to match the new time. </summary>
        ///
        /// <param name="originalTime"> Original time for the buffered sample. </param>
        /// <param name="newTime"> New time for the buffered sample. </param>
        virtual void Interpolate(TimeTickType originalTime, TimeTickType newTime) const;

        /// <summary> Sets the source function for this node for use in Compute(). </summary>
        ///
        /// <param name="function"> The source function to set. </param>
        void SetSourceFunction(SourceFunction<ValueType> function) { _source = function; }

        /// <summary> Sets the value output by this node </summary>
        ///
        /// <param name="inputValues"> The values for this node to output </param>
        void SetInput(std::vector<ValueType> inputValues);

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

        void SetOutputValuesLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, emitters::LLVMValue sample);
        void SetOutputValuesExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, emitters::LLVMValue sample);

        model::InputPort<TimeTickType> _input;
        model::OutputPort<ValueType> _output;

        SourceFunction<ValueType> _source;

        mutable std::vector<ValueType> _bufferedSample;
        mutable TimeTickType _bufferedSampleTime;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    SourceNode<ValueType>::SourceNode() :
        SourceNode({}, model::MemoryShape{ 0 }, "", nullptr)
    {
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::OutputPort<nodes::TimeTickType>& input, size_t inputVectorSize, const std::string& sourceFunctionName, SourceFunction<ValueType> source) :
        SourceNode(input, model::MemoryShape{ static_cast<int>(inputVectorSize) }, sourceFunctionName, source)
    {
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::OutputPort<nodes::TimeTickType>& input, const model::MemoryShape& shape, const std::string& sourceFunctionName, SourceFunction<ValueType> source) :
        model::SourceNodeBase(_input, _output, sourceFunctionName),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, shape),
        _source(source == nullptr ? [](auto&) { return false; } : source)
    {
        _bufferedSample.resize(shape.NumElements());
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::OutputPort<nodes::TimeTickType>& input, const model::PortMemoryLayout& layout, const std::string& sourceFunctionName, SourceFunction<ValueType> source) :
        model::SourceNodeBase(_input, _output, sourceFunctionName),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, layout),
        _source(source == nullptr ? [](auto&) { return false; } : source)
    {
        if (!layout.IsCanonicalOrder())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "SourceNode must be in canonical order");
        }
        _bufferedSample.resize(layout.NumElements());
    }

    template <typename ValueType>
    void SourceNode<ValueType>::SetInput(std::vector<ValueType> inputValues)
    {
        assert(_bufferedSample.size() == inputValues.size());
        _bufferedSample = inputValues;
    }

    template <typename ValueType>
    void SourceNode<ValueType>::Compute() const
    {
        auto sampleTime = _input.GetValue(0);

        if (_source(_bufferedSample))
        {
            // Determine if the sample time differs from the current time
            auto currentTime = _input.GetValue(1);
            if (currentTime > sampleTime)
            {
                // Interpolate _bufferedSample to match the sample time
                Interpolate(currentTime, sampleTime);
            }
        }

        _bufferedSampleTime = sampleTime;
        _output.SetOutput(_bufferedSample);
    }

    template <typename ValueType>
    void SourceNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(input);
        compiler.EnsurePortEmitted(output);
        auto& module = function.GetModule();

        std::string name = this->GetFriendlyName();
        if (name.empty())
        {
            name = "input";
        }

        // Globals
        emitters::Variable* pBufferedSampleTimeVar = module.Variables().AddVariable<emitters::InitializedScalarVariable<TimeTickType>>(emitters::VariableScope::global, _bufferedSampleTime);
        emitters::Variable* pBufferedSampleVar = module.Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, output.Size());
        emitters::LLVMValue pBufferedSampleTime = module.EnsureEmitted(*pBufferedSampleTimeVar);
        emitters::LLVMValue pBufferedSample = module.EnsureEmitted(*pBufferedSampleVar);
        emitters::LLVMValue bufferedSampleTime = function.Load(pBufferedSampleTime);
        UNUSED(bufferedSampleTime);

        // Callback function
        const emitters::NamedVariableTypeList parameters = { { "context", emitters::VariableType::BytePointer },
                                                             { name, emitters::GetPointerType(emitters::GetVariableType<ValueType>()) } };
        std::string prefixedName(compiler.GetNamespacePrefix() + "_" + GetCallbackName());
        module.DeclareFunction(prefixedName, emitters::GetVariableType<bool>(), parameters);
        module.IncludeInCallbackInterface(prefixedName, "SourceNode");

        emitters::LLVMFunction pSamplingFunction = module.GetFunction(prefixedName);

        // look up our global context object
        auto context = module.GlobalPointer(compiler.GetNamespacePrefix() + "_context", emitters::VariableType::Byte);
        auto globalContext = function.Load(context);

        // Locals
        auto sampleTime = function.ValueAt(pInput, function.Literal(0));

        // Invoke the callback and optionally interpolate.
        function.Call(pSamplingFunction, { globalContext, function.PointerOffset(pBufferedSample, 0) });

        // TODO: Interpolate if there is a sample, and currentTime > sampleTime
        // Note: currentTime can be retrieved via currentTime = function.ValueAt(pInput, function.Literal(1));

        // Set sample values to the output
        if (!IsScalar(output) && !function.GetCompilerOptions().unrollLoops)
        {
            SetOutputValuesLoop(compiler, function, pBufferedSample);
        }
        else
        {
            SetOutputValuesExpanded(compiler, function, pBufferedSample);
        }

        // Update the cached sample time
        function.Store(pBufferedSampleTime, sampleTime);
    }

    template <typename ValueType>
    void SourceNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<SourceNode<ValueType>>(newInputs, _output.GetMemoryLayout(), GetCallbackName(), _source);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    utilities::ArchiveVersion SourceNode<ValueType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion sourceNodeShapeArchiveVersion = { utilities::ArchiveVersionNumbers::v4_source_sink_shapes };

        return sourceNodeShapeArchiveVersion;
    }

    template <typename ValueType>
    bool SourceNode<ValueType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        constexpr utilities::ArchiveVersion sourceNodeNoShapeArchiveVersion = { utilities::ArchiveVersionNumbers::v0_initial };
        constexpr utilities::ArchiveVersion sourceNodeShapeArchiveVersion = { utilities::ArchiveVersionNumbers::v4_source_sink_shapes };

        return version >= sourceNodeNoShapeArchiveVersion && version <= sourceNodeShapeArchiveVersion;
    }

    template <typename ValueType>
    void SourceNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[defaultOutputPortName] << _output;
        archiver["sourceFunctionName"] << GetCallbackName();
    }

    template <typename ValueType>
    void SourceNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[defaultOutputPortName] >> _output;

        std::string sourceFunctionName;
        archiver["sourceFunctionName"] >> sourceFunctionName;
        SetCallbackName(sourceFunctionName);
       
        if (archiver.HasNextPropertyName("shape"))
        {
            // legacy support, we no longer need this "shape" property because the
            // _output port now contains it's own serializable MemoryLayout.
            std::vector<int> shapeVector;
            archiver["shape"] >> shapeVector;
            SetShape({ shapeVector });
        }
        _bufferedSample.resize(_output.GetMemoryLayout().NumElements());
    }

    template <typename ValueType>
    void SourceNode<ValueType>::Interpolate(TimeTickType /*originalTime*/, TimeTickType /*newTime*/) const
    {
        // Default to pass-through (derived classes will override).
    }

    template <typename ValueType>
    void SourceNode<ValueType>::SetOutputValuesLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, emitters::LLVMValue sample)
    {
        emitters::LLVMValue pOutput = compiler.EnsurePortEmitted(output);

        auto numValues = output.Size();
        function.For(numValues, [sample, pOutput](emitters::IRFunctionEmitter& function, emitters::LLVMValue i) {
            auto value = function.ValueAt(sample, i);
            function.SetValueAt(pOutput, i, value);
        });
    }

    template <typename ValueType>
    void SourceNode<ValueType>::SetOutputValuesExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, emitters::LLVMValue sample)
    {
        emitters::LLVMValue pOutput = compiler.EnsurePortEmitted(output);

        auto numValues = output.Size();
        for (size_t i = 0; i < numValues; ++i)
        {
            auto value = function.ValueAt(sample, i);
            function.SetValueAt(pOutput, function.Literal(static_cast<int>(i)), value);
        }
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
