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
    // Default constructor for type registration
    template <typename ValueType, SamplingFunction<ValueType> getSample>
    SourceNode<ValueType, getSample>::SourceNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0), _samplingFunctionName("")
    {
    }

    template <typename ValueType, SamplingFunction<ValueType> getSample>
    SourceNode<ValueType, getSample>::SourceNode(
        const model::PortElements<TimeTickType>& input, size_t outputSize)
        : SourceNode(input, outputSize, "SourceNode_SamplingFunction")
    {
    }

    template <typename ValueType, SamplingFunction<ValueType> getSample>
    SourceNode<ValueType, getSample>::SourceNode(
        const model::PortElements<TimeTickType>& input, size_t outputSize, const std::string& samplingFunctionName)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, outputSize), _samplingFunctionName(samplingFunctionName)
    {
    }

    template <typename ValueType, SamplingFunction<ValueType> getSample>
    void SourceNode<ValueType, getSample>::Compute() const
    {
        auto sampleTime = _input.GetValue(0);

        if ((sampleTime != _bufferedSampleTime) && getSample(_bufferedSample))
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

    template <typename ValueType, SamplingFunction<ValueType> getSample>
    void SourceNode<ValueType, getSample>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        // Globals
        emitters::Variable* pBufferedSampleTimeVar = function.GetModule().Variables().AddVariable<emitters::InitializedScalarVariable<TimeTickType>>(emitters::VariableScope::global, _bufferedSampleTime);
        emitters::Variable* pBufferedSampleVar = function.GetModule().Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, output.Size());
        llvm::Value* pBufferedSampleTime = function.GetModule().EnsureEmitted(*pBufferedSampleTimeVar);
        llvm::Value* pBufferedSample = function.GetModule().EnsureEmitted(*pBufferedSampleVar);
        llvm::Value* bufferedSampleTime = function.Load(pBufferedSampleTime);

        // Callback function
        const emitters::ValueTypeList parameters = { emitters::GetPointerType(emitters::GetVariableType<ValueType>()) };
        function.GetModule().DeclareFunction(_samplingFunctionName, emitters::GetVariableType<bool>(), parameters);
        llvm::Function* pSamplingFunction = function.GetModule().GetFunction(_samplingFunctionName);

        // Locals
        auto sampleTime = function.ValueAt(pInput, function.Literal(0));
        auto currentTime = function.ValueAt(pInput, function.Literal(1));

        // If the requested sample time is different from cached, invoke the callback and optionally interpolate.
        auto if1 = function.If(emitters::GetComparison<TimeTickType>(emitters::BinaryPredicateType::notEqual), sampleTime, bufferedSampleTime);
        {
            auto result = function.Call(pSamplingFunction, { function.PointerOffset(pBufferedSample, 0) });
            auto if2 = function.If(emitters::TypedComparison::equals, result, function.Literal(true));
            {
                auto if3 = function.If(emitters::GetComparison<TimeTickType>(emitters::BinaryPredicateType::greater), currentTime, sampleTime);
                {
                    // Interpolate(diff, _bufferedSample);
                }
                if3.End();
            }
            if2.End();
        }
        if1.End();

        // EFFICIENCY can we avoid looping over each sample?
        for (size_t i = 0; i < output.Size(); ++i)
        {
            auto sample = function.ValueAt(pBufferedSample, i);
            function.SetValueAt(pResult, function.Literal((int)i), sample);
        }
        function.Store(pBufferedSampleTime, sampleTime);
    }

    template <typename ValueType, SamplingFunction<ValueType> getSample>
    void SourceNode<ValueType, getSample>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SourceNode<ValueType, getSample>>(newPortElements, output.Size(), _samplingFunctionName);

        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, SamplingFunction<ValueType> getSample>
    void SourceNode<ValueType, getSample>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
        archiver["outputSize"] << _output.Size();
        archiver["samplingFunctionName"] << _samplingFunctionName;
    }

    template <typename ValueType, SamplingFunction<ValueType> getSample>
    void SourceNode<ValueType, getSample>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
        size_t size;
        archiver["outputSize"] >> size;
        _output.SetSize(size);
        archiver["samplingFunctionName"] >> _samplingFunctionName;
    }

    template <typename ValueType, SamplingFunction<ValueType> getSample>
    void SourceNode<ValueType, getSample>::Interpolate(TimeTickType /*originalTime*/, TimeTickType /*newTime*/) const
    {
        // Default to pass-through (derived classes will override).
    }
}
}