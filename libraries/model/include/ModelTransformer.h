////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ModelTransformer.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelGraph.h"
#include "Port.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "OutputPortElementList.h"
#include "Node.h"
//#include "InputNode.h"
#include "OutputPort.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <exception>

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    class InputNode;

    class ModelTransformer
    {
    public:
        /// <summary> Creates a ModelTransformer object than can transform the given model </summary>
        ModelTransformer(const Model& model) : _oldModel(model) {}

        /// <summary> Returns a copy of the input model, by calling Copy() on each of the model's nodes </summary>
        Model CopyModel();

        /// <summary> Returns a refined version of the input model, by calling Refine() on each of the model's nodes </summary>
        Model RefineModel();

        /// <summary> Returns the (untyped) Port from new new model corresponding to the given port on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        const Port* GetCorrespondingPort(const Port& port);

        /// <summary> Returns the  OutputPort from new new model corresponding to the given port on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        const OutputPort<ValueType>* GetCorrespondingOutputPort(const OutputPort<ValueType>& port);

        /// <summary> Returns the input node from new new model corresponding to the given input node on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        InputNode<ValueType>* GetCorrespondingInputNode(const InputNode<ValueType>* node);

        template <typename ValueType>
        InputNode<ValueType>* GetCorrespondingInputNode(const std::shared_ptr<InputNode<ValueType>>& node);

        /// <summary> Sets up a port-port mapping. Called by node implementors </summary>
        void MapPort(const Port& oldPort, const Port& newPort);

        /// <summary> Returns an OutputPortElementList for the new model corresponding the the set of inputs referenced by the given input port. Called by node implementors. </summary>
        template <typename ValueType>
        OutputPortElementList<ValueType> TransformInputPort(const InputPort<ValueType>& input);

        /// <summary> Creates a new node in the transformed model graph. Called by node implementors. </summary>
        template <typename NodeType, typename... Args>
        std::shared_ptr<NodeType> AddNode(Args&&... args);

    private:

        const Model& _oldModel;
        Model _model;
        std::unordered_map<const Port*, Port*> _portMap;
    };
}

#include "../tcc/ModelTransformer.tcc"
