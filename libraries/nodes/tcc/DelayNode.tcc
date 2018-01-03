////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DelayNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    DelayNode<ValueType>::DelayNode(const model::PortElements<ValueType>& input, size_t windowSize)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = input.Size();
        for (size_t index = 0; index < windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
    }

    template <typename ValueType>
    DelayNode<ValueType>::DelayNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0), _windowSize(0)
    {
    }

    template <typename ValueType>
    void DelayNode<ValueType>::Compute() const
    {
        auto lastBufferedSample = _samples[0];
        _samples.push_back(_input.GetValue());
        _samples.erase(_samples.begin());
        _output.SetOutput(lastBufferedSample);
    };

    template <typename ValueType>
    void DelayNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<DelayNode<ValueType>>(newPortElements, _windowSize);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void DelayNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* result = compiler.EnsurePortEmitted(output);

        size_t sampleSize = output.Size();
        size_t windowSize = this->GetWindowSize();
        size_t bufferSize = sampleSize * windowSize;

        //
        // Delay nodes are always long lived - either globals or heap. Currently, we use globals
        // Each sample chunk is of size == sampleSize. The number of chunks we hold onto == windowSize
        // We need two buffers - one for the entire lot, one for the "last" chunk forwarded to the next operator
        //
        emitters::Variable* delayLineVar = function.GetModule().Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, bufferSize);
        llvm::Value* delayLine = function.GetModule().EnsureEmitted(*delayLineVar);

        //
        // We implement a delay as a Shift Register
        //
        llvm::Value* inputBuffer = compiler.EnsurePortEmitted(input);
        function.ShiftAndUpdate<ValueType>(delayLine, bufferSize, sampleSize, inputBuffer, result);
    }

    template <typename ValueType>
    void DelayNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["windowSize"] << _windowSize;
    }

    template <typename ValueType>
    void DelayNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["windowSize"] >> _windowSize;

        auto dimension = _input.Size();
        _samples.clear();
        _samples.reserve(_windowSize);
        for (size_t index = 0; index < _windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _output.SetSize(dimension);
    }
}
}
