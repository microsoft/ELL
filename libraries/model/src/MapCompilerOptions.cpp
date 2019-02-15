////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompilerOptions.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapCompilerOptions.h"

namespace ell
{
namespace model
{
    /// <summary> Constructor from a property bag </summary>
    MapCompilerOptions::MapCompilerOptions(const utilities::PropertyBag& properties)
    {
        AddOptions(properties);
    }

    MapCompilerOptions MapCompilerOptions::AppendOptions(const utilities::PropertyBag& properties) const
    {
        MapCompilerOptions result = *this;
        result.AddOptions(properties);
        return result;
    }

    void MapCompilerOptions::AddOptions(const utilities::PropertyBag& properties)
    {
        moduleName = properties.GetOrParseEntry("moduleName", moduleName);
        mapFunctionName = properties.GetOrParseEntry("mapFunctionName", mapFunctionName);
        sourceFunctionName = properties.GetOrParseEntry("sourceFunctionName", sourceFunctionName);
        sinkFunctionName = properties.GetOrParseEntry("sinkFunctionName", sinkFunctionName);
        verifyJittedModule = properties.GetOrParseEntry("verifyJittedModule", verifyJittedModule);
        profile = properties.GetOrParseEntry("profile", profile);
        inlineNodes = properties.GetOrParseEntry("inlineNodes", inlineNodes);
    }
} // namespace model
} // namespace ell
