////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRMetadata.cpp (emitters)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRMetadata.h"

namespace ell
{
namespace emitters
{
    std::vector<FunctionTagValues> GetFunctionsWithTag(IRModuleEmitter& moduleEmitter, const std::string& tag)
    {
        std::vector<FunctionTagValues> tagValues;
        auto functions = moduleEmitter.GetLLVMModule()->functions();
        for (auto& f : functions)
        {
            if (f.hasName())
            {
                std::string functionName = f.getName();
                if (moduleEmitter.HasMetadata(functionName, tag))
                {
                    auto values = moduleEmitter.GetMetadata(functionName, tag);
                    tagValues.push_back(FunctionTagValues{ &f, std::move(values) });
                }
            }
        }
        return tagValues;
    }

    std::unordered_set<std::string> GetModuleTagValues(IRModuleEmitter& moduleEmitter, const std::string& tag)
    {
        std::unordered_set<std::string> tagValues;
        if (moduleEmitter.HasMetadata("", tag))
        {
            auto values = moduleEmitter.GetMetadata("", tag);
            for (const auto& v : values)
            {
                tagValues.insert(v);
            }
        }
        return tagValues;
    }
}
}
