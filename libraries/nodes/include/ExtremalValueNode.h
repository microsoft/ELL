////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExtremalValueNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/MapCompiler.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>

#include <utilities/include/TypeName.h>

#include <algorithm>
#include <vector>

namespace ell
{
/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> An example node that computes an extremal value (min or max) of its input, as well as the index of the extremal value. </summary>
    template <typename ValueType, bool max>
    class ExtremalValueNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* valPortName = "val";
        static constexpr const char* argValPortName = "argVal";

        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& val = _val;
        const model::OutputPort<int>& argVal = _argVal;
        /// @}

        /// <summary> Default Constructor </summary>
        ExtremalValueNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ExtremalValueNode(const model::OutputPort<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this is an argmin or argmax node </summary>
        ///
        /// <returns> `true` if this is an argmax node, `false` if is an argmin node </returns>
        bool IsMaxNode() const { return max; }

        /// <summary> Gets the emitter typed comparison type used for computing this node </summary>
        ///
        /// <returns> A `TypedComparison` indicating the comarison type and data type for this node </returns>
        emitters::TypedComparison GetComparison() const;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return false; }
        emitters::LLVMFunction GetOperator(model::IRMapCompiler& compiler) const;
        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        // My inputs
        model::InputPort<ValueType> _input;

        // My outputs
        model::OutputPort<ValueType> _val;
        model::OutputPort<int> _argVal;
    };

    /// <summary> ArgMin node subclass </summary>
    template <typename ValueType>
    class ArgMinNode : public ExtremalValueNode<ValueType, false>
    {
    public:
        /// <summary> Default Constructor </summary>
        ArgMinNode() :
            ExtremalValueNode<ValueType, false>() {}

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ArgMinNode(const model::OutputPort<ValueType>& input) :
            ExtremalValueNode<ValueType, false>(input) {}

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ArgMinNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    private:
        void Copy(model::ModelTransformer& transformer) const override;
    };

    /// <summary> ArgMax node subclass </summary>
    template <typename ValueType>
    class ArgMaxNode : public ExtremalValueNode<ValueType, true>
    {
    public:
        /// <summary> Default Constructor </summary>
        ArgMaxNode() :
            ExtremalValueNode<ValueType, true>() {}

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ArgMaxNode(const model::OutputPort<ValueType>& input) :
            ExtremalValueNode<ValueType, true>(input) {}

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ArgMaxNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    private:
        void Copy(model::ModelTransformer& transformer) const override;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType, bool max>
    ExtremalValueNode<ValueType, max>::ExtremalValueNode() :
        CompilableNode({ &_input }, { &_val, &_argVal }),
        _input(this, {}, inputPortName),
        _val(this, valPortName, 1),
        _argVal(this, argValPortName, 1)
    {
    }

    template <typename ValueType, bool max>
    ExtremalValueNode<ValueType, max>::ExtremalValueNode(const model::OutputPort<ValueType>& input) :
        CompilableNode({ &_input }, { &_val, &_argVal }),
        _input(this, input, inputPortName),
        _val(this, valPortName, 1),
        _argVal(this, argValPortName, 1)
    {
    }

    template <typename ValueType, bool max>
    std::string ExtremalValueNode<ValueType, max>::GetTypeName()
    {
        if (max)
        {
            return utilities::GetCompositeTypeName<ValueType, std::true_type>("ExtremalValueNode");
        }
        else
        {
            return utilities::GetCompositeTypeName<ValueType, std::false_type>("ExtremalValueNode");
        }
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::Compute() const
    {
        auto inputValues = _input.GetValue();
        decltype(std::max_element(inputValues.begin(), inputValues.end())) result;
        if (max)
        {
            result = std::max_element(inputValues.begin(), inputValues.end());
        }
        else
        {
            result = std::min_element(inputValues.begin(), inputValues.end());
        }
        auto val = *result;
        auto index = result - inputValues.begin();
        _val.SetOutput({ val });
        _argVal.SetOutput({ (int)index });
    };

    template <typename ValueType, bool max>
    emitters::TypedComparison ExtremalValueNode<ValueType, max>::GetComparison() const
    {
        if (IsMaxNode())
        {
            return emitters::GetComparison<ValueType>(emitters::BinaryPredicateType::greater);
        }
        else
        {
            return emitters::GetComparison<ValueType>(emitters::BinaryPredicateType::less);
        }
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        VerifyIsScalar(val);
        VerifyIsScalar(argVal);
        if (!function.GetCompilerOptions().unrollLoops)
        {
            CompileLoop(compiler, function);
        }
        else
        {
            CompileExpanded(compiler, function);
        }
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue inputVal = compiler.EnsurePortEmitted(input);
        emitters::LLVMValue outVal = compiler.EnsurePortEmitted(val);
        emitters::LLVMValue outArgVal = compiler.EnsurePortEmitted(argVal);
        auto inputType = GetPortVariableType(input);
        auto numInputs = input.Size();

        emitters::LLVMValue bestVal = function.Variable(inputType, "bestVal");
        emitters::LLVMValue bestIndex = function.Variable(ell::emitters::VariableType::Int32, "bestArgVal");

        auto val0 = function.ValueAt(inputVal, function.Literal(0));
        function.Store(bestVal, val0);
        function.StoreZero(bestIndex);

        function.For(1, numInputs, 1, [inputVal, bestVal, bestIndex, this](emitters::IRFunctionEmitter& function, emitters::LLVMValue i) {
            auto val = function.ValueAt(inputVal, i);
            function.If(GetComparison(), val, function.Load(bestVal), [bestVal, bestIndex, val, i](auto& function) {
                function.Store(bestVal, val);
                function.Store(bestIndex, i);
            });
        });

        function.Store(outVal, function.Load(bestVal));
        function.Store(outArgVal, function.Load(bestIndex));
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue outVal = compiler.EnsurePortEmitted(val);
        emitters::LLVMValue outArgVal = compiler.EnsurePortEmitted(argVal);
        auto inputType = GetPortVariableType(input);
        auto numInputs = input.Size();

        emitters::LLVMValue bestVal = function.Variable(inputType, "bestVal");
        emitters::LLVMValue bestIndex = function.Variable(ell::emitters::VariableType::Int32, "bestArgVal");

        emitters::LLVMValue val0 = compiler.LoadPortElementVariable(input.GetInputElement(0));
        function.Store(bestVal, val0);
        function.StoreZero(bestIndex);

        for (size_t i = 1; i < numInputs; ++i)
        {
            emitters::LLVMValue val = compiler.LoadPortElementVariable(input.GetInputElement(i));
            function.If(GetComparison(), val, function.Load(bestVal), [bestVal, bestIndex, val, i](auto& function) {
                function.Store(bestVal, val);
                function.Store(bestIndex, function.Literal(static_cast<int>(i)));
            });
        }

        function.Store(outVal, function.Load(bestVal));
        function.Store(outArgVal, function.Load(bestIndex));
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[valPortName] << _val;
        archiver[argValPortName] << _argVal;
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[valPortName] >> _val;
        archiver[argValPortName] >> _argVal;
    }

    //
    // Copy definitions for subclasses
    //
    template <typename ValueType>
    void ArgMinNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(this->_input);
        auto newNode = transformer.AddNode<ArgMinNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(this->val, newNode->val);
        transformer.MapNodeOutput(this->argVal, newNode->argVal);
    }

    template <typename ValueType>
    void ArgMaxNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(this->_input);
        auto newNode = transformer.AddNode<ArgMaxNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(this->val, newNode->val);
        transformer.MapNodeOutput(this->argVal, newNode->argVal);
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
