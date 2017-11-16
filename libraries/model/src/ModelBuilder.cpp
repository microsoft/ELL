////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelBuilder.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelBuilder.h"

// utilities
#include "Exception.h"
#include "JsonArchiver.h"

// stl
#include <sstream>

namespace ell
{
namespace model
{
    Node* ModelBuilder::AddNode(Model& model, const std::string& nodeTypeName, const std::vector<utilities::Variant>& args)
    {
        Node* temp = nullptr;
        bool valid = TryAddNode(model, nodeTypeName, args, temp);
        if (valid)
        {
            return temp;
        }

        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unknown node type");
    }

    bool ModelBuilder::TryAddNode(Model& model, const std::string& nodeTypeName, const std::vector<utilities::Variant>& args, Node*& nodeOut)
    {
        auto transformedArgs = TransformArgsForFunction(model, nodeTypeName, args);
        auto functionIterator = _addNodeFunctions.find(nodeTypeName);
        if (functionIterator != _addNodeFunctions.end())
        {
            nodeOut = (functionIterator->second)(model, transformedArgs);
            return true;
        }

        return false;
    }

    Node* ModelBuilder::AddNode(Model& model, const std::string& nodeTypeName, const std::vector<std::string>& args)
    {
        Node* temp = nullptr;
        bool valid = TryAddNode(model, nodeTypeName, args, temp);
        if (valid)
        {
            return temp;
        }

        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unknown node type");
    }

    bool ModelBuilder::TryAddNode(Model& model, const std::string& nodeTypeName, const std::vector<std::string>& args, Node*& nodeOut)
    {
        auto transformedArgs = TransformArgsForFunction(model, nodeTypeName, args);
        auto functionIterator = _addNodeFunctions.find(nodeTypeName);
        if (functionIterator != _addNodeFunctions.end())
        {
            nodeOut = (functionIterator->second)(model, transformedArgs);
            return true;
        }

        return false;
    }

    bool ModelBuilder::CanAddNode(std::string nodeTypeName) const
    {
        auto functionIterator = _getNodeArgsFunctions.find(nodeTypeName);
        return functionIterator != _getNodeArgsFunctions.end();
    }

    std::vector<utilities::Variant> ModelBuilder::GetAddNodeArgs(std::string nodeTypeName) const
    {
        auto functionIterator = _getNodeArgsFunctions.find(nodeTypeName);
        if (functionIterator != _getNodeArgsFunctions.end())
        {
            auto args = functionIterator->second();
            return EncodePortElements(args);
        }
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unknown node type");
    }

    //
    // private
    //
    std::vector<utilities::Variant> ModelBuilder::EncodePortElements(const std::vector<utilities::Variant>& args) const
    {
        std::vector<utilities::Variant> result;
        result.reserve(args.size());
        for (const auto& arg : args)
        {
            // if v is a PortElements<T>, replace it with a PortElementsProxy
            if (arg.IsType<PortElements<bool>>())
            {
                auto val = arg.GetValue<PortElements<bool>>();
                result.emplace_back(PortElementsToProxy(val));
            }
            else if (arg.IsType<PortElements<int>>())
            {
                auto val = arg.GetValue<PortElements<int>>();
                result.emplace_back(PortElementsToProxy(val));
            }
            else if (arg.IsType<PortElements<double>>())
            {
                auto val = arg.GetValue<PortElements<double>>();
                result.emplace_back(PortElementsToProxy(val));
            }
            else
            {
                result.emplace_back(arg);
            }
        }
        return result;
    }

    std::vector<utilities::Variant> ModelBuilder::TransformArgsForFunction(const Model& model, const std::string& nodeTypeName, const std::vector<utilities::Variant>& args) const
    {
        std::vector<utilities::Variant> result = GetAddNodeArgs(nodeTypeName);
        if (result.size() != args.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Arg sizes don't match");
        }

        for (size_t index = 0; index < args.size(); ++index)
        {
            auto v = args[index];

            // if v is a PortElementsProxy, replace it with a PortElements<T> (using model to look up nodes by ID)
            if (v.IsType<PortElementsProxy>())
            {
                PortElementsProxy originalProxy = result[index].GetValue<PortElementsProxy>();
                PortElementsProxy proxy = v.GetValue<PortElementsProxy>();
                switch (proxy.GetPortType())
                {
                    case Port::PortType::smallReal:
                        result[index] = ProxyToPortElements<typename PortTypeToValueType<Port::PortType::smallReal>::value_type>(model, proxy);
                        break;
                    case Port::PortType::real:
                        result[index] = ProxyToPortElements<typename PortTypeToValueType<Port::PortType::real>::value_type>(model, proxy);
                        break;
                    case Port::PortType::integer:
                        result[index] = ProxyToPortElements<typename PortTypeToValueType<Port::PortType::integer>::value_type>(model, proxy);
                        break;
                    case Port::PortType::bigInt:
                        result[index] = ProxyToPortElements<typename PortTypeToValueType<Port::PortType::bigInt>::value_type>(model, proxy);
                        break;
                    case Port::PortType::boolean:
                        result[index] = ProxyToPortElements<typename PortTypeToValueType<Port::PortType::boolean>::value_type>(model, proxy);
                        break;
                    default:
                        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unknown port type");
                }
            }
            else
            {
                result[index].SetValueFrom(v);
            }
        }
        return result;
    }

    std::vector<utilities::Variant> ModelBuilder::TransformArgsForFunction(const Model& model, const std::string& nodeTypeName, const std::vector<std::string>& args) const
    {
        std::vector<utilities::Variant> result = GetAddNodeArgs(nodeTypeName);
        if (result.size() != args.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Arg sizes don't match");
        }

        for (size_t index = 0; index < args.size(); ++index)
        {
            auto s = args[index];
            if (result[index].IsType<PortElementsProxy>())
            {
                PortElementsProxy originalProxy = result[index].GetValue<PortElementsProxy>();
                PortElementsProxy proxy = model::ParsePortElementsProxy(s);
                auto elementsBase = ProxyToPortElements(model, proxy);
                switch (originalProxy.GetPortType())
                {
                    case Port::PortType::smallReal:
                        result[index] = PortElements<typename PortTypeToValueType<Port::PortType::smallReal>::value_type>(elementsBase);
                        break;
                    case Port::PortType::real:
                        result[index] = PortElements<typename PortTypeToValueType<Port::PortType::real>::value_type>(elementsBase);
                        break;
                    case Port::PortType::integer:
                        result[index] = PortElements<typename PortTypeToValueType<Port::PortType::integer>::value_type>(elementsBase);
                        break;
                    case Port::PortType::bigInt:
                        result[index] = PortElements<typename PortTypeToValueType<Port::PortType::bigInt>::value_type>(elementsBase);
                        break;
                    case Port::PortType::boolean:
                        result[index] = PortElements<typename PortTypeToValueType<Port::PortType::boolean>::value_type>(elementsBase);
                        break;
                    default:
                        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unknown port type");
                }
            }
            else
            {
                bool success = result[index].TryParseInto(s);
                if (!success)
                {
                    return {};
                }
            }
        }
        return result;
    }
}
}
