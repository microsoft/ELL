////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Submodel.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Submodel.h"

#include <utilities/include/Exception.h>

#include <unordered_set>

namespace ell
{
namespace model
{
    namespace
    {
        template <typename PortType>
        const Model& GetModelOrThrow(const std::vector<PortType>& ports)
        {
            if (ports.empty())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't get model from empty output list");
            }
            return *(ports[0]->GetNode()->GetModel());
        }

        template <typename PortType1, typename PortType2>
        const Model& GetModelOrThrow(const std::vector<PortType1>& inputs, const std::vector<PortType2>& outputs)
        {
            if (outputs.empty() && inputs.empty())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't get model from empty output list");
            }
            if (inputs.empty())
            {
                return GetModelOrThrow(outputs);
            }
            else
            {
                return GetModelOrThrow(inputs);
            }
        }
    } // namespace

    Submodel::Submodel(const Model& model) :
        Submodel(model, std::vector<const InputPortBase*>{}, std::vector<const OutputPortBase*>{})
    {
    }

    Submodel::Submodel(const std::vector<const OutputPortBase*>& outputs) :
        Submodel(GetModelOrThrow(outputs), {}, outputs)
    {
    }

    Submodel::Submodel(const Model& model, const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs) :
        _model(model.ShallowCopy()),
        _inputs(inputs.begin(), inputs.end()),
        _outputs(outputs.begin(), outputs.end())
    {
        VerifyInputs();
        VerifyOutputs();
    }

    Submodel::Submodel(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs) :
        Submodel(GetModelOrThrow(inputs), inputs, outputs)
    {
    }

    Submodel::Submodel(const Model& model, const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs) :
        _model(model.ShallowCopy()),
        _inputs(inputs),
        _outputs(outputs)
    {
        VerifyInputs();
        VerifyOutputs();
    }

    Submodel::Submodel(const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs) :
        Submodel(GetModelOrThrow(inputs, outputs), inputs, outputs)
    {
    }

    Submodel::Submodel(const Submodel& other) :
        Submodel(other._model, other._inputs, other._outputs)
    {
    }

    Submodel& Submodel::operator=(Submodel other)
    {
        swap(*this, other);
        return *this;
    }

    int Submodel::Size() const
    {
        int size = 0;
        Visit([&size](const Node& node) {
            ++size;
        });
        return size;
    }

    void Submodel::VerifyInputs()
    {
        for (auto input : _inputs)
        {
            if (*(input->GetNode()->GetModel()) != _model)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Submodel input isn't from the same model as the submodel");
            }
        }

        // Verify each of the supplied inputs is necessary
        std::unordered_set<const InputPortBase*> inputs(_inputs.begin(), _inputs.end());
        std::unordered_set<const InputPortBase*> unseenInputs(_inputs.begin(), _inputs.end());
        std::unordered_set<const InputPortBase*> visitedNodes;
        for (const OutputPortBase* output : _outputs)
        {
            VerifyInputs(output, inputs, unseenInputs, visitedNodes);
        }

        if (!unseenInputs.empty())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }
    }

    void Submodel::VerifyInputs(const OutputPortBase* output, const std::unordered_set<const InputPortBase*>& inputs, std::unordered_set<const InputPortBase*>& unseenInputs, std::unordered_set<const InputPortBase*>& visitedNodes)
    {
        auto node = output->GetNode();
        for (auto input : node->GetInputPorts())
        {
            if (visitedNodes.find(input) == visitedNodes.end())
            {
                visitedNodes.insert(input);
                if (inputs.find(input) != inputs.end())
                {
                    if (unseenInputs.find(input) != unseenInputs.end())
                    {
                        unseenInputs.erase(unseenInputs.find(input));
                    }
                }
                else
                {
                    VerifyInputs(&(input->GetReferencedPort()), inputs, unseenInputs, visitedNodes);
                }
            }
        }
    }

    void Submodel::VerifyOutputs()
    {
        for (auto output : _outputs)
        {
            if (*(output->GetNode()->GetModel()) != _model)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Submodel output isn't from the same model as the submodel");
            }
        }
    }

    void swap(Submodel& a, Submodel& b)
    {
        using std::swap;
        swap(a._model, b._model);
        swap(a._inputs, b._inputs);
        swap(a._outputs, b._outputs);
    }

} // namespace model
} // namespace ell
