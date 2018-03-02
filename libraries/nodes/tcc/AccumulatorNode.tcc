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
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    AccumulatorNode<ValueType>::AccumulatorNode(const model::PortElements<ValueType>& input)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, _input.Size())
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
    void AccumulatorNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        static_assert(!std::is_same<ValueType, bool>(), "Cannot instantiate boolean accumulator nodes");
        assert(GetPortVariableType(input) == GetPortVariableType(output));

        // Allocate a global variable to accumulate the input
        emitters::Variable* pAccumulatorVar = function.GetModule().Variables().AddVariable<emitters::InitializedVectorVariable<ValueType>>(emitters::VariableScope::global, output.Size());
        llvm::Value* accumulator = function.GetModule().EnsureEmitted(*pAccumulatorVar);

        if (model::IsPureVector(input) && !compiler.GetCompilerOptions().unrollLoops)
        {
            CompileLoop(compiler, function, accumulator);
        }
        else
        {
            CompileExpanded(compiler, function, accumulator);
        }
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* accumulator)
    {
        llvm::Value* inputVector = compiler.EnsurePortEmitted(input);
        llvm::Value* result = compiler.EnsurePortEmitted(output);

        function.VectorOperator(emitters::GetAddForValueType<ValueType>(), output.Size(), accumulator, inputVector, [&accumulator, &result, &function](llvm::Value* i, llvm::Value* value) {
            function.SetValueAt(accumulator, i, value);
            function.SetValueAt(result, i, value);
        });
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* accumulator)
    {
        llvm::Value* result = compiler.EnsurePortEmitted(output);
        for (size_t index = 0; index < output.Size(); ++index)
        {
            llvm::Value* inputValue = compiler.LoadPortElementVariable(input.GetInputElement(index));
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
        archiver[defaultInputPortName] << _input;
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;

        auto dimension = _input.Size();
        _accumulator = std::vector<ValueType>(dimension);
        _output.SetSize(dimension);
    }
}
}
