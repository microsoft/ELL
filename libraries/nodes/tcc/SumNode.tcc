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
    SumNode<ValueType>::SumNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    SumNode<ValueType>::SumNode(const model::PortElements<ValueType>& input)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, 1)
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
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SumNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void SumNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (IsPureVector(input) && !compiler.GetCompilerOptions().unrollLoops)
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
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        function.StoreZero(pOutput);

        const int size = input.Size();
        const int blockSize = 4;
        bool unrollLoop = size > 4 * blockSize; // silly heuristic
        if (unrollLoop)
        {
            const int numBlocks = size / blockSize;
            auto outerLoop = function.ForLoop();
            outerLoop.Begin(numBlocks);
            {
                auto i = outerLoop.LoadIterationVariable();
                auto blockStart = function.Operator(emitters::GetMultiplyForValueType<int>(), function.Literal<int>(blockSize), i);
                for (int innerIndex = 0; innerIndex < blockSize; ++innerIndex)
                {
                    llvm::Value* pValue = function.ValueAt(pInput, function.Operator(emitters::GetAddForValueType<int>(), blockStart, function.Literal<int>(innerIndex)));
                    function.OperationAndUpdate(pOutput, emitters::GetAddForValueType<ValueType>(), pValue);
                }
            }
            outerLoop.End();

            // epilogue
            const int epilogueSize = size - (blockSize * numBlocks);
            if (epilogueSize > 0)
            {
                auto epilogueLoop = function.ForLoop();
                epilogueLoop.Begin(size * numBlocks, size, 1);
                {
                    auto i = epilogueLoop.LoadIterationVariable();
                    llvm::Value* pValue = function.ValueAt(pInput, i);
                    function.OperationAndUpdate(pOutput, emitters::GetAddForValueType<ValueType>(), pValue);
                }
                epilogueLoop.End();
            }
        }
        else
        {
            auto forLoop = function.ForLoop();
            forLoop.Begin(size);
            {
                auto i = forLoop.LoadIterationVariable();
                llvm::Value* pValue = function.ValueAt(pInput, i);
                function.OperationAndUpdate(pOutput, emitters::GetAddForValueType<ValueType>(), pValue);
            }
            forLoop.End();
        }
    }

    template <typename ValueType>
    void SumNode<ValueType>::CompileVectorizedLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const int size = input.Size();
        const int vectorSize = compiler.GetCompilerOptions().vectorWidth;
        assert(size >= vectorSize);

        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        // Get LLVM types
        auto& emitter = function.GetEmitter();
        auto elementType = emitter.Type(emitters::GetVariableType<ValueType>());
        DEBUG_USED(elementType);
        assert(llvm::VectorType::isValidElementType(elementType) && "Invalid element type for LLVM vector");
        auto vectorType = emitter.VectorType(emitters::GetVariableType<ValueType>(), vectorSize);
        auto vectorPointerType = vectorType->getPointerTo();

        // cast pInput to pointer-to-vector
        auto inputVector = function.CastPointer(pInput, vectorPointerType);

        llvm::Value* vectorAccumVar = function.Variable(vectorType, "vecAccum");
        function.Store(vectorAccumVar, emitters::FillVector<ValueType>(function, vectorType, 0));

        const int numBlocks = size / vectorSize;
        auto outerLoop = function.ForLoop();
        outerLoop.Begin(numBlocks);
        {
            auto blockIndex = outerLoop.LoadIterationVariable();
            llvm::Value* pValue = function.ValueAt(inputVector, blockIndex);
            function.OperationAndUpdate(vectorAccumVar, emitters::GetAddForValueType<ValueType>(), pValue);
        }
        outerLoop.End();

        // Accumulate horizontal sum into output
        auto sum = emitters::HorizontalVectorSum<ValueType>(function, function.Load(vectorAccumVar));

        // epilogue
        const int epilogueSize = size - (vectorSize * numBlocks);
        if (epilogueSize > 0)
        {
            for(int epilogueIndex = vectorSize * numBlocks; epilogueIndex < size; ++epilogueIndex)
            {
                llvm::Value* pValue = function.ValueAt(pInput, function.Literal<int>(epilogueIndex));
                sum = function.Operator(emitters::GetAddForValueType<ValueType>(), sum, pValue);
            }
        }
        function.Store(pOutput, sum);
    }

    template <typename ValueType>
    void SumNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        function.StoreZero(pResult);
        for (size_t i = 0; i < input.Size(); ++i)
        {
            llvm::Value* pValue = compiler.LoadPortElementVariable(input.GetInputElement(i));
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
}
}
