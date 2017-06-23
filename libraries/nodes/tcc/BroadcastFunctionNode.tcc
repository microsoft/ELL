////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BroadcastFunctionNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    //
    // BroadcastLinearFunction
    //
    template <typename ValueType>
    ValueType BroadcastLinearFunction<ValueType>::Compute(ValueType x, ValueType a, ValueType b) const
    {
        return a * x + b;
    }

    template <typename ValueType>
    llvm::Value* BroadcastLinearFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a, llvm::Value* b) const
    {
        if (a == nullptr) // bias only
        {
            return function.Operator(emitters::GetAddForValueType<ValueType>(), x, b);
        }
        else if (b == nullptr) // scale only
        {
            return function.Operator(emitters::GetMultiplyForValueType<ValueType>(), a, x);
        }
        else
        {
            assert(a != nullptr);
            assert(b != nullptr);
            return function.Operator(emitters::GetAddForValueType<ValueType>(), function.Operator(emitters::GetMultiplyForValueType<ValueType>(), a, x), b);
        }
    }

    //
    // BroadcastBinaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode()
        : BroadcastFunctionNode({ &_primaryInput, &_secondaryInput }, { &_output }), _primaryInput(this, {}, primaryInputPortName), _secondaryInput(this, {}, secondaryInputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                                                                      const model::PortElements<ValueType>& secondaryInput, size_t dimension,
                                                                                      const Shape& outputStride, const Shape& outputOffset)
        : BroadcastFunctionNode({ &_primaryInput, &_secondaryInput }, inputStride, inputOffset, inputSize, dimension,
                                { &_output }, outputStride, outputOffset)
        , _primaryInput(this, primaryInput, primaryInputPortName)
        , _secondaryInput(this, secondaryInput, secondaryInputPortName)
        , _output(this, outputPortName, NumElements(outputStride))
    {
        // Verify sizes are compatible
        size_t totalInputSize = std::accumulate(inputStride.begin(), inputStride.end(), 1, std::multiplies<size_t>());

        if (primaryInput.Size() < totalInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Primary input too small");
        }

        if (secondaryInput.Size() != inputSize[dimension])
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Broadcast vector size doesn't match input");
        }
    }

    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                                                                      const model::PortElements<ValueType>& secondaryInput, size_t dimension,
                                                                                      const Shape& outputStride, const Shape& outputOffset,
                                                                                      const FunctionType& function)
        : BroadcastBinaryFunctionNode<ValueType, FunctionType>(primaryInput, inputStride, inputOffset, inputSize,
                                                               secondaryInput, dimension,
                                                               outputStride, outputOffset)
    {
        _function = function;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto primaryInputElements = transformer.TransformPortElements(_primaryInput.GetPortElements());
        auto secondaryInputElements = transformer.TransformPortElements(_secondaryInput.GetPortElements());
        auto newNode = transformer.AddNode<BroadcastBinaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                 this->GetInputStride(),
                                                                                                 this->GetInputOffset(),
                                                                                                 this->GetInputSize(),
                                                                                                 secondaryInputElements,
                                                                                                 this->GetBroadcastDimension(),
                                                                                                 this->GetOutputStride(),
                                                                                                 this->GetOutputOffset(),
                                                                                                 _function);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::Compute() const
    {
        auto outputSize = NumElements(GetOutputStride());
        auto output = std::vector<ValueType>(outputSize);
        size_t primaryInputIndex = 0;
        size_t secondaryInput1Index = 0;
        size_t& secondaryInput2Index = secondaryInput1Index;

        const size_t prevInputOffset = 0;
        const size_t prevOutputOffset = 0;
        const ValueType secondaryValue = 0;
        ComputeDimensionLoop(0, output, prevInputOffset, prevOutputOffset, secondaryValue);

        _output.SetOutput(output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto primaryInputSize = primaryInput.Size();
        auto secondaryInputSize = secondaryInput.Size();

        assert(primaryInputSize % secondaryInputSize == 0);

        llvm::Value* pPrimaryInput = compiler.EnsurePortEmitted(primaryInput);
        llvm::Value* pSecondaryInput = compiler.EnsurePortEmitted(secondaryInput);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        if (model::IsScalar(secondaryInput))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Scalar broadcasting not implemented");
        }
        else
        {
            // Call recursive function to emit nested loops
            // Node: We could just offset the input pointer at beginning instead of adding offset every time through the loop
            // Note: We can potentially fuse adjacent loops if memory is contiguous --- it can be done by preprocessing size/stride vectors
            EmitComputeDimensionLoop(compiler, function, 0, pPrimaryInput, pSecondaryInput, pOutput, nullptr, nullptr, nullptr);
        }
    }

    //
    // Arbitrary-depth nested loops are generated recursively. The EmitComputeDimensionLoop
    // function emits `numDimensions` nested loops of the form:
    //
    // for(iz = 0; iz < sz; ++iz)
    // {
    //     zOffset = (iz+offset[2]) * stride[2];
    //     if(dimension==2) offset2 = ix;
    //     for(iy = 0; iy < sy; ++iy)
    //     {
    //         yOffset = zOffset + (iy+offset[1]) * stride[1];
    //         if(dimension==1) offset2 = iy;
    //         for(ix = 0; ix < sx; ++ix)
    //         {
    //             offset = yOffset + (ix+offset[0]) * stride[0];
    //             if(dimension==0) offset2 = iz;
    //             x = arr[offset];
    //             a = vec[offset2];
    //             val = f(x, a);
    //             output[offset] = val;
    //         }
    //     }
    // }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop(size_t dimension, std::vector<ValueType>& output, size_t prevInputDimensionOffset, size_t prevOutputDimensionOffset, ValueType secondaryValue) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing:  if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous
        auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputStride = GetInputStride();
        auto&& inputOffset = GetInputOffset();
        auto&& inputSize = GetInputSize();
        auto&& outputStride = GetOutputStride();
        auto&& outputOffset = GetOutputOffset();
        auto&& broadcastDimension = GetBroadcastDimension();

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

            if (dimension == broadcastDimension)
            {
                secondaryValue = secondaryInput[loopIndex];
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                ComputeDimensionLoop(dimension + 1, output, thisInputDimensionOffset, thisOutputDimensionOffset, secondaryValue);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto primaryValue = primaryInput[thisInputDimensionOffset];
                auto outputValue = _function.Compute(primaryValue, secondaryValue);
                output[thisOutputDimensionOffset] = outputValue;
            }
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function,
                                                                                        size_t dimension,
                                                                                        llvm::Value* primaryInput, llvm::Value* secondaryInput,
                                                                                        llvm::Value* output,
                                                                                        llvm::Value* prevInputDimensionOffset, llvm::Value* prevOutputDimensionOffset,
                                                                                        llvm::Value* secondaryValue) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing --- if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous

        auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputStride = GetInputStride();
        auto&& inputOffset = GetInputOffset();
        auto&& inputSize = GetInputSize();
        auto&& outputStride = GetOutputStride();
        auto&& outputOffset = GetOutputOffset();
        auto&& broadcastDimension = GetBroadcastDimension();

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

            if (dimension == broadcastDimension)
            {
                secondaryValue = function.ValueAt(secondaryInput, loopIndex);
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                EmitComputeDimensionLoop(compiler, function, dimension + 1, primaryInput, secondaryInput, output, thisInputDimensionOffset, thisOutputDimensionOffset, secondaryValue);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto primaryValue = function.ValueAt(primaryInput, thisInputDimensionOffset);
                auto outputValue = _function.Compile(function, primaryValue, secondaryValue);
                function.SetValueAt(output, thisOutputDimensionOffset, outputValue);
            }
        }
        loop.End();
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[primaryInputPortName] << _primaryInput;
        archiver[secondaryInputPortName] << _secondaryInput;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[primaryInputPortName] >> _primaryInput;
        archiver[secondaryInputPortName] >> _secondaryInput;
        _output.SetSize(_primaryInput.Size());
    }

    //
    // BroadcastTernaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode()
        : BroadcastFunctionNode({ &_primaryInput, &_secondaryInput1, &_secondaryInput2 }, { &_output }), _primaryInput(this, {}, primaryInputPortName), _secondaryInput1(this, {}, secondaryInput1PortName), _secondaryInput2(this, {}, secondaryInput2PortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                                                                        const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t dimension,
                                                                                        const Shape& outputStride, const Shape& outputOffset)
        : BroadcastFunctionNode({ &_primaryInput, &_secondaryInput1, &_secondaryInput2 }, inputStride, inputOffset, inputSize, dimension,
                                { &_output }, outputStride, outputOffset)
        , _primaryInput(this, primaryInput, primaryInputPortName)
        , _secondaryInput1(this, secondaryInput1, secondaryInput1PortName)
        , _secondaryInput2(this, secondaryInput2, secondaryInput2PortName)
        , _output(this, outputPortName, NumElements(outputStride))
    {
        // Verify sizes are compatible
        size_t totalInputSize = std::accumulate(inputStride.begin(), inputStride.end(), 1, std::multiplies<size_t>());

        if (primaryInput.Size() < totalInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Primary input too small");
        }

        if (std::max(secondaryInput1.Size(), secondaryInput2.Size()) != inputSize[dimension])
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Broadcast vector size doesn't match input");
        }

        if (secondaryInput1.Size() != secondaryInput2.Size() && secondaryInput1.Size() > 0 && secondaryInput2.Size() > 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "If present, secondary inputs must have the same size");
        }
    }

    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                                                                        const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t dimension,
                                                                                        const Shape& outputStride, const Shape& outputOffset,
                                                                                        const FunctionType& function)
        : BroadcastTernaryFunctionNode<ValueType, FunctionType>(primaryInput, inputStride, inputOffset, inputSize,
                                                                secondaryInput1, secondaryInput2, dimension,
                                                                outputStride, outputOffset)
    {
        _function = function;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto primaryInputElements = transformer.TransformPortElements(_primaryInput.GetPortElements());
        auto secondaryInput1Elements = transformer.TransformPortElements(_secondaryInput1.GetPortElements());
        auto secondaryInput2Elements = transformer.TransformPortElements(_secondaryInput2.GetPortElements());
        auto newNode = transformer.AddNode<BroadcastTernaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                  this->GetInputStride(),
                                                                                                  this->GetInputOffset(),
                                                                                                  this->GetInputSize(),
                                                                                                  secondaryInput1Elements,
                                                                                                  secondaryInput2Elements,
                                                                                                  this->GetBroadcastDimension(),
                                                                                                  this->GetOutputStride(),
                                                                                                  this->GetOutputOffset(),
                                                                                                  _function);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::Compute() const
    {
        auto outputSize = NumElements(GetOutputStride());
        auto output = std::vector<ValueType>(outputSize);
        size_t primaryInputIndex = 0;
        size_t secondaryInput1Index = 0;
        size_t& secondaryInput2Index = secondaryInput1Index;

        const size_t prevInputOffset = 0;
        const size_t prevOutputOffset = 0;
        const ValueType secondaryValue1 = 0;
        const ValueType secondaryValue2 = 0;
        ComputeDimensionLoop(0, output, prevInputOffset, prevOutputOffset, secondaryValue1, secondaryValue2);

        _output.SetOutput(output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto primaryInputSize = primaryInput.Size();
        auto secondaryInput1Size = secondaryInput1.Size();
        auto secondaryInput2Size = secondaryInput2.Size();
        bool hasInput1 = secondaryInput1.Size() > 0;
        bool hasInput2 = secondaryInput2.Size() > 0;
        // assert(primaryInputSize == _output.Size());

        auto secondaryInputSize = std::max(secondaryInput1Size, secondaryInput2Size);

        assert(primaryInputSize % secondaryInputSize == 0);
        assert(secondaryInput1Size == secondaryInput2Size || !hasInput1 || !hasInput2);
        assert(hasInput1 || hasInput2);

        llvm::Value* pPrimaryInput = compiler.EnsurePortEmitted(primaryInput);
        llvm::Value* pSecondaryInput1 = hasInput1 ? compiler.EnsurePortEmitted(secondaryInput1) : nullptr;
        llvm::Value* pSecondaryInput2 = hasInput2 ? compiler.EnsurePortEmitted(secondaryInput2) : nullptr;
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        if (model::IsScalar(secondaryInput1)) // secondaryInput1 (and therefore 2) are scalars
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Scalar broadcasting not implemented");
        }
        else
        {
            // Call recursive function to emit nested loops
            // Note: We could just offset the input pointer at beginning instead of adding offset every time through the loop
            // Note: We can potentially fuse adjacent loops if memory is contiguous --- it can be done by preprocessing size/stride vectors
            EmitComputeDimensionLoop(compiler, function, 0, pPrimaryInput, pSecondaryInput1, pSecondaryInput2, pOutput, nullptr, nullptr, nullptr, nullptr);
        }
    }

    //
    // Arbitrary-depth nested loops are generated recursively. The EmitComputeDimensionLoop
    // function emits `numDimensions` nested loops of the form:
    //
    // for(iz = 0; iz < sz; ++iz)
    // {
    //     zOffset = (iz+offset[2]) * stride[2];
    //     if(dimension==2) offset2 = ix;
    //     for(iy = 0; iy < sy; ++iy)
    //     {
    //         yOffset = zOffset + (iy+offset[1]) * stride[1];
    //         if(dimension==1) offset2 = iy;
    //         for(ix = 0; ix < sx; ++ix)
    //         {
    //             offset = yOffset + (ix+offset[0]) * stride[0];
    //             if(dimension==0) offset2 = iz;
    //             x = arr[offset];
    //             a = vec[offset2];
    //             val = f(x, a);
    //             output[offset] = val;
    //         }
    //     }
    // }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop(size_t dimension, std::vector<ValueType>& output, size_t prevInputDimensionOffset, size_t prevOutputDimensionOffset, ValueType secondaryValue1, ValueType secondaryValue2) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing:  if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous
        auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputStride = GetInputStride();
        auto&& inputOffset = GetInputOffset();
        auto&& inputSize = GetInputSize();
        auto&& outputStride = GetOutputStride();
        auto&& outputOffset = GetOutputOffset();
        auto&& broadcastDimension = GetBroadcastDimension();

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

            if (dimension == broadcastDimension)
            {
                bool hasInput1 = secondaryInput1.Size() > 0;
                bool hasInput2 = secondaryInput2.Size() > 0;
                if (hasInput1)
                {
                    secondaryValue1 = secondaryInput1[loopIndex];
                }
                if (hasInput2)
                {
                    secondaryValue2 = secondaryInput2[loopIndex];
                }
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                ComputeDimensionLoop(dimension + 1, output, thisInputDimensionOffset, thisOutputDimensionOffset, secondaryValue1, secondaryValue2);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto primaryValue = primaryInput[thisInputDimensionOffset];
                auto outputValue = _function.Compute(primaryValue, secondaryValue1, secondaryValue2);
                output[thisOutputDimensionOffset] = outputValue;
            }
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function,
                                                                                         size_t dimension,
                                                                                         llvm::Value* primaryInput, llvm::Value* secondaryInput1, llvm::Value* secondaryInput2,
                                                                                         llvm::Value* output,
                                                                                         llvm::Value* prevInputDimensionOffset, llvm::Value* prevOutputDimensionOffset,
                                                                                         llvm::Value* secondaryValue1, llvm::Value* secondaryValue2) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing --- if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous

        auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputStride = GetInputStride();
        auto&& inputOffset = GetInputOffset();
        auto&& inputSize = GetInputSize();
        auto&& outputStride = GetOutputStride();
        auto&& outputOffset = GetOutputOffset();
        auto&& broadcastDimension = GetBroadcastDimension();

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

            if (dimension == broadcastDimension)
            {
                secondaryValue1 = secondaryInput1 == nullptr ? nullptr : function.ValueAt(secondaryInput1, loopIndex);
                secondaryValue2 = secondaryInput2 == nullptr ? nullptr : function.ValueAt(secondaryInput2, loopIndex);
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                EmitComputeDimensionLoop(compiler, function, dimension + 1, primaryInput, secondaryInput1, secondaryInput2, output, thisInputDimensionOffset, thisOutputDimensionOffset, secondaryValue1, secondaryValue2);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto primaryValue = function.ValueAt(primaryInput, thisInputDimensionOffset);
                auto outputValue = _function.Compile(function, primaryValue, secondaryValue1, secondaryValue2);
                function.SetValueAt(output, thisOutputDimensionOffset, outputValue);
            }
        }
        loop.End();
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[primaryInputPortName] << _primaryInput;
        archiver[secondaryInput1PortName] << _secondaryInput1;
        archiver[secondaryInput2PortName] << _secondaryInput2;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[primaryInputPortName] >> _primaryInput;
        archiver[secondaryInput1PortName] >> _secondaryInput1;
        archiver[secondaryInput2PortName] >> _secondaryInput2;
        _output.SetSize(_primaryInput.Size());
    }

    //
    // BroadcastLinearFunctionNode
    //
    template <typename ValueType>
    BroadcastLinearFunctionNode<ValueType>::BroadcastLinearFunctionNode()
        : BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>()
    {
    }

    template <typename ValueType>
    BroadcastLinearFunctionNode<ValueType>::BroadcastLinearFunctionNode(const model::PortElements<ValueType>& primaryInput, const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize,
                                                                        const model::PortElements<ValueType>& scaleInput, const model::PortElements<ValueType>& biasInput, size_t dimension,
                                                                        const Shape& outputStride, const Shape& outputOffset)
        : BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>(primaryInput, inputStride, inputOffset, inputSize,
                                                                                      scaleInput, biasInput, dimension,
                                                                                      outputStride, outputOffset)
    {
    }

    template <typename ValueType>
    void BroadcastLinearFunctionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto primaryInputElements = transformer.TransformPortElements(primaryInput.GetPortElements());
        auto secondaryInput1Elements = transformer.TransformPortElements(secondaryInput1.GetPortElements());
        auto secondaryInput2Elements = transformer.TransformPortElements(secondaryInput2.GetPortElements());
        auto newNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(primaryInputElements,
                                                                                   this->GetInputStride(),
                                                                                   this->GetInputOffset(),
                                                                                   this->GetInputSize(),
                                                                                   secondaryInput1Elements,
                                                                                   secondaryInput2Elements,
                                                                                   this->GetBroadcastDimension(),
                                                                                   this->GetOutputStride(),
                                                                                   this->GetOutputOffset());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool BroadcastLinearFunctionNode<ValueType>::IsCompilable() const
    {
        return !CanCombineWithPrimaryInput();
    }

    template <typename ValueType>
    bool BroadcastLinearFunctionNode<ValueType>::HasSimpleConstantSecondaryInputs() const
    {
        // First verify our inputs are compatible
        int secondaryInput1Size = secondaryInput1.Size();
        int secondaryInput2Size = secondaryInput2.Size();

        if (secondaryInput1Size > 0 && secondaryInput2Size > 0 && secondaryInput1Size != secondaryInput2Size)
        {
            return false; // sizes incompatible
        }

        const auto& el1 = secondaryInput1.GetPortElements();
        const auto& el2 = secondaryInput2.GetPortElements();

        if (!el1.IsFullPortOutput() || !el2.IsFullPortOutput())
        {
            return false; // we require all inputs to a port to come from the same place (though we could relax this requirement in the future, perhaps)
        }

        const ConstantNode<ValueType>* secondaryInput1Node = secondaryInput1Size == 0 ? nullptr : dynamic_cast<const ConstantNode<ValueType>*>(el1.GetElement(0).ReferencedPort()->GetNode());
        const ConstantNode<ValueType>* secondaryInput2Node = secondaryInput2Size == 0 ? nullptr : dynamic_cast<const ConstantNode<ValueType>*>(el2.GetElement(0).ReferencedPort()->GetNode());

        if (secondaryInput1Node == nullptr && secondaryInput2Node == nullptr)
        {
            return false; // need at least one secondary input
        }

        return true;
    }

    template <typename ValueType>
    bool BroadcastLinearFunctionNode<ValueType>::CanCombineWithPrimaryInput() const
    {
        // First verify our inputs are constant nodes
        if (!HasSimpleConstantSecondaryInputs())
        {
            return false;
        }

        const auto& el1 = primaryInput.GetPortElements();
        if (!el1.IsFullPortOutput())
        {
            return false; // we require all inputs to a port to come from the same place (though we could relax this requirement in the future, perhaps)
        }

        const BroadcastLinearFunctionNode<ValueType>* primaryInputNode = dynamic_cast<const BroadcastLinearFunctionNode<ValueType>*>(el1.GetElement(0).ReferencedPort()->GetNode());
        if (primaryInputNode == nullptr)
        {
            return false; // primary input must be another linear function
        }

        // Our secondary inputs are OK and the primary input comes from a single lineary function node, now check that its
        // secondary inputs are simple
        if (!primaryInputNode->HasSimpleConstantSecondaryInputs())
        {
            return false;
        }

        // Check the Shapes are compatible
        if (!BroadcastFunctionNode::ShapesEqual(this->GetInputStride(), primaryInputNode->GetInputStride()))
        {
            return false;
        }

        if (!BroadcastFunctionNode::ShapesEqual(this->GetInputOffset(), primaryInputNode->GetInputOffset()))
        {
            return false;
        }

        if (!BroadcastFunctionNode::ShapesEqual(this->GetInputSize(), primaryInputNode->GetInputSize()))
        {
            return false;
        }

        if (!BroadcastFunctionNode::ShapesEqual(this->GetOutputStride(), primaryInputNode->GetOutputStride()))
        {
            return false;
        }

        if (!BroadcastFunctionNode::ShapesEqual(this->GetOutputOffset(), primaryInputNode->GetOutputOffset()))
        {
            return false;
        }

        // Now we just have to check that our secondary input sizes are compatible with the others
        if (this->GetSecondaryInputSize() != primaryInputNode->GetSecondaryInputSize())
        {
            return false;
        }

        return true;
    }

    template <typename ValueType>
    typename BroadcastLinearFunctionNode<ValueType>::LinearCoeffNodes BroadcastLinearFunctionNode<ValueType>::GetConstantSecondaryInputNodes() const
    {
        const auto& el1 = secondaryInput1.GetPortElements();
        const auto& el2 = secondaryInput2.GetPortElements();

        if (!el1.IsFullPortOutput() || !el2.IsFullPortOutput())
        {
            return { nullptr, nullptr }; // we require all inputs to a port to come from the same place (though we could relax this requirement in the future, perhaps)
        }

        int secondaryInput1Size = el1.Size();
        int secondaryInput2Size = el2.Size();
        if (secondaryInput1Size > 0 && secondaryInput2Size > 0 && secondaryInput1Size != secondaryInput2Size)
        {
            return { nullptr, nullptr };
        }

        const ConstantNode<ValueType>* secondaryInput1Node = secondaryInput1Size == 0 ? nullptr : dynamic_cast<const ConstantNode<ValueType>*>(el1.GetElement(0).ReferencedPort()->GetNode());
        const ConstantNode<ValueType>* secondaryInput2Node = secondaryInput2Size == 0 ? nullptr : dynamic_cast<const ConstantNode<ValueType>*>(el2.GetElement(0).ReferencedPort()->GetNode());

        return { secondaryInput1Node, secondaryInput2Node };
    }

    template <typename ValueType>
    void BroadcastLinearFunctionNode<ValueType>::GetCombinedLinearCoeffs(const BroadcastLinearFunctionNode<ValueType>& prevNode, std::vector<ValueType>& scale, std::vector<ValueType>& bias) const
    {
        auto prevSecondaryInputs = prevNode.GetConstantSecondaryInputNodes();
        auto thisSecondaryInputs = GetConstantSecondaryInputNodes();

        if (thisSecondaryInputs.scaleNode == nullptr)
        {
            scale = prevSecondaryInputs.scaleNode->GetValues();
        }
        else if (prevSecondaryInputs.scaleNode == nullptr)
        {
            scale = thisSecondaryInputs.scaleNode->GetValues();
        }
        else
        {
            scale = prevSecondaryInputs.scaleNode->GetValues();
            const auto& thisScaleValues = thisSecondaryInputs.scaleNode->GetValues();
            for (int index = 0; index < scale.size(); ++index)
            {
                scale[index] *= thisScaleValues[index];
            }
        }

        if (prevSecondaryInputs.biasNode == nullptr)
        {
            bias = thisSecondaryInputs.biasNode->GetValues();
        }
        else
        {
            bias = prevSecondaryInputs.biasNode->GetValues();
            if (thisSecondaryInputs.scaleNode != nullptr)
            {
                const auto& thisScaleValues = thisSecondaryInputs.scaleNode->GetValues();
                for (int index = 0; index < bias.size(); ++index)
                {
                    bias[index] *= thisScaleValues[index];
                }
            }
            if (thisSecondaryInputs.biasNode != nullptr)
            {
                const auto& thisBiasValues = thisSecondaryInputs.biasNode->GetValues();
                for (int index = 0; index < bias.size(); ++index)
                {
                    bias[index] += thisBiasValues[index];
                }
            }
        }
    }

    template <typename ValueType>
    bool BroadcastLinearFunctionNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        if (!CanCombineWithPrimaryInput())
        {
            Copy(transformer);
            return false;
        }

        // These are the elements in the new model that correspond to our inputs -- that is, the outputs
        // of the refined version of the linear function node attached to our primaryInput
        auto primaryInputElements = transformer.TransformPortElements(primaryInput.GetPortElements());

        // Now, we just want to get the primaryInput elements of _that_ node
        auto prevNode = dynamic_cast<const BroadcastLinearFunctionNode<ValueType>*>(primaryInputElements.GetElement(0).ReferencedPort()->GetNode());
        assert(prevNode != nullptr);
        auto prevPrimaryInputElements = prevNode->primaryInput.GetPortElements();

        std::vector<ValueType> newScale;
        std::vector<ValueType> newBias;
        GetCombinedLinearCoeffs(*prevNode, newScale, newBias);

        auto scaleValuesNode = transformer.AddNode<ConstantNode<ValueType>>(newScale);
        auto biasValuesNode = transformer.AddNode<ConstantNode<ValueType>>(newBias);
        auto newNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(prevPrimaryInputElements,
                                                                                   this->GetInputOffset(),
                                                                                   this->GetInputSize(),
                                                                                   this->GetInputStride(),
                                                                                   scaleValuesNode->output,
                                                                                   biasValuesNode->output,
                                                                                   this->GetBroadcastDimension(),
                                                                                   this->GetOutputStride(),
                                                                                   this->GetOutputOffset());
        transformer.MapNodeOutput(output, newNode->output);
        return true;
    }

} // nodes
} // ell
