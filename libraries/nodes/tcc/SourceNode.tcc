////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SourceNode.tcc (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    SourceNode<ValueType>::SourceNode()
        : SourceNode({}, model::MemoryShape{ 0 }, "", nullptr)
    {
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::OutputPort<nodes::TimeTickType>& input, size_t inputVectorSize, const std::string& sourceFunctionName, SourceFunction<ValueType> source)
        : SourceNode(input, model::MemoryShape{ static_cast<int>(inputVectorSize) }, sourceFunctionName, source)
    {
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::OutputPort<nodes::TimeTickType>& input, const model::MemoryShape& shape, const std::string& sourceFunctionName, SourceFunction<ValueType> source)
        : model::SourceNodeBase(_input, _output, sourceFunctionName),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, shape),
        _source(source == nullptr ? [](auto&){ return false; } : source)
    {
        _bufferedSample.resize(shape.NumElements());
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::OutputPort<nodes::TimeTickType>& input, const model::PortMemoryLayout& layout, const std::string& sourceFunctionName, SourceFunction<ValueType> source)
        : model::SourceNodeBase(_input, _output, sourceFunctionName),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, layout),
        _source(source == nullptr ? [](auto&){ return false; } : source)
    {
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

        // Globals
        emitters::Variable* pBufferedSampleTimeVar = module.Variables().AddVariable<emitters::InitializedScalarVariable<TimeTickType>>(emitters::VariableScope::global, _bufferedSampleTime);
        emitters::Variable* pBufferedSampleVar = module.Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, output.Size());
        emitters::LLVMValue pBufferedSampleTime = module.EnsureEmitted(*pBufferedSampleTimeVar);
        emitters::LLVMValue pBufferedSample = module.EnsureEmitted(*pBufferedSampleVar);
        emitters::LLVMValue bufferedSampleTime = function.Load(pBufferedSampleTime);
        UNUSED(bufferedSampleTime);

        // Callback function
        const emitters::NamedVariableTypeList parameters = { { "context", emitters::VariableType::BytePointer },
                                                             { "input", emitters::GetPointerType(emitters::GetVariableType<ValueType>()) } };
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
        if (!IsScalar(output) && !compiler.GetCompilerOptions().unrollLoops)
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
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<SourceNode<ValueType>>(newPortElements, GetShape(), GetCallbackName(), _source);
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
        archiver["shape"] << GetShape().ToVector();
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

        std::vector<int> shapeVector;
        archiver["shape"] >> shapeVector;
        SetShape({ shapeVector });
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
}
}