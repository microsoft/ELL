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

public:
    DgmlNode& GetOrCreateGroup(const std::string& id, const std::string& label)
    {
        if (nodes.find(id) == nodes.end())
        {
            nodes[id] = DgmlNode{ id, label, true };
        }
        return nodes[id];
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
    void replaceAll(std::string& str, const std::string& from, const std::string& to)
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
        replaceAll(result, "&", "&amp;");
        replaceAll(result, "<", "&lt;");
        replaceAll(result, ">", "&gt;");
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
        fout << "</DirectedGraph>" << std::endl;
    }
};
