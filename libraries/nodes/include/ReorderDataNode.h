////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReorderDataNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/Port.h>
#include <model/include/PortMemoryLayout.h>

#include <utilities/include/ArchiveVersion.h>
#include <utilities/include/IArchivable.h>

#include <algorithm>
#include <array>
#include <numeric>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that can reorder dimensions (transpose) and add or remove padding </summary>
    template <typename ValueType>
    class ReorderDataNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        ReorderDataNode();

        /// <summary> Constructor with no reordering </summary>
        ///
        /// <param name="input"> The input to reorder. </param>
        /// <param name="outputMemoryLayout"> The memory layout of the output. Data will be copied into the "active" area, and the rest will be zeroed out. </param>
        ReorderDataNode(const model::OutputPort<ValueType>& input, const model::PortMemoryLayout& outputMemoryLayout, ValueType paddingValue = 0);

        /// <summary> Constructor with no reordering </summary>
        ///
        /// <param name="input"> The input to reorder. </param>
        /// <param name="inputMemoryLayout"> The memory layout of the input. Only data in the "active" area will be copied. </param>
        /// <param name="outputMemoryLayout"> The memory layout of the output. Data will be copied into the "active" area, and the rest will be zeroed out. </param>
        ReorderDataNode(const model::OutputPort<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, ValueType paddingValue = 0);

        /// <summary> Constructor with reordering </summary>
        ///
        /// <param name="input"> The input to reorder. </param>
        /// <param name="order"> The permutation vector to apply to the dimensions when copying. Input dimension `i` will get copied to output dimension `order[i]`. If left empty, no reordering is done.
        //    For instance, to reorder the normal interleaved image order into a planar order, the `order` parameter would be
        ///   set to {2, 0, 1} --- reordering {row, column, channel} to {channel, row, column} </param>
        ReorderDataNode(const model::OutputPort<ValueType>& input, const model::DimensionOrder& order);

        /// <summary> Constructor with reordering </summary>
        ///
        /// <param name="input"> The input to reorder. </param>
        /// <param name="outputMemoryLayout"> The memory layout of the output. Data will be copied into the "active" area, and the rest will be zeroed out. </param>
        /// <param name="order"> The permutation vector to apply to the dimensions when copying. Input dimension `i` will get copied to output dimension `order[i]`. If left empty, no reordering is done.
        ///    For instance, to reorder the normal interleaved image order into a planar order, the `order` parameter would be
        ///    set to {2, 0, 1} --- reordering {row, column, channel} to {channel, row, column} </param>
        /// <param name="paddingValue"> The value to use for output padding, if output shape is larger than input shape. </param>
        ReorderDataNode(const model::OutputPort<ValueType>& input, const model::PortMemoryLayout& outputMemoryLayout, const model::DimensionOrder& order, ValueType paddingValue = 0);

        /// <summary> Constructor with reordering </summary>
        ///
        /// <param name="input"> The input to reorder. </param>
        /// <param name="inputMemoryLayout"> The memory layout of the input. Only data in the "active" area is guaranteed to be copied. </param>
        /// <param name="outputMemoryLayout"> The memory layout of the output. Data will be copied into the "active" area, and the rest will be zeroed out. </param>
        /// <param name="order"> The permutation vector to apply to the dimensions when copying. Input dimension `i` will get copied to output dimension `order[i]`. If left empty, no reordering is done.
        ///   For instance, to reorder the normal interleaved image order into a planar order, the `order` parameter would be
        ///   set to {2, 0, 1} --- reordering {row, column, channel} to {channel, row, column} </param>
        /// <param name="paddingValue"> The value to use for output padding, if output shape is larger than input shape. </param>
        ReorderDataNode(const model::OutputPort<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, const model::DimensionOrder& order, ValueType paddingValue = 0);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the input memory layout </summary>
        model::PortMemoryLayout GetOutputMemoryLayout() const { return _output.GetMemoryLayout(); }

        /// <summary> Returns padding value </summary>
        ///
        /// <returns> Padding value </returns>
        ValueType GetPaddingValue() const { return _paddingValue; }

        /// <summary> Returns true if the node can accept input with this memory layout order, else false </summary>
        ///
        /// <param name="order"> The memory layout order for all the input ports </summary>
        /// <returns> If the node can accept the input memory layout order, true, else false </returns>
        bool CanAcceptInputLayout(const utilities::DimensionOrder& order) const override
        {
            return GetInputMemoryLayout().GetLogicalDimensionOrder() == order;
        }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ReorderDataNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        model::MemoryCoordinates ReorderOutputToInputLocation(model::MemoryCoordinates outputLocation) const;
        std::vector<emitters::IRLocalScalar> ReorderOutputToInputLocation(std::vector<emitters::IRLocalScalar> outputLocation) const;

        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: inputMemoryLayout, paddingValue

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void ComputeDimensionLoop(const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, int dimension, std::vector<int>& coordinates, std::vector<ValueType>& output) const;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;

        ValueType _paddingValue;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/OutputNode.h>

#include <vector>

namespace ell
{
namespace nodes
{
    namespace ReorderDataNodeDetail
    {
        using emitters::IRLocalScalar;
        using model::DimensionOrder;
        using model::MemoryCoordinates;

        inline MemoryCoordinates LogicalToPhysical(const MemoryCoordinates& coordinates, const DimensionOrder& order)
        {
            const int numDimensions = coordinates.NumDimensions();
            std::vector<int> result(numDimensions);
            for (int index = 0; index < numDimensions; ++index)
            {
                result[index] = coordinates[order[index]];
            }
            return { result };
        }

        inline std::vector<IRLocalScalar> LogicalToPhysical(const std::vector<IRLocalScalar>& coordinates,
                                                            const DimensionOrder& order)
        {
            const int numDimensions = order.NumDimensions();
            // copying coordinates[0] just because IRLocalScalar doesn't have a default c'tor
            std::vector<IRLocalScalar> result(numDimensions, coordinates[0]);
            for (int index = 0; index < numDimensions; ++index)
            {
                result[index] = coordinates[order[index]];
            }
            return result;
        }

        inline MemoryCoordinates PhysicalToLogical(const MemoryCoordinates& coordinates, const DimensionOrder& order)
        {
            const int numDimensions = coordinates.NumDimensions();
            std::vector<int> result(numDimensions);
            for (int index = 0; index < numDimensions; ++index)
            {
                result[order[index]] = coordinates[index];
            }
            return { result };
        }

        inline std::vector<IRLocalScalar> PhysicalToLogical(const std::vector<IRLocalScalar>& coordinates,
                                                            const DimensionOrder& order)
        {
            const int numDimensions = order.NumDimensions();
            // copying coordinates[0] just because IRLocalScalar doesn't have a default c'tor
            std::vector<emitters::IRLocalScalar> result(numDimensions, coordinates[0]);
            for (int index = 0; index < numDimensions; ++index)
            {
                result[order[index]] = coordinates[index];
            }
            return result;
        }
    } // namespace ReorderDataNodeDetail

    //
    // ReorderDataNode
    //
    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {}

    //
    // Without reordering ("reshape" / slicing)
    //
    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::OutputPort<ValueType>& input,
                                                const model::PortMemoryLayout& outputMemoryLayout,
                                                ValueType paddingValue) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, outputMemoryLayout),
        _paddingValue(paddingValue)
    {
        _inputMemoryLayout = _input.GetMemoryLayout();
        if (_inputMemoryLayout.NumDimensions() != outputMemoryLayout.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            "Error: input and output layouts must have same dimension");
        }
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::OutputPort<ValueType>& input,
                                                const model::PortMemoryLayout& inputMemoryLayout,
                                                const model::PortMemoryLayout& outputMemoryLayout,
                                                ValueType paddingValue) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, outputMemoryLayout),
        _inputMemoryLayout(inputMemoryLayout),
        _paddingValue(paddingValue)
    {
        if (inputMemoryLayout.NumDimensions() != outputMemoryLayout.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            "Error: input and output layouts must have same dimension");
        }
    }

    //
    // With reordering ("reshape" / slicing, followed by transpose / dimension reordering)
    //
    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::OutputPort<ValueType>& input,
                                                const model::DimensionOrder& order) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, _input.GetMemoryLayout().ReorderedCopy(order))
    {
        _inputMemoryLayout = _input.GetMemoryLayout();
        if (_inputMemoryLayout.NumDimensions() != order.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            "Error: input and output layouts must have same dimension");
        }
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::OutputPort<ValueType>& input,
                                                const model::PortMemoryLayout& outputMemoryLayout,
                                                const model::DimensionOrder& order,
                                                ValueType paddingValue) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, outputMemoryLayout.ReorderedCopy(order)),
        _paddingValue(paddingValue)
    {
        _inputMemoryLayout = _input.GetMemoryLayout();
        if (_inputMemoryLayout.NumDimensions() != outputMemoryLayout.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            "Error: input and output layouts must have same dimension");
        }
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::OutputPort<ValueType>& input,
                                                const model::PortMemoryLayout& inputMemoryLayout,
                                                const model::PortMemoryLayout& outputMemoryLayout,
                                                const model::DimensionOrder& order,
                                                ValueType paddingValue) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, outputMemoryLayout.ReorderedCopy(order)),
        _inputMemoryLayout(inputMemoryLayout),
        _paddingValue(paddingValue)
    {
        if (inputMemoryLayout.NumDimensions() != outputMemoryLayout.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            "Error: input and output layouts must have same dimension");
        }
    }

    template <typename ValueType>
    model::MemoryCoordinates ReorderDataNode<ValueType>::ReorderOutputToInputLocation(
        model::MemoryCoordinates physicalOutputCoordinates) const
    {
        const auto inputDimensionOrder = GetInputMemoryLayout().GetLogicalDimensionOrder();
        const auto outputDimensionOrder = GetOutputMemoryLayout().GetLogicalDimensionOrder();

        auto logicalCoordinates =
            ReorderDataNodeDetail::PhysicalToLogical(physicalOutputCoordinates, outputDimensionOrder);
        auto physicalInputCoordinates =
            ReorderDataNodeDetail::LogicalToPhysical(logicalCoordinates, inputDimensionOrder);
        return physicalInputCoordinates;
    }

    // TODO: for each dimension, loop over minimum of input and output interval. Then we don't have to check if the value is out-of-bounds
    template <typename ValueType>
    std::vector<emitters::IRLocalScalar> ReorderDataNode<ValueType>::ReorderOutputToInputLocation(
        std::vector<emitters::IRLocalScalar> physicalOutputCoordinates) const
    {
        const auto inputDimensionOrder = GetInputMemoryLayout().GetLogicalDimensionOrder();
        const auto outputDimensionOrder = GetOutputMemoryLayout().GetLogicalDimensionOrder();

        auto logicalCoordinates =
            ReorderDataNodeDetail::PhysicalToLogical(physicalOutputCoordinates, outputDimensionOrder);
        auto physicalInputCoordinates =
            ReorderDataNodeDetail::LogicalToPhysical(logicalCoordinates, inputDimensionOrder);
        return physicalInputCoordinates;
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<ReorderDataNode>(newPortElements,
                                                            _inputMemoryLayout,
                                                            _output.GetMemoryLayout(),
                                                            _paddingValue);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::ComputeDimensionLoop(const model::PortMemoryLayout& inputMemoryLayout,
                                                          const model::PortMemoryLayout& outputMemoryLayout,
                                                          int dimension,
                                                          std::vector<int>& coordinates,
                                                          std::vector<ValueType>& output) const
    {
        if (dimension == inputMemoryLayout.NumDimensions() - 1) // last dimension
        {
            for (int index = 0; index < outputMemoryLayout.GetActiveSize(dimension); ++index)
            {
                coordinates[dimension] = index;

                auto inputLocation = ReorderOutputToInputLocation(coordinates);
                auto inputIndex = inputMemoryLayout.GetEntryOffset(inputLocation);
                auto outputIndex = outputMemoryLayout.GetEntryOffset(coordinates);
                output[outputIndex] = _input[inputIndex];
            }
        }
        else
        {
            for (int index = 0; index < outputMemoryLayout.GetActiveSize(dimension); ++index)
            {
                coordinates[dimension] = index;
                ComputeDimensionLoop(inputMemoryLayout, outputMemoryLayout, dimension + 1, coordinates, output);
            }
        }
    }

    // TODO: for each dimension, loop over minimum of input and output interval. Then we don't have to check if the value is out-of-bounds
    template <typename ValueType>
    void ReorderDataNode<ValueType>::Compute() const
    {
        const auto inputMemoryLayout = GetInputMemoryLayout();
        const auto outputMemoryLayout = _output.GetMemoryLayout();
        if (outputMemoryLayout == inputMemoryLayout)
        {
            _output.SetOutput(_input.GetValue());
        }
        else
        {
            const int numDimensions = inputMemoryLayout.NumDimensions();
            const int outputSize = outputMemoryLayout.GetMemorySize();
            if (numDimensions != outputMemoryLayout.NumDimensions())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                                "Error: input and output layouts must have same dimension");
            }

            std::vector<ValueType> output(outputSize, _paddingValue); // initialize to padding value
            std::vector<int> coordinates(numDimensions);
            ComputeDimensionLoop(inputMemoryLayout, outputMemoryLayout, 0, coordinates, output);
            _output.SetOutput(output);
        }
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        assert(this->input.Size() > 1);
        auto input = function.LocalArray(compiler.EnsurePortEmitted(this->input));
        auto output = function.LocalArray(compiler.EnsurePortEmitted(this->output, _paddingValue));

        const auto inputMemoryLayout = GetInputMemoryLayout();
        const auto outputMemoryLayout = GetOutputMemoryLayout();

        const int numDimensions = inputMemoryLayout.NumDimensions();
        const int outputSize = outputMemoryLayout.GetMemorySize();
        UNUSED(outputSize);

        std::vector<emitters::IRFunctionEmitter::ConstLoopRange> ranges;
        for (int dimensionIndex = 0; dimensionIndex < numDimensions; ++dimensionIndex)
        {
            ranges.push_back({ 0, outputMemoryLayout.GetActiveSize(dimensionIndex) });
        }

        function.For(ranges,
                     [input,
                      output,
                      inputMemoryLayout,
                      outputMemoryLayout,
                      this](emitters::IRFunctionEmitter& function, std::vector<emitters::IRLocalScalar> indices) {
                         auto inputLocation = ReorderOutputToInputLocation(indices);
                         auto inputIndex = model::EmitGetEntryOffset(function, inputLocation, inputMemoryLayout);
                         auto outputIndex = model::EmitGetEntryOffset(function, indices, outputMemoryLayout);
                         output[outputIndex] = input[inputIndex];
                     });
    }

    template <typename ValueType>
    ell::utilities::ArchiveVersion ReorderDataNode<ValueType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion currentArchiveVersion = {
            utilities::ArchiveVersionNumbers::v8_port_memory_layout
        };
        return std::max(currentArchiveVersion, CompilableNode::GetArchiveVersion());
    }

    template <typename ValueType>
    bool ReorderDataNode<ValueType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        return CompilableNode::CanReadArchiveVersion(version);
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["inputLayout"] << _inputMemoryLayout;
        archiver["outputLayout"] << GetOutputMemoryLayout();
        archiver["paddingValue"] << _paddingValue;
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["inputLayout"] >> _inputMemoryLayout;
        model::PortMemoryLayout outputMemoryLayout;
        if (archiver.HasNextPropertyName("outputLayout"))
        {
            // backward-compatability
            archiver["outputLayout"] >> outputMemoryLayout;

            if (archiver.HasNextPropertyName("order"))
            {
                std::vector<int> order;
                archiver["order"] >> order;
                outputMemoryLayout = model::PortMemoryLayout(outputMemoryLayout.GetActiveSize(),
                                                             outputMemoryLayout.GetExtent(),
                                                             outputMemoryLayout.GetOffset(),
                                                             outputMemoryLayout.GetCumulativeIncrement(),
                                                             order);
            }
            _output.SetMemoryLayout(outputMemoryLayout);
        }
        else
        {
            _output.SetMemoryLayout(_inputMemoryLayout);
            if (archiver.HasNextPropertyName("order"))
            {
                std::vector<int> order;
                archiver["order"] >> order;
                _output.SetMemoryLayout(GetOutputMemoryLayout().ReorderedCopy(order));
            }
        }

        if (archiver.HasNextPropertyName("order"))
        {
            std::vector<int> order;
            archiver["order"] >> order;
            _output.SetMemoryLayout(GetOutputMemoryLayout().ReorderedCopy(order));
        }

        archiver["paddingValue"] >> _paddingValue;
    }

} // namespace nodes
} // namespace ell

#pragma endregion implementation
