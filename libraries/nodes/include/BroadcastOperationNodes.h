////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BroadcastOperationNodes.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NodeOperations.h"

#include <emitters/include/CompilableFunction.h>
#include <emitters/include/IRAsyncTask.h>
#include <emitters/include/IREmitter.h>
#include <emitters/include/IRVectorUtilities.h>
#include <emitters/include/LLVMUtilities.h>

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/PortMemoryLayout.h>

#include <utilities/include/Exception.h>
#include <utilities/include/TypeName.h>

#include <functional>
#include <numeric>
#include <string>
#include <type_traits>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary>
    ///
    /// Broadcast operation nodes perform elementwise operations on multidimensional arrays, using "broadcast" semantics. If the
    /// shape of the operands doesn't match, any dimension with a size of '1' will be "broadcast" to match the other operands.
    /// For instance, if one input is a 2x3x4 array, the other input could be an array of size 1x3x4, 2x1x4, or 2x3x1 (in addition
    /// to matching exactly).
    ///
    /// BroadcastUnaryOperationNodes don't really broadcast anything, but perform unary operations of the form out = f(x), where x is
    /// an element from the primary input.
    ///
    /// </summary>

    // Base class for broadcast nodes
    template <typename ValueType, typename FunctionType>
    class BroadcastOperationNode : public model::CompilableNode
    {
    public:
        /// @name Output Port
        /// @{
        const model::OutputPort<ValueType>& output = _output;
        /// @}

    protected:
        BroadcastOperationNode(const std::vector<model::InputPortBase*>& inputsPortRefs,
                               const std::vector<const model::OutputPortBase*>& inputs,
                               ValueType padding = 0);

        BroadcastOperationNode(const std::vector<model::InputPortBase*>& inputsPortRefs,
                               const std::vector<const model::OutputPortBase*>& inputs,
                               const model::PortMemoryLayout& outputLayout,
                               ValueType padding = 0);

        int NumDimensions() const { return GetOutputMemoryLayout().NumDimensions(); }
        model::PortMemoryLayout GetOutputMemoryLayout() const;
        model::PortMemoryLayout GetInputMemoryLayout(int index) const;
        const model::OutputPort<ValueType>& GetOutput() const;
        const model::InputPort<ValueType>& GetInput(int index) const;

        const FunctionType& GetFunction() const;
        template <typename OpFunctionType>
        void SetFunction(OpFunctionType&& function);
        virtual ValueType ComputeOperation(const std::vector<ValueType>& args) const = 0;
        virtual emitters::IRLocalScalar CompileOperation(const std::vector<emitters::IRLocalScalar>& args) const = 0;

        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        bool HasState() const override { return true; } // stored state: function and padding value

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void ComputeDimensionLoop(int dimension,
                                  const std::vector<int>& prevInputDimensionOffsets,
                                  const std::vector<int>& lastActiveInputDimensions,
                                  const std::vector<ValueType>& inputValues,
                                  int prevOutputDimensionOffset,
                                  std::vector<ValueType>& output) const;

        void CompileDimensionLoop(model::IRMapCompiler& compiler,
                                  emitters::IRFunctionEmitter& function,
                                  int dimension,
                                  const std::vector<emitters::IRLocalArray>& inputs,
                                  const std::vector<emitters::IRLocalScalar>& prevInputDimensionOffsets,
                                  const std::vector<int>& lastActiveInputDimensions,
                                  const std::vector<emitters::IRLocalScalar>& inputValues,
                                  emitters::IRLocalScalar prevOutputDimensionOffset,
                                  emitters::IRLocalArray& output) const;
        std::vector<int> GetLastActiveInputDimensions() const;

        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;

        ValueType GetOutputPadding() const { return _paddingValue; }

        model::OutputPort<ValueType> _output;
        std::unique_ptr<FunctionType> _function;
        ValueType _paddingValue;
    };

    //
    // BroadcastUnaryOperationNode
    //
    template <typename ValueType>
    class BroadcastUnaryOperationNode : public BroadcastOperationNode<ValueType, UnaryFunctionType<ValueType>>
    {
    public:
        using OperationType = UnaryOperationType;
        using FunctionType = UnaryFunctionType<ValueType>;

        /// <summary> Default constructor. </summary>
        BroadcastUnaryOperationNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The input of the operation. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        ///
        /// Note: the output will use a contiguous block of memory in canonical order
        BroadcastUnaryOperationNode(const model::OutputPort<ValueType>& input,
                                    OperationType operation,
                                    ValueType padding = 0);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The input of the operation. </param>
        /// <param name="outputLayout"> The layout to use for the output. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        ///
        /// Note: the output will use a contiguous block of memory in canonical order
        BroadcastUnaryOperationNode(const model::OutputPort<ValueType>& input,
                                    const model::PortMemoryLayout& outputLayout,
                                    OperationType operation,
                                    ValueType padding = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastUnaryOperationNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        using BroadcastOperationNode<ValueType, FunctionType>::GetOutput;
        using BroadcastOperationNode<ValueType, FunctionType>::GetFunction;
        using BroadcastOperationNode<ValueType, FunctionType>::SetFunction;
        ValueType ComputeOperation(const std::vector<ValueType>& args) const override;
        emitters::IRLocalScalar CompileOperation(const std::vector<emitters::IRLocalScalar>& args) const override;

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void SetOperationFunction();
        void Copy(model::ModelTransformer& transformer) const override;

        model::InputPort<ValueType> _input;
        OperationType _operation = OperationType::none;
    };

    //
    // BroadcastBinaryOperationNode
    //
    template <typename ValueType>
    class BroadcastBinaryOperationNode : public BroadcastOperationNode<ValueType, BinaryFunctionType<ValueType>>
    {
    public:
        using OperationType = BinaryOperationType;
        using FunctionType = BinaryFunctionType<ValueType>;

        /// <summary> Default constructor. </summary>
        BroadcastBinaryOperationNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the operation. </param>
        /// <param name="input2"> The right-hand input of the operation. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        ///
        /// Note: the output will use a contiguous block of memory in canonical order
        BroadcastBinaryOperationNode(const model::OutputPort<ValueType>& input1,
                                     const model::OutputPort<ValueType>& input2,
                                     OperationType operation,
                                     ValueType padding = 0);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the operation. </param>
        /// <param name="input2"> The right-hand input of the operation. </param>
        /// <param name="outputLayout"> The layout to use for the output. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        ///
        /// Note: the output will use a contiguous block of memory in canonical order
        BroadcastBinaryOperationNode(const model::OutputPort<ValueType>& input1,
                                     const model::OutputPort<ValueType>& input2,
                                     const model::PortMemoryLayout& outputLayout,
                                     OperationType operation,
                                     ValueType padding = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastBinaryOperationNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        using BroadcastOperationNode<ValueType, FunctionType>::GetOutput;
        using BroadcastOperationNode<ValueType, FunctionType>::GetFunction;
        using BroadcastOperationNode<ValueType, FunctionType>::SetFunction;
        ValueType ComputeOperation(const std::vector<ValueType>& args) const override;
        emitters::IRLocalScalar CompileOperation(const std::vector<emitters::IRLocalScalar>& args) const override;

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void SetOperationFunction();
        void Copy(model::ModelTransformer& transformer) const override;

        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;
        OperationType _operation = OperationType::none;
    };

    //
    // BroadcastTernaryOperationNode
    //
    template <typename ValueType>
    class BroadcastTernaryOperationNode : public BroadcastOperationNode<ValueType, TernaryFunctionType<ValueType>>
    {
    public:
        using OperationType = TernaryOperationType;
        using FunctionType = TernaryFunctionType<ValueType>;

        /// <summary> Default constructor. </summary>
        BroadcastTernaryOperationNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The first input of the operation. </param>
        /// <param name="input2"> The second input of the operation. </param>
        /// <param name="input3"> The third input of the operation. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        ///
        /// Note: the output will use a contiguous block of memory in canonical order
        BroadcastTernaryOperationNode(const model::OutputPort<ValueType>& input1,
                                      const model::OutputPort<ValueType>& input2,
                                      const model::OutputPort<ValueType>& input3,
                                      OperationType operation,
                                      ValueType padding = 0);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The first input of the operation. </param>
        /// <param name="input2"> The second input of the operation. </param>
        /// <param name="input3"> The third input of the operation. </param>
        /// <param name="outputLayout"> The layout to use for the output. </param>
        /// <param name="operation"> The type of operation to perform. </param>
        ///
        /// Note: the output will use a contiguous block of memory in canonical order
        BroadcastTernaryOperationNode(const model::OutputPort<ValueType>& input1,
                                      const model::OutputPort<ValueType>& input2,
                                      const model::OutputPort<ValueType>& input3,
                                      const model::PortMemoryLayout& outputLayout,
                                      OperationType operation,
                                      ValueType padding = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BroadcastTernaryOperationNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        using BroadcastOperationNode<ValueType, FunctionType>::GetOutput;
        using BroadcastOperationNode<ValueType, FunctionType>::GetFunction;
        using BroadcastOperationNode<ValueType, FunctionType>::SetFunction;
        ValueType ComputeOperation(const std::vector<ValueType>& args) const override;
        emitters::IRLocalScalar CompileOperation(const std::vector<emitters::IRLocalScalar>& args) const override;

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void SetOperationFunction();
        void Copy(model::ModelTransformer& transformer) const override;

        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;
        model::InputPort<ValueType> _input3;
        OperationType _operation = OperationType::none;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    utilities::MemoryLayout ComputeBroadcastedLayout(const std::vector<utilities::MemoryLayout>& arguments);
    utilities::MemoryLayout ComputeBroadcastedLayout(const std::vector<const model::OutputPortBase*>& arguments);
    void VerifyLayoutsCompatible(const std::vector<utilities::MemoryLayout>& inputs);

    //
    // BroadcastOperationNode
    //
    template <typename ValueType, typename FunctionType>
    BroadcastOperationNode<ValueType, FunctionType>::BroadcastOperationNode(const std::vector<model::InputPortBase*>& inputPortRefs,
                                                                            const std::vector<const model::OutputPortBase*>& inputs,
                                                                            ValueType paddingValue) :
        CompilableNode(inputPortRefs, { &_output }),
        _output(this, ell::model::Node::defaultOutputPortName, ComputeBroadcastedLayout(inputs)),
        _paddingValue(paddingValue)
    {
    }

    template <typename ValueType, typename FunctionType>
    BroadcastOperationNode<ValueType, FunctionType>::BroadcastOperationNode(const std::vector<model::InputPortBase*>& inputPortRefs,
                                                                            const std::vector<const model::OutputPortBase*>& inputs,
                                                                            const model::PortMemoryLayout& outputLayout,
                                                                            ValueType paddingValue) :
        CompilableNode(inputPortRefs, { &_output }),
        _output(this, ell::model::Node::defaultOutputPortName, outputLayout),
        _paddingValue(paddingValue)
    {
        auto canonicalOutputLayout = ComputeBroadcastedLayout(inputs);
        VerifyLayoutsCompatible({ outputLayout, canonicalOutputLayout });
    }

    template <typename ValueType, typename FunctionType>
    model::PortMemoryLayout BroadcastOperationNode<ValueType, FunctionType>::GetOutputMemoryLayout() const
    {
        return GetOutputPort(0)->GetMemoryLayout();
    }

    template <typename ValueType, typename FunctionType>
    model::PortMemoryLayout BroadcastOperationNode<ValueType, FunctionType>::GetInputMemoryLayout(int inputIndex) const
    {
        return GetInputPort(inputIndex)->GetMemoryLayout();
    }

    template <typename ValueType, typename FunctionType>
    const model::OutputPort<ValueType>& BroadcastOperationNode<ValueType, FunctionType>::GetOutput() const
    {
        return _output;
    }

    template <typename ValueType, typename FunctionType>
    const model::InputPort<ValueType>& BroadcastOperationNode<ValueType, FunctionType>::GetInput(int index) const
    {
        return static_cast<const model::InputPort<ValueType>&>(*GetInputPort(index));
    }

    template <typename ValueType, typename FunctionType>
    template <typename OpFunctionType>
    void BroadcastOperationNode<ValueType, FunctionType>::SetFunction(OpFunctionType&& function)
    {
        _function = std::make_unique<OpFunctionType>(std::move(function));
    }

    template <typename ValueType, typename FunctionType>
    const FunctionType& BroadcastOperationNode<ValueType, FunctionType>::GetFunction() const
    {
        return *_function;
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

    template <typename ValueType, typename FunctionType>
    std::vector<int> BroadcastOperationNode<ValueType, FunctionType>::GetLastActiveInputDimensions() const
    {
        const auto numDimensions = NumDimensions();
        auto numInputs = NumInputPorts();
        std::vector<int> lastActiveInputDimensions(numInputs, 0);
        for (int i = 0; i < numInputs; ++i)
        {
            const auto& inputLayout = GetInput(i).GetMemoryLayout();
            const auto& activeSize = inputLayout.GetLogicalDimensionActiveSize();
            for (int j = numDimensions - 1; j >= 0; --j)
            {
                if (activeSize[j] != 1)
                {
                    lastActiveInputDimensions[i] = j;
                    break;
                }
            }
        }

        return lastActiveInputDimensions;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastOperationNode<ValueType, FunctionType>::ComputeDimensionLoop(int dimension,
                                                                               const std::vector<int>& prevInputDimensionOffsetsIn,
                                                                               const std::vector<int>& lastActiveInputDimensions,
                                                                               const std::vector<ValueType>& inputValuesIn,
                                                                               int prevOutputDimensionOffset,
                                                                               std::vector<ValueType>& output) const
    {
        auto prevInputDimensionOffsets = prevInputDimensionOffsetsIn;
        auto inputValues = inputValuesIn;
        const auto& outputLayout = GetOutputMemoryLayout();
        const auto outputGlobalOffset = outputLayout.GetFirstEntryOffset();
        const auto& outputSize = outputLayout.GetLogicalDimensionActiveSize();
        const auto& outputIncrement = outputLayout.GetLogicalDimensionIncrement();

        const auto numDimensions = outputLayout.NumDimensions();
        const auto numInputs = NumInputPorts();

        for (int loopIndex = 0; loopIndex < outputSize[dimension]; ++loopIndex)
        {
            auto thisOutputDimensionOffset = prevOutputDimensionOffset + loopIndex * outputIncrement[dimension];
            std::vector<int> thisInputDimensionOffsets(numInputs, 0);
            for (int inputIndex = 0; inputIndex < numInputs; ++inputIndex)
            {
                const auto& input = GetInput(inputIndex);
                const auto& inputLayout = input.GetMemoryLayout();
                const auto inputGlobalOffset = inputLayout.GetFirstEntryOffset();
                const auto& inputSize = inputLayout.GetLogicalDimensionActiveSize();
                const auto& inputIncrement = inputLayout.GetLogicalDimensionIncrement();

                // Account for broadcasting dimensions by setting loopIndex to 0 if this is a broadcast dimension for this input
                auto thisLoopIndex = inputSize[dimension] == 1 ? 0 : loopIndex;
                auto thisInputDimensionOffset = prevInputDimensionOffsets[inputIndex] + thisLoopIndex * inputIncrement[dimension];
                thisInputDimensionOffsets[inputIndex] = thisInputDimensionOffset;
                if (dimension == lastActiveInputDimensions[inputIndex])
                {
                    inputValues[inputIndex] = input[inputGlobalOffset + thisInputDimensionOffset];
                }
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                ComputeDimensionLoop(dimension + 1, thisInputDimensionOffsets, lastActiveInputDimensions, inputValues, thisOutputDimensionOffset, output);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto outputValue = ComputeOperation(inputValues);
                output[outputGlobalOffset + thisOutputDimensionOffset] = outputValue;
            }
        }
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastOperationNode<ValueType, FunctionType>::CompileDimensionLoop(model::IRMapCompiler& compiler,
                                                                               emitters::IRFunctionEmitter& function,
                                                                               int dimension,
                                                                               const std::vector<emitters::IRLocalArray>& inputsIn,
                                                                               const std::vector<emitters::IRLocalScalar>& prevInputDimensionOffsetsIn,
                                                                               const std::vector<int>& lastActiveInputDimensions,
                                                                               const std::vector<emitters::IRLocalScalar>& inputValuesIn,
                                                                               emitters::IRLocalScalar prevOutputDimensionOffset,
                                                                               emitters::IRLocalArray& output) const
    {
        auto inputs = inputsIn;
        auto prevInputDimensionOffsets = prevInputDimensionOffsetsIn;
        auto inputValues = inputValuesIn;

        model::PortMemoryLayout outputLayout = GetOutputMemoryLayout();
        const auto outputGlobalOffset = static_cast<int>(outputLayout.GetFirstEntryOffset());
        const auto& outputSize = outputLayout.GetLogicalDimensionActiveSize();
        const auto& outputIncrement = outputLayout.GetLogicalDimensionIncrement();

        const auto numDimensions = outputLayout.NumDimensions();
        const auto numInputs = NumInputPorts();

        function.For(0, outputSize[dimension], [&](emitters::IRFunctionEmitter& function, auto loopIndex) {
            auto thisOutputDimensionOffset = prevOutputDimensionOffset + loopIndex * outputIncrement[dimension];
            std::vector<emitters::IRLocalScalar> thisInputDimensionOffsets(numInputs, function.LocalScalar<int>(0));
            for (int inputIndex = 0; inputIndex < numInputs; ++inputIndex)
            {
                const auto& inputPort = GetInput(inputIndex);
                const auto& inputLayout = inputPort.GetMemoryLayout();
                const auto inputGlobalOffset = static_cast<int>(inputLayout.GetFirstEntryOffset());
                const auto& inputSize = inputLayout.GetLogicalDimensionActiveSize();
                const auto& inputIncrement = inputLayout.GetLogicalDimensionIncrement();
                const auto& input = inputs[inputIndex];

                // Account for broadcasting dimensions by setting loopIndex to 0 if this is a broadcast dimension for this input
                auto thisLoopIndex = inputSize[dimension] == 1 ? function.LocalScalar<int>(0) : loopIndex;
                auto thisInputDimensionOffset = prevInputDimensionOffsets[inputIndex] + thisLoopIndex * inputIncrement[dimension];
                thisInputDimensionOffsets[inputIndex] = thisInputDimensionOffset;
                if (dimension == lastActiveInputDimensions[inputIndex])
                {
                    inputValues[inputIndex] = input[thisInputDimensionOffset + inputGlobalOffset];
                }
            }

            if (dimension < numDimensions - 1)
            {
                // Recursive call to emit nested loop
                CompileDimensionLoop(compiler, function, dimension + 1, inputs, thisInputDimensionOffsets, lastActiveInputDimensions, inputValues, thisOutputDimensionOffset, output);
            }
            else
            {
                // We're in the innermost loop --- compute the value
                auto outputValue = CompileOperation(inputValues);
                output[outputGlobalOffset + thisOutputDimensionOffset] = outputValue;
            }
        });
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastOperationNode<ValueType, FunctionType>::Compute() const
    {
        const auto& outputLayout = GetOutputMemoryLayout();
        const auto numInputs = NumInputPorts();

        auto outputSize = outputLayout.GetMemorySize();
        auto output = std::vector<ValueType>(outputSize);

        const int startDimension = 0;
        std::vector<int> prevInputOffsets(numInputs, 0);
        auto lastActiveInputDimensions = GetLastActiveInputDimensions();
        std::vector<ValueType> inputValues(numInputs);
        const int startOffset = 0;
        ComputeDimensionLoop(startDimension, prevInputOffsets, lastActiveInputDimensions, inputValues, startOffset, output);

        GetOutput().SetOutput(output);
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastOperationNode<ValueType, FunctionType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const auto numInputs = NumInputPorts();

        std::vector<emitters::IRLocalArray> inputs;
        for (int index = 0; index < numInputs; ++index)
        {
            const auto& inputPort = GetInput(index);
            auto inputVar = function.LocalArray(compiler.EnsurePortEmitted(inputPort));
            inputs.push_back(inputVar);
        }

        auto output = function.LocalArray(compiler.EnsurePortEmitted(GetOutput(), this->GetOutputPadding()));

        const int startDimension = 0;
        std::vector<emitters::IRLocalScalar> prevInputOffsets(numInputs, function.LocalScalar<int>(0));
        auto lastActiveInputDimensions = GetLastActiveInputDimensions();
        std::vector<emitters::IRLocalScalar> inputValues(numInputs, function.LocalScalar());
        const emitters::IRLocalScalar startOffset = function.LocalScalar<int>(0);
        CompileDimensionLoop(compiler, function, startDimension, inputs, prevInputOffsets, lastActiveInputDimensions, inputValues, startOffset, output);
    }

    template <typename ValueType, typename FunctionType>
    utilities::ArchiveVersion BroadcastOperationNode<ValueType, FunctionType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v5_refined_nodes };

        return archiveVersion;
    }

    template <typename ValueType, typename FunctionType>
    bool BroadcastOperationNode<ValueType, FunctionType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v5_refined_nodes };

        return version >= archiveVersion;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastOperationNode<ValueType, FunctionType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);
        auto outputLayout = GetOutputMemoryLayout();
        archiver["outputLayout"] << outputLayout;
        archiver["padding"] << _paddingValue;
    }

    template <typename ValueType, typename FunctionType>
    void BroadcastOperationNode<ValueType, FunctionType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);
        model::PortMemoryLayout outputLayout;
        archiver["outputLayout"] >> outputLayout;
        _output.SetMemoryLayout(outputLayout);
        archiver["padding"] >> _paddingValue;
    }

    //
    // BroadcastUnaryOperationNode
    //
    template <typename ValueType>
    BroadcastUnaryOperationNode<ValueType>::BroadcastUnaryOperationNode() :
        BroadcastOperationNode<ValueType, FunctionType>({ &_input }, {}, static_cast<ValueType>(0)),
        _input(this, {}, model::CompilableNode::defaultInputPortName),
        _operation(OperationType::none)
    {
    }

    template <typename ValueType>
    BroadcastUnaryOperationNode<ValueType>::BroadcastUnaryOperationNode(const model::OutputPort<ValueType>& input, OperationType operation, ValueType paddingValue) :
        BroadcastOperationNode<ValueType, FunctionType>({ &_input }, { &input }, paddingValue),
        _input(this, input, model::CompilableNode::defaultInputPortName),
        _operation(operation)
    {
        SetOperationFunction();
    }

    template <typename ValueType>
    BroadcastUnaryOperationNode<ValueType>::BroadcastUnaryOperationNode(const model::OutputPort<ValueType>& input, const model::PortMemoryLayout& outputLayout, OperationType operation, ValueType paddingValue) :
        BroadcastOperationNode<ValueType, FunctionType>({ &_input }, { &input }, outputLayout, paddingValue),
        _input(this, input, model::CompilableNode::defaultInputPortName),
        _operation(operation)
    {
        SetOperationFunction();
    }

    template <typename ValueType>
    ValueType BroadcastUnaryOperationNode<ValueType>::ComputeOperation(const std::vector<ValueType>& args) const
    {
        if (args.size() != 1)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return GetFunction().Compute(args[0]);
    }

    template <typename ValueType>
    emitters::IRLocalScalar BroadcastUnaryOperationNode<ValueType>::CompileOperation(const std::vector<emitters::IRLocalScalar>& args) const
    {
        if (args.size() != 1)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return GetFunction().Compile(args[0].function, args[0]);
    }

    template <typename ValueType>
    void BroadcastUnaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<BroadcastUnaryOperationNode<ValueType>>(newInput,
                                                                                   this->GetOutputMemoryLayout(),
                                                                                   _operation);
        transformer.MapNodeOutput(GetOutput(), newNode->output);
    }

    template <typename ValueType>
    void BroadcastUnaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BroadcastOperationNode<ValueType, FunctionType>::WriteToArchive(archiver);
        archiver[model::CompilableNode::defaultInputPortName] << _input;
        archiver["operation"] << ToString(_operation);
    }

    template <typename ValueType>
    void BroadcastUnaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BroadcastOperationNode<ValueType, FunctionType>::ReadFromArchive(archiver);
        archiver[model::CompilableNode::defaultInputPortName] >> _input;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = FromString<UnaryOperationType>(operation);
        SetOperationFunction();
    }

    //
    // BroadcastBinaryOperationNode
    //
    template <typename ValueType>
    BroadcastBinaryOperationNode<ValueType>::BroadcastBinaryOperationNode() :
        BroadcastOperationNode<ValueType, FunctionType>({ &_input1, &_input2 }, {}),
        _input1(this, {}, model::CompilableNode::defaultInput1PortName),
        _input2(this, {}, model::CompilableNode::defaultInput2PortName),
        _operation(OperationType::none)
    {
    }

    template <typename ValueType>
    BroadcastBinaryOperationNode<ValueType>::BroadcastBinaryOperationNode(const model::OutputPort<ValueType>& input1,
                                                                          const model::OutputPort<ValueType>& input2,
                                                                          OperationType operation,
                                                                          ValueType paddingValue) :
        BroadcastOperationNode<ValueType, FunctionType>({ &_input1, &_input2 }, { &input1, &input2 }, paddingValue),
        _input1(this, input1, model::CompilableNode::defaultInput1PortName),
        _input2(this, input2, model::CompilableNode::defaultInput2PortName),
        _operation(operation)
    {
        SetOperationFunction();
    }

    template <typename ValueType>
    BroadcastBinaryOperationNode<ValueType>::BroadcastBinaryOperationNode(const model::OutputPort<ValueType>& input1,
                                                                          const model::OutputPort<ValueType>& input2,
                                                                          const model::PortMemoryLayout& outputLayout,
                                                                          OperationType operation,
                                                                          ValueType paddingValue) :
        BroadcastOperationNode<ValueType, FunctionType>({ &_input1, &_input2 }, { &input1, &input2 }, outputLayout, paddingValue),
        _input1(this, input1, model::CompilableNode::defaultInput1PortName),
        _input2(this, input2, model::CompilableNode::defaultInput2PortName),
        _operation(operation)
    {
        SetOperationFunction();
    }

    template <typename ValueType>
    ValueType BroadcastBinaryOperationNode<ValueType>::ComputeOperation(const std::vector<ValueType>& args) const
    {
        if (args.size() != 2)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return GetFunction().Compute(args[0], args[1]);
    }

    template <typename ValueType>
    emitters::IRLocalScalar BroadcastBinaryOperationNode<ValueType>::CompileOperation(const std::vector<emitters::IRLocalScalar>& args) const
    {
        if (args.size() != 2)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return GetFunction().Compile(args[0].function, args[0], args[1]);
    }

    template <typename ValueType>
    void BroadcastBinaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput1 = transformer.GetCorrespondingInputs(_input1);
        const auto& newInput2 = transformer.GetCorrespondingInputs(_input2);
        auto newNode = transformer.AddNode<BroadcastBinaryOperationNode<ValueType>>(newInput1,
                                                                                    newInput2,
                                                                                    this->GetOutputMemoryLayout(),
                                                                                    _operation);
        transformer.MapNodeOutput(GetOutput(), newNode->output);
    }

    template <typename ValueType>
    void BroadcastBinaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BroadcastOperationNode<ValueType, FunctionType>::WriteToArchive(archiver);
        archiver[model::CompilableNode::defaultInput1PortName] << _input1;
        archiver[model::CompilableNode::defaultInput1PortName] << _input2;
        archiver["operation"] << ToString(_operation);
    }

    template <typename ValueType>
    void BroadcastBinaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BroadcastOperationNode<ValueType, FunctionType>::ReadFromArchive(archiver);
        archiver[model::CompilableNode::defaultInput1PortName] >> _input1;
        archiver[model::CompilableNode::defaultInput1PortName] >> _input2;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = FromString<BinaryOperationType>(operation);
        SetOperationFunction();
    }

    //
    // BroadcastTernaryOperationNode
    //
    template <typename ValueType>
    BroadcastTernaryOperationNode<ValueType>::BroadcastTernaryOperationNode() :
        BroadcastOperationNode<ValueType, FunctionType>({ &_input1, &_input2, &_input3 }, {}),
        _input1(this, {}, model::CompilableNode::defaultInput1PortName),
        _input2(this, {}, model::CompilableNode::defaultInput2PortName),
        _input3(this, {}, model::CompilableNode::defaultInput3PortName),
        _operation(OperationType::none)
    {
    }

    template <typename ValueType>
    BroadcastTernaryOperationNode<ValueType>::BroadcastTernaryOperationNode(const model::OutputPort<ValueType>& input1,
                                                                            const model::OutputPort<ValueType>& input2,
                                                                            const model::OutputPort<ValueType>& input3,
                                                                            OperationType operation,
                                                                            ValueType paddingValue) :
        BroadcastOperationNode<ValueType, FunctionType>({ &_input1, &_input2, &_input3 }, { &input1, &input2, &input3 }, paddingValue),
        _input1(this, input1, model::CompilableNode::defaultInput1PortName),
        _input2(this, input2, model::CompilableNode::defaultInput2PortName),
        _input3(this, input3, model::CompilableNode::defaultInput3PortName),
        _operation(operation)
    {
        SetOperationFunction();
    }

    template <typename ValueType>
    BroadcastTernaryOperationNode<ValueType>::BroadcastTernaryOperationNode(const model::OutputPort<ValueType>& input1,
                                                                            const model::OutputPort<ValueType>& input2,
                                                                            const model::OutputPort<ValueType>& input3,
                                                                            const model::PortMemoryLayout& outputLayout,
                                                                            OperationType operation,
                                                                            ValueType paddingValue) :
        BroadcastOperationNode<ValueType, FunctionType>({ &_input1, &_input2, &_input3 }, { &input1, &input2, &input3 }, outputLayout, paddingValue),
        _input1(this, input1, model::CompilableNode::defaultInput1PortName),
        _input2(this, input2, model::CompilableNode::defaultInput2PortName),
        _input3(this, input3, model::CompilableNode::defaultInput3PortName),
        _operation(operation)
    {
        SetOperationFunction();
    }

    template <typename ValueType>
    ValueType BroadcastTernaryOperationNode<ValueType>::ComputeOperation(const std::vector<ValueType>& args) const
    {
        if (args.size() != 3)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return GetFunction().Compute(args[0], args[1], args[2]);
    }

    template <typename ValueType>
    emitters::IRLocalScalar BroadcastTernaryOperationNode<ValueType>::CompileOperation(const std::vector<emitters::IRLocalScalar>& args) const
    {
        if (args.size() != 3)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return GetFunction().Compile(args[0].function, args[0], args[1], args[2]);
    }

    template <typename ValueType>
    void BroadcastTernaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput1 = transformer.GetCorrespondingInputs(_input1);
        const auto& newInput2 = transformer.GetCorrespondingInputs(_input2);
        const auto& newInput3 = transformer.GetCorrespondingInputs(_input3);
        auto newNode = transformer.AddNode<BroadcastTernaryOperationNode<ValueType>>(newInput1,
                                                                                     newInput2,
                                                                                     newInput3,
                                                                                     this->GetOutputMemoryLayout(),
                                                                                     _operation);
        transformer.MapNodeOutput(GetOutput(), newNode->output);
    }

    template <typename ValueType>
    void BroadcastTernaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        BroadcastOperationNode<ValueType, FunctionType>::WriteToArchive(archiver);
        archiver[model::CompilableNode::defaultInput1PortName] << _input1;
        archiver[model::CompilableNode::defaultInput2PortName] << _input2;
        archiver[model::CompilableNode::defaultInput3PortName] << _input3;
        archiver["operation"] << ToString(_operation);
    }

    template <typename ValueType>
    void BroadcastTernaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        BroadcastOperationNode<ValueType, FunctionType>::ReadFromArchive(archiver);
        archiver[model::CompilableNode::defaultInput1PortName] >> _input1;
        archiver[model::CompilableNode::defaultInput2PortName] >> _input2;
        archiver[model::CompilableNode::defaultInput3PortName] >> _input3;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = FromString<TernaryOperationType>(operation);
        SetOperationFunction();
    }

    template <typename ValueType>
    void BroadcastUnaryOperationNode<ValueType>::SetOperationFunction()
    {
        switch (_operation)
        {
        case UnaryOperationType::abs:
            SetFunction(AbsFunction<ValueType>());
            break;
        case UnaryOperationType::exp:
            SetFunction(ExpFunction<ValueType>());
            break;
        case UnaryOperationType::log:
            SetFunction(LogFunction<ValueType>());
            break;
        case UnaryOperationType::sqrt:
            SetFunction(SqrtFunction<ValueType>());
            break;
        case UnaryOperationType::logicalNot:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Not implemented");
        case UnaryOperationType::tanh:
            SetFunction(TanhFunction<ValueType>());
            break;
        case UnaryOperationType::square:
            SetFunction(SquareFunction<ValueType>());
            break;
        case UnaryOperationType::sin:
            SetFunction(SinFunction<ValueType>());
            break;
        case UnaryOperationType::cos:
            SetFunction(CosFunction<ValueType>());
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Illegal operation");
        }
    }

    template <typename ValueType>
    void BroadcastBinaryOperationNode<ValueType>::SetOperationFunction()
    {
        switch (_operation)
        {
        case BinaryOperationType::add:
            SetFunction(AddFunction<ValueType>());
            break;
        case BinaryOperationType::subtract:
            SetFunction(SubtractFunction<ValueType>());
            break;
        case BinaryOperationType::multiply:
            SetFunction(MultiplyFunction<ValueType>());
            break;
        case BinaryOperationType::divide:
            SetFunction(DivideFunction<ValueType>());
            break;
        case BinaryOperationType::logicalAnd:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Not implemented");
        case BinaryOperationType::logicalOr:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Not implemented");
        case BinaryOperationType::logicalXor:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Not implemented");
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Illegal operation");
        }
    }

    template <typename ValueType>
    void BroadcastTernaryOperationNode<ValueType>::SetOperationFunction()
    {
        switch (_operation)
        {
        case TernaryOperationType::fma:
            SetFunction(FMAFunction<ValueType>());
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Illegal operation");
        }
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
