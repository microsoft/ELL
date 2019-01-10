////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DotProductNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SumNode.h"

#include "BinaryOperationNode.h"
#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/MapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/OutputPort.h>
#include <model/include/PortElements.h>

#include <utilities/include/TypeName.h>

#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that takes two vector inputs and returns their dot product </summary>
    template <typename ValueType>
    class DotProductNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DotProductNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input1"> One of the signals to take the dot product of </param>
        /// <param name="input2"> The other signal to take the dot product of </param>
        DotProductNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DotProductNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` currently refining the model </param>
        bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return false; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void CompileDotProductLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileDotProductExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
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
    DotProductNode<ValueType>::DotProductNode() :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, {}, defaultInput1PortName),
        _input2(this, {}, defaultInput2PortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    DotProductNode<ValueType>::DotProductNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2) :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, input1, defaultInput1PortName),
        _input2(this, input2, defaultInput2PortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for (size_t index = 0; index < _input1.Size(); ++index)
        {
            result += _input1[index] * _input2[index];
        }
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void DotProductNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInput1 = transformer.GetCorrespondingInputs(_input1);
        const auto& newInput2 = transformer.GetCorrespondingInputs(_input2);
        auto newNode = transformer.AddNode<DotProductNode<ValueType>>(newInput1, newInput2);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool DotProductNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        // Maybe... in reality, dot product will likely want to be computed as in Compute() above
        const auto& newInput1 = transformer.GetCorrespondingInputs(_input1);
        const auto& newInput2 = transformer.GetCorrespondingInputs(_input2);
        const auto& product = AppendBinaryOperation(transformer, newInput1, newInput2, nodes::BinaryOperationType::multiply);
        const auto& sum = AppendSum(transformer, product);

        transformer.MapNodeOutput(output, sum);
        return true;
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        static_assert(!std::is_same<ValueType, bool>(), "Cannot instantiate boolean dot product nodes");
        if (!compiler.GetCompilerOptions().unrollLoops)
        {
            CompileDotProductLoop(compiler, function);
        }
        else
        {
            CompileDotProductExpanded(compiler, function);
        }
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::CompileDotProductLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pLVector = compiler.EnsurePortEmitted(input1);
        emitters::LLVMValue pRVector = compiler.EnsurePortEmitted(input2);
        int count = static_cast<int>(input1.Size());
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);
        if (compiler.GetCompilerOptions().inlineOperators)
        {
            function.DotProduct(count, pLVector, pRVector, pResult);
        }
        else
        {
            function.Call(function.GetModule().GetRuntime().GetDotProductFunction<ValueType>(), { function.Literal(count), function.PointerOffset(pLVector, 0), function.PointerOffset(pRVector, 0), function.PointerOffset(pResult, 0) });
        }
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::CompileDotProductExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        function.StoreZero(pResult);
        for (size_t i = 0; i < input1.Size(); ++i)
        {
            emitters::LLVMValue pLeftValue = compiler.LoadPortElementVariable(input1.GetInputElement(i));
            emitters::LLVMValue pRightValue = compiler.LoadPortElementVariable(input2.GetInputElement(i));
            emitters::LLVMValue pMultiplyResult = function.Operator(emitters::GetMultiplyForValueType<ValueType>(), pLeftValue, pRightValue);
            function.OperationAndUpdate(pResult, emitters::GetAddForValueType<ValueType>(), pMultiplyResult);
        }
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInput1PortName] << _input1;
        archiver[defaultInput2PortName] << _input2;
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInput1PortName] >> _input1;
        archiver[defaultInput2PortName] >> _input2;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
