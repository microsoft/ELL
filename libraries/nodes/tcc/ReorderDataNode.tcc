////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReorderDataNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// model
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "OutputNode.h"

// stl
#include <vector>

namespace ell
{
namespace nodes
{
    namespace ReorderDataNodeDetail
    {
        inline model::MemoryCoordinates LogicalToPhysical(const model::MemoryCoordinates& coordinates, const model::DimensionOrder& order)
        {
            const int numDimensions = coordinates.NumDimensions();
            std::vector<int> result(numDimensions);
            for(int index = 0; index < numDimensions; ++index)
            {
                result[index] = coordinates[order[index]];
            }
            return { result };
        }

        inline std::vector<emitters::IRLocalScalar> LogicalToPhysical(const std::vector<emitters::IRLocalScalar>& coordinates, const model::DimensionOrder& order)
        {
            const int numDimensions = order.NumDimensions();
            std::vector<emitters::IRLocalScalar> result(numDimensions, coordinates[0]); // copying coordinates[0] just because IRLocalScalar doesn't have a default c'tor
            for(int index = 0; index < numDimensions; ++index)
            {
                result[index] = coordinates[order[index]];
            }
            return result;
        }

        inline model::MemoryCoordinates PhysicalToLogical(const model::MemoryCoordinates& coordinates, const model::DimensionOrder& order)
        {
            const int numDimensions = coordinates.NumDimensions();
            std::vector<int> result(numDimensions);
            for(int index = 0; index < numDimensions; ++index)
            {
                result[order[index]] = coordinates[index];
            }
            return { result };
        }

        inline std::vector<emitters::IRLocalScalar> PhysicalToLogical(const std::vector<emitters::IRLocalScalar>& coordinates, const model::DimensionOrder& order)
        {
            const int numDimensions = order.NumDimensions();
            std::vector<emitters::IRLocalScalar> result(numDimensions, coordinates[0]); // copying coordinates[0] just because IRLocalScalar doesn't have a default c'tor
            for(int index = 0; index < numDimensions; ++index)
            {
                result[order[index]] = coordinates[index];
            }
            return result;
        }
    }

    //
    // ReorderDataNode
    //
    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    //
    // Without reordering ("reshape" / slicing)
    //
    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& outputMemoryLayout, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _paddingValue(paddingValue)
    {
        _inputMemoryLayout = _input.GetMemoryLayout();
        if (_inputMemoryLayout.NumDimensions() != outputMemoryLayout.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: input and output layouts must have same dimension");
        }
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _inputMemoryLayout(inputMemoryLayout), _paddingValue(paddingValue)
    {
        if (inputMemoryLayout.NumDimensions() != outputMemoryLayout.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: input and output layouts must have same dimension");
        }
    }

    //
    // With reordering ("reshape" / slicing, followed by transpose / dimension reordering)
    //
    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::DimensionOrder& order)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, _input.GetMemoryLayout().ReorderedCopy(order))
    {
        _inputMemoryLayout = _input.GetMemoryLayout();
        if (_inputMemoryLayout.NumDimensions() != order.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: input and output layouts must have same dimension");
        }
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& outputMemoryLayout, const model::DimensionOrder& order, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout.ReorderedCopy(order)), _paddingValue(paddingValue)
    {
        _inputMemoryLayout = _input.GetMemoryLayout();
        if (_inputMemoryLayout.NumDimensions() != outputMemoryLayout.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: input and output layouts must have same dimension");
        }
    }

    template <typename ValueType>
    ReorderDataNode<ValueType>::ReorderDataNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, const model::DimensionOrder& order, ValueType paddingValue)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout.ReorderedCopy(order)), _inputMemoryLayout(inputMemoryLayout), _paddingValue(paddingValue)
    {
        if (inputMemoryLayout.NumDimensions() != outputMemoryLayout.NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: input and output layouts must have same dimension");
        }
    }

    template <typename ValueType>
    model::MemoryCoordinates ReorderDataNode<ValueType>::ReorderOutputToInputLocation(model::MemoryCoordinates physicalOutputCoordinates) const
    {
        const auto inputDimensionOrder = GetInputMemoryLayout().GetLogicalDimensionOrder();
        const auto outputDimensionOrder = GetOutputMemoryLayout().GetLogicalDimensionOrder();

        auto logicalCoordinates = ReorderDataNodeDetail::PhysicalToLogical(physicalOutputCoordinates, outputDimensionOrder);
        auto physicalInputCoordinates = ReorderDataNodeDetail::LogicalToPhysical(logicalCoordinates, inputDimensionOrder);
        return physicalInputCoordinates;
    }

    // TODO: for each dimension, loop over minimum of input and output interval. Then we don't have to check if the value is out-of-bounds
    template <typename ValueType>
    std::vector<emitters::IRLocalScalar> ReorderDataNode<ValueType>::ReorderOutputToInputLocation(std::vector<emitters::IRLocalScalar> physicalOutputCoordinates) const
    {
        const auto inputDimensionOrder = GetInputMemoryLayout().GetLogicalDimensionOrder();
        const auto outputDimensionOrder = GetOutputMemoryLayout().GetLogicalDimensionOrder();

        auto logicalCoordinates = ReorderDataNodeDetail::PhysicalToLogical(physicalOutputCoordinates, outputDimensionOrder);
        auto physicalInputCoordinates = ReorderDataNodeDetail::LogicalToPhysical(logicalCoordinates, inputDimensionOrder);
        return physicalInputCoordinates;
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ReorderDataNode>(newPortElements, _inputMemoryLayout, _output.GetMemoryLayout(), _paddingValue);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void ReorderDataNode<ValueType>::ComputeDimensionLoop(const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout, int dimension, std::vector<int>& coordinates, std::vector<ValueType>& output) const
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
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: input and output layouts must have same dimension");
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

        function.For(ranges, [input, output, inputMemoryLayout, outputMemoryLayout, this](emitters::IRFunctionEmitter& function, std::vector<emitters::IRLocalScalar> indices) {
            auto inputLocation = ReorderOutputToInputLocation(indices);
            auto inputIndex = model::EmitGetEntryOffset(function, inputLocation, inputMemoryLayout);
            auto outputIndex = model::EmitGetEntryOffset(function, indices, outputMemoryLayout);
            output[outputIndex] = input[inputIndex];
        });
    }

    template <typename ValueType>
    ell::utilities::ArchiveVersion ReorderDataNode<ValueType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion currentArchiveVersion = { utilities::ArchiveVersionNumbers::v8_port_memory_layout };
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
                outputMemoryLayout = model::PortMemoryLayout(outputMemoryLayout.GetActiveSize(), outputMemoryLayout.GetStride(), outputMemoryLayout.GetOffset(), order);
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

} // nodes
} // ell
