////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformContext.h (model)
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
#include "Submodel.h"

#include <utilities/include/Exception.h>

#include <cassert>
#include <exception>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace ell
{
namespace model
{
    class MapCompiler;

    /// <summary> An action to perform on a node during transformation (refinement/compilation) </summary>
    enum class NodeAction
    {
        abstain,
        refine,
        compile
    };

    /// <summary> A function that determines how to process a node </summary>
    using NodeActionFunction = std::function<NodeAction(const Node&)>;

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
        TransformContext(const MapCompiler* compiler);

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
} // namespace model
} // namespace ell
