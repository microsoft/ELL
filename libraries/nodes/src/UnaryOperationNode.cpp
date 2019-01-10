////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnaryOperationNode.cpp(nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnaryOperationNode.h"

#include <emitters/include/IRMath.h>

#include <utilities/include/TypeTraits.h>

#include <cmath>

#define ADD_TO_STRING_ENTRY(NAMESPACE, OPERATOR) \
    case NAMESPACE::OPERATOR:                    \
        return #OPERATOR;
#define BEGIN_FROM_STRING if (false)
#define ADD_FROM_STRING_ENTRY(NAMESPACE, OPERATOR) else if (name == #OPERATOR) return NAMESPACE::OPERATOR

using namespace ell;

namespace ell
{
namespace nodes
{
namespace operations
{
inline std::string to_string(UnaryOperationType op)
{
    switch (op)
    {
        ADD_TO_STRING_ENTRY(UnaryOperationType, none);
        ADD_TO_STRING_ENTRY(UnaryOperationType, abs);
        ADD_TO_STRING_ENTRY(UnaryOperationType, exp);
        ADD_TO_STRING_ENTRY(UnaryOperationType, hardSigmoid);
        ADD_TO_STRING_ENTRY(UnaryOperationType, log);
        ADD_TO_STRING_ENTRY(UnaryOperationType, logicalNot);
        ADD_TO_STRING_ENTRY(UnaryOperationType, sigmoid);
        ADD_TO_STRING_ENTRY(UnaryOperationType, sqrt);
        ADD_TO_STRING_ENTRY(UnaryOperationType, square);
        ADD_TO_STRING_ENTRY(UnaryOperationType, sin);
        ADD_TO_STRING_ENTRY(UnaryOperationType, cos);
        ADD_TO_STRING_ENTRY(UnaryOperationType, tanh);

    default:
        throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown unary operation");
    }
}

inline UnaryOperationType from_string(std::string name)
{
    BEGIN_FROM_STRING;
    ADD_FROM_STRING_ENTRY(UnaryOperationType, none);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, abs);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, exp);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, hardSigmoid);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, log);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, logicalNot);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, sigmoid);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, sqrt);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, square);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, sin);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, cos);
    ADD_FROM_STRING_ENTRY(UnaryOperationType, tanh);

    throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown unary operation");
}
} // namespace operations

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
        return ComputeLoop<ValueType>(input, operations::Abs<ValueType>);
    case UnaryOperationType::sqrt:
        return ComputeLoop<ValueType>(input, operations::Sqrt<ValueType>);
    case UnaryOperationType::logicalNot:
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Cannot perform logicalNot operation on numeric inputs");
    case UnaryOperationType::exp:
        return ComputeLoop<ValueType>(input, operations::Exp<ValueType>);
    case UnaryOperationType::sin:
        return ComputeLoop<ValueType>(input, operations::Sin<ValueType>);
    case UnaryOperationType::cos:
        return ComputeLoop<ValueType>(input, operations::Cos<ValueType>);
    case UnaryOperationType::tanh:
        return ComputeLoop<ValueType>(input, operations::Tanh<ValueType>);
    case UnaryOperationType::square:
        return ComputeLoop<ValueType>(input, operations::Square<ValueType>);
    case UnaryOperationType::log:
        return ComputeLoop<ValueType>(input, operations::Log<ValueType>);
    case UnaryOperationType::sigmoid:
        return ComputeLoop<ValueType>(input, operations::Sigmoid<ValueType>);
    case UnaryOperationType::hardSigmoid:
        return ComputeLoop<ValueType>(input, operations::HardSigmoid<ValueType>);
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
    archiver["operation"] << operations::to_string(_operation);
}

template <typename ValueType>
void UnaryOperationNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
{
    Node::ReadFromArchive(archiver);
    archiver[defaultInputPortName] >> _input;
    std::string operation;
    archiver["operation"] >> operation;
    _operation = operations::from_string(operation);
    _output.SetSize(_input.Size());
}

// Explicit specializations
template class UnaryOperationNode<float>;
template class UnaryOperationNode<double>;
template class UnaryOperationNode<int>;
template class UnaryOperationNode<bool>;
} // namespace nodes
} // namespace ell
