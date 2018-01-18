////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DgmlGraph.h
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Graph.h"
#include "StringUtil.h"

using namespace ell::utilities;

void Graph::AddStyle(GraphStyle style)
{
    _styles.push_back(style);
}

void Graph::AddProperty(GraphProperty prop)
{
    _properties.push_back(prop);
}

GraphNode& Graph::GetOrCreateGroup(const std::string& id, const std::string& label)
{
    if (_nodes.find(id) == _nodes.end())
    {
        GraphNode node{ id, label, true };
        _nodes[id] = node;
    }
    return _nodes[id];
}
GraphNode* Graph::GetNode(const std::string& id)
{
    if (_nodes.find(id) == _nodes.end())
    {
        return nullptr;
    }
    return &_nodes[id];
}
GraphNode& Graph::GetOrCreateNode(const std::string& id, const std::string& label)
{
    if (_nodes.find(id) == _nodes.end())
    {
        GraphNode node{ id, label };
        _nodes[id] = node;
    }
    return _nodes[id];
}
GraphLink& Graph::GetOrCreateLink(GraphNode& source, GraphNode& target, const std::string& category)
{
    std::string key = source.GetId() + "->" + target.GetId();
    if (_links.find(key) == _links.end())
    {
        GraphLink link{ source, target, category };
        _links[key] = link;
    }
    return _links[key];
}
std::string Graph::EscapeAttribute(const std::string& value)
{
    std::string result(value);
    ReplaceAll(result, "&", "&amp;");
    ReplaceAll(result, "<", "&lt;");
    ReplaceAll(result, ">", "&gt;");
    ReplaceAll(result, "'", "&apos;");
    return result;
}

std::string Graph::ValidDotIdentifier(const std::string& value)
{
    std::string result(value);
    ReplaceAll(result, "&", "_");
    ReplaceAll(result, "<", "_");
    ReplaceAll(result, ">", "_");
    ReplaceAll(result, "(", "_");
    ReplaceAll(result, ")", "_");
    ReplaceAll(result, "'", "_");
    ReplaceAll(result, ",", "_");
    return result;
}

void Graph::SaveDot(std::ostream& fout)
{
    fout << "digraph network_graph {";

    for (auto ptr = _nodes.begin(), end = _nodes.end(); ptr != end; ptr++)
    {
        GraphNode& node = ptr->second;
        fout << ValidDotIdentifier(node.GetId()) << " [ label=\"" << node.GetLabel() << "\"";
        auto node_properties = node.GetProperties();
        for (auto iter = node_properties.begin(), lastProperty = node_properties.end(); iter != lastProperty; iter++)
        {
            auto pair = *iter;
            fout << ", " << pair.first << "=\"" << pair.second << "\"";
        }
        fout << "];\n";
    }

    for (auto ptr = _links.begin(), end = _links.end(); ptr != end; ptr++)
    {
        GraphLink link = ptr->second;
        fout << ValidDotIdentifier(link.GetSource().GetId()) << " -> " << ValidDotIdentifier(link.GetTarget().GetId()) << ";\n";
    }

    fout << "}";
}

void Graph::SaveDgml(std::ostream& fout)
{
    fout << "<DirectedGraph xmlns='http://schemas.microsoft.com/vs/2009/dgml'>" << std::endl;
    fout << "  <Nodes>" << std::endl;
    for (auto ptr = _nodes.begin(), end = _nodes.end(); ptr != end; ptr++)
    {
        GraphNode& node = ptr->second;
        fout << "    <Node Id='" << EscapeAttribute(node.GetId()) << "' Label='" << EscapeAttribute(node.GetLabel()) << "'";
        if (node.GetIsGroup())
        {
            fout << " Group='Expanded'";
        }
        auto node_properties = node.GetProperties();
        for (auto iter = node_properties.begin(), lastProperty = node_properties.end(); iter != lastProperty; iter++)
        {
            auto pair = *iter;
            fout << " " << pair.first << "='" << EscapeAttribute(pair.second) << "'";
        }
        fout << "/>" << std::endl;
    }
    fout << "  </Nodes>" << std::endl;
    fout << "  <Links>" << std::endl;
    for (auto ptr = _links.begin(), end = _links.end(); ptr != end; ptr++)
    {
        GraphLink& link = ptr->second;
        fout << "    <Link Source='" << EscapeAttribute(link.GetSource().GetId()) << "' Target='" << EscapeAttribute(link.GetTarget().GetId()) << "'";
        if (link.GetCategory().size() > 0)
        {
            fout << " Category='" << EscapeAttribute(link.GetCategory()) << "'";
        }
        fout << "/>" << std::endl;
    }
    fout << "  </Links>" << std::endl;

    fout << "  <Properties>" << std::endl;
    for (auto ptr = _properties.begin(), end = _properties.end(); ptr != end; ptr++)
    {
        GraphProperty p = *ptr;
        fout << "    <Property Id='" << EscapeAttribute(p.GetId()) << "' Label='" << EscapeAttribute(p.GetLabel()) << "' Description='" << EscapeAttribute(p.GetDescription()) << "' DataType='" << EscapeAttribute(p.GetDataType()) << "'/>" << std::endl;
    }

    fout << "  </Properties>" << std::endl;

    fout << "  <Styles>" << std::endl;

    for (auto ptr = _styles.begin(), end = _styles.end(); ptr != end; ptr++)
    {
        GraphStyle style = *ptr;
        fout << "    <Style TargetType='" << EscapeAttribute(style.GetTargetType()) << "' GroupLabel='" << EscapeAttribute(style.GetGroupLabel()) << "' ValueLabel='" << EscapeAttribute(style.GetValueLabel()) << "'";
        fout << ">" << std::endl;

        if (style.GetCondition().GetExpression() != "")
        {
            fout << "        <Condition Expression='" << EscapeAttribute(style.GetCondition().GetExpression()) << "'/>" << std::endl;
        }

        auto setters = style.GetSetters();
        for (auto s = setters.begin(), e = setters.end(); s != e; s++)
        {
            GraphStyleSetter setter = *s;
            fout << "        <Setter Property='" << EscapeAttribute(setter.GetProperty()) << "'";
            if (setter.GetExpression() != "")
            {
                fout << " Expression='" << EscapeAttribute(setter.GetExpression()) << "'/>" << std::endl;
            }
            else
            {
                fout << " Value='" << EscapeAttribute(setter.GetValue()) << "'/>" << std::endl;
            }
        }

        fout << "    </Style>" << std::endl;
    }
    fout << "  </Styles>" << std::endl;

    fout << "</DirectedGraph>" << std::endl;
}

GraphNode::GraphNode(std::string id, std::string label, bool isGroup)
{
    _id = id;
    _label = label;
    _is_group = isGroup;
}

GraphNode::GraphNode(const GraphNode& other)
    : _id(other._id), _label(other._label), _is_group(other._is_group)
{
}

GraphNode& GraphNode::operator=(GraphNode& other)
{
    this->_id = other.GetId();
    this->_label = other.GetLabel();
    this->_is_group = other.GetIsGroup();
    return *this;
}

const std::string& GraphNode::GetProperty(const std::string& name)
{
    return _properties[name];
}

void GraphNode::SetProperty(const std::string& name, const std::string& value)
{
    _properties[name] = value;
}

GraphStyleSetter::GraphStyleSetter(std::string propertyName, std::string value, std::string expression)
{
    _property = propertyName;
    _value = value;
    _expression = expression;
}

GraphStyleCondition::GraphStyleCondition(std::string expression)
{
    _expression = expression;
}
GraphStyleCondition::GraphStyleCondition(const GraphStyleCondition& other)
{
    _expression = other._expression;
}

GraphStyle::GraphStyle(std::string targetType, std::string groupLabel, std::string valueLabel, GraphStyleCondition condition)
    : _condition(condition)
{
    _targetType = targetType;
    _groupLabel = groupLabel;
    _valueLabel = valueLabel;
}

GraphProperty::GraphProperty(std::string id, std::string label, std::string description, std::string dataType)
{
    _id = id;
    _label = label;
    _description = description;
    _dataType = dataType;
}

GraphLink::GraphLink(GraphNode& source, GraphNode& target, std::string category)
    : _source(source), _target(target), _category(category)
{
}

GraphLink::GraphLink(const GraphLink& other)
    : _source(other._source), _target(other._target), _category(other._category)
{
}

GraphLink& GraphLink::operator=(GraphLink& other)
{
    this->_source = other.GetSource();
    this->_target = other.GetTarget();
    this->_category = other.GetCategory();
    return *this;
}

GraphLink::GraphLink() = default;
GraphNode::GraphNode() = default;
GraphLink::GraphLink(GraphLink&& other) = default;
GraphNode::GraphNode(GraphNode&& other) = default;