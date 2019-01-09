////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnaryOperationNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/MapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>
#include <model/include/PortElements.h>

#include <emitters/include/EmitterTypes.h>

#include <utilities/include/TypeName.h>

#include <cmath>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    using UnaryOperationType = emitters::UnaryOperationType;

    /// <summary> A node that represents a unary function of its input </summary>
    template <typename ValueType>
    class UnaryOperationNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        UnaryOperationNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to process. </param>
        /// <param name="operation"> The function to use to process the signal. </param>
        UnaryOperationNode(const model::OutputPort<ValueType>& input, UnaryOperationType operation);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("UnaryOperationNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Gets the operation performed by this node </summary>
        ///
        /// <returns> The operation </returns>
        UnaryOperationType GetOperation() const { return _operation; }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: operation

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        emitters::LLVMFunction GetOperator(emitters::IRFunctionEmitter& function) const;
        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        template <typename Operation>
        std::vector<ValueType> ComputeOutput(Operation&& function) const;

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Operation
        UnaryOperationType _operation;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="model"> The Model or ModelTransformer to add the node to. </param>
    /// <param name="input"> The port to get the input data from </param>
    /// <param name="operation"> The function to use to process the signal. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ModelLikeType, typename ValueType>
    const model::OutputPort<ValueType>& AppendUnaryOperation(ModelLikeType& model, const model::OutputPort<ValueType>& input, UnaryOperationType operation);
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
    namespace UnaryOperations
    {
        inline std::string to_string(UnaryOperationType op)
        {
            switch (op)
            {
                ADD_TO_STRING_ENTRY(UnaryOperationType, none);
                ADD_TO_STRING_ENTRY(UnaryOperationType, sqrt);
                ADD_TO_STRING_ENTRY(UnaryOperationType, logicalNot);
                ADD_TO_STRING_ENTRY(UnaryOperationType, tanh);
                ADD_TO_STRING_ENTRY(UnaryOperationType, exp);
                ADD_TO_STRING_ENTRY(UnaryOperationType, square);
                ADD_TO_STRING_ENTRY(UnaryOperationType, log);

            default:
                throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown unary operation");
            }
        }

        inline UnaryOperationType from_string(std::string name)
        {
            BEGIN_FROM_STRING;
            ADD_FROM_STRING_ENTRY(UnaryOperationType, none);
            ADD_FROM_STRING_ENTRY(UnaryOperationType, sqrt);
            ADD_FROM_STRING_ENTRY(UnaryOperationType, logicalNot);
            ADD_FROM_STRING_ENTRY(UnaryOperationType, tanh);
            ADD_FROM_STRING_ENTRY(UnaryOperationType, exp);
            ADD_FROM_STRING_ENTRY(UnaryOperationType, square);
            ADD_FROM_STRING_ENTRY(UnaryOperationType, log);

            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown unary operation");
        }

        template <typename ValueType>
        ValueType Sqrt(ValueType a)
        {
            return std::sqrt(a);
        }

        template <>
        inline bool Sqrt(bool x)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking sqrt of a boolean value");
        }

        template <typename ValueType>
        ValueType LogicalNot(ValueType a)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking not of a non-boolean value");
        }

        template <>
        inline bool LogicalNot(bool x)
        {
            return !x;
        }

        template <typename ValueType>
        ValueType Tanh(ValueType a)
        {
            return std::tanh(a);
        }

        template <>
        inline bool Tanh(bool x)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking tanh of a boolean value");
        }

        template <typename ValueType>
        ValueType Exp(ValueType a)
        {
            return std::exp(a);
        }

        template <>
        inline bool Exp(bool x)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking exp of a boolean value");
        }

        template <typename ValueType>
        ValueType Square(ValueType a)
        {
            return a * a;
        }

        template <>
        inline bool Square(bool)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking square of a boolean value");
        }

        template <typename ValueType>
        ValueType Log(ValueType a)
        {
            return std::log(a);
        }

        template <>
        inline bool Log(bool)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking log of a boolean value");
        }
    } // namespace UnaryOperations

    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0),
        _operation(UnaryOperationType::none)
    {
    }

    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode(const model::OutputPort<ValueType>& input, UnaryOperationType operation) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, _input.Size()),
        _operation(operation)
    {
    }

    template <typename ValueType>
    template <typename Operation>
    std::vector<ValueType> UnaryOperationNode<ValueType>::ComputeOutput(Operation&& function) const
    {
        auto output = std::vector<ValueType>(_input.Size());
        for (size_t index = 0; index < _input.Size(); index++)
        {
            output[index] = function(_input[index]);
        }
        return output;
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output;
        switch (_operation)
        {
        case UnaryOperationType::sqrt:
            output = ComputeOutput(UnaryOperations::Sqrt<ValueType>);
            break;
        case UnaryOperationType::logicalNot:
            output = ComputeOutput(UnaryOperations::LogicalNot<ValueType>);
            break;
        case UnaryOperationType::exp:
            output = ComputeOutput(UnaryOperations::Exp<ValueType>);
            break;
        case UnaryOperationType::tanh:
            output = ComputeOutput(UnaryOperations::Tanh<ValueType>);
            break;
        case UnaryOperationType::square:
            output = ComputeOutput(UnaryOperations::Square<ValueType>);
            break;
        case UnaryOperationType::log:
            output = ComputeOutput(UnaryOperations::Log<ValueType>);
            break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown operation type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<UnaryOperationNode<ValueType>>(newPortElements, _operation);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    emitters::LLVMFunction UnaryOperationNode<ValueType>::GetOperator(emitters::IRFunctionEmitter& function) const
    {
        switch (this->GetOperation())
        {
        case UnaryOperationType::sqrt:
            return function.GetModule().GetRuntime().GetSqrtFunction<ValueType>();
        case UnaryOperationType::exp:
            return function.GetModule().GetRuntime().GetExpFunction<ValueType>();
        case UnaryOperationType::log:
            return function.GetModule().GetRuntime().GetLogFunction<ValueType>();
        case UnaryOperationType::logicalNot:
        {
            auto& module = function.GetModule();
            auto& f = module.BeginFunction("logicalNot", emitters::GetVariableType<bool>(), { { "value", emitters::GetVariableType<ValueType>() } });
            auto args = f.Arguments().begin();
            llvm::Argument& val = *args;
            f.Return(f.LogicalNot(&val));
            module.EndFunction();
            return f.GetFunction();
        }
        case UnaryOperationType::square:
        {
            auto& module = function.GetModule();
            auto& f = module.BeginFunction("square", emitters::GetVariableType<ValueType>(), { { "value", emitters::GetVariableType<ValueType>() } });
            auto args = f.Arguments().begin();
            llvm::Argument& val = *args;
            f.Return(f.Operator(emitters::GetMultiplyForValueType<ValueType>(), &val, &val));
            module.EndFunction();
            return f.GetFunction();
        }
        case UnaryOperationType::tanh:
            return function.GetModule().GetRuntime().GetTanhFunction<ValueType>();
        case UnaryOperationType::none:
        default:
            throw emitters::EmitterException(emitters::EmitterError::unaryOperationNotSupported);
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
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

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Loop version broken
        auto count = input.Size();
        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(input);
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        function.For(count, [pInput, pResult, this](emitters::IRFunctionEmitter& function, emitters::LLVMValue i) {
            emitters::LLVMValue inputValue = function.ValueAt(pInput, i);
            emitters::LLVMValue pOpResult = function.Call(GetOperator(function), { inputValue });
            function.SetValueAt(pResult, i, pOpResult);
        });
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        for (size_t i = 0; i < input.Size(); ++i)
        {
            emitters::LLVMValue inputValue = compiler.LoadPortElementVariable(input.GetInputElement(i));
            emitters::LLVMValue pOpResult = function.Call(GetOperator(function), { inputValue });
            function.SetValueAt(pResult, function.Literal((int)i), pOpResult);
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["operation"] << UnaryOperations::to_string(_operation);
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = UnaryOperations::from_string(operation);
        _output.SetSize(_input.Size());
    }

    template <typename ModelLikeType, typename ValueType>
    const model::OutputPort<ValueType>& AppendUnaryOperation(ModelLikeType& model, const model::OutputPort<ValueType>& input, UnaryOperationType operation)
    {
        static_assert(std::is_same_v<ModelLikeType, model::Model> || std::is_same_v<ModelLikeType, model::ModelTransformer>, "'model' parameter must be a model::Model or model::ModelTransformer");
        auto node = model.template AddNode<UnaryOperationNode<ValueType>>(input, operation);
        return node->output;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
