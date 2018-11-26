////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SpliceNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"

#include <utilities/include/Exception.h>
#include <utilities/include/TypeName.h>

#include <memory>
#include <string>
#include <vector>

namespace ell
{
namespace model
{
    class IRMapCompiler;
    class ModelTransformer;

    /// <summary> A node that concatenates the values from a number of output ports. </summary>
    template <typename ValueType>
    class SpliceNode : public CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        SpliceNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="inputPorts"> The output ports to concatenate. </param>
        SpliceNode(const std::vector<const OutputPortBase*>& inputPorts);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SpliceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool ShouldCompileInline() const override { return true; }
        bool HasState() const override { return true; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(ModelTransformer& transformer) const override;
        static PortMemoryLayout ComputeOutputLayout(const std::vector<const OutputPortBase*>& inputPorts);

        std::vector<std::unique_ptr<InputPort<ValueType>>> _inputPorts;
        OutputPort<ValueType> _output;
    };
} // namespace model
} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    template <typename ValueType>
    SpliceNode<ValueType>::SpliceNode() :
        CompilableNode({}, { &_output }),
        _output(this, defaultOutputPortName, 0)
    {}

    template <typename ValueType>
    SpliceNode<ValueType>::SpliceNode(const std::vector<const OutputPortBase*>& inputs) :
        CompilableNode({}, { &_output }),
        _output(this, defaultOutputPortName, ComputeOutputLayout(inputs))
    {
        auto layout = _output.GetMemoryLayout();
        if (layout.HasPadding())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "SpliceNode must not have padding on its input");
        }

        // Add 1 input port per port in the input list
        auto increment = layout.GetCumulativeIncrement(0);
        int index = 0;
        for (const auto& inputPort : inputs)
        {
            if (inputPort->Size() % increment != 0)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "SpliceNode input port size must be multiple of largest dimension increment");
            }

            // Create a new InputPort object
            auto portName = std::string("input_") + std::to_string(index);
            _inputPorts.emplace_back(std::make_unique<InputPort<ValueType>>(this, static_cast<const OutputPort<ValueType>&>(*inputPort), portName));

            // And add it to this node
            auto rawPtr = _inputPorts.back().get();
            AddInputPort(rawPtr);
            ++index;
        }
    }

    template <typename ValueType>
    PortMemoryLayout SpliceNode<ValueType>::ComputeOutputLayout(const std::vector<const OutputPortBase*>& inputPorts)
    {
        std::vector<PortRange> ranges;
        for (auto port : inputPorts)
        {
            ranges.emplace_back(*port);
        }
        PortElementsBase elements(ranges);
        return elements.GetMemoryLayout();
    }

    template <typename ValueType>
    void SpliceNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output;
        output.reserve(_output.Size());
        for (const auto& input : _inputPorts)
        {
            auto value = input->GetValue();
            std::copy(value.begin(), value.end(), std::back_inserter(output));
        }
        _output.SetOutput(output);
    }

    template <typename ValueType>
    void SpliceNode<ValueType>::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pOutput = compiler.EnsurePortEmitted(_output);
        // check if the pOutput variable is null
        function.If(ell::emitters::TypedComparison::notEquals, pOutput, function.NullPointer(pOutput->getType()->getPointerElementType()->getPointerTo()), [pOutput, &compiler, this](emitters::IRFunctionEmitter& function) {
            if (_inputPorts.size() == 1 && _inputPorts[0]->Size() == 1)
            {
                llvm::Value* pVal = compiler.LoadPortElementVariable(_inputPorts[0]->GetInputElement(0));
                function.Store(pOutput, pVal);
            }
            else
            {
                int rangeStart = 0;
                for (const auto& inputPort : _inputPorts)
                {
                    const auto& referencedPort = inputPort->GetReferencedPort();
                    auto input = function.LocalArray(compiler.EnsurePortEmitted(referencedPort));
                    auto output = function.LocalArray(pOutput);
                    auto rangeSize = referencedPort.Size();

                    function.For(rangeSize, [=](emitters::IRFunctionEmitter& function, auto i) {
                        output[i + rangeStart] = input[i];
                    });
                    rangeStart += rangeSize;
                }
            }
        });
    }

    template <typename ValueType>
    void SpliceNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        std::vector<const OutputPortBase*> newInputs;
        for (const auto& inputPort : _inputPorts)
        {
            const auto& newPort = transformer.GetCorrespondingInputs(*inputPort);
            newInputs.emplace_back(&newPort);
        }
        auto newNode = transformer.AddNode<SpliceNode<ValueType>>(newInputs);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void SpliceNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        int numInputs = static_cast<int>(_inputPorts.size());
        archiver["numInputs"] << numInputs;
        for (int index = 0; index < numInputs; ++index)
        {
            archiver[std::string("input_") + std::to_string(index)] << *_inputPorts[index];
        }
    }

    template <typename ValueType>
    void SpliceNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        int numInputs = 0;
        archiver["numInputs"] >> numInputs;
        _inputPorts.clear();
        std::vector<const OutputPortBase*> referencedPorts;
        for (int index = 0; index < numInputs; ++index)
        {
            InputPort<ValueType> port;
            auto portName = std::string("input_") + std::to_string(index);
            archiver[portName] >> port;
            const auto& referencedPort = port.GetReferencedPort();
            _inputPorts.emplace_back(std::make_unique<InputPort<ValueType>>(this, referencedPort, portName));
            auto rawPtr = _inputPorts.back().get();
            AddInputPort(rawPtr);
            referencedPorts.push_back(&(_inputPorts.back()->GetReferencedPort()));
        }

        _output.SetMemoryLayout(ComputeOutputLayout(referencedPorts));
    }
} // namespace model
} // namespace ell

#pragma endregion implementation
