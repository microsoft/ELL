////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AccumulatorNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    AccumulatorNode<ValueType>::AccumulatorNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    AccumulatorNode<ValueType>::AccumulatorNode(const model::PortElements<ValueType>& input)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size())
    {
        auto dimension = input.Size();
        _accumulator = std::vector<ValueType>(dimension);
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::Compute() const
    {
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            _accumulator[index] += _input[index];
        }
        _output.SetOutput(_accumulator);
    };

    template <typename ValueType>
    void AccumulatorNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<AccumulatorNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::Compile(model::IRMapCompiler& compiler)
    {
        static_assert(!std::is_same<ValueType, bool>(), "Cannot instantiate boolean accumulator nodes");
        compiler.NewBlockRegion(*this);

        // AccumulatorNode has exactly 1 input and 1 output
        // Accumulators are always long lived - either globals or heap. Currently, we use globals
        auto inputPort = GetInputPorts()[0];
        auto outputPort = GetOutputPorts()[0];
        assert(GetPortVariableType(*inputPort) == GetPortVariableType(*outputPort));

        emitters::Variable* pAccumulatorVar = compiler.Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, outputPort->Size());
        llvm::Value* accumulator = compiler.EnsureEmitted(*pAccumulatorVar);

        if (model::IsPureVector(*inputPort) && !compiler.GetCompilerParameters().unrollLoops)
        {
            CompileAccumulatorLoop(compiler, accumulator);
        }
        else
        {
            CompileAccumulatorExpanded(compiler, accumulator);
        }

        compiler.TryMergeRegion(*this);
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::CompileAccumulatorLoop(model::IRMapCompiler& compiler, llvm::Value* accumulator)
    {
        auto inputPort = this->GetInputPorts()[0];
        auto outputPort = this->GetOutputPorts()[0];
        llvm::Value* result = compiler.EnsureEmitted(outputPort);
        llvm::Value* inputVector = compiler.EnsureEmitted(inputPort);
        auto& function = compiler.GetCurrentFunction();

        function.VectorOperator(emitters::GetAddForValueType<ValueType>(), outputPort->Size(), accumulator, inputVector, [&accumulator, &result, &function, this](llvm::Value* i, llvm::Value* value) {
            function.SetValueAt(accumulator, i, value);
            function.SetValueAt(result, i, value);
        });
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::CompileAccumulatorExpanded(model::IRMapCompiler& compiler, llvm::Value* accumulator)
    {
        auto inputPort = GetInputPorts()[0];
        auto outputPort = GetOutputPorts()[0];
        llvm::Value* result = compiler.EnsureEmitted(outputPort);
        auto& function = compiler.GetCurrentFunction();
        for (size_t index = 0; index < outputPort->Size(); ++index)
        {
            llvm::Value* inputValue = compiler.LoadVariable(inputPort->GetInputElement(index));
            llvm::Value* accumValue = function.ValueAt(accumulator, function.Literal((int)index));
            llvm::Value* sum = function.Operator(emitters::GetAddForValueType<ValueType>(), inputValue, accumValue);
            function.SetValueAt(accumulator, function.Literal((int)index), sum);
            function.SetValueAt(result, function.Literal((int)index), sum);
        }
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;

        auto dimension = _input.Size();
        _accumulator = std::vector<ValueType>(dimension);
    }
}
}
