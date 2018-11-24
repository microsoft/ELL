////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SumNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Unused.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    SumNode<ValueType>::SumNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    SumNode<ValueType>::SumNode(const model::OutputPort<ValueType>& input) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    void SumNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            auto v = _input[index];
            result += v;
        }
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void SumNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<SumNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void SumNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (!compiler.GetCompilerOptions().unrollLoops)
        {
            size_t vectorSize = compiler.GetCompilerOptions().vectorWidth;
            bool vectorize = compiler.GetCompilerOptions().allowVectorInstructions && (input.Size() > vectorSize);
            if (vectorize)
            {
                CompileVectorizedLoop(compiler, function);
            }
            else
            {
                CompileLoop(compiler, function);
            }
        }
        else
        {
            CompileExpanded(compiler, function);
        }
    }

    template <typename ValueType>
    void SumNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto input = function.LocalArray(compiler.EnsurePortEmitted(_input));
        auto output = function.LocalArray(compiler.EnsurePortEmitted(_output));

        function.StoreZero(output);

        const int size = _input.Size();
        constexpr int blockSize = 4;
        bool unrollLoop = size > 4 * blockSize; // silly heuristic
        if (unrollLoop)
        {
            const int numBlocks = size / blockSize;
            function.For(numBlocks, [input, output, blockSize](emitters::IRFunctionEmitter& function, auto i) {
                auto blockStart = blockSize * i;
                for (int innerIndex = 0; innerIndex < blockSize; ++innerIndex)
                {
                    emitters::IRLocalScalar value = input[blockStart + innerIndex];
                    function.OperationAndUpdate(output, emitters::GetAddForValueType<ValueType>(), value);
                }
            });

            // epilogue
            const int epilogueSize = size - (blockSize * numBlocks);
            if (epilogueSize > 0)
            {
                function.For(epilogueSize, [input, output](emitters::IRFunctionEmitter& function, auto i) {
                    emitters::IRLocalScalar value = input[i];
                    function.OperationAndUpdate(output, emitters::GetAddForValueType<ValueType>(), value);
                });
            }
        }
        else
        {
            function.For(size, [input, output](emitters::IRFunctionEmitter& function, auto i) {
                emitters::IRLocalScalar value = input[i];
                function.OperationAndUpdate(output, emitters::GetAddForValueType<ValueType>(), value);
            });
        }
    }

    template <typename ValueType>
    void SumNode<ValueType>::CompileVectorizedLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const int size = _input.Size();
        const int vectorSize = compiler.GetCompilerOptions().vectorWidth;
        assert(size >= vectorSize);

        emitters::LLVMValue input = compiler.EnsurePortEmitted(_input);
        emitters::LLVMValue output = compiler.EnsurePortEmitted(_output);

        // Get LLVM types
        auto& emitter = function.GetEmitter();
        auto elementType = emitter.Type(emitters::GetVariableType<ValueType>());
        DEBUG_USED(elementType);
        assert(llvm::VectorType::isValidElementType(elementType) && "Invalid element type for LLVM vector");
        auto vectorType = emitter.VectorType(emitters::GetVariableType<ValueType>(), vectorSize);
        auto vectorPointerType = vectorType->getPointerTo();

        // cast input to pointer-to-vector
        auto inputVector = function.CastPointer(input, vectorPointerType);

        emitters::LLVMValue vectorAccumVar = function.Variable(vectorType, "vecAccum");
        function.Store(vectorAccumVar, emitters::FillVector<ValueType>(function, vectorType, 0));

        const int numBlocks = size / vectorSize;
        function.For(numBlocks, [inputVector, vectorAccumVar](emitters::IRFunctionEmitter& function, auto blockIndex) {
            auto value = function.ValueAt(inputVector, blockIndex);
            function.OperationAndUpdate(vectorAccumVar, emitters::GetAddForValueType<ValueType>(), value);
        });

        // Accumulate horizontal sum into output
        auto sum = emitters::HorizontalVectorSum<ValueType>(function, function.Load(vectorAccumVar));

        // epilogue
        const int epilogueSize = size - (vectorSize * numBlocks);
        if (epilogueSize > 0)
        {
            for (int epilogueIndex = vectorSize * numBlocks; epilogueIndex < size; ++epilogueIndex)
            {
                emitters::LLVMValue pValue = function.ValueAt(input, function.Literal<int>(epilogueIndex));
                sum = function.Operator(emitters::GetAddForValueType<ValueType>(), sum, pValue);
            }
        }
        function.Store(output, sum);
    }

    template <typename ValueType>
    void SumNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        function.StoreZero(pResult);
        for (size_t i = 0; i < input.Size(); ++i)
        {
            auto pValue = compiler.LoadPortElementVariable(input.GetInputElement(i));
            function.OperationAndUpdate(pResult, emitters::GetAddForValueType<ValueType>(), pValue);
        }
    }

    template <typename ValueType>
    void SumNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
    }

    template <typename ValueType>
    void SumNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
    }
} // namespace nodes
} // namespace ell
