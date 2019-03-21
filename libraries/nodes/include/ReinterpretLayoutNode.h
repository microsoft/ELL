////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReinterpretLayoutNode.h (nodes)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>

namespace ell
{
namespace nodes
{
    /// <summary> A node that can reinterpret the input port layout to a new shape, so long as the total memory size remains the same </summary>
    template <typename ValueType>
    class ReinterpretLayoutNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        ReinterpretLayoutNode();

        /// <summary> Constructor with input and new output memory layout </summary>
        ///
        /// <param name="input"> The input to reinterpret. </param>
        /// <param name="outputMemoryLayout"> The memory layout of the output.  </param>
        ReinterpretLayoutNode(const model::OutputPort<ValueType>& input, const model::PortMemoryLayout& outputMemoryLayout);

        /// <summary> Gets information about the input memory layout </summary>
        model::PortMemoryLayout GetInputMemoryLayout() const { return _input.GetMemoryLayout(); }

        /// <summary> Gets information about the output memory layout </summary>
        model::PortMemoryLayout GetOutputMemoryLayout() const { return _output.GetMemoryLayout(); }

        /// <summary> Returns true if the node can accept input with this memory layout order, else false </summary>
        ///
        /// <param name="order"> The memory layout order for all the input ports </summary>
        /// <returns> If the node can accept the input memory layout order, true, else false </returns>
        bool CanAcceptInputLayout(const utilities::DimensionOrder& order) const override
        {
            return true;;
        }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ReinterpretLayoutNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:

        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } 

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

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
    //
    // ReinterpretLayoutNode
    //
    template <typename ValueType>
    ReinterpretLayoutNode<ValueType>::ReinterpretLayoutNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {}

    //
    // Without reordering ("reshape" / slicing)
    //
    template <typename ValueType>
    ReinterpretLayoutNode<ValueType>::ReinterpretLayoutNode(const model::OutputPort<ValueType>& input,
                                                const model::PortMemoryLayout& outputMemoryLayout) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, outputMemoryLayout)
    {
        auto inputMemoryLayout = _input.GetMemoryLayout();
        if (inputMemoryLayout.GetMemorySize() != outputMemoryLayout.GetMemorySize())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            "Error: input and output layouts must have same memory size");
        }
    }

    template <typename ValueType>
    void ReinterpretLayoutNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<ReinterpretLayoutNode>(newPortElements,
                                                            _output.GetMemoryLayout());
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void ReinterpretLayoutNode<ValueType>::Compute() const
    {
        // since our data is actually a flat vector, "reinterpret" operation is free.
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void ReinterpretLayoutNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto input = function.LocalArray(compiler.EnsurePortEmitted(this->input));
        auto output = function.LocalArray(compiler.EnsurePortEmitted(this->output));
        // simple pass through.  Would be nice to optimize this copy out all together...
        function.MemoryCopy<ValueType>(input, output, _output.GetMemoryLayout().GetMemorySize());
    }

    template <typename ValueType>
    ell::utilities::ArchiveVersion ReinterpretLayoutNode<ValueType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion currentArchiveVersion = {
            utilities::ArchiveVersionNumbers::v8_port_memory_layout
        };
        return std::max(currentArchiveVersion, CompilableNode::GetArchiveVersion());
    }

    template <typename ValueType>
    bool ReinterpretLayoutNode<ValueType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        return CompilableNode::CanReadArchiveVersion(version);
    }

    template <typename ValueType>
    void ReinterpretLayoutNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["outputLayout"] << GetOutputMemoryLayout();
    }

    template <typename ValueType>
    void ReinterpretLayoutNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        model::PortMemoryLayout outputMemoryLayout;
        archiver["outputLayout"] >> outputMemoryLayout;
        _output.SetMemoryLayout(outputMemoryLayout);        
    }

} // namespace nodes
} // namespace ell

#pragma endregion implementation
