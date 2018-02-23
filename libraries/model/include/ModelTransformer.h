////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTransformer.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputPort.h"
#include "Model.h"
#include "Node.h"
#include "OutputPort.h"
#include "Port.h"
#include "PortElements.h"

// utilities
#include "Exception.h"

// stl
#include <exception>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace ell
{
namespace model
{
    // Forward declarations
    class InputNodeBase;

    template <typename ValueType> class InputNode;

    class MapCompiler;

    /// <summary> An action to perform on a node during transformation (refinement/compilation)
    enum class NodeAction
    {
        abstain,
        refine,
        compile
    };

    /// <summary> A function that determines how to process a node </summary>
    typedef std::function<NodeAction(const Node&)> NodeActionFunction;

    /// <summary> A context object that carries information about the compiler or other process driving the transformation. </summary>
    class TransformContext
    {
    public:
        /// <summary> Default Constructor. </summary>
        TransformContext();

        /// <summary> Constructor </summary>
        ///
        /// <param name='nodeActionFunction'> A function that indicates how to override the default refinement or compilation of a node </param>
        TransformContext(const NodeActionFunction& nodeActionFunction);

        /// <summary> Constructor </summary>
        ///
        /// <param name='compiler'> The MapCompiler that is currently compiling the model </param>
        /// <param name='nodeActionFunction'> A function that indicates how to override the default refinement or compilation of a node </param>
        TransformContext(const MapCompiler* compiler, const NodeActionFunction& nodeActionFunction);

        /// <summary> Indicates if a node is compilable. </summary>
        ///
        /// <param name="node"> A node. </param>
        /// <returns> Returns true if the node is compilable. </returns>
        bool IsNodeCompilable(const Node& node) const;

        /// <summary> Gets the map compiler. </summary>
        ///
        /// <returns> Returns a pointer to the map compiler (or nullptr if one isn't defined). </returns>
        const MapCompiler* GetCompiler() const { return _compiler; }

        /// <summary> Adds a custom node action function to call during refinement </summary>
        ///
        /// <param name='nodeActionFunction'> A function that indicates how to override the default refinement or compilation of a node </param>
        void AddNodeActionFunction(const NodeActionFunction& nodeActionFunction);

        /// <summary>
        /// Gets the action to take on the node during refinement. If any custom node action
        /// have been registered with this context, return the result of the last one that
        /// returns something other than `NodeAction::abstain`. If all of the functions
        /// abstain, or there are no custom functions, return `NodeAction::compile` if the node
        /// is compilable, otherwise return `NodeAction::refine`.
        /// </summary>
        ///
        /// <param name="node"> A node. </param>
        /// <returns> A `NodeAction` enum indicating what action to take on the node </returns>
        NodeAction GetNodeAction(const Node& node) const;

    private:
        std::vector<NodeActionFunction> _nodeActionFunctions;
        const MapCompiler* _compiler;
    };

    /// <summary> A utility class that maps output ports in a model to elements in a transformed model. </summary>
    class PortOutputsMap
    {
    public:
        /// <summary> Clears the map </summary>
        void Clear();

        /// <summary> Checks if the map is empty </summary>
        bool IsEmpty() const;

        /// <summary> Transforms a set of output port references from the input model space to the output model space. Called by node implementors. </summary>
        ///
        /// <param name="elements"> The elements in the input model to transform to the output model space. </param>
        /// <returns> A `PortElementsBase` object representing the transformed elements in the space of the new model. </returns>
        PortElementsBase GetCorrespondingPortElements(const PortElementsBase& elements) const;

        /// <summary> Sets up an old-to-new model output mapping. Called by node implementors </summary>
        ///
        /// <param name="oldPort"> The port in the old model to map to the new model. </param>
        /// <param name="newElements"> The elements in the new model to be mapped from the old model port. </param>
        void MapNodeOutput(const OutputPortBase* oldPort, const PortElementsBase& newElements);

        /// <summary> Merges two partial port mappings. Takes a map A->B and a map B->C and creates the map A->C. </summary>
        ///
        /// <param name="oldMap"> The port mapping from the original model to an intermediate state. </param>
        /// <param name="newMap"> The port mapping from the intermediate state to the new model. </param>
        /// <returns> A new mapping from the original model outputs to the new model outputs. </returns>
        static PortOutputsMap ConcatenateMaps(const PortOutputsMap& oldMap, const PortOutputsMap& newMap);

    private:
        std::unordered_map<const OutputPortBase*, PortElementsBase> _map;
    };

    /// <summary> A class that transforms models (including refinement and copying) </summary>
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

        /// <summary>
        /// Returns a copy of a subset of the input model, by calling Copy() on each of the model's nodes. The
        /// model returned contains the nodes sufficient to compute the given output.
        /// </summary>
        ///
        /// <param name="model"> The model. </param>
        /// <param name="outputNode"> The output node we are interested in </param>
        /// <param name="context"> The context. </param>
        /// <param name="outputNode"> The output that must be computable in the result model </param>
        ///
        /// <returns> The copied model. </returns>
        Model CopyModel(const Model& model, const Node* outputNode, const TransformContext& context);

        /// <summary>
        /// Returns a copy of a subset of the input model, by calling Copy() on each of the model's nodes. The
        /// model returned contains the nodes sufficient to compute the given output.
        /// </summary>
        ///
        /// <param name="model"> The model. </param>
        /// <param name="outputNodes"> The output nodes we are interested in </param>
        /// <param name="context"> The context. </param>
        /// <param name="outputNodes"> The outputs that must be computable in the result model </param>
        ///
        /// <returns> The copied Model. </returns>
        Model CopyModel(const Model& model, const std::vector<const Node*>& outputNodes, const TransformContext& context);

        /// <summary>
        /// Performs one or more refinement iterations on a given model and returns the result.
        /// If context.IsNodeCompilable is not set, this call performs one refinement iteration. If
        /// context.IsNodeCompilable is set, this call refines the model until all its nodes are
        /// compilable or until none of the nodes refine themselves.
        /// </summary>
        ///
        /// <param name="model"> The model. </param>
        /// <param name="context"> The context. </param>
        /// <param name="maxIterations"> The maximum number of refinement iterations. </param>
        ///
        /// <returns> The refined Model. </returns>
        Model RefineModel(Model model, const TransformContext& context, int maxIterations = 10);

        /// <summary> Transforms the model by applying a transformation function to each node </summary>
        ///
        /// <param name="model"> The model to transform. </param>
        /// <param name="context"> The TransformContext to use during the transformation </param>
        /// <param name="transformFunction"> The function to apply on each node </param>
        ///
        /// <returns> The transformed Model. </returns>
        Model TransformModel(const Model& model, const TransformContext& context, const std::function<void(const Node&, ModelTransformer&)>& transformFunction);

        /// <summary> Resets the internal state of the transformer </summary>
        void Reset();

        /// <summary> Returns the port elements from the new model corresponding to the given port on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        PortElements<ValueType> GetCorrespondingOutputs(const OutputPort<ValueType>& port) const;

        /// <summary> Returns the port elements from the new model corresponding to the given port on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        PortElementsBase GetCorrespondingOutputs(const OutputPortBase& port) const;

        /// <summary> Returns the port elements from the new model corresponding to the given elements on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        PortElements<ValueType> GetCorrespondingOutputs(const PortElements<ValueType>& elements) const;

        /// <summary> Returns the port elements from the new model corresponding to the given elements on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        PortElementsBase GetCorrespondingOutputs(const PortElementsBase& elements) const;

        /// <summary> Returns the input node from the new model corresponding to the given input node on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        template <typename ValueType>
        InputNode<ValueType>* GetCorrespondingInputNode(const InputNode<ValueType>* node) const;

        /// <summary> Returns the input node from the new model corresponding to the given input node on the input model </summary>
        /// <remarks> Only available after calling CopyModel or RefineModel </remarks>
        InputNodeBase* GetCorrespondingInputNode(const InputNodeBase* node) const;

        ///
        /// Functions used by node implementors
        ///

        /// <summary> Creates a new node in the transformed model. Called by node implementors. </summary>
        ///
        /// <typeparam name="Args"> The arguments to the constructor of NodeType. </typeparam>
        template <typename NodeType, typename... Args>
        NodeType* AddNode(Args&&... args);

        /// <summary> Sets up an old-to-new model output mapping. Called by node implementors </summary>
        ///
        /// <param name="oldPort"> The port in the old model to map to the new model. </param>
        /// <param name="newPort"> The port in the new model to be mapped from the old model. </param>
        template <typename ValueType>
        void MapNodeOutput(const OutputPort<ValueType>& oldPort, const OutputPort<ValueType>& newPort);

        /// <summary> Sets up an old-to-new model output mapping. Called by node implementors </summary>
        ///
        /// <param name="oldPort"> The port in the old model to map to the new model. </param>
        /// <param name="newPort"> The port in the new model to be mapped from the old model. </param>
        template <typename ValueType>
        void MapNodeOutput(const OutputPort<ValueType>& oldPort, const PortElements<ValueType>& newElements);

        /// <summary> Sets up an old-to-new model output mapping. Called by node implementors </summary>
        ///
        /// <param name="oldPort"> The port in the old model to map to the new model. </param>
        /// <param name="newElements"> The elements in the new model to be mapped from the old model. </param>
        template <typename ValueType>
        void MapNodeOutput(const OutputPort<ValueType>& oldPort, const PortElementsBase& newElements);

        /// <summary> Get the context used by the transformer. Called by node implementors </summary>
        ///
        /// <returns> The context in use by the transformer. </returns>
        TransformContext& GetContext() { return _context; }

        /// <summary> Get the context used by the transformer. Called by node implementors </summary>
        ///
        /// <returns> The context in use by the transformer. </returns>
        const TransformContext& GetContext() const { return _context; }

        //
        // Deprecated functions
        //

        /// <summary> Transforms a set of output port references from the input model space to the output model space. DEPRECATED: use GetCorrespondingOutputs instead. </summary>
        ///
        /// <param name="elements"> The elements in the input model to transform to the output model space. </param>
        /// <returns> A `PortElements` object representing the transformed elements in the space of the new model. </returns>
        template <typename ValueType>
        PortElements<ValueType> TransformPortElements(const PortElements<ValueType>& elements) const;

        /// <summary> Transforms a set of output port references from the input model space to the output model space. DEPRECATED: use GetCorrespondingOutputs instead. </summary>
        ///
        /// <param name="elements"> The elements in the input model to transform to the output model space. </param>
        /// <returns> A `PortElementsBase` object representing the transformed elements in the space of the new model. </returns>
        PortElementsBase TransformPortElements(const PortElementsBase& elements) const;

    private:
        friend class Node;

        template <typename NodeType>
        NodeType* GetCorrespondingInputNodeAs(const NodeType* node) const;

        // Collect nodes that are't compilable
        std::vector<const Node*> FindUncompilableNodes(const Model& model, const TransformContext& context) const;

        Model _model;
        TransformContext _context;
        PortOutputsMap _elementsMap;
        bool _isModelCompilable;
    };
}
}

#include "../tcc/ModelTransformer.tcc"
