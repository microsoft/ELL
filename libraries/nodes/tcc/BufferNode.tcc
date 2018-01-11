////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BufferNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    BufferNode<ValueType>::BufferNode(const model::PortElements<ValueType>& input, size_t windowSize)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, windowSize), _windowSize(windowSize)
    {
        _samples.resize(windowSize);
    }

    template <typename ValueType>
    BufferNode<ValueType>::BufferNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0), _windowSize(0)
    {
    }

    template <typename ValueType>
    void BufferNode<ValueType>::Compute() const
    {
        auto inputSize = input.Size();
        auto offset = _samples.size() - inputSize;

        // Copy samples forward to make room for new samples
        std::copy_n(_samples.begin() + offset, inputSize, _samples.begin());

        // Copy input samples to tail
        for (size_t index = 0; index < inputSize; ++index)
        {
            _samples[index + offset] = _input[index];
        }
        _output.SetOutput(_samples);
    };

    template <typename ValueType>
    void BufferNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<BufferNode<ValueType>>(newPortElements, _windowSize);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void BufferNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        int inputSize = input.Size();
        size_t windowSize = this->GetWindowSize();
        auto offset = windowSize - inputSize;

        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        auto bufferVar = function.GetModule().Variables().AddVectorVariable<ValueType>(emitters::VariableScope::global, windowSize);
        function.GetModule().AllocateVariable(*bufferVar);
        llvm::Value* buffer = function.GetModule().EnsureEmitted(*bufferVar);

        // Copy samples forward to make room for new samples
        function.MemoryMove<ValueType>(buffer, offset, 0, inputSize);

        // Copy input samples to tail
        function.MemoryCopy<ValueType>(pInput, 0, buffer, offset, inputSize);

        // Copy to output
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);
        function.MemoryCopy<ValueType>(buffer, 0, pOutput, 0, windowSize);
    }

    template <typename ValueType>
    void BufferNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["windowSize"] << _windowSize;
    }

    template <typename ValueType>
    void BufferNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["windowSize"] >> _windowSize;

        _samples.resize(_windowSize);
        _samples.clear();
        _output.SetSize(_windowSize);
    }
}
}
