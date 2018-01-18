////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRMetadata.cpp (emitters)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRMetadata.h"
#include "IRModuleEmitter.h"

namespace ell
{
namespace emitters
{
    static const std::string c_structFieldsTagNamePrefix = "ell.type.fields";
    std::string GetStructFieldsTagName(llvm::StructType* structType)
    {
        if(!structType->hasName())
        {
            return "";
        }

        return c_structFieldsTagNamePrefix + "." + std::string(structType->getName());
    }

    std::vector<FunctionTagValues> GetFunctionsWithTag(IRModuleEmitter& moduleEmitter, const std::string& tag)
    {
        std::vector<FunctionTagValues> tagValues;
        auto functions = moduleEmitter.GetLLVMModule()->functions();
        for (auto& f : functions)
        {
            if (f.hasName())
            {
                std::string functionName = f.getName();
                if (moduleEmitter.HasFunctionMetadata(functionName, tag))
                {
                    auto values = moduleEmitter.GetFunctionMetadata(functionName, tag);
                    tagValues.push_back(FunctionTagValues{ &f, std::move(values) });
                }
            }
        }
        return tagValues;
    }

    std::unordered_set<std::string> GetSingletonModuleTagValues(IRModuleEmitter& moduleEmitter, const std::string& tag)
    {
        std::unordered_set<std::string> tagValues;
        if (moduleEmitter.HasMetadata(tag))
        {
            auto values = moduleEmitter.GetMetadata(tag);
            for (const auto& v : values)
            {
                tagValues.insert(v[0]);
            }
        }
        return tagValues;
    }
}
}
