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

    class TransformContext
    {
    };

    // TODO: template transformer on the context type
    class ModelTransformer
    {
    public:
        ModelTransformer(TransformContext context) : _context(context) {}

        /// <summary> Returns a copy of the input model, by calling Copy() on each of the model's nodes </summary>
        Model CopyModel(const Model& model);

        /// <summary> Returns a refined version of the input model, by calling Refine() on each of the model's nodes </summary>
        Model RefineModel(const Model& model);

        /// <summary> Returns the  OutputPort from new new model corresponding to the given port on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        const OutputPort<ValueType>* GetCorrespondingOutputPort(const OutputPort<ValueType>& port);

        /// <summary> Returns the input node from new new model corresponding to the given input node on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        InputNode<ValueType>* GetCorrespondingInputNode(const InputNode<ValueType>* node);

        /// <summary> Returns the input node from new new model corresponding to the given input node on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        InputNode<ValueType>* GetCorrespondingInputNode(const std::shared_ptr<InputNode<ValueType>>& node);

        /// <summary> Returns the (untyped) Port from new new model corresponding to the given port on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        const Port* GetCorrespondingPort(const Port& port);

        ///
        /// Functions used by node implementors
        ///

        /// <summary> Returns an OutputPortElementList for the new model corresponding the the set of inputs referenced by the given input port. Called by node implementors. </summary>
        template <typename ValueType>
        OutputPortElementList<ValueType> TransformInputPort(const InputPort<ValueType>& input);

        /// <summary> Creates a new node in the transformed model graph. Called by node implementors. </summary>
        template <typename NodeType, typename... Args>
        NodeType* AddNode(Args&&... args);

        /// <summary> Sets up a port-port mapping. Called by node implementors </summary>
        template <typename ValueType>
        void MapOutputPort(const OutputPort<ValueType>& oldPort, const OutputPort<ValueType>& newPort);

        TransformContext& GetContext() { return _context; }

    private:
        friend class Node;

        /// <summary> Sets up a port-port mapping. Called by node implementors </summary>
        void MapPort(const Port& oldPort, const Port& newPort);

        Model _model;
        TransformContext _context;
        std::unordered_map<const Port*, Port*> _portMap;
    };
}

#include "../tcc/ModelTransformer.tcc"
