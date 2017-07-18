////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRMapCompiler.tcc (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    template <typename ValueType>
    llvm::Value* IRMapCompiler::EnsurePortEmitted(const OutputPortBase& port, ValueType initialValue)
    {
        auto pVar = GetOrAllocatePortVariable(port, initialValue);
        return GetModule().EnsureEmitted(*pVar);
    }
}
}
