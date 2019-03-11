////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Submodel.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputPort.h"
#include "Model.h"
#include "OutputPort.h"

#include <string>
#include <utility>
#include <vector>

namespace ell
{
namespace model
{
    /// <summary> A submodel represents a subset of a model that is able to compute a specified set of outputs. </summary>
    ///
    /// The inputs of a submodel are a set of input ports that are necessary to compute the outputs. If
    /// no inputs are specified in the constructor, the inputs to the submodel are the necessary source/input nodes
    /// from the original model.
    ///
    /// If inputs are specified, the submodel uses those inputs, rather than tracing back to the original source/input nodes of the
    /// model. If the specified inputs are not sufficient to compute the desired outputs, then any necessary inputs are taken from
    /// the original model.
    ///
    /// If more inputs are specified than necessary to compute the outputs, an exception is thrown.
    class Submodel
    {
    public:
        /// <summary> Constructor </summary>
        Submodel(const Model& model);

        /// <summary> Constructor </summary>
        Submodel(const Model& model, const std::vector<const OutputPortBase*>& outputs);

        /// <summary> Constructor </summary>
        Submodel(const Model& model, const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs);

        /// <summary> Constructor </summary>
        Submodel(const Model& model, const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs);

        Submodel(const Submodel& other);
        Submodel(Submodel&& other) = default;

        Submodel& operator=(Submodel other);

        /// Returns the size (the number of nodes in the submodel)
        int Size() const;

        /// <summary>
        /// Visits all the nodes in the submodel in dependency order. No nodes will be visited until all
        /// its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="visitor"> The visitor functor to use. The type signature should be of the form `void visitor(const Node&)`. </param>
        template <typename Visitor>
        void Visit(Visitor&& visitor) const;

        /// <summary> Returns the model this is a subset of </summary>
        const Model& GetModel() const { return _model; }
        Model& GetModel() { return _model; }

        /// <summary> Returns the number of input ports for this submodel </summary>
        int NumInputs() const { return _inputs.size(); }

        /// <summary> Returns the number of output ports for this submodel </summary>
        int NumOutputs() const { return _outputs.size(); }

        /// <summary> Returns the input ports for this submodel </summary>
        const std::vector<const InputPortBase*>& GetInputs() const { return _inputs; }

        /// <summary> Returns the output ports for this submodel </summary>
        const std::vector<const OutputPortBase*>& GetOutputs() const { return _outputs; }

    private:
        friend void swap(Submodel& a, Submodel& b);
        void VerifyInputs();
        void VerifyInputs(const OutputPortBase* output, const std::unordered_set<const InputPortBase*>& inputs, std::unordered_set<const InputPortBase*>& unseenInputs, std::unordered_set<const InputPortBase*>& visitedNodes);

        Model _model;
        std::vector<const InputPortBase*> _inputs;
        std::vector<const OutputPortBase*> _outputs;
    };
} // namespace model
} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    // Submodel implementation
    template <typename Visitor>
    void Submodel::Visit(Visitor&& visitor) const
    {
        _model.VisitSubmodel(_inputs, _outputs, std::forward<Visitor>(visitor));
    }
} // namespace model
} // namespace ell

#pragma endregion implementation
