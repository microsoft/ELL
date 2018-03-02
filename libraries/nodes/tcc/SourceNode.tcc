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
        : SourceNode({}, math::TensorShape{ 0, 0, 0 }, "", nullptr)
    {
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::PortElements<nodes::TimeTickType>& input, size_t inputVectorSize, const std::string& sourceFunctionName, SourceFunction<ValueType> source)
        : SourceNode(input, math::TensorShape{ inputVectorSize, 1, 1 }, sourceFunctionName, source)
    {
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::PortElements<nodes::TimeTickType>& input, const math::TensorShape& shape, const std::string& sourceFunctionName, SourceFunction<ValueType> source)
        : model::SourceNodeBase(_input, _output, shape, sourceFunctionName),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, shape.Size()),
        _source(source == nullptr ? [](auto&){ return false; } : source)
    {
        _bufferedSample.resize(shape.Size());
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
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        compiler.EnsurePortEmitted(output);
        auto& module = function.GetModule();

        // Globals
        emitters::Variable* pBufferedSampleTimeVar = module.Variables().AddVariable<emitters::InitializedScalarVariable<TimeTickType>>(emitters::VariableScope::global, _bufferedSampleTime);
        emitters::Variable* pBufferedSampleVar = module.Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, output.Size());
        llvm::Value* pBufferedSampleTime = module.EnsureEmitted(*pBufferedSampleTimeVar);
        llvm::Value* pBufferedSample = module.EnsureEmitted(*pBufferedSampleVar);
        llvm::Value* bufferedSampleTime = function.Load(pBufferedSampleTime);
        UNUSED(bufferedSampleTime);

        // Callback function
        const emitters::NamedVariableTypeList parameters = { { "input", emitters::GetPointerType(emitters::GetVariableType<ValueType>()) } };
        std::string prefixedName(compiler.GetNamespacePrefix() + "_" + GetCallbackName());
        module.DeclareFunction(prefixedName, emitters::GetVariableType<bool>(), parameters);
        module.IncludeInCallbackInterface(prefixedName, "SourceNode");

        llvm::Function* pSamplingFunction = module.GetFunction(prefixedName);

        // Locals
        auto sampleTime = function.ValueAt(pInput, function.Literal(0));

        // Invoke the callback and optionally interpolate.
        function.Call(pSamplingFunction, { function.PointerOffset(pBufferedSample, 0) });

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
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
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
        archiver["shape"] << static_cast<std::vector<size_t>>(GetShape());
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

        std::vector<size_t> shapeVector;
        archiver["shape"] >> shapeVector;
        SetShape(math::TensorShape{ shapeVector });
        _output.SetSize(GetShape().Size());
    }

    template <typename ValueType>
    void SourceNode<ValueType>::Interpolate(TimeTickType /*originalTime*/, TimeTickType /*newTime*/) const
    {
        // Default to pass-through (derived classes will override).
    }

    template <typename ValueType>
    void SourceNode<ValueType>::SetOutputValuesLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* sample)
    {
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        auto numValues = output.Size();
        auto forLoop = function.ForLoop();
        forLoop.Begin(numValues);
        {
            auto i = forLoop.LoadIterationVariable();
            auto value = function.ValueAt(sample, i);
            function.SetValueAt(pOutput, i, value);
        }
        forLoop.End();
    }

    template <typename ValueType>
    void SourceNode<ValueType>::SetOutputValuesExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* sample)
    {
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        auto numValues = output.Size();
        for (size_t i = 0; i < numValues; ++i)
        {
            auto value = function.ValueAt(sample, i);
            function.SetValueAt(pOutput, function.Literal(static_cast<int>(i)), value);
        }
    }
}
}