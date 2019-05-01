////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConcatenationNode.h (nodes)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <utilities/include/Exception.h>
#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that concatenates inputs from a number of nodes. </summary>
    template <typename ValueType>
    class ConcatenationNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* shapeName = "shape";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;

        /// @}

        /// <summary> Default Constructor </summary>
        ConcatenationNode();

        /// <summary> Constructor </summary>
        /// <param name="input"> The inputs to concatenate specified as an Output port. </param>
        ConcatenationNode(const model::OutputPort<ValueType>& input);

        /// <summary> Constructor </summary>
        /// <param name="input"> The inputs to concatenate specified as an Output port. </param>
        /// <param name="shape"> The  memory layout of the output. </param>
        ConcatenationNode(const model::OutputPort<ValueType>& input, const model::PortMemoryLayout& outputMemoryLayout);

        /// <summary> Gets the output shape. </summary>
        ///
        /// <returns> The output shape. </returns>
        model::MemoryShape GetShape() const { return _output.GetMemoryLayout().GetActiveSize(); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ConcatenationNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool ShouldCompileInline() const override { return true; }
        bool HasState() const override { return true; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        void SetShape(const model::MemoryShape& shape) { _output.SetMemoryLayout({ shape }); }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        model::InputPort<ValueType> _input;
        model::OutputPort<ValueType> _output;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    ConcatenationNode<ValueType>::ConcatenationNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0){};

    template <typename ValueType>
    ConcatenationNode<ValueType>::ConcatenationNode(const model::OutputPort<ValueType>& input) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, input.GetMemoryLayout()){};

    template <typename ValueType>
    ConcatenationNode<ValueType>::ConcatenationNode(const model::OutputPort<ValueType>& input, const model::PortMemoryLayout& outputMemoryLayout) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, outputMemoryLayout){};

    template <typename ValueType>
    void ConcatenationNode<ValueType>::Compute() const
    {
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void ConcatenationNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        assert(GetPortVariableType(_input) == GetPortVariableType(_output));

        auto inputIsInputNode = (dynamic_cast<const model::InputNodeBase*>(_input.GetInputElement(0).ReferencedPort()->GetNode()) != nullptr);
        // TODO: re-enable this branch when scalar port bug is fixed
        if (_input.Size() != 1 && _output.Size() != 1 && !inputIsInputNode && false)
        {
            auto pVar = compiler.GetVariableForPort(_input.GetReferencedPort());
            compiler.SetVariableForPort(_output, pVar);
        }
        else
        {
            auto input = function.LocalArray(compiler.EnsurePortEmitted(_input));
            auto output = function.LocalArray(compiler.EnsurePortEmitted(_output));
            // check if the output variable is null.
            function.If(ell::emitters::TypedComparison::notEquals, output, function.NullPointer(output.value->getType()->getPointerElementType()->getPointerTo()), [input, output, this](emitters::IRFunctionEmitter& function) {
                auto size = _input.Size();
                function.For(size, [input, output](emitters::IRFunctionEmitter& function, auto i) {
                    output[i] = input[i];
                });
            });
        }
    }

    template <typename ValueType>
    void ConcatenationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<ConcatenationNode<ValueType>>(newPortElements, GetShape());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void ConcatenationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[shapeName] << GetShape().ToVector();
    }

    template <typename ValueType>
    void ConcatenationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        std::vector<int> shapeVector;
        archiver[shapeName] >> shapeVector;
        _output.SetSize(_input.Size());
        if (shapeVector.size() >= 3)
        {
            SetShape({ shapeVector });
        }
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
