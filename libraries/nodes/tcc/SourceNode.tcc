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
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::PortElements<TimeTickType>& input, size_t outputSize, const std::string& sourceFunctionName)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, outputSize),
        _sourceFunctionName(sourceFunctionName), _source([](std::vector<ValueType>&){ return false; })
    {
        _bufferedSample.resize(outputSize);
    }

    template <typename ValueType>
    SourceNode<ValueType>::SourceNode(const model::PortElements<TimeTickType>& input, size_t outputSize, SourceFunction<ValueType> source)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, outputSize), _source(std::move(source))
    {
        _bufferedSample.resize(outputSize);
    }

    template <typename ValueType>
    void SourceNode<ValueType>::Compute() const
    {
        auto sampleTime = _input.GetValue(0);

        if ((sampleTime != _bufferedSampleTime) && _source(_bufferedSample))
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

        // Globals
        emitters::Variable* pBufferedSampleTimeVar = function.GetModule().Variables().AddVariable<emitters::InitializedScalarVariable<TimeTickType>>(emitters::VariableScope::global, _bufferedSampleTime);
        emitters::Variable* pBufferedSampleVar = function.GetModule().Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, output.Size());
        llvm::Value* pBufferedSampleTime = function.GetModule().EnsureEmitted(*pBufferedSampleTimeVar);
        llvm::Value* pBufferedSample = function.GetModule().EnsureEmitted(*pBufferedSampleVar);
        llvm::Value* bufferedSampleTime = function.Load(pBufferedSampleTime);

        // Callback function
        const emitters::VariableTypeList parameters = { emitters::GetPointerType(emitters::GetVariableType<ValueType>()) };
        function.GetModule().DeclareFunction(_sourceFunctionName, emitters::GetVariableType<bool>(), parameters);
        function.GetModule().IncludeInHeader(_sourceFunctionName);
        function.GetModule().IncludeInCallbackInterface(_sourceFunctionName, "SourceNode");

        llvm::Function* pSamplingFunction = function.GetModule().GetFunction(_sourceFunctionName);

        // Locals
        auto sampleTime = function.ValueAt(pInput, function.Literal(0));
        auto currentTime = function.ValueAt(pInput, function.Literal(1));

        // If the requested sample time is different from cached, invoke the callback and optionally interpolate.
        auto if1 = function.If(emitters::GetComparison<TimeTickType>(emitters::BinaryPredicateType::notEqual), sampleTime, bufferedSampleTime);
        {
            DEBUG_EMIT_PRINTF(function, _sourceFunctionName + "\n");

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

        // Set sample values to the output
        if (!IsScalar(output) && !compiler.GetCompilerParameters().unrollLoops)
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
        auto newNode = transformer.AddNode<SourceNode<ValueType>>(newPortElements, output.Size(), _sourceFunctionName);
        newNode->_source = _source;
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void SourceNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
        archiver["sourceFunctionName"] << _sourceFunctionName;
    }

    template <typename ValueType>
    void SourceNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
        archiver["sourceFunctionName"] >> _sourceFunctionName;
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
        forLoop.Begin(0, numValues, 1);
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