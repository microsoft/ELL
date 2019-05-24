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
GraphLink& Graph::GetOrCreateLink(GraphNode& source, GraphNode& target, const std::string& label, const std::string& category)
{
    std::string key = source.GetId() + "->" + target.GetId();
    if (_links.find(key) == _links.end())
    {
        GraphLink link{ source, target, label, category };
        _links[key] = link;
    }
    return _links[key];
}

bool ValidEntity(std::string e)
{
    size_t s = e.size();
    if (s == 0)
    {
        return false;
    }
    if (e[0] == '#')
    {
        // numeric entity
        if (s == 1)
        {
            return false;
        }
        else if (e[1] == 'x')
        {
            // hex entity
            for (size_t i = 2; i < s; i++)
            {
                char c = e[i];
                if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' || c <= 'f')))
                {
                    return false;
                }
            }
            return true;
        }
        else
        {
            // remainder must be digits
            for (size_t i = 1; i < s; i++)
            {
                char c = e[i];
                if (c < '0' || c > '9')
                {
                    return false;
                }
            }
            return true;
        }
    }
    else
    {
        // named entity
        return true;
    }
}

std::string Graph::EscapeAttribute(const std::string& value)
{
    std::string result;
    for (size_t i = 0, n = value.size(); i < n; ++i)
    {
        char c = value[i];
        switch (c)
        {
        case '&':
            for (size_t j = i + 1; j < n; j++)
            {
                char d = value[j];
                if (d == ';' || d == '&' || d == ' ' || d == '\t' || d == '\n' || d == '\r')
                {
                    // see if this is a valid "entity" already.
                    if (ValidEntity(value.substr(i + 1, j - i - 1)))
                    {
                        result.push_back(c);
                    }
                    else
                    {
                        result += "&amp;";
                    }
                    break;
                }
            }
            break;
        case '<':
            result += "&lt;";
            break;
        case '>':
            result += "&gt;";
            break;
        case '\'':
            result += "&apos;";
            break;
        default:
            result.push_back(c);
            break;
        }
    }
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
    fout << "digraph network_graph {\n";
    fout << "node [shape=box style=\"rounded, filled\"];\n";

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
        fout << ValidDotIdentifier(link.GetSource().GetId()) << " -> " << ValidDotIdentifier(link.GetTarget().GetId());
        fout << "[";
        bool first = true;
        if (!link.GetLabel().empty())
        {
            fout << "label=\"" << EscapeAttribute(link.GetLabel()) << "\"";
            first = false;
        }

        auto link_properties = link.GetProperties();
        if (!link_properties.empty())
        {
            for (auto iter = link_properties.begin(), lastProperty = link_properties.end(); iter != lastProperty; iter++)
            {
                if (!first)
                {
                    fout << ", ";
                }
                else
                {
                    first = false;
                }
                auto pair = *iter;
                fout << pair.first << "=\"" << EscapeAttribute(pair.second) << "\"";
            }
            fout << "];\n";
        }
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
        if (!link.GetLabel().empty())
        {
            fout << " Label='" << EscapeAttribute(link.GetLabel()) << "'";
        }
        if (!link.GetCategory().empty())
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
    _isGroup = isGroup;
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

GraphStyle::GraphStyle(std::string targetType, std::string groupLabel, std::string valueLabel, GraphStyleCondition condition) :
    _condition(condition)
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

GraphLink::GraphLink(GraphNode& source, GraphNode& target, std::string label, std::string category) :
    _source(source),
    _target(target),
    _label(label),
    _category(category)
{
}

const std::string& GraphLink::GetProperty(const std::string& name)
{
    return _properties[name];
}

void GraphLink::SetProperty(const std::string& name, const std::string& value)
{
    _properties[name] = value;
}

GraphLink::GraphLink() = default;
GraphNode::GraphNode() = default;
