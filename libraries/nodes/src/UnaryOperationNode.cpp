////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnaryOperationNode.cpp(nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnaryOperationNode.h"
#include "NodeOperations.h"

#include <emitters/include/IRMath.h>

#include <utilities/include/TypeTraits.h>

#include <cmath>

using namespace ell;

namespace ell
{
namespace nodes
{
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
    void UnaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<UnaryOperationNode<ValueType>>(newPortElements, _operation);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename Operation>
    std::vector<ValueType> ComputeLoop(std::vector<ValueType> input, Operation&& operation)
    {
        auto length = input.size();
        auto output = std::vector<ValueType>(length);
        std::transform(input.begin(), input.end(), output.begin(), [&](ValueType x) { return operation(x); });
        return output;
    }

    template <typename ValueType, utilities::IsFloatingPoint<ValueType> = true>
    std::vector<ValueType> ComputeOutput(std::vector<ValueType> input, UnaryOperationType operation)
    {
        switch (operation)
        {
        case UnaryOperationType::abs:
            return ComputeLoop<ValueType>(input, Abs<ValueType>);
        case UnaryOperationType::sqrt:
            return ComputeLoop<ValueType>(input, Sqrt<ValueType>);
        case UnaryOperationType::logicalNot:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Cannot perform logicalNot operation on numeric inputs");
        case UnaryOperationType::exp:
            return ComputeLoop<ValueType>(input, Exp<ValueType>);
        case UnaryOperationType::sin:
            return ComputeLoop<ValueType>(input, Sin<ValueType>);
        case UnaryOperationType::cos:
            return ComputeLoop<ValueType>(input, Cos<ValueType>);
        case UnaryOperationType::tanh:
            return ComputeLoop<ValueType>(input, Tanh<ValueType>);
        case UnaryOperationType::square:
            return ComputeLoop<ValueType>(input, Square<ValueType>);
        case UnaryOperationType::log:
            return ComputeLoop<ValueType>(input, Log<ValueType>);
        case UnaryOperationType::sigmoid:
            return ComputeLoop<ValueType>(input, Sigmoid<ValueType>);
        case UnaryOperationType::hardSigmoid:
            return ComputeLoop<ValueType>(input, HardSigmoid<ValueType>);
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown operation type");
        }
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> = true, utilities::IsNotBoolean<ValueType> = true>
    std::vector<ValueType> ComputeOutput(std::vector<ValueType> input, UnaryOperationType operation)
    {
        // math functions require floating point, so do it in float, then cast result back to int.
        std::vector<float> temp(input.size());
        std::transform(input.begin(), input.end(), temp.begin(), [&](ValueType x) { return static_cast<float>(x); });
        std::vector<float> floatResult = ComputeOutput<float>(temp, operation);
        std::vector<ValueType> result(input.size());
        std::transform(floatResult.begin(), floatResult.end(), result.begin(), [&](float x) { return static_cast<ValueType>(x); });
        return result;
    }

    template <typename ValueType, utilities::IsBoolean<ValueType> = true>
    std::vector<ValueType> ComputeOutput(std::vector<ValueType> input, UnaryOperationType operation)
    {
        switch (operation)
        {
        case UnaryOperationType::logicalNot:
        {
            auto length = input.size();
            auto output = std::vector<ValueType>(length);
            for (size_t index = 0; index < length; index++)
            {
                bool x = input[index];
                output[index] = !x;
            }
            return output;
        }
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Cannot perform numeric operation on boolean inputs");
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output = ComputeOutput<ValueType>(_input.GetValue(), _operation);
        _output.SetOutput(output);
    };

    template <typename ValueType, utilities::IsFloatingPoint<ValueType> = true>
    emitters::IRLocalScalar CompileOperator(emitters::IRLocalScalar value, UnaryOperationType operation)
    {
        emitters::IRFunctionEmitter& function = value.function;
        emitters::LLVMValue result;
        switch (operation)
        {
        case UnaryOperationType::abs:
            result = emitters::Abs(value);
            break;
        case UnaryOperationType::sqrt:
            result = emitters::Sqrt(value);
            break;
        case UnaryOperationType::exp:
            result = emitters::Exp(value);
            break;
        case UnaryOperationType::log:
            result = emitters::Log(value);
            break;
        case UnaryOperationType::logicalNot:
            result = function.LogicalNot(value);
            break;
        case UnaryOperationType::square:
            result = function.Operator(emitters::GetMultiplyForValueType<ValueType>(), value, value);
            break;
        case UnaryOperationType::sin:
            result = emitters::Sin(value);
            break;
        case UnaryOperationType::cos:
            result = emitters::Cos(value);
            break;
        case UnaryOperationType::tanh:
            result = emitters::Tanh<ValueType>(value);
            break;
        case UnaryOperationType::sigmoid:
        {
            SigmoidActivationFunction<ValueType> sigmoid;
            return sigmoid.Compile(value);
        }

        case UnaryOperationType::hardSigmoid:
        {
            HardSigmoidActivationFunction<ValueType> hardSigmoid;
            return hardSigmoid.Compile(value);
        }
        case UnaryOperationType::none:
        default:
            throw emitters::EmitterException(emitters::EmitterError::unaryOperationNotSupported);
        }
        return { function, result };
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> = true, utilities::IsNotBoolean<ValueType> = true>
    emitters::IRLocalScalar CompileOperator(emitters::IRLocalScalar value, UnaryOperationType operation)
    {
        emitters::LLVMValue x = value.value;
        auto function = value.function;
        emitters::LLVMValue xf = function.CastValue<float>(x);
        emitters::IRLocalScalar y = CompileOperator<float>({ function, xf }, operation);
        emitters::LLVMValue yi = function.CastValue<ValueType>(y.value);
        return { function, yi };
    }

    template <typename ValueType, utilities::IsBoolean<ValueType> = true>
    emitters::IRLocalScalar CompileOperator(emitters::IRLocalScalar value, UnaryOperationType operation)
    {
        if (operation == UnaryOperationType::logicalNot)
        {
            return { value.function, value.function.LogicalNot(value) };
        }
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Cannot perform numeric operation on boolean inputs");
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
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
    void UnaryOperationNode<ValueType>::CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Loop version broken
        auto count = input.Size();
        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(input);
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        function.For(count, [pInput, pResult, this](emitters::IRFunctionEmitter& function, emitters::LLVMValue i) {
            emitters::LLVMValue inputValue = function.ValueAt(pInput, i);
            emitters::IRLocalScalar pOpResult = CompileOperator<ValueType>({ function, inputValue }, _operation);
            function.SetValueAt(pResult, i, pOpResult.value);
        });
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);

        for (size_t i = 0; i < input.Size(); ++i)
        {
            emitters::LLVMValue inputValue = compiler.LoadPortElementVariable(input.GetInputElement(i));
            emitters::IRLocalScalar pOpResult = CompileOperator<ValueType>({ function, inputValue }, _operation);
            function.SetValueAt(pResult, function.Literal((int)i), pOpResult.value);
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["operation"] << ToString(_operation);
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        std::string operation;
        archiver["operation"] >> operation;
        _operation = FromString<UnaryOperationType>(operation);
        _output.SetSize(_input.Size());
    }

    // Explicit specializations
    template class UnaryOperationNode<float>;
    template class UnaryOperationNode<double>;
    template class UnaryOperationNode<int>;
    template class UnaryOperationNode<bool>;
} // namespace nodes
} // namespace ell
