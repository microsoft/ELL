////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DgmlGraph.h
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// stl
#include <ostream>
#include <map>
#include <string>
#include <vector>

// These helper classes make it possible to model a graph of nodes and links so various ELL tools
// can write out either DOT or Directed Graph Markup Language (DGML) file formats.
// See https://en.wikipedia.org/wiki/DOT_(graph_description_language)
// See https://en.wikipedia.org/wiki/DGML

namespace ell
{
namespace utilities
{
    class GraphNode;
    class GraphLink;
    class GraphStyle;
    class GraphProperty;

    /// <summary> This class represents a directed graph consisting of nodes and links. </summary>
    class Graph
    {
    public:
        /// <summary> This method is how you add nodes to the graph. </summary>
        /// <param name="id"> Each node in the graph must have a unique id, try and use valid programming identifiers to make serialization easier. </param>
        /// <param name="label"> Each node cna have a display label. </param>
        GraphNode& GetOrCreateNode(const std::string& id, const std::string& label);

        /// <summary> This method is how you add links to the graph. </summary>
        /// <param name="source"> The node on the source end the link. </param>
        /// <param name="target"> The node on the target end the link. </param>
        /// <param name="category"> An optional category. </param>
        GraphLink& GetOrCreateLink(GraphNode& source, GraphNode& target, const std::string& category);

        /// <summary> This method is how you find a node by it's id. </summary>
        /// <returns> The node with given id or nullptr if node is not defined in this graph. </returns>
        GraphNode* GetNode(const std::string& id);

        /// <summary> DGML graphs can also contain groups of nodes.  You build a group by adding links from this group node to it's children
        /// with links that have the category 'contains'.</summary>
        GraphNode& GetOrCreateGroup(const std::string& id, const std::string& label);

        /// <summary> Serialize the graph in DOT Language format. </summary>
        void SaveDot(std::ostream& fout);

        /// <summary> Serialize the graph in DGML format. </summary>
        void SaveDgml(std::ostream& fout);

        /// <summary> This provides a way to style a given type of node or link. </summary>
        void AddStyle(GraphStyle style);

        /// <summary> This provides a way to extend the types of properties you can define on a node. </summary>
        void AddProperty(GraphProperty prop);

    private:
        std::string EscapeAttribute(const std::string& value);
        std::string ValidDotIdentifier(const std::string& value);
        std::map<std::string, GraphNode> _nodes;
        std::map<std::string, GraphLink> _links;
        std::vector<GraphStyle> _styles;
        std::vector<GraphProperty> _properties;
    };

    /// <summary> A single node in the graph. </summary>
    class GraphNode
    {
    private:
        friend class Graph;
        friend class GraphLink;
        /// <summary> Create a new GraphNode - see Graph::GetOrCreateNode</summary>
        GraphNode(std::string id, std::string label, bool isGroup = false);

    public:
        GraphNode();
        GraphNode(GraphNode&& other);
        GraphNode(const GraphNode& other);
        GraphNode& operator=(GraphNode& other);

        /// <summary> Get the unique Id of this node </summary>
        const std::string& GetId() { return _id; }

        /// <summary> Get the optional display label for this node </summary>
        const std::string& GetLabel() { return _label; }

        /// <summary> Set the optional display label for this node </summary>
        void SetLabel(std::string label) { _label = label; }

        /// <summary> Get the properties and associated values defined on this node </summary>
        std::map<std::string, std::string>& GetProperties() { return _properties; }

        /// <summary> Set a property and associated values defined on this node </summary>
        const std::string& GetProperty(const std::string& name);

        /// <summary> Set a property and associated values defined on this node </summary>
        void SetProperty(const std::string& name, const std::string& value);

        void SetIsGroup(bool isGroup) { _is_group = isGroup; }
        bool GetIsGroup() { return _is_group; }

    private:
        std::map<std::string, std::string> _properties;
        bool _isGroup = false;
        std::string _id;
        std::string _label;
        bool _is_group = false;
    };

    /// <summary> Represents a directed link between two nodes in the Graph</summary>
    class GraphLink
    {
    private:
        friend class Graph;
        /// <summary> Create a new GraphLink - see Graph::GetOrCreateLink</summary>
        GraphLink(GraphNode& source, GraphNode& target, std::string category);

    public:
        GraphLink();
        GraphLink(GraphLink&& other);
        GraphLink(const GraphLink& other);
        GraphLink& operator=(GraphLink& other);

        /// <summary> Get the node at the Source end of this link</summary>
        GraphNode& GetSource() { return _source; }
        /// <summary> Get the node at the Target end of this link</summary>
        GraphNode& GetTarget() { return _target; }
        /// <summary> Get the optional Category defined on this link</summary>
        std::string GetCategory() { return _category; }

    private:
        GraphNode _source;
        GraphNode _target;
        std::string _category;
    };

    /// <summary> A GraphStyleSetter represents a conditional setter of a given property of a node.
    /// These can be used in a GraphStyle and can be used to style a whole set of nodes that
    /// match the GraphStyleCondition.</summary>
    class GraphStyleSetter
    {
    public:
        /// <summary> Create a new GraphStyleSetter</summary>
        /// <param name="propertyName"> The property being conditionally set on the node. </param>
        /// <param name="value"> The value of the property. </param>
        /// <param name="expression"> An expression to compute the value of the property. </param>
        GraphStyleSetter(std::string propertyName, std::string value, std::string expression);

        /// <summary> Get the name of the property being acted on by this style setter</summary>
        const std::string& GetProperty() { return _property; }
        /// <summary> Get the property value</summary>
        const std::string& GetValue() { return _value; }
        /// <summary> Get the expression value</summary>
        const std::string& GetExpression() { return _expression; }

    private:
        std::string _property;
        std::string _value;
        std::string _expression;
    };

    /// <summary> A GraphStyleCondition a single expression for selecting a set of nodes.
    /// For example, the expression "Error > 10" would select all nodes that have an Error property with a value greater than 10.
    /// </summary>
    class GraphStyleCondition
    {
    public:
        /// <summary> Create a new GraphStyleCondition</summary>
        /// <param name="expression"> The expression. </param>
        GraphStyleCondition(std::string expression);
        GraphStyleCondition(const GraphStyleCondition& other);
        /// <summary> Get the value of the expression</summary>
        const std::string& GetExpression() { return _expression; }

    private:
        std::string _expression;
    };

    /// <summary> A GraphStyle can style nodes or links selected by the given GraphStyleCondition, by applying all the
    /// setters to those matching nodes/links</summary>
    class GraphStyle
    {
    public:
        /// <summary> Create a new GraphStyle</summary>
        /// <param name="targetType"> Must be 'node' or 'link'. </param>
        /// <param name="groupLabel"> The display name of this style for the legend. </param>
        /// <param name="valueLabel"> The second display name of this style. </param>
        /// <param name="condition"> The conditional expression that selects matching nodes or links. </param>
        GraphStyle(std::string targetType, std::string groupLabel, std::string valueLabel, GraphStyleCondition condition);
        /// <summary> Get the target type defined on this style</summary>
        const std::string& GetTargetType() { return _targetType; }
        /// <summary> Get the group label defined on this style</summary>
        const std::string& GetGroupLabel() { return _groupLabel; }
        /// <summary> Get the value label defined style</summary>
        const std::string& GetValueLabel() { return _valueLabel; }
        /// <summary> Get the conditional expression defined on this style</summary>
        GraphStyleCondition GetCondition() { return _condition; }
        /// <summary> Get the the editable list of setters in this GraphStyle</summary>
        std::vector<GraphStyleSetter>& GetSetters() { return _setters; }

    private:
        std::string _targetType;
        std::string _groupLabel;
        std::string _valueLabel;
        GraphStyleCondition _condition;
        std::vector<GraphStyleSetter> _setters;
    };

    /// <summary> A GraphProperty is like metadata, it defines the types of properties that can be defined on a node or link</summary>
    class GraphProperty
    {
    public:
        /// <summary> Create a new GraphProperty</summary>
        /// <param name="id"> The unique id of this property. </param>
        /// <param name="label"> The display name of this property. </param>
        /// <param name="description"> The long description of this property. </param>
        /// <param name="dataType"> The data type of this property, (int, double, float, string, bool, color). </param>
        GraphProperty(std::string id, std::string label, std::string description, std::string dataType);
        /// <summary> Get the id defined on this property</summary>
        const std::string& GetId() { return _id; }
        /// <summary> Get the label defined on this property</summary>
        const std::string& GetLabel() { return _label; }
        /// <summary> Get the description defined on this property</summary>
        const std::string& GetDescription() { return _description; }
        /// <summary> Get the data type defined on this property</summary>
        const std::string& GetDataType() { return _dataType; }

    private:
        std::string _id;
        std::string _label;
        std::string _description;
        std::string _dataType;
    };
}
}