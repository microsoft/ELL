////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Submodel.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Submodel.h"

#include <unordered_set>

namespace ell
{
namespace model
{
    Submodel::Submodel(const Model& model, const std::vector<const OutputPortBase*>& outputs) :
        Submodel(model, {}, outputs)
    {
    }

    Submodel::Submodel(const Model& model, const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs) :
        _model(model.ShallowCopy()),
        _inputs(inputs),
        _outputs(outputs)
    {
        VerifyInputs();
    }

    Submodel::Submodel(const Submodel& other) : Submodel(other._model, other._inputs, other._outputs)
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
        // TODO: verify that each of the supplied inputs is necessary
        std::unordered_set<const InputPortBase*> inputs(_inputs.begin(), _inputs.end());
        std::unordered_set<const InputPortBase*> unseenInputs(_inputs.begin(), _inputs.end());
        for (const OutputPortBase* output : _outputs)
        {
            VerifyInputs(output, inputs, unseenInputs);
        }

        if (!unseenInputs.empty())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }
    }

    void Submodel::VerifyInputs(const OutputPortBase* output, const std::unordered_set<const InputPortBase*>& inputs, std::unordered_set<const InputPortBase*>& unseenInputs)
    {
        auto node = output->GetNode();
        for (auto input: node->GetInputPorts())
        {
            if(inputs.find(input) != inputs.end())
            {
                if (unseenInputs.find(input) != unseenInputs.end())
                {
                    unseenInputs.erase(unseenInputs.find(input));
                }
            }
            else
            {
                VerifyInputs(&(input->GetReferencedPort()), inputs, unseenInputs);
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
