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

    void DoUnaryOp(value::Vector& data, value::Vector& result, UnaryOperationType op)
    {
        if (op == UnaryOperationType::softmax)
        {
            Softmax(data, result);
        }
        else
        {
            For(data, [&](value::Scalar index) {
                value::Scalar v = data(index);
                value::Scalar r;
                switch (op)
                {
                case UnaryOperationType::none:
                    // this is a no-op on purpose
                    r = v;
                    break;
                case UnaryOperationType::abs:
                    r = Abs(v);
                    break;
                case UnaryOperationType::sqrt:
                    r = Sqrt(v);
                    break;
                case UnaryOperationType::logicalNot:
                    if (v.GetType() == value::ValueType::Boolean)
                    {
                        ell::utilities::Boolean t(true);
                        r = (v != t);
                    }
                    else
                    {
                        If(v == Cast(0, v.GetType()), [&]
                        {
                            r = Cast(1, v.GetType());
                        }).Else([&]
                        {
                            r = Cast(0, v.GetType());
                        });
                    }
                    break;
                case UnaryOperationType::exp:
                    r = Exp(v);
                    break;
                case UnaryOperationType::sin:
                    r = Sin(v);
                    break;
                case UnaryOperationType::cos:
                    r = Cos(v);
                    break;
                case UnaryOperationType::tanh:
                    r = Tanh(v);
                    break;
                case UnaryOperationType::square:
                    r = v * v;
                    break;
                case UnaryOperationType::log:
                    r = Log(v);
                    break;
                case UnaryOperationType::sigmoid:
                    r = emittable_functions::Sigmoid(v);
                    break;
                case UnaryOperationType::hardSigmoid:
                    r = emittable_functions::HardSigmoid(v);
                    break;
                default:
                    throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown unary operation type");
                }
                result(index) = r;
            });
        }
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Define(value::FunctionDeclaration& fn)
    {
        (void)fn.Define([this](value::Vector data, value::Vector result) {
            DoUnaryOp(data, result, _operation);
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
    template class UnaryOperationNode<bool>;
} // namespace nodes
} // namespace ell
