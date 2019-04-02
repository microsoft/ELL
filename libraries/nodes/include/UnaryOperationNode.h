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

#include "ActivationFunctions.h"
#include "NodeOperations.h"

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
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

        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Operation
        UnaryOperationType _operation;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="input"> The port to get the input data from </param>
    /// <param name="operation"> The function to use to process the signal. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType>
    const model::OutputPort<ValueType>& UnaryOperation(const model::OutputPort<ValueType>& input, UnaryOperationType operation);

    /// @{
    /// <summary> Convenience functions for adding a node to a model. </summary>
    /// <param name="input"> The port to get the input data from </param>
    /// <returns> The output of the new node. </returns>
    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& Abs(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& Cos(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& Exp(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& HardSigmoid(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& Log(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& LogicalNot(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& Sigmoid(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& Sin(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& Sqrt(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& Square(const model::OutputPort<ValueType>& input);

    template <typename ValueType, utilities::IsNotBoolean<ValueType> = true>
    const model::OutputPort<ValueType>& Tanh(const model::OutputPort<ValueType>& input);
    /// @}

    inline namespace UnaryOperations
    {
        template <typename ValueType>
        ValueType Abs(ValueType a);

        template <>
        inline bool Abs(bool x);

        template <typename ValueType>
        ValueType Sqrt(ValueType a);

        template <>
        inline bool Sqrt(bool x);

        template <typename ValueType>
        ValueType LogicalNot(ValueType a);

        template <>
        inline bool LogicalNot(bool x);

        template <typename ValueType>
        ValueType Tanh(ValueType a);

        template <>
        inline bool Tanh(bool x);

        template <typename ValueType>
        ValueType Exp(ValueType a);

        template <>
        inline bool Exp(bool x);

        template <typename ValueType>
        ValueType Square(ValueType a);

        template <>
        inline bool Square(bool);

        template <typename ValueType>
        ValueType Log(ValueType a);

        template <>
        inline bool Log(bool);

        template <typename ValueType>
        ValueType Sigmoid(ValueType x);

        template <>
        inline bool Sigmoid(bool x);

        template <typename ValueType>
        ValueType HardSigmoid(ValueType input);

        template <>
        inline bool HardSigmoid(bool x);

    } // namespace UnaryOperations
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    inline namespace UnaryOperations
    {
        template <typename ValueType>
        ValueType Abs(ValueType a)
        {
            return std::abs(a);
        }

        template <>
        inline bool Abs(bool x)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking absolute value of a boolean");
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
        ValueType Sin(ValueType a)
        {
            return std::sin(a);
        }

        template <typename ValueType>
        ValueType Cos(ValueType a)
        {
            return std::cos(a);
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

        template <typename ValueType>
        ValueType Sigmoid(ValueType x)
        {
            SigmoidActivationFunction<ValueType> sigmoid;
            return sigmoid.Compute(x);
        }

        template <>
        inline bool Sigmoid(bool x)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking sigmoid of a boolean value");
        }

        template <typename ValueType>
        ValueType HardSigmoid(ValueType input)
        {
            HardSigmoidActivationFunction<ValueType> hardSigmoid;
            return hardSigmoid.Compute(input);
        }

        template <>
        inline bool HardSigmoid(bool x)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Error: taking hard sigmoid of a boolean value");
        }
    } // namespace UnaryOperations

    template <typename ValueType>
    const model::OutputPort<ValueType>& UnaryOperation(const model::OutputPort<ValueType>& input, UnaryOperationType operation)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }
        auto node = model->AddNode<UnaryOperationNode<ValueType>>(input, operation);
        return node->output;
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& Abs(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::abs);
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& Cos(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::cos);
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& Exp(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::exp);
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& HardSigmoid(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::hardSigmoid);
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& Log(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::log);
    }

    template <typename ValueType, utilities::IsBoolean<ValueType>>
    const model::OutputPort<ValueType>& LogicalNot(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::logicalNot);
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& Sigmoid(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::sigmoid);
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& Sin(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::sin);
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& Sqrt(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::sqrt);
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& Square(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::square);
    }

    template <typename ValueType, utilities::IsNotBoolean<ValueType>>
    const model::OutputPort<ValueType>& Tanh(const model::OutputPort<ValueType>& input)
    {
        return UnaryOperation(input, UnaryOperationType::tanh);
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
