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
    emitters::LLVMValue BroadcastUnaryFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, const std::vector<emitters::LLVMValue>& secondaryArgs) const
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
    emitters::LLVMValue BroadcastBinaryFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, const std::vector<emitters::LLVMValue>& secondaryArgs) const
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
    emitters::LLVMValue BroadcastTernaryFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, const std::vector<emitters::LLVMValue>& secondaryArgs) const
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
    emitters::LLVMValue BroadcastLinearFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue x, emitters::LLVMValue scale, emitters::LLVMValue bias) const
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
        : CompilableNode(inputs, outputs), _inputLayout(inputLayout), _broadcastDimension(broadcastDimension), _function(function), _paddingValue(paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    model::PortMemoryLayout BroadcastFunctionNode<ValueType, FunctionType>::GetOutputMemoryLayout() const
    {
        return GetOutputPort(0)->GetMemoryLayout();
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
        //       If broadcastDimension = outermost dimension (0), we may want to parallelize over that dimension
        const auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputLayout = GetInputMemoryLayout();
        auto&& inputStride = inputLayout.GetExtent();
        auto&& inputOffset = inputLayout.GetOffset();
        auto&& inputSize = inputLayout.GetActiveSize();
        auto&& outputLayout = GetOutputMemoryLayout();
        auto&& outputStride = outputLayout.GetExtent();
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
                thisOutputDimensionOffset += prevOutputDimensionOffset * outputStride[dimension];
            }

            if (dimension == broadcastDimension)
            {
                for (int index = 0; index < numSecondaryInputs; ++index)
                {
                    auto&& secondaryInput = GetSecondaryInput(index);
                    if (IsSecondaryInputPresent(index))
                    {
                        secondaryValues[index] = (*secondaryInput)[loopIndex];
                    }
                    else
                    {
                        // Dubious hack to deal with linear function nodes missing a coefficient
                        if (std::is_same<FunctionType, BroadcastLinearFunction<ValueType>>::value && index == 0) // "scale" value, which should be 1 if not specified
                        {
                            secondaryValues[index] = static_cast<ValueType>(1.0);
                        }
                        else
                        {
                            secondaryValues[index] = 0;
                        }
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

    // wrapper around EmitComputeDimensionLoop for use by parallel tasks
    template <typename ValueType, typename FunctionType>
    emitters::IRFunctionEmitter BroadcastFunctionNode<ValueType, FunctionType>::GetTaskFunction(model::IRMapCompiler& compiler,
                                                                                                emitters::IRFunctionEmitter& function,
                                                                                                const emitters::LLVMTypeList& portTypes) const
    {
        auto& module = function.GetModule();
        auto& emitter = module.GetIREmitter();
        auto& context = module.GetLLVMContext();
        auto int32Type = emitter.Type(emitters::VariableType::Int32);
        auto voidType = llvm::Type::getVoidTy(context);

        // ASSUME dimension == 0 --- we're only parallelizing on the outermost loop
        int dimension = 0;

        emitters::LLVMTypeList argTypes = portTypes;
        // int numValuePorts = 2 + NumSecondaryInputs(); // primary input, secondary inputs, output
        // argTypes.insert(argTypes.end(), numValuePorts, valuePtrType);
        argTypes.insert(argTypes.end(), 2, int32Type); // begin, end

        auto taskFunction = function.GetModule().BeginFunction(utilities::to_string(GetId()) + "_task", voidType, argTypes);
        {
            // get stuff from arguments
            auto arguments = taskFunction.Arguments().begin();
            auto primaryInput = &(*arguments++);
            std::vector<emitters::LLVMValue> secondaryInputs;
            std::vector<emitters::LLVMValue> secondaryValues;
            for (int index = 0; index < NumSecondaryInputs(); ++index)
            {
                auto secondaryInput = &(*arguments++);
                // if we really have an input, push it, else push a nullptr (note: we know this at compile-time)
                if (IsSecondaryInputPresent(index))
                {
                    secondaryInputs.push_back(secondaryInput);
                }
                else
                {
                    secondaryInputs.push_back(nullptr);
                }
                secondaryValues.push_back(nullptr);
            }
            auto output = &(*arguments++);
            auto begin = function.LocalScalar(&(*arguments++));
            auto end = function.LocalScalar(&(*arguments++));
            auto prevInputDimensionOffset = function.LocalScalar();
            auto prevOutputDimensionOffset = function.LocalScalar();

            EmitComputeDimensionLoop(compiler, taskFunction, dimension, begin, end, primaryInput, secondaryInputs, output, prevInputDimensionOffset, prevOutputDimensionOffset, secondaryValues);
            taskFunction.Return();
        }
        function.GetModule().EndFunction();

        return taskFunction;
    }

    // Note: secondaryValues is passed by non-const reference to avoid copies. It doesn't function as an output parameter.
    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function,
                                                                                  size_t dimension,
                                                                                  emitters::IRLocalScalar begin,
                                                                                  emitters::IRLocalScalar end,
                                                                                  emitters::LLVMValue primaryInput, const std::vector<emitters::LLVMValue>& secondaryInputs,
                                                                                  emitters::LLVMValue output,
                                                                                  emitters::IRLocalScalar prevInputDimensionOffset, emitters::IRLocalScalar prevOutputDimensionOffset,
                                                                                  std::vector<emitters::LLVMValue>& secondaryValues) const
    {
        // Note: It should be easy to unroll the last K levels by putting a real loop here when dimension < k
        //       Or, instead of unrolling, vectorizing --- if broadcastDimension = 1, let secondaryValue be a vector and load it one loop previous
        //       If broadcastDimension = outermost dimension (0), we may want to parallelize over that dimension
        const auto numDimensions = NumPrimaryInputDimensions();
        auto&& inputLayout = GetInputMemoryLayout();
        auto&& inputStride = inputLayout.GetExtent();
        auto&& inputOffset = inputLayout.GetOffset();
        auto&& inputSize = inputLayout.GetActiveSize();
        auto&& outputLayout = GetOutputMemoryLayout();
        auto&& outputStride = outputLayout.GetExtent();
        auto&& outputOffset = outputLayout.GetOffset();
        const auto broadcastDimension = GetBroadcastDimension();
        const auto numSecondaryInputs = NumSecondaryInputs();

        function.For(begin, end, [dimension, numDimensions, inputSize, inputOffset, inputStride, outputOffset, outputStride, broadcastDimension, numSecondaryInputs, prevInputDimensionOffset, prevOutputDimensionOffset, primaryInput, secondaryInputs, output, &secondaryValues, &compiler, this](emitters::IRFunctionEmitter& function, auto loopIndex) {
            // Calculate the offset within this dimension = (loopIndex + offset[dimension])
            auto thisInputDimensionInternalOffset = loopIndex + inputOffset[dimension];
            auto thisOutputDimensionInternalOffset = loopIndex + outputOffset[dimension];

            // Calculate the total offset from beginning of memory:
            //   * if in the outermost loop, the offset into this dimension
            //   * otherwise, the offset into this dimension plus the previous offset scaled by the previous dimension's stride
            auto thisInputDimensionOffset = function.LocalScalar();
            auto thisOutputDimensionOffset = function.LocalScalar();
            if (dimension == 0)
            {
                assert(!prevInputDimensionOffset.IsValid());
                assert(!prevOutputDimensionOffset.IsValid());
                thisInputDimensionOffset = thisInputDimensionInternalOffset;
                thisOutputDimensionOffset = thisOutputDimensionInternalOffset;
            }
            else
            {
                thisInputDimensionOffset = thisInputDimensionInternalOffset + (prevInputDimensionOffset * inputStride[dimension]);
                thisOutputDimensionOffset = thisOutputDimensionInternalOffset + (prevOutputDimensionOffset * outputStride[dimension]);
            }

            if (dimension == broadcastDimension)
            {
                for (int index = 0; index < numSecondaryInputs; ++index)
                {
                    auto&& secondaryInput = secondaryInputs[index];
                    secondaryValues[index] = this->IsSecondaryInputPresent(index) ? function.ValueAt(secondaryInput, loopIndex) : nullptr;
                }
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                auto nextBegin = function.LocalScalar<int>(0);
                auto nextEnd = function.LocalScalar<int>(inputSize[dimension + 1]);
                this->EmitComputeDimensionLoop(compiler, function, dimension + 1, nextBegin, nextEnd, primaryInput, secondaryInputs, output, thisInputDimensionOffset, thisOutputDimensionOffset, secondaryValues);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto primaryValue = function.ValueAt(primaryInput, thisInputDimensionOffset);
                auto outputValue = this->GetFunction().Compile(function, primaryValue, secondaryValues);
                function.SetValueAt(output, thisOutputDimensionOffset, outputValue);
            }
        });
    }

    template <typename ValueType, typename FunctionType>
    bool BroadcastFunctionNode<ValueType, FunctionType>::IsSecondaryInputPresent(int index) const
    {
        auto secondaryInput = GetSecondaryInput(index);
        if (secondaryInput)
        {
            return secondaryInput->Size() > 0;
        }
        else
        {
            return false;
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::Compute() const
    {
        auto outputSize = GetOutputMemoryLayout().GetExtent().NumElements();
        auto output = std::vector<ValueType>(outputSize);

        const size_t prevInputOffset = 0;
        const size_t prevOutputOffset = 0;
        std::vector<ValueType> secondaryValues(NumSecondaryInputs(), static_cast<ValueType>(0));
        ComputeDimensionLoop(0, output, prevInputOffset, prevOutputOffset, secondaryValues);

        GetOutput().SetOutput(output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const auto& compilerSettings = compiler.GetCompilerOptions();

        auto& module = function.GetModule();
        auto& emitter = module.GetIREmitter();
        auto valueType = emitter.Type(emitters::GetVariableType<ValueType>());
        auto valuePtrType = valueType->getPointerTo();

        const auto& primaryInput = GetPrimaryInput();
        auto primaryInputSize = primaryInput.Size();
        auto&& inputLayout = GetInputMemoryLayout();
        auto&& inputSize = inputLayout.GetActiveSize();
        auto secondaryInputSize = GetSecondaryInputSize();
        DEBUG_USED(secondaryInputSize);
        assert(secondaryInputSize == 0 || primaryInputSize % secondaryInputSize == 0);

        emitters::LLVMValue pPrimaryInput = compiler.EnsurePortEmitted(primaryInput);
        std::vector<emitters::LLVMValue> secondaryInputs;
        std::vector<emitters::LLVMValue> secondaryValues;
        for (int index = 0; index < NumSecondaryInputs(); ++index)
        {
            auto secondaryInputPort = GetSecondaryInput(index);
            auto secondaryInputSize = secondaryInputPort->Size();
            emitters::LLVMValue secondaryInput = (secondaryInputSize > 0) ? compiler.EnsurePortEmitted(*secondaryInputPort) : function.NullPointer(valuePtrType);
            secondaryInputs.push_back(secondaryInput);
            secondaryValues.push_back(nullptr);
        }
        emitters::LLVMValue pOutput = compiler.EnsurePortEmitted(GetOutput(), this->GetOutputPadding());

        // Call recursive function to emit nested loops
        // Note: We could just offset the input pointer at beginning instead of adding offset every time through the loop
        // Note: We can potentially fuse adjacent loops if memory is contiguous --- it can be done by preprocessing size/stride vectors
        bool allSecondaryInputsValid = true;
        for (int index = 0; index < NumSecondaryInputs(); ++index)
        {
            if (!IsSecondaryInputPresent(index))
            {
                allSecondaryInputsValid = false;
            }
        }

        const int minimumTaskSize = 4000;
        if (compilerSettings.parallelize && allSecondaryInputsValid && primaryInputSize > 2 * minimumTaskSize)
        {
            // computes ceil(a/b)
            auto CeilDiv = [](int a, int b) {
                return (a - 1) / b + 1;
            };

            // TODO: fix up logic for deciding how many tasks to use.
            //   want to specify minimum amount of work per task, and create fewer tasks
            //   if we don't have enough work.
            auto numOuterIterations = inputSize[0];
            const int numDesiredTasks = compilerSettings.maxThreads;
            int taskSize = std::max(CeilDiv(primaryInputSize, numDesiredTasks), minimumTaskSize);
            const int numTasks = std::min(CeilDiv(primaryInputSize, taskSize), compilerSettings.maxThreads);
            taskSize = CeilDiv(numOuterIterations, numTasks);

            // Ugly type-getting code to get around the type of the emitted port variables being different depending
            // on whether the node is inlined (or something).
            emitters::LLVMTypeList taskFunctionArgTypes{ pPrimaryInput->getType() };
            for (auto& secondaryInput : secondaryInputs)
            {
                taskFunctionArgTypes.push_back(secondaryInput->getType());
            }
            taskFunctionArgTypes.push_back(pOutput->getType());

            auto taskFunction = this->GetTaskFunction(compiler, function, taskFunctionArgTypes);
            std::vector<std::vector<emitters::LLVMValue>> taskArgs;
            for (int taskIndex = 0; taskIndex < numTasks; ++taskIndex)
            {
                auto begin = function.Literal<int>(taskIndex * taskSize);
                auto end = function.Literal<int>(std::min((taskIndex + 1) * taskSize, numOuterIterations));

                std::vector<emitters::LLVMValue> args{ pPrimaryInput };
                args.insert(args.end(), secondaryInputs.begin(), secondaryInputs.end());
                args.insert(args.end(), { pOutput, begin, end });
            }
            auto tasks = function.StartTasks(taskFunction, taskArgs);
            tasks.WaitAll(function);
        }
        else
        {
            auto prevInputDimensionOffset = function.LocalScalar();
            auto prevOutputDimensionOffset = function.LocalScalar();
            auto begin = function.LocalScalar<int>(0);
            auto end = function.LocalScalar<int>(inputSize[0]);
            EmitComputeDimensionLoop(compiler, function, 0, begin, end, pPrimaryInput, secondaryInputs, pOutput, prevInputDimensionOffset, prevOutputDimensionOffset, secondaryValues);
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);

        archiver["inputLayout"] << _inputLayout;
        archiver["outputLayout"] << GetOutputMemoryLayout();
        archiver["broadcastDimension"] << _broadcastDimension;
        archiver["paddingValue"] << _paddingValue;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastFunctionNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);

        archiver["inputLayout"] >> _inputLayout;
        model::PortMemoryLayout outputLayout;
        archiver["outputLayout"] >> outputLayout;
        auto outputs = GetOutputPorts();
        for(auto p: outputs)
        {
            p->SetMemoryLayout(outputLayout);
        }
        archiver["broadcastDimension"] >> _broadcastDimension;
        archiver["paddingValue"] >> _paddingValue;
    }

    //
    // BroadcastUnaryFunctionNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastUnaryFunctionNode<ValueType, FunctionType>::BroadcastUnaryFunctionNode()
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput }, { &_output }), _primaryInput(this, {}, primaryInputPortName), _output(this, ell::model::Node::defaultOutputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastUnaryFunctionNode<ValueType, FunctionType>::BroadcastUnaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                    const model::PortMemoryLayout& outputLayout,
                                                                                    ValueType paddingValue)
        : BroadcastUnaryFunctionNode<ValueType, FunctionType>(primaryInput, inputLayout, outputLayout, FunctionType{}, paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastUnaryFunctionNode<ValueType, FunctionType>::BroadcastUnaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                    const model::PortMemoryLayout& outputLayout,
                                                                                    FunctionType function,
                                                                                    ValueType paddingValue)
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput }, inputLayout, 0, { &_output }, outputLayout, function, paddingValue)
        , _primaryInput(this, primaryInput, primaryInputPortName)
        , _output(this, ell::model::Node::defaultOutputPortName, outputLayout)
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
        const auto& primaryInputElements = transformer.GetCorrespondingInputs(_primaryInput);
        auto broadcastFunction = GetFunction();
        auto newNode = transformer.AddNode<BroadcastUnaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                this->GetInputMemoryLayout(),
                                                                                                this->GetOutputMemoryLayout(),
                                                                                                broadcastFunction);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename FunctionType>
    utilities::ArchiveVersion BroadcastUnaryFunctionNode<ValueType, FunctionType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v5_refined_nodes };

        return archiveVersion;
    }

    template <typename ValueType, typename FunctionType>
    bool BroadcastUnaryFunctionNode<ValueType, FunctionType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v5_refined_nodes };

        return version >= archiveVersion;
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
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput }, { &_output }), _primaryInput(this, {}, primaryInputPortName), _secondaryInput(this, {}, secondaryInputPortName), _output(this, ell::model::Node::defaultOutputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                      const model::OutputPort<ValueType>& secondaryInput, size_t dimension,
                                                                                      const model::PortMemoryLayout& outputLayout,
                                                                                      ValueType paddingValue)
        : BroadcastBinaryFunctionNode<ValueType, FunctionType>(primaryInput, inputLayout,
                                                               secondaryInput, dimension,
                                                               outputLayout, FunctionType{}, paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastBinaryFunctionNode<ValueType, FunctionType>::BroadcastBinaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                      const model::OutputPort<ValueType>& secondaryInput, size_t dimension,
                                                                                      const model::PortMemoryLayout& outputLayout,
                                                                                      FunctionType function, ValueType paddingValue)
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput }, inputLayout, dimension,
                                                         { &_output }, outputLayout, function, paddingValue)
        , _primaryInput(this, primaryInput, primaryInputPortName)
        , _secondaryInput(this, secondaryInput, secondaryInputPortName)
        , _output(this, ell::model::Node::defaultOutputPortName, outputLayout)
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
        const auto& primaryInputElements = transformer.GetCorrespondingInputs(_primaryInput);
        const auto& secondaryInputElements = transformer.GetCorrespondingInputs(_secondaryInput);
        auto newNode = transformer.AddNode<BroadcastBinaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                 this->GetInputMemoryLayout(),
                                                                                                 secondaryInputElements,
                                                                                                 this->GetBroadcastDimension(),
                                                                                                 this->GetOutputMemoryLayout(),
                                                                                                 GetFunction());
        transformer.MapNodeOutput(output, newNode->output);
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
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput1, &_secondaryInput2 }, { &_output }), _primaryInput(this, {}, primaryInputPortName), _secondaryInput1(this, {}, secondaryInput1PortName), _secondaryInput2(this, {}, secondaryInput2PortName), _output(this, ell::model::Node::defaultOutputPortName, 0)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                        const model::OutputPort<ValueType>& secondaryInput1, const model::OutputPort<ValueType>& secondaryInput2, size_t dimension,
                                                                                        const model::PortMemoryLayout& outputLayout,
                                                                                        ValueType paddingValue)
        : BroadcastTernaryFunctionNode<ValueType, FunctionType>(primaryInput, inputLayout,
                                                                secondaryInput1, secondaryInput2, dimension,
                                                                outputLayout, FunctionType{}, paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastTernaryFunctionNode<ValueType, FunctionType>::BroadcastTernaryFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                                        const model::OutputPort<ValueType>& secondaryInput1, const model::OutputPort<ValueType>& secondaryInput2, size_t dimension,
                                                                                        const model::PortMemoryLayout& outputLayout,
                                                                                        FunctionType function,
                                                                                        ValueType paddingValue)
        : BroadcastFunctionNode<ValueType, FunctionType>({ &_primaryInput, &_secondaryInput1, &_secondaryInput2 }, inputLayout, dimension,
                                                         { &_output }, outputLayout, function, paddingValue)
        , _primaryInput(this, primaryInput, primaryInputPortName)
        , _secondaryInput1(this, secondaryInput1, secondaryInput1PortName)
        , _secondaryInput2(this, secondaryInput2, secondaryInput2PortName)
        , _output(this, ell::model::Node::defaultOutputPortName, outputLayout)
    {
        // Verify sizes are compatible
        size_t totalInputSize = inputLayout.GetMemorySize();
        if (primaryInput.Size() < totalInputSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Primary input too small");
        }

        if (std::max(secondaryInput1.Size(), secondaryInput2.Size()) != static_cast<size_t>(inputLayout.GetActiveSize(dimension)))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, std::string("Broadcast vector size doesn't match input") + "_" + std::to_string(secondaryInput1.Size()) + "_" + std::to_string(secondaryInput2.Size()) + "_" + std::to_string(inputLayout.GetActiveSize(dimension)));
        }

        if (secondaryInput1.Size() != secondaryInput2.Size() && secondaryInput1.Size() > 0 && secondaryInput2.Size() > 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "If present, secondary inputs must have the same size");
        }

        if (inputLayout.GetActiveSize() != outputLayout.GetActiveSize())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "BroadcastFunctionNode: Input and output active area sizes don't match");
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastTernaryFunctionNode<ValueType, FunctionType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& primaryInputElements = transformer.GetCorrespondingInputs(_primaryInput);
        const auto& secondaryInput1Elements = transformer.GetCorrespondingInputs(_secondaryInput1);
        const auto& secondaryInput2Elements = transformer.GetCorrespondingInputs(_secondaryInput2);
        auto newNode = transformer.AddNode<BroadcastTernaryFunctionNode<ValueType, FunctionType>>(primaryInputElements,
                                                                                                  this->GetInputMemoryLayout(),
                                                                                                  secondaryInput1Elements,
                                                                                                  secondaryInput2Elements,
                                                                                                  this->GetBroadcastDimension(),
                                                                                                  this->GetOutputMemoryLayout(),
                                                                                                  GetFunction());
        transformer.MapNodeOutput(output, newNode->output);
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
    BroadcastLinearFunctionNode<ValueType>::BroadcastLinearFunctionNode(const model::OutputPort<ValueType>& primaryInput, const model::PortMemoryLayout& inputLayout,
                                                                        const model::OutputPort<ValueType>& scaleInput, const model::OutputPort<ValueType>& biasInput, size_t dimension,
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
        const auto& primaryInputElements = transformer.GetCorrespondingInputs(primaryInput);
        const auto& scaleInputElements = transformer.GetCorrespondingInputs(secondaryInput1);
        const auto& biasInputElements = transformer.GetCorrespondingInputs(secondaryInput2);
        auto newNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(primaryInputElements,
                                                                                   this->GetInputMemoryLayout(),
                                                                                   scaleInputElements,
                                                                                   biasInputElements,
                                                                                   this->GetBroadcastDimension(),
                                                                                   this->GetOutputMemoryLayout());
        transformer.MapNodeOutput(output, newNode->output);
    }

} // nodes
} // ell
