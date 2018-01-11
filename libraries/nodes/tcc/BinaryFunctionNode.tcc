////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryFunctionNode.tcc (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType, typename FunctionType>
    BinaryFunctionNode<ValueType, FunctionType>::BinaryFunctionNode()
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, {}, defaultInput1PortName), _input2(this, {}, defaultInput2PortName), _output(this, defaultOutputPortName, 0), _paddingValue(0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BinaryFunctionNode<ValueType, FunctionType>::BinaryFunctionNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2,
        const model::PortMemoryLayout& inputLayout, const model::PortMemoryLayout& outputLayout, FunctionType function, ValueType padding)
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, input1, defaultInput1PortName), _input2(this, input2, defaultInput2PortName), _inputLayout(inputLayout), _output(this, defaultOutputPortName, model::NumElements(outputLayout.GetStride())), _outputLayout(outputLayout), _function(std::move(function)), _paddingValue(padding)
    {
        if (input1.Size() != input2.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input sizes must match");
        }

        if (!model::ShapesEqual(inputLayout.GetActiveSize(), outputLayout.GetActiveSize()))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output active areas must match");
        }
    }

    template <typename ValueType, typename FunctionType>
    void BinaryFunctionNode<ValueType, FunctionType>::Compute() const
    {
        auto outputSize = model::NumElements(_outputLayout.GetStride());
        auto output = std::vector<ValueType>(outputSize);

        const size_t prevInputOffset = 0;
        const size_t prevOutputOffset = 0;
        ComputeDimensionLoop(0, output, prevInputOffset, prevOutputOffset);

        _output.SetOutput(output);
    }

    //
    // Arbitrary-depth nested loops are generated recursively. The ComputeDimensionLoop
    // function emits `numDimensions` nested loops of the form:
    //
    // for(iz = 0; iz < sz; ++iz)
    // {
    //     zOffset = (iz+offset[2]) * stride[2];
    //     for(iy = 0; iy < sy; ++iy)
    //     {
    //         yOffset = zOffset + (iy+offset[1]) * stride[1];
    //         for(ix = 0; ix < sx; ++ix)
    //         {
    //             offset = yOffset + (ix+offset[0]) * stride[0];
    //             x = arr[offset];
    //             val = f(x);
    //             output[offset] = val;
    //         }
    //     }
    // }
    //

    template <typename ValueType, typename FunctionType>
    void BinaryFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop(size_t dimension,
                                                                           std::vector<ValueType>& output,
                                                                           size_t prevInputDimensionOffset,
                                                                           size_t prevOutputDimensionOffset) const
    {
        const auto numDimensions = _inputLayout.NumDimensions();
        auto&& inputStride = _inputLayout.GetStride();
        auto&& inputOffset = _inputLayout.GetOffset();
        auto&& inputSize = _inputLayout.GetActiveSize();
        auto&& outputOffset = _outputLayout.GetOffset();

        for (int loopIndex = 0; loopIndex < inputSize[dimension]; ++loopIndex)
        {
            // offset within start of this dimension = (loopIndex + offset[dimension])
            auto thisInputDimensionInternalOffset = loopIndex + inputOffset[dimension];
            auto thisOutputDimensionInternalOffset = loopIndex + outputOffset[dimension];

            size_t thisInputDimensionOffset = thisInputDimensionInternalOffset;
            size_t thisOutputDimensionOffset = thisOutputDimensionInternalOffset;
            if (dimension != 0)
            {
                thisInputDimensionOffset += prevInputDimensionOffset * inputStride[dimension];
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                ComputeDimensionLoop(dimension + 1, output, thisInputDimensionOffset, thisOutputDimensionOffset);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto value1 = _input1[thisInputDimensionOffset];
                auto value2 = _input2[thisInputDimensionOffset];
                auto outputValue = _function.Compute(value1, value2);
                output[thisOutputDimensionOffset] = outputValue;
            }
        }
    }

    template <typename ValueType, typename FunctionType>
    void BinaryFunctionNode<ValueType, FunctionType>::Compile(model::IRMapCompiler& compiler,
                                                              emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput1 = compiler.EnsurePortEmitted(input1);
        llvm::Value* pInput2 = compiler.EnsurePortEmitted(input2);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output, _paddingValue);

        // Call recursive function to emit nested loops
        llvm::Value* prevInputDimensionOffset = nullptr;
        llvm::Value* prevOutputDimensionOffset = nullptr;
        EmitComputeDimensionLoop(compiler, function, 0, pInput1, pInput2, pResult, prevInputDimensionOffset, prevOutputDimensionOffset);
    }

    template <typename ValueType, typename FunctionType>
    void BinaryFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler,
                                                                               emitters::IRFunctionEmitter& function,
                                                                               size_t dimension,
                                                                               llvm::Value* input1,
                                                                               llvm::Value* input2,
                                                                               llvm::Value* output,
                                                                               llvm::Value* prevInputDimensionOffset,
                                                                               llvm::Value* prevOutputDimensionOffset) const
    {
        const auto numDimensions = _inputLayout.NumDimensions();
        auto&& inputStride = _inputLayout.GetStride();
        auto&& inputOffset = _inputLayout.GetOffset();
        auto&& inputSize = _inputLayout.GetActiveSize();
        auto&& outputStride = _outputLayout.GetStride();
        auto&& outputOffset = _outputLayout.GetOffset();

        auto loop = function.ForLoop();
        loop.Begin(inputSize[dimension]);
        {
            auto loopIndex = loop.LoadIterationVariable();

            // Calculate the offset within this dimension = (loopIndex + offset[dimension])
            llvm::Value* thisInputDimensionInternalOffset = function.Operator(emitters::GetAddForValueType<int>(), loopIndex, function.Literal<int>(inputOffset[dimension]));
            llvm::Value* thisOutputDimensionInternalOffset = function.Operator(emitters::GetAddForValueType<int>(), loopIndex, function.Literal<int>(outputOffset[dimension]));

            // Calculate the total offset from beginning of memory:
            //   * if in the outermost loop, the offset into this dimension
            //   * otherwise, the offset into this dimension plus the previous offset scaled by the previous dimension's stride
            llvm::Value* thisInputDimensionOffset = nullptr;
            llvm::Value* thisOutputDimensionOffset = nullptr;
            if (dimension == 0)
            {
                assert(prevInputDimensionOffset == nullptr);
                assert(prevOutputDimensionOffset == nullptr);
                thisInputDimensionOffset = thisInputDimensionInternalOffset;
                thisOutputDimensionOffset = thisOutputDimensionInternalOffset;
            }
            else
            {
                auto scaledInputDimensionOffset = function.Operator(emitters::GetMultiplyForValueType<int>(), prevInputDimensionOffset, function.Literal<int>(inputStride[dimension]));
                thisInputDimensionOffset = function.Operator(emitters::GetAddForValueType<int>(), scaledInputDimensionOffset, thisInputDimensionInternalOffset);

                auto scaledOutputDimensionOffset = function.Operator(emitters::GetMultiplyForValueType<int>(), prevOutputDimensionOffset, function.Literal<int>(outputStride[dimension]));
                thisOutputDimensionOffset = function.Operator(emitters::GetAddForValueType<int>(), scaledOutputDimensionOffset, thisOutputDimensionInternalOffset);
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                EmitComputeDimensionLoop(compiler, function, dimension + 1, input1, input2, output, thisInputDimensionOffset, thisOutputDimensionOffset);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto value1 = function.ValueAt(input1, thisInputDimensionOffset);
                auto value2 = function.ValueAt(input2, thisInputDimensionOffset);
                auto outputValue = _function.Compile(function, value1, value2);
                function.SetValueAt(output, thisOutputDimensionOffset, outputValue);
            }
        }
        loop.End();
    }

    template <typename ValueType, typename FunctionType>
    void BinaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto portElements1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto portElements2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<BinaryFunctionNode<ValueType, FunctionType>>(portElements1, portElements2, _inputLayout, _outputLayout, _function, _paddingValue);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    void BinaryFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);
        archiver[defaultInput1PortName] << _input1;
        archiver[defaultInput2PortName] << _input2;
        archiver["paddingValue"] << _paddingValue;
    }

    template <typename ValueType, typename FunctionType>
    void BinaryFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInput1PortName] >> _input1;
        archiver[defaultInput2PortName] >> _input2;
        archiver["paddingValue"] >> _paddingValue;
    }
}
}
