////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryOperationNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#define ADD_TO_STRING_ENTRY(NAMESPACE, OPERATOR) \
    case NAMESPACE::OPERATOR:                    \
        return #OPERATOR;
#define BEGIN_FROM_STRING if (false)
#define ADD_FROM_STRING_ENTRY(NAMESPACE, OPERATOR) else if (name == #OPERATOR) return NAMESPACE::OPERATOR

namespace ell
{
namespace nodes
{
    namespace BinaryOperations
    {
        inline std::string to_string(emitters::BinaryOperationType op)
        {
            switch (op)
            {
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, none);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, add);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, subtract);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, coordinatewiseMultiply);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, coordinatewiseDivide);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, logicalAnd);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, logicalOr);
                ADD_TO_STRING_ENTRY(emitters::BinaryOperationType, logicalXor);
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown binary operation");
            }
        }

        inline emitters::BinaryOperationType from_string(std::string name)
        {
            BEGIN_FROM_STRING;
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, none);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, add);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, subtract);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, coordinatewiseMultiply);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, coordinatewiseDivide);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, logicalAnd);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, logicalOr);
            ADD_FROM_STRING_ENTRY(emitters::BinaryOperationType, logicalXor);

            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown binary operation");
        }

        template <typename ValueType>
        ValueType Add(ValueType a, ValueType b)
        {
            return a + b;
        }

        template <>
        inline bool Add(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <typename ValueType>
        ValueType Subtract(ValueType a, ValueType b)
        {
            return a - b;
        }

        template <>
        inline bool Subtract(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <typename ValueType>
        ValueType Multiply(ValueType a, ValueType b)
        {
            return a * b;
        }

        template <>
        inline bool Multiply(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <typename ValueType>
        ValueType Divide(ValueType a, ValueType b)
        {
            return a / b;
        }

        template <>
        inline bool Divide(bool a, bool b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        //
        // Logical operations
        //
        template <typename ValueType>
        ValueType LogicalAnd(ValueType a, ValueType b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <>
        inline bool LogicalAnd(bool a, bool b)
        {
            return a && b;
        }

        template <typename ValueType>
        ValueType LogicalOr(ValueType a, ValueType b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <>
        inline bool LogicalOr(bool a, bool b)
        {
            return a || b;
        }

        template <typename ValueType>
        ValueType LogicalXor(ValueType a, ValueType b)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        template <>
        inline bool LogicalXor(bool a, bool b)
        {
            return (!a) != (!b);
        }
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode()
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, {}, defaultInput1PortName), _input2(this, {}, defaultInput2PortName), _output(this, defaultOutputPortName, 0), _operation(emitters::BinaryOperationType::none)
    {
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2, emitters::BinaryOperationType operation)
        : CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, input1, defaultInput1PortName),  _inputLayout1({ static_cast<int>(input1.Size()), 1, 1 }),
        _input2(this, input2, defaultInput2PortName),  _inputLayout2({ static_cast<int>(input2.Size()), 1, 1 }),
        _output(this, defaultOutputPortName, _input1.Size()), _outputLayout({ static_cast<int>(input1.Size()), 1, 1 }),
        _operation(operation),
        _paddingValue(0)
    {
        if (input1.Size() != input2.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input sizes must match");
        }
        assert(input1.Size() == input2.Size());
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::PortElements<ValueType>& input1,
                                                        const model::PortMemoryLayout& inputLayout1,
                                                        const model::PortElements<ValueType>& input2,
                                                        const model::PortMemoryLayout& inputLayout2,
                                                        const model::PortMemoryLayout& outputLayout,
                                                        emitters::BinaryOperationType operation,
                                                        ValueType padding)
        : CompilableNode({ &_input1, &_input2 }, { &_output }), 
          _input1(this, input1, defaultInput1PortName), _inputLayout1(inputLayout1),
          _input2(this, input2, defaultInput2PortName), _inputLayout2(inputLayout2),
          _output(this, defaultOutputPortName, outputLayout.GetMemorySize()), _outputLayout(outputLayout),
          _operation(operation),
          _paddingValue(padding)
    {
        if (!model::ShapesEqual(inputLayout1.GetActiveSize(), inputLayout2.GetActiveSize()))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Active areas must match for both inputs");
        }
        if (!model::ShapesEqual(inputLayout1.GetActiveSize(), outputLayout.GetActiveSize()))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output active areas must match");
        }
    }

    template <typename ValueType>
    template <typename Operation>
    std::vector<ValueType> BinaryOperationNode<ValueType>::ComputeOutput(Operation&& function) const
    {
        auto outputSize = model::NumElements(_outputLayout.GetStride());
        auto output = std::vector<ValueType>(outputSize);

        const size_t prevInput1Offset = 0;
        const size_t prevInput2Offset = 0;
        const size_t prevOutputOffset = 0;
        ComputeDimensionLoop(function, 0, output, prevInput1Offset, prevInput2Offset, prevOutputOffset);

        return output;
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output;
        switch (_operation)
        {
        case emitters::BinaryOperationType::add:
            output = ComputeOutput(BinaryOperations::Add<ValueType>);
            break;
        case emitters::BinaryOperationType::subtract:
            output = ComputeOutput(BinaryOperations::Subtract<ValueType>);
            break;
        case emitters::BinaryOperationType::coordinatewiseMultiply:
            output = ComputeOutput(BinaryOperations::Multiply<ValueType>);
            break;
        case emitters::BinaryOperationType::coordinatewiseDivide:
            output = ComputeOutput(BinaryOperations::Divide<ValueType>);
            break;
        case emitters::BinaryOperationType::logicalAnd:
            output = ComputeOutput(BinaryOperations::LogicalAnd<ValueType>);
            break;
        case emitters::BinaryOperationType::logicalOr:
            output = ComputeOutput(BinaryOperations::LogicalOr<ValueType>);
            break;
        case emitters::BinaryOperationType::logicalXor:
            output = ComputeOutput(BinaryOperations::LogicalXor<ValueType>);
            break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown operation type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto PortElements1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto PortElements2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<BinaryOperationNode<ValueType>>(PortElements1, _inputLayout1, PortElements2, _inputLayout2, _outputLayout, _operation);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (_inputLayout1.GetMemorySize() == _inputLayout2.GetMemorySize())
        {
            if (IsPureVector(input1) && IsPureVector(input2) && !compiler.GetCompilerOptions().unrollLoops)
            {
                CompileLoop(compiler, function);
            }
            else
            {
                CompileExpanded(compiler, function);
            }
        }
        else
        {
            llvm::Value* pInput1 = compiler.EnsurePortEmitted(input1);
            llvm::Value* pInput2 = compiler.EnsurePortEmitted(input2);
            llvm::Value* pResult = compiler.EnsurePortEmitted(output, _paddingValue);

            // Call recursive function to emit nested loops
            llvm::Value* prevInput1DimensionOffset = nullptr;
            llvm::Value* prevInput2DimensionOffset = nullptr;
            llvm::Value* prevOutputDimensionOffset = nullptr;
            EmitComputeDimensionLoop(compiler, function, 0, pInput1, pInput2, pResult, prevInput1DimensionOffset, prevInput2DimensionOffset, prevOutputDimensionOffset);
        }
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput1 = compiler.EnsurePortEmitted(input1);
        llvm::Value* pInput2 = compiler.EnsurePortEmitted(input2);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        auto count = input1.Size();
        function.VectorOperator(emitters::GetOperator<ValueType>(GetOperation()), count, pInput1, pInput2, [&pResult, &function](llvm::Value* i, llvm::Value* pValue) {
            function.SetValueAt(pResult, i, pValue);
        });
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);

        auto count = input1.Size();
        for (size_t i = 0; i < count; ++i)
        {
            llvm::Value* inputValue1 = compiler.LoadPortElementVariable(input1.GetInputElement(i));
            llvm::Value* inputValue2 = compiler.LoadPortElementVariable(input2.GetInputElement(i));
            llvm::Value* pOpResult = function.Operator(emitters::GetOperator<ValueType>(GetOperation()), inputValue1, inputValue2);
            function.SetValueAt(pResult, function.Literal<int>(i), pOpResult);
        }
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

    template <typename ValueType>
    template <typename Operation>
    void BinaryOperationNode<ValueType>::ComputeDimensionLoop(Operation& function,
                                                              size_t dimension,
                                                              std::vector<ValueType>& output,
                                                              size_t prevInput1DimensionOffset,
                                                              size_t prevInput2DimensionOffset,
                                                              size_t prevOutputDimensionOffset) const
    {
        const auto numDimensions = _inputLayout1.NumDimensions();
        auto&& inputStride1 = _inputLayout1.GetStride();
        auto&& inputOffset1 = _inputLayout1.GetOffset();
        auto&& inputStride2 = _inputLayout2.GetStride();
        auto&& inputOffset2 = _inputLayout2.GetOffset();
        auto&& inputSize = _inputLayout1.GetActiveSize();
        auto&& outputOffset = _outputLayout.GetOffset();
        auto&& outputStride = _outputLayout.GetStride();

        for (int loopIndex = 0; loopIndex < inputSize[dimension]; ++loopIndex)
        {
            // offset within start of this dimension = (loopIndex + offset[dimension])
            auto thisInput1DimensionInternalOffset = loopIndex + inputOffset1[dimension];
            auto thisInput2DimensionInternalOffset = loopIndex + inputOffset2[dimension];
            auto thisOutputDimensionInternalOffset = loopIndex + outputOffset[dimension];

            size_t thisInput1DimensionOffset = thisInput1DimensionInternalOffset;
            size_t thisInput2DimensionOffset = thisInput2DimensionInternalOffset;
            size_t thisOutputDimensionOffset = thisOutputDimensionInternalOffset;
            if (dimension != 0)
            {
                thisInput1DimensionOffset += prevInput1DimensionOffset * inputStride1[dimension];
                thisInput2DimensionOffset += prevInput2DimensionOffset * inputStride2[dimension];
                thisOutputDimensionOffset += prevOutputDimensionOffset * outputStride[dimension];
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                ComputeDimensionLoop(function, dimension + 1, output, thisInput1DimensionOffset, thisInput2DimensionOffset, thisOutputDimensionOffset);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto value1 = _input1[thisInput1DimensionOffset];
                auto value2 = _input2[thisInput2DimensionOffset];
                auto outputValue = function(value1, value2);
                output[thisOutputDimensionOffset] = outputValue;
            }
        }
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::EmitComputeDimensionLoop(model::IRMapCompiler& compiler,
                                                                  emitters::IRFunctionEmitter& function,
                                                                  size_t dimension,
                                                                  llvm::Value* input1,
                                                                  llvm::Value* input2,
                                                                  llvm::Value* output,
                                                                  llvm::Value* prevInput1DimensionOffset,
                                                                  llvm::Value* prevInput2DimensionOffset,
                                                                  llvm::Value* prevOutputDimensionOffset) const
    {
        const auto numDimensions = _inputLayout1.NumDimensions();
        auto&& inputStride1 = _inputLayout1.GetStride();
        auto&& inputOffset1 = _inputLayout1.GetOffset();
        auto&& inputStride2 = _inputLayout2.GetStride();
        auto&& inputOffset2 = _inputLayout2.GetOffset();
        auto&& inputSize = _inputLayout1.GetActiveSize();
        auto&& outputStride = _outputLayout.GetStride();
        auto&& outputOffset = _outputLayout.GetOffset();

        function.For(inputSize[dimension], [input1, input2, output, inputOffset1, inputOffset2, inputStride1, inputStride2, outputStride, outputOffset, prevInput1DimensionOffset, prevInput2DimensionOffset, prevOutputDimensionOffset, dimension, numDimensions, &compiler, this](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex) {
            // Calculate the offset within this dimension = (loopIndex + offset[dimension])
            llvm::Value* thisInput1DimensionInternalOffset = function.Operator(emitters::GetAddForValueType<int>(), loopIndex, function.Literal<int>(inputOffset1[dimension]));
            llvm::Value* thisInput2DimensionInternalOffset = function.Operator(emitters::GetAddForValueType<int>(), loopIndex, function.Literal<int>(inputOffset2[dimension]));
            llvm::Value* thisOutputDimensionInternalOffset = function.Operator(emitters::GetAddForValueType<int>(), loopIndex, function.Literal<int>(outputOffset[dimension]));

            // Calculate the total offset from beginning of memory:
            //   * if in the outermost loop, the offset into this dimension
            //   * otherwise, the offset into this dimension plus the previous offset scaled by the previous dimension's stride
            llvm::Value* thisInput1DimensionOffset = nullptr;
            llvm::Value* thisInput2DimensionOffset = nullptr;
            llvm::Value* thisOutputDimensionOffset = nullptr;
            if (dimension == 0)
            {
                assert(prevInput1DimensionOffset == nullptr);
                assert(prevInput2DimensionOffset == nullptr);
                assert(prevOutputDimensionOffset == nullptr);
                thisInput1DimensionOffset = thisInput1DimensionInternalOffset;
                thisInput2DimensionOffset = thisInput2DimensionInternalOffset;
                thisOutputDimensionOffset = thisOutputDimensionInternalOffset;
            }
            else
            {
                auto scaledInput1DimensionOffset = function.Operator(emitters::GetMultiplyForValueType<int>(), prevInput1DimensionOffset, function.Literal<int>(inputStride1[dimension]));
                auto scaledInput2DimensionOffset = function.Operator(emitters::GetMultiplyForValueType<int>(), prevInput2DimensionOffset, function.Literal<int>(inputStride2[dimension]));
                thisInput1DimensionOffset = function.Operator(emitters::GetAddForValueType<int>(), scaledInput1DimensionOffset, thisInput1DimensionInternalOffset);
                thisInput2DimensionOffset = function.Operator(emitters::GetAddForValueType<int>(), scaledInput2DimensionOffset, thisInput2DimensionInternalOffset);

                auto scaledOutputDimensionOffset = function.Operator(emitters::GetMultiplyForValueType<int>(), prevOutputDimensionOffset, function.Literal<int>(outputStride[dimension]));
                thisOutputDimensionOffset = function.Operator(emitters::GetAddForValueType<int>(), scaledOutputDimensionOffset, thisOutputDimensionInternalOffset);
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                EmitComputeDimensionLoop(compiler, function, dimension + 1, input1, input2, output, thisInput1DimensionOffset, thisInput2DimensionOffset, thisOutputDimensionOffset);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto value1 = function.ValueAt(input1, thisInput1DimensionOffset);
                auto value2 = function.ValueAt(input2, thisInput2DimensionOffset);
                auto outputValue = function.Operator(emitters::GetOperator<ValueType>(GetOperation()), value1, value2);
                function.SetValueAt(output, thisOutputDimensionOffset, outputValue);
            }
        });
    }

    template <typename ValueType>
    utilities::ArchiveVersion BinaryOperationNode<ValueType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v7_binary_operation_active_regions };

        return archiveVersion;
    }

    template <typename ValueType>
    bool BinaryOperationNode<ValueType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v7_binary_operation_active_regions };

        return version >= archiveVersion;
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInput1PortName] << _input1;
        archiver[defaultInput2PortName] << _input2;
        archiver["inputLayout1"] << _inputLayout1;
        archiver["inputLayout2"] << _inputLayout2;
        archiver["operation"] << BinaryOperations::to_string(_operation);
        archiver["outputLayout"] << _outputLayout;
        archiver["padding"] << _paddingValue;
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInput1PortName] >> _input1;
        archiver[defaultInput2PortName] >> _input2;
        archiver["inputLayout1"] >> _inputLayout1;
        archiver["inputLayout2"] >> _inputLayout2;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = BinaryOperations::from_string(operation);
        _output.SetSize(_input1.Size());
        archiver["outputLayout"] >> _outputLayout;
        archiver["padding"] >> _paddingValue;
    }
}
}
