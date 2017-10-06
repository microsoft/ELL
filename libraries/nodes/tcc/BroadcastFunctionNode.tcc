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
    // BroadcastUnaryFunction
    //
    template <typename ValueType>
    ValueType BroadcastUnaryFunction<ValueType>::Compute(ValueType x, const std::vector<ValueType>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 0);
        return Compute(x);
    }

    template <typename ValueType>
    llvm::Value* BroadcastUnaryFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, const std::vector<llvm::Value*>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 0);
        return this->Compile(function, x);
    }

    //
    // BroadcastBinaryFunction
    //
    template <typename ValueType>
    ValueType BroadcastBinaryFunction<ValueType>::Compute(ValueType x, const std::vector<ValueType>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 1);
        return Compute(x, secondaryArgs[0]);
    }

    template <typename ValueType>
    llvm::Value* BroadcastBinaryFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, const std::vector<llvm::Value*>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 1);
        return this->Compile(function, x, secondaryArgs[0]);
    }

    //
    // BroadcastTernaryFunction
    //
    template <typename ValueType>
    ValueType BroadcastTernaryFunction<ValueType>::Compute(ValueType x, const std::vector<ValueType>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 2);
        return Compute(x, secondaryArgs[0], secondaryArgs[1]);
    }

    template <typename ValueType>
    llvm::Value* BroadcastTernaryFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, const std::vector<llvm::Value*>& secondaryArgs) const
    {
        assert(secondaryArgs.size() == 2);
        return this->Compile(function, x, secondaryArgs[0], secondaryArgs[1]);
    }

    //
    // BroadcastLinearFunction
    //
    template <typename ValueType>
    ValueType BroadcastLinearFunction<ValueType>::Compute(ValueType x, ValueType scale, ValueType bias) const
    {
        return scale * x + bias;
    }

    template <typename ValueType>
    llvm::Value* BroadcastLinearFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* scale, llvm::Value* bias) const
    {
        if (scale == nullptr) // bias only
        {
            return function.Operator(emitters::GetAddForValueType<ValueType>(), x, bias);
        }
        else if (bias == nullptr) // scale only
        {
            return function.Operator(emitters::GetMultiplyForValueType<ValueType>(), scale, x);
        }
        else
        {
            return function.Operator(emitters::GetAddForValueType<ValueType>(), function.Operator(emitters::GetMultiplyForValueType<ValueType>(), scale, x), bias);
        }
    }

    //
    // BroadcastFunctionNode
    //

    template <typename ValueType, typename FunctionType>
    BroadcastFunctionNode<ValueType, FunctionType>::BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs, const std::vector<model::OutputPortBase*>& outputs)
        : CompilableNode(inputs, outputs), _paddingValue(0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastFunctionNode<ValueType, FunctionType>::BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs,
                                                                          const model::PortMemoryLayout& inputLayout, size_t broadcastDimension,
                                                                          const std::vector<model::OutputPortBase*>& outputs,
                                                                          const model::PortMemoryLayout& outputLayout,
                                                                          FunctionType function,
                                                                          ValueType paddingValue)
        : CompilableNode(inputs, outputs), _inputLayout(inputLayout), _outputLayout(outputLayout), _broadcastDimension(broadcastDimension), _function(function), _paddingValue(paddingValue)
    {
    }

    //
    // Arbitrary-depth nested loops are generated recursively. The EmitComputeDimensionLoop
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

    // Note: secondaryValues is passed by non-const reference to avoid copies. It doesn't function as an output parameter.
    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::ComputeDimensionLoop(size_t dimension, std::vector<ValueType>& output, size_t prevInputDimensionOffset, size_t prevOutputDimensionOffset, std::vector<ValueType>& secondaryValues) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing:  if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous
        const auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputLayout = GetInputLayout();
        auto&& inputStride = inputLayout.GetStride();
        auto&& inputOffset = inputLayout.GetOffset();
        auto&& inputSize = inputLayout.GetActiveSize();
        auto&& outputLayout = GetOutputLayout();
        auto&& outputStride = outputLayout.GetStride();
        auto&& outputOffset = outputLayout.GetOffset();
        auto&& primaryInput = GetPrimaryInput();
        const auto broadcastDimension = GetBroadcastDimension();
        const auto numSecondaryInputs = NumSecondaryInputs();

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
                for (int index = 0; index < numSecondaryInputs; ++index)
                {
                    auto&& secondaryInput = GetSecondaryInput(index);
                    if (secondaryInput->Size() > 0) // input is present
                    {
                        secondaryValues[index] = (*secondaryInput)[loopIndex];
                    }
                }
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                ComputeDimensionLoop(dimension + 1, output, thisInputDimensionOffset, thisOutputDimensionOffset, secondaryValues);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto primaryValue = primaryInput[thisInputDimensionOffset];
                auto outputValue = GetFunction().Compute(primaryValue, secondaryValues);
                output[thisOutputDimensionOffset] = outputValue;
            }
        }
    }

    // Note: secondaryValues is passed by non-const reference to avoid copies. It doesn't function as an output parameter.
    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function,
                                                                                  size_t dimension,
                                                                                  llvm::Value* primaryInput, const std::vector<llvm::Value*>& secondaryInputs,
                                                                                  llvm::Value* output,
                                                                                  llvm::Value* prevInputDimensionOffset, llvm::Value* prevOutputDimensionOffset,
                                                                                  std::vector<llvm::Value*>& secondaryValues) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing --- if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous

        const auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputLayout = GetInputLayout();
        auto&& inputStride = inputLayout.GetStride();
        auto&& inputOffset = inputLayout.GetOffset();
        auto&& inputSize = inputLayout.GetActiveSize();
        auto&& outputLayout = GetOutputLayout();
        auto&& outputStride = outputLayout.GetStride();
        auto&& outputOffset = outputLayout.GetOffset();
        const auto broadcastDimension = GetBroadcastDimension();
        const auto numSecondaryInputs = NumSecondaryInputs();
        const auto secondaryInputSize = GetSecondaryInputSize();

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
                for (int index = 0; index < numSecondaryInputs; ++index)
                {
                    auto&& secondaryInput = secondaryInputs[index];
                    if (secondaryInputSize == 1) // scalar
                    {
                        secondaryValues[index] = secondaryInput;
                    }
                    else
                    {
                        secondaryValues[index] = secondaryInput == nullptr ? nullptr : function.ValueAt(secondaryInput, loopIndex);
                    }
                }
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                EmitComputeDimensionLoop(compiler, function, dimension + 1, primaryInput, secondaryInputs, output, thisInputDimensionOffset, thisOutputDimensionOffset, secondaryValues);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto primaryValue = function.ValueAt(primaryInput, thisInputDimensionOffset);
                auto outputValue = GetFunction().Compile(function, primaryValue, secondaryValues);
                function.SetValueAt(output, thisOutputDimensionOffset, outputValue);
            }
        }
        loop.End();
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);
        archiver["paddingValue"] << _paddingValue;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);
        archiver["paddingValue"] >> _paddingValue;
    }

    //
    // BroadcastUnaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastUnaryFunctionNode<ValueType, FunctionType>::BroadcastUnaryFunctionNode()
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput }, { &_output }), _primaryInput(this, {}, primaryInputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastUnaryFunctionNode<ValueType, FunctionType>::BroadcastUnaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                    const model::PortMemoryLayout& outputLayout,
                                                                                    ValueType paddingValue)
        : BroadcastUnaryFunctionNode<ValueType, FunctionType>(primaryInput, inputLayout, outputLayout, FunctionType{}, paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastUnaryFunctionNode<ValueType, FunctionType>::BroadcastUnaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                    const model::PortMemoryLayout& outputLayout,
                                                                                    FunctionType function,
                                                                                    ValueType paddingValue)
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput }, inputLayout, 0, { &_output }, outputLayout, function, paddingValue)
        , _primaryInput(this, primaryInput, primaryInputPortName)
        , _output(this, outputPortName, model::NumElements(outputLayout.GetStride()))
    {
        // Verify sizes are compatible
        size_t totalInputSize = inputLayout.GetMemorySize();
        if (primaryInput.Size() < totalInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Primary input too small");
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastUnaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto primaryInputElements = transformer.TransformPortElements(_primaryInput.GetPortElements());
        auto broadcastFunction = GetFunction();
        auto newNode = transformer.AddNode<BroadcastUnaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                this->GetInputLayout(),
                                                                                                this->GetOutputLayout(),
                                                                                                broadcastFunction);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastUnaryFunctionNode<ValueType, FunctionType>::Compute() const
    {
        auto outputSize = model::NumElements(GetOutputLayout().GetStride());
        auto output = std::vector<ValueType>(outputSize);
        size_t primaryInputIndex = 0;

        const size_t prevInputOffset = 0;
        const size_t prevOutputOffset = 0;
        std::vector<ValueType> secondaryValues{};
        ComputeDimensionLoop(0, output, prevInputOffset, prevOutputOffset, secondaryValues);

        _output.SetOutput(output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastUnaryFunctionNode<ValueType, FunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto primaryInputSize = primaryInput.Size();

        llvm::Value* pPrimaryInput = compiler.EnsurePortEmitted(primaryInput);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output, this->GetOutputPadding());

        // Call recursive function to emit nested loops
        // Note: We could just offset the input pointer at beginning instead of adding offset every time through the loop
        // Note: We can potentially fuse adjacent loops if memory is contiguous --- it can be done by preprocessing size/stride vectors
        llvm::Value* prevInputDimensionOffset = nullptr;
        llvm::Value* prevOutputDimensionOffset = nullptr;
        std::vector<llvm::Value*> secondaryValues{};
        EmitComputeDimensionLoop(compiler, function, 0, pPrimaryInput, {}, pOutput, prevInputDimensionOffset, prevOutputDimensionOffset, secondaryValues);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastUnaryFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BroadcastFunctionNode<ValueType, FunctionType>::WriteToArchive(archiver);
        archiver[primaryInputPortName] << _primaryInput;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastUnaryFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BroadcastFunctionNode<ValueType, FunctionType>::ReadFromArchive(archiver);
        archiver[primaryInputPortName] >> _primaryInput;
        _output.SetSize(_primaryInput.Size()); // ???
    }

    template <typename ValueType, typename FunctionType>
    const model::InputPort<ValueType>* BroadcastUnaryFunctionNode<ValueType, FunctionType>::GetSecondaryInput(int index) const
    {
        assert(index == 0);
        return nullptr;
    }

    //
    // BroadcastBinaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode()
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput }, { &_output }), _primaryInput(this, {}, primaryInputPortName), _secondaryInput(this, {}, secondaryInputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                      const model::PortElements<ValueType>& secondaryInput, size_t dimension,
                                                                                      const model::PortMemoryLayout& outputLayout,
                                                                                      ValueType paddingValue)
        : BroadcastBinaryFunctionNode<ValueType, FunctionType>(primaryInput, inputLayout,
                                                               secondaryInput, dimension,
                                                               outputLayout, FunctionType{}, paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                      const model::PortElements<ValueType>& secondaryInput, size_t dimension,
                                                                                      const model::PortMemoryLayout& outputLayout,
                                                                                      FunctionType function, ValueType paddingValue)
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput }, inputLayout, dimension,
                                                         { &_output }, outputLayout, function, paddingValue)
        , _primaryInput(this, primaryInput, primaryInputPortName)
        , _secondaryInput(this, secondaryInput, secondaryInputPortName)
        , _output(this, outputPortName, model::NumElements(outputLayout.GetStride()))
    {
        // Verify sizes are compatible
        size_t totalInputSize = inputLayout.GetMemorySize();
        if (primaryInput.Size() < totalInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Primary input too small");
        }

        if (secondaryInput.Size() != inputLayout.GetActiveSize(dimension))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Broadcast vector size doesn't match input");
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto primaryInputElements = transformer.TransformPortElements(_primaryInput.GetPortElements());
        auto secondaryInputElements = transformer.TransformPortElements(_secondaryInput.GetPortElements());
        auto newNode = transformer.AddNode<BroadcastBinaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                 this->GetInputLayout(),
                                                                                                 secondaryInputElements,
                                                                                                 this->GetBroadcastDimension(),
                                                                                                 this->GetOutputLayout(),
                                                                                                 GetFunction());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::Compute() const
    {
        auto outputSize = model::NumElements(GetOutputLayout().GetStride());
        auto output = std::vector<ValueType>(outputSize);
        size_t primaryInputIndex = 0;
        size_t secondaryInput1Index = 0;
        size_t& secondaryInput2Index = secondaryInput1Index;

        const size_t prevInputOffset = 0;
        const size_t prevOutputOffset = 0;
        std::vector<ValueType> secondaryValues{ 0 };
        ComputeDimensionLoop(0, output, prevInputOffset, prevOutputOffset, secondaryValues);

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
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output, this->GetOutputPadding());

        // Call recursive function to emit nested loops
        // Note: We could just offset the input pointer at beginning instead of adding offset every time through the loop
        // Note: We can potentially fuse adjacent loops if memory is contiguous --- it can be done by preprocessing size/stride vectors
        llvm::Value* prevInputDimensionOffset = nullptr;
        llvm::Value* prevOutputDimensionOffset = nullptr;
        std::vector<llvm::Value*> secondaryValues{ nullptr };
        EmitComputeDimensionLoop(compiler, function, 0, pPrimaryInput, { pSecondaryInput }, pOutput, prevInputDimensionOffset, prevOutputDimensionOffset, secondaryValues);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BroadcastFunctionNode<ValueType, FunctionType>::WriteToArchive(archiver);
        archiver[primaryInputPortName] << _primaryInput;
        archiver[secondaryInputPortName] << _secondaryInput;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastBinaryFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BroadcastFunctionNode<ValueType, FunctionType>::ReadFromArchive(archiver);
        archiver[primaryInputPortName] >> _primaryInput;
        archiver[secondaryInputPortName] >> _secondaryInput;
        _output.SetSize(_primaryInput.Size());
    }

    template <typename ValueType, typename FunctionType>
    const model::InputPort<ValueType>* BroadcastBinaryFunctionNode<ValueType, FunctionType>::GetSecondaryInput(int index) const
    {
        assert(index == 0);
        return &_secondaryInput;
    }

    //
    // BroadcastTernaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode()
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput1, &_secondaryInput2 }, { &_output }), _primaryInput(this, {}, primaryInputPortName), _secondaryInput1(this, {}, secondaryInput1PortName), _secondaryInput2(this, {}, secondaryInput2PortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                        const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t dimension,
                                                                                        const model::PortMemoryLayout& outputLayout,
                                                                                        ValueType paddingValue)
        : BroadcastTernaryFunctionNode<ValueType, FunctionType>(primaryInput, inputLayout,
                                                                secondaryInput1, secondaryInput2, dimension,
                                                                outputLayout, FunctionType{}, paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                        const model::PortElements<ValueType>& secondaryInput1, const model::PortElements<ValueType>& secondaryInput2, size_t dimension,
                                                                                        const model::PortMemoryLayout& outputLayout,
                                                                                        FunctionType function,
                                                                                        ValueType paddingValue)
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput1, &_secondaryInput2 }, inputLayout, dimension,
                                                         { &_output }, outputLayout, function, paddingValue)
        , _primaryInput(this, primaryInput, primaryInputPortName)
        , _secondaryInput1(this, secondaryInput1, secondaryInput1PortName)
        , _secondaryInput2(this, secondaryInput2, secondaryInput2PortName)
        , _output(this, outputPortName, model::NumElements(outputLayout.GetStride()))
    {
        // Verify sizes are compatible
        size_t totalInputSize = inputLayout.GetMemorySize();
        if (primaryInput.Size() < totalInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Primary input too small");
        }

        if (std::max(secondaryInput1.Size(), secondaryInput2.Size()) != inputLayout.GetActiveSize(dimension))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Broadcast vector size doesn't match input");
        }

        if (secondaryInput1.Size() != secondaryInput2.Size() && secondaryInput1.Size() > 0 && secondaryInput2.Size() > 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "If present, secondary inputs must have the same size");
        }

        if (!model::ShapesEqual(inputLayout.GetActiveSize(), outputLayout.GetActiveSize()))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output active area sizes don't match");
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        auto primaryInputElements = transformer.TransformPortElements(_primaryInput.GetPortElements());
        auto secondaryInput1Elements = transformer.TransformPortElements(_secondaryInput1.GetPortElements());
        auto secondaryInput2Elements = transformer.TransformPortElements(_secondaryInput2.GetPortElements());
        auto newNode = transformer.AddNode<BroadcastTernaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                  this->GetInputLayout(),
                                                                                                  secondaryInput1Elements,
                                                                                                  secondaryInput2Elements,
                                                                                                  this->GetBroadcastDimension(),
                                                                                                  this->GetOutputLayout(),
                                                                                                  GetFunction());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::Compute() const
    {
        auto outputSize = model::NumElements(GetOutputLayout().GetStride());
        auto output = std::vector<ValueType>(outputSize);
        size_t primaryInputIndex = 0;
        size_t secondaryInput1Index = 0;
        size_t& secondaryInput2Index = secondaryInput1Index;

        const size_t prevInputOffset = 0;
        const size_t prevOutputOffset = 0;
        std::vector<ValueType> secondaryValues{ 0, 0 };
        ComputeDimensionLoop(0, output, prevInputOffset, prevOutputOffset, secondaryValues);

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
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output, this->GetOutputPadding());

        // Call recursive function to emit nested loops
        // Note: We could just offset the input pointer at beginning instead of adding offset every time through the loop
        // Note: We can potentially fuse adjacent loops if memory is contiguous --- it can be done by preprocessing size/stride vectors
        llvm::Value* prevInputDimensionOffset = 0;
        llvm::Value* prevOutputDimensionOffset = 0;
        std::vector<llvm::Value*> secondaryValues{ nullptr, nullptr };
        EmitComputeDimensionLoop(compiler, function, 0, pPrimaryInput, { pSecondaryInput1, pSecondaryInput2 }, pOutput, prevInputDimensionOffset, prevOutputDimensionOffset, secondaryValues);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BroadcastFunctionNode<ValueType, FunctionType>::WriteToArchive(archiver);
        archiver[primaryInputPortName] << _primaryInput;
        archiver[secondaryInput1PortName] << _secondaryInput1;
        archiver[secondaryInput2PortName] << _secondaryInput2;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BroadcastFunctionNode<ValueType, FunctionType>::ReadFromArchive(archiver);
        archiver[primaryInputPortName] >> _primaryInput;
        archiver[secondaryInput1PortName] >> _secondaryInput1;
        archiver[secondaryInput2PortName] >> _secondaryInput2;
        _output.SetSize(_primaryInput.Size());
    }

    template <typename ValueType, typename FunctionType>
    const model::InputPort<ValueType>* BroadcastTernaryFunctionNode<ValueType, FunctionType>::GetSecondaryInput(int index) const
    {
        assert(index < 2);
        if (index == 0)
        {
            return &secondaryInput1;
        }
        else if (index == 1)
        {
            return &secondaryInput2;
        }
        return nullptr;
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
    BroadcastLinearFunctionNode<ValueType>::BroadcastLinearFunctionNode(const model::PortElements<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                        const model::PortElements<ValueType>& scaleInput, const model::PortElements<ValueType>& biasInput, size_t dimension,
                                                                        const model::PortMemoryLayout& outputLayout,
                                                                        ValueType paddingValue)
        : BroadcastTernaryFunctionNode<ValueType, BroadcastLinearFunction<ValueType>>(primaryInput, inputLayout,
                                                                                      scaleInput, biasInput, dimension,
                                                                                      outputLayout, paddingValue)
    {
    }

    template <typename ValueType>
    void BroadcastLinearFunctionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto primaryInputElements = transformer.TransformPortElements(primaryInput.GetPortElements());
        auto scaleInputElements = transformer.TransformPortElements(secondaryInput1.GetPortElements());
        auto biasInputElements = transformer.TransformPortElements(secondaryInput2.GetPortElements());
        auto newNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(primaryInputElements,
                                                                                   this->GetInputLayout(),
                                                                                   scaleInputElements,
                                                                                   biasInputElements,
                                                                                   this->GetBroadcastDimension(),
                                                                                   this->GetOutputLayout());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool BroadcastLinearFunctionNode<ValueType>::IsCompilable(const model::MapCompiler* compiler) const
    {
        bool refinable = (compiler != nullptr && compiler->GetMapCompilerParameters().fuseLinearFunctionNodes && CanCombineWithPrimaryInput());
        return !refinable;
    }

    template <typename ValueType>
    bool BroadcastLinearFunctionNode<ValueType>::HasSimpleConstantSecondaryInputs() const
    {
        // First verify our inputs are compatible
        int scaleInputSize = secondaryInput1.Size();
        int biasInputSize = secondaryInput2.Size();

        if (scaleInputSize > 0 && biasInputSize > 0 && scaleInputSize != biasInputSize)
        {
            return false; // sizes incompatible
        }

        const auto& scaleElements = secondaryInput1.GetPortElements();
        const auto& biasElements = secondaryInput2.GetPortElements();

        if (!scaleElements.IsFullPortOutput() || !biasElements.IsFullPortOutput())
        {
            return false; // we require all inputs to a port to come from the same place (though we could relax this requirement in the future, perhaps)
        }

        const ConstantNode<ValueType>* scaleInputNode = scaleInputSize == 0 ? nullptr : dynamic_cast<const ConstantNode<ValueType>*>(scaleElements.GetElement(0).ReferencedPort()->GetNode());
        const ConstantNode<ValueType>* biasInputNode = biasInputSize == 0 ? nullptr : dynamic_cast<const ConstantNode<ValueType>*>(biasElements.GetElement(0).ReferencedPort()->GetNode());

        if (scaleInputNode == nullptr && biasInputNode == nullptr)
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

        const auto& primaryElements = primaryInput.GetPortElements();
        if (!primaryElements.IsFullPortOutput())
        {
            return false; // we require all inputs to a port to come from the same place (though we could relax this requirement in the future, perhaps)
        }

        const BroadcastLinearFunctionNode<ValueType>* primaryInputNode = dynamic_cast<const BroadcastLinearFunctionNode<ValueType>*>(primaryElements.GetElement(0).ReferencedPort()->GetNode());
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

        // Check that the Shapes are compatible
        if (!PortMemoryLayoutsEqual(this->GetInputLayout(), primaryInputNode->GetInputLayout()))
        {
            return false;
        }

        if (!PortMemoryLayoutsEqual(this->GetOutputLayout(), primaryInputNode->GetOutputLayout()))
        {
            return false;
        }

        return true;
    }

    template <typename ValueType>
    typename BroadcastLinearFunctionNode<ValueType>::LinearCoeffNodes BroadcastLinearFunctionNode<ValueType>::GetConstantSecondaryInputNodes() const
    {
        const auto& scaleElements = secondaryInput1.GetPortElements();
        const auto& biasElements = secondaryInput2.GetPortElements();

        if ((scaleElements.Size() != 0 && !scaleElements.IsFullPortOutput()) || (biasElements.Size() != 0 && !biasElements.IsFullPortOutput()))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Combined linear function coefficients must be full port output");
        }

        int scaleInputSize = scaleElements.Size();
        int biasInputSize = biasElements.Size();
        if (scaleInputSize > 0 && biasInputSize > 0 && scaleInputSize != biasInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Combined linear function coefficients must have same size");
        }

        const ConstantNode<ValueType>* scaleInputNode = scaleInputSize == 0 ? nullptr : dynamic_cast<const ConstantNode<ValueType>*>(scaleElements.GetElement(0).ReferencedPort()->GetNode());
        const ConstantNode<ValueType>* biasInputNode = biasInputSize == 0 ? nullptr : dynamic_cast<const ConstantNode<ValueType>*>(biasElements.GetElement(0).ReferencedPort()->GetNode());

        return { scaleInputNode, biasInputNode };
    }

    template <typename ValueType>
    void BroadcastLinearFunctionNode<ValueType>::GetCombinedLinearCoeffs(const BroadcastLinearFunctionNode<ValueType>& prevNode, std::vector<ValueType>& scale, std::vector<ValueType>& bias) const
    {
        // Here, we have two linear functions, f1(x) = s1*x + b1; f2(x) = s2*x + b2
        // and we want to find their composition f' = s'*x + b' = f2(f1(x)) = (f2 * f1)(x) = s2*(s1*x + b1) + b2 = s1*s2*x + (b1*s2 + b2)
        // (Where `prevNode` is the node computing f1, and `this` is the node representing f2)

        auto prevSecondaryInputs = prevNode.GetConstantSecondaryInputNodes();
        auto thisSecondaryInputs = GetConstantSecondaryInputNodes();

        // Compute the combined scale, s' = s1*s2
        if (prevSecondaryInputs.scaleNode == nullptr && thisSecondaryInputs.scaleNode == nullptr) // s1 == 1, s2 == 1, so s' = 1
        {
            scale.resize(0); // signal there's no scale (scale = 1)
        }
        else if (prevSecondaryInputs.scaleNode == nullptr) // s1 == 1, so s' = s2
        {
            scale = thisSecondaryInputs.scaleNode->GetValues(); // s2
        }
        else if (thisSecondaryInputs.scaleNode == nullptr) // s2 == 1, so s' = s1,
        {
            scale = prevSecondaryInputs.scaleNode->GetValues(); // s1
        }
        else // s' = s1*s2*x
        {
            scale = prevSecondaryInputs.scaleNode->GetValues(); // scale = s1
            const auto& s2 = thisSecondaryInputs.scaleNode->GetValues();
            assert(s2.size() == scale.size());
            for (int index = 0; index < scale.size(); ++index)
            {
                scale[index] *= s2[index];
            }
        }

        // Now compute the combined bias, b' = (b1*s2) + b2
        if (prevSecondaryInputs.biasNode == nullptr && thisSecondaryInputs.biasNode == nullptr) // b1 == 0, b2 == 0, so b' == 0
        {
            bias.resize(0); // signal there's no bias (bias = 0)
        }
        else if (prevSecondaryInputs.biasNode == nullptr) // b1 == 0, so b' = b2
        {
            bias = thisSecondaryInputs.biasNode->GetValues(); // b2
        }
        else // b' = (b1*s2) + b1 (but s2 may be 1, and b1 may be zero)
        {
            bias = prevSecondaryInputs.biasNode->GetValues(); // bias == b1
            if (thisSecondaryInputs.scaleNode != nullptr) // if s2 is present, set bias = bias*s2
            {
                const auto& s2 = thisSecondaryInputs.scaleNode->GetValues();
                assert(s2.size() == bias.size());
                for (int index = 0; index < bias.size(); ++index)
                {
                    bias[index] *= s2[index];
                }
            }

            if (thisSecondaryInputs.biasNode != nullptr) // b2 == 0, so b' = b1*s2, but perhaps s2 == 1
            {
                const auto& b2 = thisSecondaryInputs.biasNode->GetValues(); // now add b2
                for (int index = 0; index < bias.size(); ++index)
                {
                    bias[index] += b2[index];
                }
            }
        }
    }

    template <typename ValueType>
    bool BroadcastLinearFunctionNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto compiler = transformer.GetContext().GetCompiler();
        if (IsCompilable(compiler))
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

        std::vector<ValueType> newScale;
        std::vector<ValueType> newBias;
        GetCombinedLinearCoeffs(*prevNode, newScale, newBias);

        auto prevPrimaryInputElements = prevNode->primaryInput.GetPortElements();
        auto scaleValuesNode = transformer.AddNode<ConstantNode<ValueType>>(newScale);
        auto biasValuesNode = transformer.AddNode<ConstantNode<ValueType>>(newBias);
        auto newNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(prevPrimaryInputElements,
                                                                                   this->GetInputLayout(),
                                                                                   scaleValuesNode->output,
                                                                                   biasValuesNode->output,
                                                                                   this->GetBroadcastDimension(),
                                                                                   this->GetOutputLayout());
        transformer.MapNodeOutput(output, newNode->output);
        return true;
    }

} // nodes
} // ell
