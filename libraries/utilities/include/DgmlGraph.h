////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DgmlGraph.h
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class DgmlNode
{
public:
    std::string Id;
    std::string Label;
    bool IsGroup;
    std::map<std::string, std::string> Properties;

    void SetProperty(const std::string& name, const std::string& value)
    {
        Properties[name] = value;
    }
};

class DgmlStyleSetter
{
public:
    std::string Property;
    std::string Value;
    std::string Expression;
};

class DgmlStyleCondition
{
public:
    std::string Expression;
};

class DgmlStyle
{
public:
    std::string TargetType;
    std::string GroupLabel;
    std::string ValueLabel;
    DgmlStyleCondition Condition;
    std::vector<DgmlStyleSetter> Setters;
};

class DgmlProperty
{
public:
    std::string Id;
    std::string Label;
    std::string Description;
    std::string DataType;
};

class DgmlLink
{
public:
    DgmlNode Source;
    DgmlNode Target;
    std::string Category;
};
class DgmlGraph
{
    std::map<std::string, DgmlNode> nodes;
    std::map<std::string, DgmlLink> links;
    std::vector<DgmlStyle> styles;
    std::vector<DgmlProperty> properties;

public:
    void AddStyle(DgmlStyle style)
    {
        styles.push_back(style);
    }
    void AddProperty(DgmlProperty prop)
    {
        properties.push_back(prop);
    }

    DgmlNode& GetOrCreateGroup(const std::string& id, const std::string& label)
    {
        if (nodes.find(id) == nodes.end())
        {
            nodes[id] = DgmlNode{ id, label, true };
        }
        return nodes[id];
    }
    DgmlNode* GetNode(const std::string& id)
    {
        if (nodes.find(id) == nodes.end())
        {
            return nullptr;
        }
        return &nodes[id];
    }
    DgmlNode& GetOrCreateNode(const std::string& id, const std::string& label)
    {
        if (nodes.find(id) == nodes.end())
        {
            nodes[id] = DgmlNode{ id, label, false };
        }
        return nodes[id];
    }
    DgmlLink& GetOrCreateLink(DgmlNode& source, DgmlNode& target, const std::string& category)
    {
        std::string key = source.Id + "->" + target.Id;
        if (links.find(key) == links.end())
        {
            links[key] = DgmlLink{ source, target, category };
        }
        return links[key];
    }
    static void ReplaceAll(std::string& str, const std::string& from, const std::string& to)
    {
        if (from.empty())
            return;
        std::string wsRet;
        wsRet.reserve(str.length());
        size_t start_pos = 0, pos;
        while ((pos = str.find(from, start_pos)) != std::string::npos)
        {
            wsRet += str.substr(start_pos, pos - start_pos);
            wsRet += to;
            pos += from.length();
            start_pos = pos;
        }
        wsRet += str.substr(start_pos);
        str.swap(wsRet); // faster than str = wsRet;
    }
    std::string EscapeAttribute(const std::string& value)
    {
        std::string result(value);
        ReplaceAll(result, "&", "&amp;");
        ReplaceAll(result, "<", "&lt;");
        ReplaceAll(result, ">", "&gt;");
        ReplaceAll(result, "'", "&apos;");
        return result;
    }
    void Save(std::ostream& fout)
    {
        fout << "<DirectedGraph xmlns='http://schemas.microsoft.com/vs/2009/dgml'>" << std::endl;
        fout << "  <Nodes>" << std::endl;
        for (auto ptr = nodes.begin(), end = nodes.end(); ptr != end; ptr++)
        {
            DgmlNode node = ptr->second;
            fout << "    <Node Id='" << EscapeAttribute(node.Id) << "' Label='" << EscapeAttribute(node.Label) << "'";
            if (node.IsGroup)
            {
                fout << " Group='Expanded'";
            }
            for (auto iter = node.Properties.begin(), lastProperty = node.Properties.end(); iter != lastProperty; iter++)
            {
                auto pair = *iter;
                fout << " " << pair.first << "='" << EscapeAttribute(pair.second) << "'";
            }
            fout << "/>" << std::endl;
        }
        fout << "  </Nodes>" << std::endl;
        fout << "  <Links>" << std::endl;
        for (auto ptr = links.begin(), end = links.end(); ptr != end; ptr++)
        {
            DgmlLink link = ptr->second;
            fout << "    <Link Source='" << EscapeAttribute(link.Source.Id) << "' Target='" << EscapeAttribute(link.Target.Id) << "'";
            if (link.Category.size() > 0)
            {
                fout << " Category='" << EscapeAttribute(link.Category) << "'";
            }
            fout << "/>" << std::endl;
        }
        fout << "  </Links>" << std::endl;

        fout << "  <Properties>" << std::endl;
        for (auto ptr = properties.begin(), end = properties.end(); ptr != end; ptr++)
        {
            DgmlProperty p = *ptr;
            fout << "    <Property Id='" << EscapeAttribute(p.Id) << "' Label='" << EscapeAttribute(p.Label) << "' Description='" << EscapeAttribute(p.Description) << 
                "' DataType='" << EscapeAttribute(p.DataType) << "'/>" << std::endl;
        }

        fout << "  </Properties>" << std::endl;

        fout << "  <Styles>" << std::endl;

        for (auto ptr = styles.begin(), end = styles.end(); ptr != end; ptr++)
        {
            DgmlStyle style = *ptr;
            fout << "    <Style TargetType='" << EscapeAttribute(style.TargetType) << "' GroupLabel='" << EscapeAttribute(style.GroupLabel) << "' ValueLabel='" << EscapeAttribute(style.ValueLabel) << "'";
            fout << ">" << std::endl;

            if (style.Condition.Expression != "")
            {
                fout << "        <Condition Expression='" << EscapeAttribute(style.Condition.Expression) << "'/>" << std::endl;
            }

            for (auto s = style.Setters.begin(), e = style.Setters.end(); s != e; s++)
            {
                DgmlStyleSetter setter = *s;
                fout << "        <Setter Property='" << EscapeAttribute(setter.Property) << "'";
                if (setter.Expression != "") {
                    fout << " Expression='" << EscapeAttribute(setter.Expression) << "'/>" << std::endl;
                } 
                else 
                {
                    fout << " Value='" << EscapeAttribute(setter.Value) << "'/>" << std::endl;
                }
            }

            fout << "    </Style>" << std::endl;
        }
        fout << "  </Styles>" << std::endl;

        fout << "</DirectedGraph>" << std::endl;
    }
};
