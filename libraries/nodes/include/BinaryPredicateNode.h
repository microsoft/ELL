////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryPredicateNode.h (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NodeOperations.h"

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <emitters/include/EmitterTypes.h>

#include <utilities/include/Exception.h>
#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that performs a coordinatewise binary boolean-valued operation on its inputs. </summary>
    template <typename ValueType>
    class BinaryPredicateNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<bool>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        BinaryPredicateNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input1"> The left-hand input of the arithmetic expression. </param>
        /// <param name="input2"> The right-hand input of the arithmetic expression. </param>
        /// <param name="predicate"> The type of predicate to apply. </param>
        BinaryPredicateNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, BinaryPredicateType predicate);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BinaryPredicateNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Gets the predicate performed by this node </summary>
        ///
        /// <returns> The predicate </returns>
        BinaryPredicateType GetPredicate() const { return _predicate; }

    protected:
        void Compute() const override;

        template <typename Operation>
        std::vector<bool> ComputeOutput(Operation&& fn) const;

        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: predicate

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<bool> _output;

        // Operation
        BinaryPredicateType _predicate;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    inline namespace BinaryPredicates
    {
        template <typename ValueType>
        bool Equal(ValueType a, ValueType b)
        {
            return a == b;
        }

        template <typename ValueType>
        bool Less(ValueType a, ValueType b)
        {
            return a < b;
        }

        template <typename ValueType>
        bool Greater(ValueType a, ValueType b)
        {
            return a > b;
        }

        template <typename ValueType>
        bool NotEqual(ValueType a, ValueType b)
        {
            return a != b;
        }

        template <typename ValueType>
        bool LessOrEqual(ValueType a, ValueType b)
        {
            return a <= b;
        }

        template <typename ValueType>
        bool GreaterOrEqual(ValueType a, ValueType b)
        {
            return a >= b;
        }
    } // namespace BinaryPredicates

    template <typename ValueType>
    BinaryPredicateNode<ValueType>::BinaryPredicateNode() :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, {}, defaultInput1PortName),
        _input2(this, {}, defaultInput2PortName),
        _output(this, defaultOutputPortName, 0),
        _predicate(BinaryPredicateType::none)
    {
    }

    template <typename ValueType>
    BinaryPredicateNode<ValueType>::BinaryPredicateNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, BinaryPredicateType predicate) :
        CompilableNode({ &_input1, &_input2 }, { &_output }),
        _input1(this, input1, defaultInput1PortName),
        _input2(this, input2, defaultInput2PortName),
        _output(this, defaultOutputPortName, _input1.Size()),
        _predicate(predicate)
    {
        if (input1.Size() != input2.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input sizes must match");
        }
        assert(input1.Size() == input2.Size());
    }

    template <typename ValueType>
    template <typename Operation>
    std::vector<bool> BinaryPredicateNode<ValueType>::ComputeOutput(Operation&& fn) const
    {
        auto output = std::vector<bool>(_input1.Size());
        for (size_t index = 0; index < _input1.Size(); index++)
        {
            output[index] = fn(_input1[index], _input2[index]);
        }
        return output;
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::Compute() const
    {
        std::vector<bool> output;
        switch (_predicate)
        {
        case BinaryPredicateType::equal:
            output = ComputeOutput(BinaryPredicates::Equal<ValueType>);
            break;
        case BinaryPredicateType::less:
            output = ComputeOutput(BinaryPredicates::Less<ValueType>);
            break;
        case BinaryPredicateType::greater:
            output = ComputeOutput(BinaryPredicates::Greater<ValueType>);
            break;
        case BinaryPredicateType::notEqual:
            output = ComputeOutput(BinaryPredicates::NotEqual<ValueType>);
            break;
        case BinaryPredicateType::lessOrEqual:
            output = ComputeOutput(BinaryPredicates::LessOrEqual<ValueType>);
            break;
        case BinaryPredicateType::greaterOrEqual:
            output = ComputeOutput(BinaryPredicates::GreaterOrEqual<ValueType>);
            break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown predicate type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& portElements1 = transformer.GetCorrespondingInputs(_input1);
        const auto& portElements2 = transformer.GetCorrespondingInputs(_input2);
        auto newNode = transformer.AddNode<BinaryPredicateNode<ValueType>>(portElements1, portElements2, _predicate);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (!function.GetCompilerOptions().unrollLoops)
        {
            CompileLoop(compiler, function);
        }
        else
        {
            CompileExpanded(compiler, function);
        }
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pInput1 = compiler.EnsurePortEmitted(input1);
        emitters::LLVMValue pInput2 = compiler.EnsurePortEmitted(input2);
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);
        emitters::TypedComparison cmp = emitters::GetComparison<ValueType>(ToEmitterType(GetPredicate()));

        function.For(input1.Size(), [pInput1, pInput2, pResult, cmp](emitters::IRFunctionEmitter& function, emitters::LLVMValue i) {
            emitters::LLVMValue inputValue1 = function.ValueAt(pInput1, i);
            emitters::LLVMValue inputValue2 = function.ValueAt(pInput2, i);
            emitters::LLVMValue pOpResult = function.Comparison(cmp, inputValue1, inputValue2);
            // LLVM internally uses 1 bit for boolean. We use integers to store boolean results. That requires a typecast in LLVM
            function.SetValueAt(pResult, i, function.CastBoolToByte(pOpResult));
        });
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        auto count = input1.Size();
        for (size_t i = 0; i < count; ++i)
        {
            emitters::LLVMValue inputValue1 = compiler.LoadPortElementVariable(input1.GetInputElement(i));
            emitters::LLVMValue inputValue2 = compiler.LoadPortElementVariable(input2.GetInputElement(i));
            emitters::LLVMValue pOpResult = function.Comparison(emitters::GetComparison<ValueType>(ToEmitterType(GetPredicate())), inputValue1, inputValue2);
            function.SetValueAt(pResult, function.Literal((int)i), function.CastBoolToByte(pOpResult));
        }
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInput1PortName] << _input1;
        archiver[defaultInput2PortName] << _input2;
        archiver["predicate"] << ToString(_predicate);
    }

    template <typename ValueType>
    void BinaryPredicateNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInput1PortName] >> _input1;
        archiver[defaultInput2PortName] >> _input2;
        std::string predicate;
        archiver["predicate"] >> predicate;
        _predicate = FromString<BinaryPredicateType>(predicate);
        _output.SetSize(_input1.Size());
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
