////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryOperationNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/PortMemoryLayout.h>

#include <emitters/include/EmitterTypes.h>
#include <emitters/include/LLVMUtilities.h>

#include <utilities/include/ArchiveVersion.h>
#include <utilities/include/Exception.h>
#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <type_traits>
#include <vector>

namespace ell
{
namespace nodes
    {
    /// <summary> Binary operations supported by BinaryOperationNode. </summary>
    enum class BinaryOperationType
    {
        none,
        add,
        subtract,
        multiply,
        divide,
        logicalAnd,
        logicalOr,
        logicalXor
    };

    /// <summary> A node that performs a coordinatewise binary arithmetic operation on its inputs. </summary>
    template <typename ValueType>
    class BinaryOperationNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        BinaryOperationNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the arithmetic expression. </param>
        /// <param name="input2"> The right-hand input of the arithmetic expression. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        ///
        /// Note: the output will use the same memory layout as input1
        BinaryOperationNode(const model::OutputPort<ValueType>& input1,
                            const model::OutputPort<ValueType>& input2,
                            BinaryOperationType operation);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the function. </param>
        /// <param name="input2"> The right-hand input of the function. </param>
        /// <param name="layout"> The memory layout for the inputs and outputs. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        /// <param name="padding"> The padding value. </param>
        BinaryOperationNode(const model::OutputPort<ValueType>& input1,
                            const model::OutputPort<ValueType>& input2,
                            const model::PortMemoryLayout& layout,
                            BinaryOperationType operation,
                            ValueType padding = 0);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the function. </param>
        /// <param name="inputLayout1"> The layout for the left-hand input. </param>
        /// <param name="input2"> The right-hand input of the function. </param>
        /// <param name="inputLayout2"> The layout for the right-hand input. </param>
        /// <param name="outputLayout"> The output layout. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        /// <param name="padding"> The padding value. </param>
        BinaryOperationNode(const model::OutputPort<ValueType>& input1,
                            const model::PortMemoryLayout& inputLayout1,
                            const model::OutputPort<ValueType>& input2,
                            const model::PortMemoryLayout& inputLayout2,
                            const model::PortMemoryLayout& outputLayout,
                            BinaryOperationType operation,
                            ValueType padding = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BinaryOperationNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Gets the operation performed by this node </summary>
        ///
        /// <returns> The operation </returns>
        BinaryOperationType GetOperation() const { return _operation; }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: operation

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler,
                                      emitters::IRFunctionEmitter& function,
                                      size_t dimension,
                                      emitters::LLVMValue input1,
                                      emitters::LLVMValue input2,
                                      emitters::LLVMValue output,
                                      emitters::LLVMValue prevInput1DimensionOffset,
                                      emitters::LLVMValue prevInput2DimensionOffset,
                                      emitters::LLVMValue prevOutputDimensionOffset) const;
        template <typename Operation>
        std::vector<ValueType> ComputeOutput(Operation&& function) const;
        template <typename Operation>
        void ComputeDimensionLoop(Operation& function,
                                  size_t dimension,
                                  std::vector<ValueType>& output,
                                  size_t prevInput1DimensionOffset,
                                  size_t prevInput2DimensionOffset,
                                  size_t prevOutputDimensionOffset) const;

        // Inputs
        model::InputPort<ValueType> _input1;
        model::PortMemoryLayout _inputLayout1;
        model::InputPort<ValueType> _input2;
        model::PortMemoryLayout _inputLayout2;

        // Output
        model::OutputPort<ValueType> _output;

        // Operation
        BinaryOperationType _operation;

        // Padding
        ValueType _paddingValue;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="model"> The Model or ModelTransformer to add the node to. </param>
    /// <param name="input1"> The left-hand input of the arithmetic expression. </param>
    /// <param name="input2"> The right-hand input of the arithmetic expression. </param>
    /// <param name="operation"> The type of operation to perform. </param>
    ///
    /// <returns> The output of the new node. </returns>
    /// Note: the output will use the same memory layout as input1
    template <typename ModelLikeType, typename ValueType>
    const model::OutputPort<ValueType>& AppendBinaryOperation(ModelLikeType& model,
                                                              const model::OutputPort<ValueType>& input1,
                                                              const model::OutputPort<ValueType>& input2,
                                                              BinaryOperationType operation);
} // namespace nodes
} // namespace ell

#pragma region implementation

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
        inline std::string to_string(BinaryOperationType op)
        {
            switch (op)
            {
                ADD_TO_STRING_ENTRY(BinaryOperationType, none);
                ADD_TO_STRING_ENTRY(BinaryOperationType, add);
                ADD_TO_STRING_ENTRY(BinaryOperationType, subtract);
                ADD_TO_STRING_ENTRY(BinaryOperationType, multiply);
                ADD_TO_STRING_ENTRY(BinaryOperationType, divide);
                ADD_TO_STRING_ENTRY(BinaryOperationType, logicalAnd);
                ADD_TO_STRING_ENTRY(BinaryOperationType, logicalOr);
                ADD_TO_STRING_ENTRY(BinaryOperationType, logicalXor);
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown binary operation");
            }
        }

        inline BinaryOperationType from_string(std::string name)
        {
            BEGIN_FROM_STRING;
            ADD_FROM_STRING_ENTRY(BinaryOperationType, none);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, add);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, subtract);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, multiply);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, divide);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, logicalAnd);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, logicalOr);
            ADD_FROM_STRING_ENTRY(BinaryOperationType, logicalXor);

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
    } // namespace BinaryOperations

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode() :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, {}, defaultInput1PortName),
        _input2(this, {}, defaultInput2PortName),
        _output(this, defaultOutputPortName, 0),
        _operation(BinaryOperationType::none)
    {
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, BinaryOperationType operation) :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, input1, defaultInput1PortName),
        _inputLayout1(input1.GetMemoryLayout()),
        _input2(this, input2, defaultInput2PortName),
        _inputLayout2(input2.GetMemoryLayout()),
        _output(this, defaultOutputPortName, input1.GetMemoryLayout()),
        _operation(operation),
        _paddingValue(0)
    {
        if (_inputLayout1.GetActiveSize() != _inputLayout2.GetActiveSize())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Active areas must match for both inputs");
        }
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::OutputPort<ValueType>& input1,
                                                        const model::OutputPort<ValueType>& input2,
                                                        const model::PortMemoryLayout& layout,
                                                        BinaryOperationType operation,
                                                        ValueType padding) :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, input1, defaultInput1PortName),
        _inputLayout1(layout),
        _input2(this, input2, defaultInput2PortName),
        _inputLayout2(layout),
        _output(this, defaultOutputPortName, layout),
        _operation(operation),
        _paddingValue(padding)
    {
    }

    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::OutputPort<ValueType>& input1,
                                                        const model::PortMemoryLayout& inputLayout1,
                                                        const model::OutputPort<ValueType>& input2,
                                                        const model::PortMemoryLayout& inputLayout2,
                                                        const model::PortMemoryLayout& outputLayout,
                                                        BinaryOperationType operation,
                                                        ValueType padding) :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, input1, defaultInput1PortName),
        _inputLayout1(inputLayout1),
        _input2(this, input2, defaultInput2PortName),
        _inputLayout2(inputLayout2),
        _output(this, defaultOutputPortName, outputLayout),
        _operation(operation),
        _paddingValue(padding)
    {
        if (inputLayout1.GetActiveSize() != inputLayout2.GetActiveSize())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Active areas must match for both inputs");
        }
        if (inputLayout1.GetActiveSize() != outputLayout.GetActiveSize())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output active areas must match");
        }
    }

    template <typename ValueType>
    template <typename Operation>
    std::vector<ValueType> BinaryOperationNode<ValueType>::ComputeOutput(Operation&& function) const
    {
        auto outputLayout = _output.GetMemoryLayout();
        auto outputSize = outputLayout.GetExtent().NumElements();
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
        case BinaryOperationType::add:
            output = ComputeOutput(BinaryOperations::Add<ValueType>);
            break;
        case BinaryOperationType::subtract:
            output = ComputeOutput(BinaryOperations::Subtract<ValueType>);
            break;
        case BinaryOperationType::multiply:
            output = ComputeOutput(BinaryOperations::Multiply<ValueType>);
            break;
        case BinaryOperationType::divide:
            output = ComputeOutput(BinaryOperations::Divide<ValueType>);
            break;
        case BinaryOperationType::logicalAnd:
            output = ComputeOutput(BinaryOperations::LogicalAnd<ValueType>);
            break;
        case BinaryOperationType::logicalOr:
            output = ComputeOutput(BinaryOperations::LogicalOr<ValueType>);
            break;
        case BinaryOperationType::logicalXor:
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
        const auto& PortElements1 = transformer.GetCorrespondingInputs(_input1);
        const auto& PortElements2 = transformer.GetCorrespondingInputs(_input2);
        auto outputLayout = _output.GetMemoryLayout();
        auto newNode = transformer.AddNode<BinaryOperationNode<ValueType>>(PortElements1, _inputLayout1, PortElements2, _inputLayout2, outputLayout, _operation);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (_inputLayout1.GetMemorySize() == _inputLayout2.GetMemorySize())
        {
            if (!compiler.GetCompilerOptions().unrollLoops)
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
            emitters::LLVMValue pInput1 = compiler.EnsurePortEmitted(input1);
            emitters::LLVMValue pInput2 = compiler.EnsurePortEmitted(input2);
            emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output, _paddingValue);

            // Call recursive function to emit nested loops
            emitters::LLVMValue prevInput1DimensionOffset = nullptr;
            emitters::LLVMValue prevInput2DimensionOffset = nullptr;
            emitters::LLVMValue prevOutputDimensionOffset = nullptr;
            EmitComputeDimensionLoop(compiler, function, 0, pInput1, pInput2, pResult, prevInput1DimensionOffset, prevInput2DimensionOffset, prevOutputDimensionOffset);
        }
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pInput1 = compiler.EnsurePortEmitted(input1);
        emitters::LLVMValue pInput2 = compiler.EnsurePortEmitted(input2);
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        auto count = input1.Size();
        function.VectorOperator(emitters::GetOperator<ValueType>(static_cast<emitters::BinaryOperatorType>(GetOperation())), count, pInput1, pInput2, [&pResult, &function](emitters::LLVMValue i, emitters::LLVMValue pValue) {
            function.SetValueAt(pResult, i, pValue);
        });
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        auto count = input1.Size();
        for (size_t i = 0; i < count; ++i)
        {
            emitters::LLVMValue inputValue1 = compiler.LoadPortElementVariable(input1.GetInputElement(i));
            emitters::LLVMValue inputValue2 = compiler.LoadPortElementVariable(input2.GetInputElement(i));
            emitters::LLVMValue pOpResult = function.Operator(emitters::GetOperator<ValueType>(static_cast<emitters::BinaryOperatorType>(GetOperation())), inputValue1, inputValue2);
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
        auto outputLayout = _output.GetMemoryLayout();
        const auto numDimensions = _inputLayout1.NumDimensions();
        auto&& inputStride1 = _inputLayout1.GetExtent();
        auto&& inputOffset1 = _inputLayout1.GetOffset();
        auto&& inputStride2 = _inputLayout2.GetExtent();
        auto&& inputOffset2 = _inputLayout2.GetOffset();
        auto&& inputSize = _inputLayout1.GetActiveSize();
        auto&& outputOffset = outputLayout.GetOffset();
        auto&& outputStride = outputLayout.GetExtent();

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

            if (static_cast<int>(dimension) < numDimensions - 1)
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
                                                                  emitters::LLVMValue input1,
                                                                  emitters::LLVMValue input2,
                                                                  emitters::LLVMValue output,
                                                                  emitters::LLVMValue prevInput1DimensionOffset,
                                                                  emitters::LLVMValue prevInput2DimensionOffset,
                                                                  emitters::LLVMValue prevOutputDimensionOffset) const
    {
        auto outputLayout = _output.GetMemoryLayout();
        const auto numDimensions = _inputLayout1.NumDimensions();
        auto&& inputStride1 = _inputLayout1.GetExtent();
        auto&& inputOffset1 = _inputLayout1.GetOffset();
        auto&& inputStride2 = _inputLayout2.GetExtent();
        auto&& inputOffset2 = _inputLayout2.GetOffset();
        auto&& inputSize = _inputLayout1.GetActiveSize();
        auto&& outputStride = outputLayout.GetExtent();
        auto&& outputOffset = outputLayout.GetOffset();

        function.For(inputSize[dimension], [input1, input2, output, inputOffset1, inputOffset2, inputStride1, inputStride2, outputStride, outputOffset, prevInput1DimensionOffset, prevInput2DimensionOffset, prevOutputDimensionOffset, dimension, numDimensions, &compiler, this](emitters::IRFunctionEmitter& function, emitters::LLVMValue loopIndex) {
            // Calculate the offset within this dimension = (loopIndex + offset[dimension])
            emitters::LLVMValue thisInput1DimensionInternalOffset = function.Operator(emitters::GetAddForValueType<int>(), loopIndex, function.Literal<int>(inputOffset1[dimension]));
            emitters::LLVMValue thisInput2DimensionInternalOffset = function.Operator(emitters::GetAddForValueType<int>(), loopIndex, function.Literal<int>(inputOffset2[dimension]));
            emitters::LLVMValue thisOutputDimensionInternalOffset = function.Operator(emitters::GetAddForValueType<int>(), loopIndex, function.Literal<int>(outputOffset[dimension]));

            // Calculate the total offset from beginning of memory:
            //   * if in the outermost loop, the offset into this dimension
            //   * otherwise, the offset into this dimension plus the previous offset scaled by the previous dimension's stride
            emitters::LLVMValue thisInput1DimensionOffset = nullptr;
            emitters::LLVMValue thisInput2DimensionOffset = nullptr;
            emitters::LLVMValue thisOutputDimensionOffset = nullptr;
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

            if (static_cast<int>(dimension) < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                EmitComputeDimensionLoop(compiler, function, dimension + 1, input1, input2, output, thisInput1DimensionOffset, thisInput2DimensionOffset, thisOutputDimensionOffset);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto value1 = function.ValueAt(input1, thisInput1DimensionOffset);
                auto value2 = function.ValueAt(input2, thisInput2DimensionOffset);
                auto outputValue = function.Operator(emitters::GetOperator<ValueType>(static_cast<emitters::BinaryOperatorType>(GetOperation())), value1, value2);
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
        auto outputLayout = _output.GetMemoryLayout();
        archiver["outputLayout"] << outputLayout;
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
        model::PortMemoryLayout outputLayout;
        archiver["outputLayout"] >> outputLayout;
        _output.SetMemoryLayout(outputLayout);
        archiver["padding"] >> _paddingValue;
    }

    template <typename ModelLikeType, typename ValueType>
    const model::OutputPort<ValueType>& AppendBinaryOperation(ModelLikeType& model,
                                                              const model::OutputPort<ValueType>& input1,
                                                              const model::OutputPort<ValueType>& input2,
                                                              BinaryOperationType operation)
    {
        static_assert(std::is_same_v<ModelLikeType, model::Model> || std::is_same_v<ModelLikeType, model::ModelTransformer>, "'model' parameter must be a model::Model or model::ModelTransformer");
        auto node = model.template AddNode<BinaryOperationNode<ValueType>>(input1, input2, operation);
        return node->output;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
