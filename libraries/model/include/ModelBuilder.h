////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelBuilder.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Model.h"
#include "Node.h"

#include <utilities/include/FunctionUtils.h>
#include <utilities/include/TupleUtils.h>
#include <utilities/include/Variant.h>

#include <map>
#include <string>
#include <vector>

namespace ell
{
namespace model
{
    class Node;

    /// <summary> Helper class for building models </summary>
    class ModelBuilder
    {
    public:
        /// <summary> Creates a node and adds it to the model being built </summary>
        ///
        /// <param name="args"> The arguments to pass to `Model::AddNode` </param>
        template <typename NodeType, typename... Args>
        NodeType* AddNode(Model& model, Args&&... args);

        /// <summary> Creates a node and adds it to the model being built </summary>
        ///
        /// <param named="args"> A vector of `Variant`s to pass to the node creator function registered with `ModelBuilder` </param>
        /// <remarks> Throws an exception on error </remarks>
        Node* AddNode(Model& model, const std::string& nodeTypeName, const std::vector<ell::utilities::Variant>& args);

        /// <summary> Creates a node and adds it to the model being built </summary>
        ///
        /// <param named="args"> A vector of `Variant`s to pass to the node creator function registered with `ModelBuilder` </param>
        /// <returns> Returns false on error </returns>
        bool TryAddNode(Model& model, const std::string& nodeTypeName, const std::vector<ell::utilities::Variant>& args, Node*& node);

        /// <summary> Creates a node and adds it to the model being built, using a vector of strings </summary>
        ///
        /// <param named="args"> A vector of strings to pass to the node creator function registered with `ModelBuilder` </param>
        /// <remarks> Throws an exception on error </remarks>
        Node* AddNode(Model& model, const std::string& nodeTypeName, const std::vector<std::string>& args);

        /// <summary> Creates a node and adds it to the model being built, using a vector of strings </summary>
        ///
        /// <param named="args"> A vector of strings to pass to the node creator function registered with `ModelBuilder` </param>
        /// <returns> Returns false on error </returns>
        bool TryAddNode(Model& model, const std::string& nodeTypeName, const std::vector<std::string>& args, Node*& node);

        /// <summary>
        /// Returns a vector of default-initialized `Variant`s of the types expected by the node creator function for the given node type.
        /// <summary>
        template <typename NodeType>
        std::vector<ell::utilities::Variant> GetAddNodeArgs();

        /// <summary>
        /// Returns a vector of default-initialized `Variant`s of the types expected by the node creator function for the given node type.
        /// <summary>
        std::vector<ell::utilities::Variant> GetAddNodeArgs(std::string nodeTypeName) const;

        /// <summary>
        /// Returns `true` if the node type has a creator function registered with the model builder
        /// <summary>
        bool CanAddNode(std::string nodeTypeName) const;

        /// <summary>
        /// Registers a node-creation function for a node type that already has an AddNode<NodeType>() function.
        /// </summary>
        ///
        /// Example:
        /// ```
        /// ModelBuilder::RegisterNodeCreator<InputNode<double>, size_t>();
        /// ```
        /// </example>
        /// </summary>
        template <typename NodeType, typename... ArgTypes>
        void RegisterNodeCreator();

        /// <summary>
        /// Registers a node-creation function that creates a node and adds it to the model being built.
        /// </summary>
        ///
        /// A node-creation function is of the form `Node* Create(ModelBuilder& builder, args...)`, and
        /// is often a simple function that merely returns the result of `builder.AddNode<NodeType>(args...)`
        ///
        /// <param name="creatorName"> The name to use when registering the creator function </param>
        /// Example:
        /// ```
        /// Node* CreateMyNode(ModelBuilder& builder, int param1, float param2);
        /// ...
        /// ModelBuilder::RegisterNodeCreator<MyNode>(CreateMyNode);
        /// ```
        template <typename NodeType, typename FunctionType>
        void RegisterNodeCreator(const std::string& creatorName, FunctionType creatorFunction);

    private:
        std::vector<ell::utilities::Variant> EncodePortElements(const std::vector<ell::utilities::Variant>& args) const;
        std::vector<ell::utilities::Variant> TransformArgsForFunction(const Model& model, const std::string& nodeTypeName, const std::vector<ell::utilities::Variant>& args) const;
        std::vector<ell::utilities::Variant> TransformArgsForFunction(const Model& model, const std::string& nodeTypeName, const std::vector<std::string>& args) const;

        // List of add-node functions and get-node-creator-arguments functions
        std::map<std::string, std::function<Node*(Model& model, const std::vector<ell::utilities::Variant>&)>> _addNodeFunctions;
        std::map<std::string, std::function<std::vector<ell::utilities::Variant>()>> _getNodeArgsFunctions;
    };
} // namespace model
} // namespace ell

#include "../tcc/ModelBuilder.tcc"
