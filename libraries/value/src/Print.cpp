////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Print.cpp (value)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Print.h"
#include "LLVMContext.h"

#include <cstdlib>
#include <functional>

namespace ell
{
namespace value
{
    void Print(const std::string& text)
    {
        if (!InvokeForContext<LLVMContext>([&](LLVMContext& context) -> bool {
                context.GetFunctionEmitter().Print(text);
                return true;
            }))
        {
            std::printf("%s", text.c_str());
        }
    }

    void Printf(const std::vector<Value>& arguments)
    {
        if (!InvokeForContext<LLVMContext>([&](LLVMContext& context) -> bool {
                std::vector<emitters::LLVMValue> args;
                std::transform(arguments.begin(), arguments.end(), std::back_inserter(args), [&](auto x) {
                    return ToLLVMValue(x);
                });

                context.GetFunctionEmitter().Printf(args);
                return true;
            }))
        {
            std::printf("<args>");
        }
    }

    void Printf(const std::string& format, const std::vector<Value>& arguments)
    {
        if (!InvokeForContext<LLVMContext>([&](LLVMContext& context) -> bool {
                std::vector<emitters::LLVMValue> args;
                std::transform(arguments.begin(), arguments.end(), std::back_inserter(args), [&](auto x) {
                    return ToLLVMValue(x);
                });

                context.GetFunctionEmitter().Printf(format, args);
                return true;
            }))
        {
            std::printf("%s", format.c_str());
        }
    }

} // namespace value
} // namespace ell

#pragma endregion implementation
