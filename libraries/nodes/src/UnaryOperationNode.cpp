////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnaryOperationNode.cpp(nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnaryOperationNode.h"
#include "NodeOperations.h"

#include <value/include/EmitterContext.h>
#include <value/include/Value.h>
#include <value/include/ValueOperations.h>
#include <value/include/Scalar.h>
#include <value/include/ScalarOperations.h>

#include <emittable_functions/include/LogisticFunctions.h>

#include <utilities/include/Boolean.h>
#include <utilities/include/TypeTraits.h>

#include <cmath>

using namespace ell;
using namespace ell::emittable_functions;

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode() :
        CompilableCodeNode("UnaryOperationNode", { &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0),
        _operation(UnaryOperationType::none)
    {
    }

    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode(const model::OutputPort<ValueType>& input, UnaryOperationType operation) :
        CompilableCodeNode("UnaryOperationNode", { &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, _input.GetMemoryLayout()),
        _operation(operation)
    {
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<UnaryOperationNode<ValueType>>(newInputs, _operation);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Define(value::FunctionDeclaration& fn)
    {
        (void)fn.Define([this](const value::Vector data, value::Vector result) {

            auto op = _operation;
            if (op == UnaryOperationType::softmax)
            {
                Softmax(data, result);
            }
            else
            {
                For(data, [&](value::Scalar index) {
                    auto v = data(index);
                    switch (op)
                    {
                    case UnaryOperationType::none:
                        // this is a no-op on purpose
                        break;
                    case UnaryOperationType::abs:
                        v = Abs(v);
                        break;
                    case UnaryOperationType::sqrt:
                        v = Sqrt(v);
                        break;
                    case UnaryOperationType::logicalNot:
                        v = LogicalNot(v);
                        break;
                    case UnaryOperationType::exp:
                        v = Exp(v);
                        break;
                    case UnaryOperationType::sin:
                        v = Sin(v);
                        break;
                    case UnaryOperationType::cos:
                        v = Cos(v);
                        break;
                    case UnaryOperationType::tanh:
                        v = Tanh(v);
                        break;
                    case UnaryOperationType::sign:
                        v = Sign(v);
                        break;
                    case UnaryOperationType::square:
                        v = Square(v);
                        break;
                    case UnaryOperationType::log:
                        v = Log(v);
                        break;
                    case UnaryOperationType::sigmoid:
                        v = emittable_functions::Sigmoid(v);
                        break;
                    case UnaryOperationType::hardSigmoid:
                        v = emittable_functions::HardSigmoid(v);
                        break;
                    case UnaryOperationType::hardTanh:
                        v = emittable_functions::HardTanh(v);
                        break;
                    default:
                        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown unary operation type");
                    }
                    result(index) = v;
                });
            }
        });
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
    template class UnaryOperationNode<int64_t>;
    template class UnaryOperationNode<bool>;
} // namespace nodes
} // namespace ell
