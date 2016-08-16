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
#include "PortElements.h"
#include "Node.h"

// utilities
#include "Exception.h"

// stl
#include <memory>
#include <vector>
#include <unordered_map>
#include <exception>
#include <functional>

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    class InputNode;

    /// <summary> A context object that carries information about the compiler or other process driving the transformation. </summary>
    class TransformContext
    {
    public:
        /// <summary> Default Constructor. </summary>
        TransformContext();

        /// <summary> Constructor </summary>
        ///
        /// <param name='IsNodeCompilable'> A function that indicates which nodes are compilable </param>
        TransformContext(const std::function<bool(const Node&)>& isNodeCompilable);

        /// <summary> Indicates if a node is compilable. </summary>
        ///
        /// <param name="node"> A node. </param>
        /// <returns> Returns true if the node is compilable. </returns>
        bool IsNodeCompilable(const Node& node) const;

    private:
        std::function<bool(const Node&)> _isNodeCompilableFunction;
    };

    /// <summary> A class that refines or copies models </summary>
    class ModelTransformer
    {
    public:
        /// <summary> Returns a copy of the input model, by calling Copy() on each of the model's nodes </summary>
        ///
        /// <param name="model"> The model. </param>
        /// <param name="context"> The context. </param>
        ///
        /// <returns> The copied Model. </returns>
        Model CopyModel(const Model& model, const TransformContext& context);

        /// <summary> Performs one or more refinement iterations on a given model and returns the result.
        /// If context.IsNodeCompilable is not set, this call performs one refinement iteration. If
        /// context.IsNodeCompilable is set, this call refines the model until all its nodes are
        /// compilable or until none of the nodes refine themselves. </summary>
        ///
        /// <param name="model"> The model. </param>
        /// <param name="context"> The context. </param>
        ///
        /// <returns> The refined Model. </returns>
        Model RefineModel(const Model& model, const TransformContext& context);

        /// <summary> Indicates if the last call to RefineModel produced a model that is compilable. </summary>
        ///
        /// <returns> true if the model returned by RefineModel is compilable. </returns>
        /// <remarks> Only available after calling CopyModel or RefineModel. </remarks>
        bool IsModelCompilable() const { return _isModelCompilable; }

        /// <summary> Returns the port elements from the new model corresponding to the given port on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        PortElements<ValueType> GetCorrespondingOutputs(const OutputPort<ValueType>& port);

        /// <summary> Returns the port elements from the new model corresponding to the given elements on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        PortElements<ValueType> GetCorrespondingOutputs(const PortElements<ValueType>& elements);

        /// <summary> Returns the input node from new new model corresponding to the given input node on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        InputNode<ValueType>* GetCorrespondingInputNode(const InputNode<ValueType>* node);

        ///
        /// Functions used by node implementors
        ///

        /// <summary> Transforms a set of output port references from the input model space to the output model space. Called by node implementors. </summary>
        ///
        /// <param name="elements"> The elements in the input model graph to transform to the output model space. </param>
        /// <returns> An PortElements object representing the transformed elements in the space of the new model. </returns>
        template <typename ValueType>
        PortElements<ValueType> TransformPortElements(const PortElements<ValueType>& elements);

        /// <summary> Creates a new node in the transformed model graph. Called by node implementors. </summary>
        ///
        /// <typeparam name="Args"> The arguments to the constructor of NodeType. </typeparam>
        template <typename NodeType, typename... Args>
        NodeType* AddNode(Args&&... args);

        /// <summary> Sets up a port-port mapping. Called by node implementors </summary>
        ///
        /// <param name="oldPort"> The port in the old model to map to the new model. </param>
        /// <param name="newPort"> The port in the new model to be mapped from the old model. </param>
        template <typename ValueType>
        void MapNodeOutput(const OutputPort<ValueType>& oldPort, const OutputPort<ValueType>& newPort); // TODO: need to have MapOutput(elements->elements)

        // template <typename ValueType>
        // void MapNodeOutput(const OutputPort<ValueType>& oldPort, const OutputPort<ValueType>& newPort); // TODO: need to have MapOutput(elements->elements)

        /// <summary> Get the context used by the transformer. Called by node implementors </summary>
        ///
        /// <returns> The context in use by the transformer. </returns>
        TransformContext& GetContext() { return _context; }

        /// <summary> Gets the underlying model. </summary>
        ///
        /// <returns> The model. </returns>
        Model& GetModel() { return _model; }

    private:
        friend class Node;

        // Sets up a port-port mapping. Called by node implementors
        void MapPort(const OutputPortBase& oldPort, const OutputPortBase& newPort);

        // Find the name of a node that isn't compilable (if there are several, it just finds one)
        const Node* FindFirstUncompilableNode(const Model& model, const TransformContext& context) const;

        Model _model;
        TransformContext _context;
        std::unordered_map<PortElementBase, PortElementBase> _elementToElementMap;
        bool _isModelCompilable;


        // the maximal number of refinement iterations to allow
        const int maxRefinementIterations = 10;
    };
}

#include "../tcc/ModelTransformer.tcc"
